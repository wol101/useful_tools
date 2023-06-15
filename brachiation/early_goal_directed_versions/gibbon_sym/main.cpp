// modified by wis to include a specific definintion of the model
// to allow me to customise things a bit more easily

#include <assert.h>

#include <glut.h>

#include <dm.h>
#include <dmGLMouse.hpp>
#include <dmGLPolarCamera_zup.hpp>
#include <dmEnvironment.hpp>
#include <dmArticulation.hpp>

#include "Bar.h"
#include "ForceList.h"
#include "Simulation.h"
#include "Controller.h"
#include "ControllerList.h"
#include "GripModel.h"

// simulation global
Simulation *gSimulation;

// local globals
const int NAME_SIZE = 256;
double gRunLimit;				 // the time to run the simulation for (in simulation units)

// viewing variables
dmGLMouse *mouse;
dmGLPolarCamera_zup *camera;
GLfloat view_mat[4][4];
bool paused_flag = true;
bool step_flag = false;

// movie
static char gMovieDirectory[256] = "movie";
static int gWriteMovie = false;

void InitOpenGL(void);				 // initialise the OpenGL environment
void StartOpenGLLoop();				 // start the OpenGL loop
static void Display(void);			 // the OpenGL display loop
static void Reshape(int w, int h);		 // reshape the OpenGL view
static void ProcessKeyboard(unsigned char key, int, int);	// the OpenGL keyboard loop
static void ProcessSpecialKeys(int key, int, int);	// the OpenGL special key loop
static void Idle(void);				 // called by Open GL idle routine
static void WritePPM(char *pathname, int width, int height, unsigned char *rgb);

bool GetQuotedString(ifstream & in, char *string);	// useful routine
bool ReadParameter(ifstream & inp, char *m, int &i);
bool ReadParameter(ifstream & inp, char *m, double &i);

int main(int argc, char ** argv)
{
	double simData[100];			 // limits me to 100 parameters per controller ;->
	int i, j;
	int nGenes, startGene;
	ifstream geneMapingFile("gene_mapping.dat");
	ifstream controlGenomeFile("ControllerGenome.dat");
	char string[NAME_SIZE];
	ControllerList *controllers;
	Controller *theController;
	double *data;
	ofstream decodedGenome("DecodedGenome.txt");
	int genomeLength;
	ifstream controlFile("ControlFile.dat");
	bool err;

  	for (i = 0; i < argc; i++)
  	{
    		if (strcmp(argv[i], "--movie") == 0) gWriteMovie = true;
    	}
            
 	err = ReadParameter(controlFile, "genomeLength", genomeLength);
	if (err)
	{
		cerr << "Must Specify genomeLength in ControlFile.dat\n";
		return 1;
	}
	err = ReadParameter(controlFile, "runLimit", gRunLimit);
	if (err)
		gRunLimit = 5;

	data = new double[genomeLength];

	gSimulation = new Simulation();

	// intialise open GL (necessary for model creation)
	InitOpenGL();

	// turn off SetControllerDataFromFile

	gSimulation->SetControllerDataFromFile(false);

	// load the simulation data file
	gSimulation->Load("gibbon.cfg");

	// load the genome data into the controllers
	for (i = 0; i < genomeLength; i++)
		controlGenomeFile >> data[i];
	controllers = gSimulation->GetControllerList();
	while (GetQuotedString(geneMapingFile, string))
	{
		geneMapingFile >> nGenes >> startGene;
		decodedGenome << string << "\t";
		for (j = 0; j < nGenes; j++)
		{
			simData[j] = data[startGene + j];
			if (j < nGenes - 1)
				decodedGenome << simData[j] << "\t";
			else
				decodedGenome << simData[j] << "\n";
		}
		theController = controllers->FindByName(string);
		assert(theController);
		theController->SetParameters(nGenes, simData);
	}

	// log some forces
	((GripModel *)(gSimulation->GetForceList()->FindByName("left_hand_grip")))->startLog();
	((GripModel *)(gSimulation->GetForceList()->FindByName("right_hand_grip")))->startLog();

	//((GripModel *) (gSimulation->GetForceList ()->FindByName ("right_shoulder_abductor_insertion")))->startLog ();
	//((GripModel *)(gSimulation->GetForceList()->FindByName("right_shoulder_adductor_insertion")))->startLog();
	//((GripModel *)(gSimulation->GetForceList()->FindByName("right_shoulder_extensor_insertion")))->startLog();
	//((GripModel *)(gSimulation->GetForceList()->FindByName("right_shoulder_flexor_insertion")))->startLog();

	// a start
	StartOpenGLLoop();

	delete[]data;

}

//----------------------------------------------------------------------------
void InitOpenGL(void)
{
	int i, j;
	// this is the command line intialisation - obviously we currently don't support
	// all the standard options but adding them is easy enough
	{
		int argc = 1;
		char *progname = "Simulation";
		char **argv = &progname;
		glutInit(&argc, argv);
	}

	// create the required window
	glutInitWindowSize(640, 480);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutCreateWindow("DynaMechs Example");

	// lighting etc.
	GLfloat light_ambient[] = { 0.0, 0.0, 0.0, 1.0 };
	GLfloat light_diffuse[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat light_specular[] = { 1.0, 1.0, 1.0, 1.0 };
//              light_position is NOT default value
	GLfloat light_position[] = { 1.0, 1.0, 1.0, 0.0 };

	glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glDepthFunc(GL_LESS);
	glEnable(GL_DEPTH_TEST);

	glShadeModel(GL_FLAT);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	// mouse
	mouse = dmGLMouse::dmInitGLMouse();

	// transformation
	for (i = 0; i < 4; i++)
	{
		for (j = 0; j < 4; j++)
		{
			view_mat[i][j] = 0.0;
		}
		view_mat[i][i] = 1.0;
	}
	view_mat[3][2] = -10.0;

	// camera
	camera = new dmGLPolarCamera_zup();
	camera->setRadius(40.0);
	camera->setCOI(15.0, 10.0, 5.0);
	camera->setTranslationScale(0.02f);

}

//----------------------------------------------------------------------------
void StartOpenGLLoop()
{

	glutReshapeFunc(Reshape);
	glutKeyboardFunc(ProcessKeyboard);
	glutSpecialFunc(ProcessSpecialKeys);
	glutDisplayFunc(Display);
	glutIdleFunc(Idle);

	glutMainLoop();

}

//----------------------------------------------------------------------------
void Display(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();

	// ===============================================================

	(dmEnvironment::getEnvironment())->draw();
	gSimulation->GetBar()->draw();
	gSimulation->GetForceList()->draw();

	glPushAttrib(GL_ALL_ATTRIB_BITS);
	gSimulation->GetRobot()->draw();
	glPopAttrib();

	// ===============================================================
	// draw axes
	glDisable(GL_LIGHTING);

	glBegin(GL_LINES);
	glColor3f(1.0, 0.0, 0.0);
	glVertex3f(2.0, 0.0, 0.0);
	glVertex3f(0.0, 0.0, 0.0);
	glEnd();

	glBegin(GL_LINES);
	glColor3f(0.0, 1.0, 0.0);
	glVertex3f(0.0, 2.0, 0.0);
	glVertex3f(0.0, 0.0, 0.0);
	glEnd();

	glBegin(GL_LINES);
	glColor3f(0.0, 0.0, 1.0);
	glVertex3f(0.0, 0.0, 2.0);
	glVertex3f(0.0, 0.0, 0.0);
	glEnd();

	glEnable(GL_LIGHTING);

	glPopMatrix();

	glFlush();
	glutSwapBuffers();
}

//----------------------------------------------------------------------------
void Reshape(int w, int h)
{
	glViewport(0, 0, w, h);
	mouse->win_size_x = w;
	mouse->win_size_y = h;

	camera->setPerspective(45.0, (GLfloat) w / (GLfloat) h, 1.0, 200.0);

	camera->setViewMat(view_mat);
	camera->applyView();
}

//----------------------------------------------------------------------------
void ProcessKeyboard(unsigned char key, int, int)
{
	switch (key)
	{
	case 27:
		glutDestroyWindow(glutGetWindow());
		exit(1);
		break;

	case 'p':
		paused_flag = !paused_flag;
		break;

	case 's':
		step_flag = true;
		break;

	}
}


//----------------------------------------------------------------------------
void ProcessSpecialKeys(int key, int, int)
{
	switch (key)
	{
	case GLUT_KEY_LEFT:
		break;
	case GLUT_KEY_RIGHT:
		break;
	case GLUT_KEY_UP:
		break;
	case GLUT_KEY_DOWN:
		break;
	}
}


//----------------------------------------------------------------------------
void Idle(void)
{
  	bool newPicture = false;
  
	if (!paused_flag)
	{
		gSimulation->UpdateSimulation();
		// stop after gRunLimit because that's how long it's optimised for by the GA
                newPicture = true;
		if (gSimulation->GetTime() > gRunLimit)
			paused_flag = true;
	} else
	{
		if (step_flag)
		{
			step_flag = false;
			gSimulation->UpdateSimulation();
                	newPicture = true;
		}
	}

	camera->update(mouse);
	camera->applyView();

	Display();
        
        if (newPicture && gWriteMovie && 
            fabs((gSimulation->GetTime() * 25) - int(gSimulation->GetTime() * 25 + 0.5)) < 0.0000000001)
        {
          int tw, th;
          tw = mouse->win_size_x;
          th = mouse->win_size_y;
          unsigned char *rgb = new unsigned char[tw * th * 3];
          glReadBuffer(GL_FRONT);
          glPixelStorei(GL_PACK_ALIGNMENT, 1);
          glReadPixels(0, 0, tw, th, GL_RGB, GL_UNSIGNED_BYTE, rgb);
          char pathname[sizeof(gMovieDirectory) + 256];
          // gcc gives a warning with %lf - probably not necessary anyway
          // sprintf(pathname, "%s/Frame%010.4lf.ppm", gMovieDirectory, gSimulation->GetTime());
          sprintf(pathname, "%s/Frame%010.4f.ppm", gMovieDirectory, gSimulation->GetTime());
          WritePPM(pathname, tw, th, (unsigned char *)rgb);   
          delete [] rgb;
        }
}

bool GetQuotedString(ifstream & in, char *string)
{
	if (in.getline(string, NAME_SIZE, '\042'))
	{
		if (in.getline(string, NAME_SIZE, '\042'))
		{
			if (strlen(string) > 0)
			{
				//cerr << "Got name: " << name << endl;
				return true;
			}
		}
	}
	return false;
}

bool ReadParameter(ifstream & inp, char *m, int &i)
{
	char buffer[NAME_SIZE];

	inp.seekg(0);

	do
	{
		inp >> buffer;
		if (buffer[0] == 0)
			return true;
	}
	while (strcmp(m, buffer) != 0);

	inp >> i;

	return false;
}

bool ReadParameter(ifstream & inp, char *m, double &i)
{
	char buffer[NAME_SIZE];

	inp.seekg(0);

	do
	{
		inp >> buffer;
		if (buffer[0] == 0)
			return true;
	}
	while (strcmp(m, buffer) != 0);

	inp >> i;

	return false;
}

// write a PPM file (need to invert the y axis)
void WritePPM(char *pathname, int width, int height, unsigned char *rgb)
{
  FILE *out;
  int i;
  
  out = fopen(pathname, "wb");
  
  // need to invert write order
  fprintf(out, "P6\n# Produced by objective\n%d %d\n255\n", width, height);
  for (i = height - 1; i >= 0; i--)
    fwrite(rgb + (i * width * 3), width * 3, 1, out);

  fclose(out);
}
