#include <stdio.h>
#include <iostream.h>
#include <iomanip.h>
#include <fstream.h>
#include <assert.h>
#include <unistd.h>

#include <glut.h>

#include <dmGLTime.h>
#include <dmGLMouse.h>
#include <dmGLPolarCamera_zup.h>
#include <dmEnvironment.h>
#include <dmArticulation.h>

#include "Fitness.h"
#include "Simulation.h"
#include "ForceList.h"
#include "Controller.h"
#include "ControllerList.h"

// Simulation - must be a global instantiated somewhere
extern Simulation *gSimulation;

// debugging control - set to zero for no debugging output
int gDebug = 0;

// viewing variables
dmGLMouse *mouse;
dmGLPolarCamera_zup *camera;
GLfloat view_mat[4][4];
bool paused_flag = true;
bool step_flag = false;
bool slow_flag = false;

void InitOpenGL(void);				 // initialise the OpenGL environment
void StartOpenGLLoop();				 // start the OpenGL loop
static void Display(void);			 // the OpenGL display loop
static void Reshape(int w, int h);		 // reshape the OpenGL view
static void ProcessKeyboard(unsigned char key, int, int);	// the OpenGL keyboard loop
static void ProcessSpecialKeys(int key, int, int);	// the OpenGL special key loop
static void Idle(void);				 // called by Open GL idle routine

int main(int argc, char ** argv)
{
	char *genomeFilenamePtr = 0;
	bool outputKineticsFlag = false;
	bool displayFlag = false;
	float score;
	int i;
	Fitness myFitness;
	
	// do some simple stuff with command line arguments
	
	for (i = 1; i < argc; i++)
	{
		if (strcmp(argv[i], "--kinetics") == 0 ||
			strcmp(argv[i], "-k") == 0)
			outputKineticsFlag = true;
		else
		if (strcmp(argv[i], "--display") == 0 ||
			strcmp(argv[i], "-d") == 0)
			displayFlag = true;
		else
		if (strcmp(argv[i], "--genome") == 0 ||
			strcmp(argv[i], "-g") == 0)
		{
			i++;
			if (i >= argc) 
			{
				cerr << "Error parsing genome filename\n";
				return 1;
			}
			genomeFilenamePtr = argv[i];
		}
		else
		if (strcmp(argv[i], "--debug") == 0 ||
			strcmp(argv[i], "-e") == 0)
		{
			i++;
			if (i >= argc) 
			{
				cerr << "Error parsing debug level\n";
				return 1;
			}
			if (sscanf(argv[i], "%d", &gDebug) == 0)
			{
				cerr << "Error parsing debug level\n";
				return 1;
			}
		}
		else
		if (strcmp(argv[i], "--help") == 0 ||
			strcmp(argv[i], "-h") == 0 ||
			strcmp(argv[i], "-?") == 0)
		{
			cout << 			
"\nobjective produces a fitness score in score.tmp depending on the input data in\n\
ControlFile.dat, genome.tmp, and GeneMapping.dat and the model information in\n\
Biped.cfg (which generally loads in Biped.env, Biped.dm30, Terrain.dat and a\n\
number of .xan files).\n\n\
There are a number of command line options available:\n\n\
-k, --kinetics\n\
Creates a kinetics file (Kinetics.dat) containing kinetic data for all the\n\
segments in the model\n\n\
-d, --display\n\
Switches to interactive mode. No score.tmp file is produced and an animation of\n\
the model is produced in a new window. The animation is started and stopped with\n\
the (p) key, stepped with the (s) key, toggled between slow and fast with the\n\
(l) key, and quit with the (esc) key\n\n\
-g filename, --genome filename\n\
Reads filename rather than the default genome.tmp as the genome data\n\n\
-e n, --debug n\n\
Prints out a lot of extra debugging information on stderr if n is higher than 0.\n\
Eventually the value of n might make a difference to how much extra debugging\n\
information you get but currently anything above 0 gives you everything\n\n\
-h, -?, --help\n\
Prints this message!\n\n";
			return 1;
		}
		else
		{
			cerr << "Unrecognised option. Try 'objective --help' for more info\n";
			return 1;
		}
	}

	if (outputKineticsFlag) myFitness.SetKineticsFileName("OutputKinetics.dat");
	if (genomeFilenamePtr) myFitness.SetGenomeFileName(genomeFilenamePtr);
	
	if (displayFlag) InitOpenGL();
	
	myFitness.ReadInitialisationData(displayFlag);
	
	if (displayFlag)
	{
		StartOpenGLLoop();	
	}
	else
	{								
		score = (float)myFitness.CalculateFitness();

		ofstream outFile("score.tmp");
		if (outFile.bad()) return 1;
		outFile << score;
		outFile.close();
	}
	
	return 0;
}

//----------------------------------------------------------------------------
void InitOpenGL(void)
{
	int i, j;
	// this is the command line intialisation - obviously we currently don't support
	// all the standard options but adding them is easy enough
	{
		int argc = 1;
		char *progname = "objective";
		char **argv = &progname;
		glutInit(&argc, argv);
	}

	// create the required window
	glutInitWindowSize(800, 600);
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
	camera->setRadius(4.0);
	camera->setCOI(5.0, 5.0, 0.5);
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
		if (paused_flag == false) cerr << "Running\n";
		break;

	case 's':
		step_flag = true;
		break;

	case 'l':
		slow_flag = !slow_flag;
		if (slow_flag == true) cerr << "Slow\n";
		else cerr << "Fast\n";
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
	if (!paused_flag)
	{
		gSimulation->UpdateSimulation();
		cout << gSimulation->GetTime() << "\n";
		cout.flush();
		if (slow_flag) sleep(1);
	} else
	{
		if (step_flag)
		{
			step_flag = false;
			gSimulation->UpdateSimulation();
			cout << gSimulation->GetTime() << "\n";
			cout.flush();
		}
	}

	camera->update(mouse);
	camera->applyView();

	Display();
	
}
