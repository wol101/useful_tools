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
#include "Segments.h"
#include "SegmentParameters.h"
#include "LoggedForce.h"

#define DEBUG_EXTERN
#include "DebugControl.h"

// Simulation - must be a global instantiated somewhere
extern Simulation *gSimulation;

// viewing variables
dmGLMouse *mouse;
dmGLPolarCamera_zup *camera;
GLfloat view_mat[4][4];
bool paused_flag = true;
bool step_flag = false;
bool slow_flag = false;
// set kDisplaySkip to 1 to display every frame
// 250 seems approx real time and 200 gives 25 fps at 0.0002 kIntegrationStepsize
int kDisplaySkip = 200; 
double gTimeLimit = 1;

// tracking + view
// some suggestions
// for tracking set gCameraRadius to 15 and gTrackingFlag to true
// for full view set gCameraRadius to 80 and gCOIx to 18
// for close up of start set gCameraRadius to 10 and gCOIx to 6
bool gTrackingFlag = true;
double gCameraRadius = 12;
double gCOIx = 5;
double gCOIy = 5;
double gCOIz = 0;
  
// movie
char *gMovieDirectoryPtr = 0;
int gWidth;
int gHeight;

void InitOpenGL(void);            // initialise the OpenGL environment
void StartOpenGLLoop();           // start the OpenGL loop
static void Display(void);        // the OpenGL display loop
static void Reshape(int w, int h);      // reshape the OpenGL view
static void ProcessKeyboard(unsigned char key, int, int); // the OpenGL keyboard loop
static void ProcessSpecialKeys(int key, int, int);  // the OpenGL special key loop
static void Idle(void);           // called by Open GL idle routine
static void WritePPM(char *pathname, int gWidth, int gHeight, unsigned char *rgb);   
static void GLOutputText(GLfloat x, GLfloat y, GLfloat z, char *text, double size, int plane);

int main(int argc, char ** argv)
{
  char *genomeFilenamePtr = "genome.tmp";
  char *configFilenamePtr = "ModelData.txt";
  char *scoreFilenamePtr = "score.tmp";
  bool outputKineticsFlag = false;
  bool displayFlag = false;
  double score;
  int i, j;
  Fitness myFitness;
  
  // debugging control - set to zero for no debugging output
  gDebug = NoDebug;

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
    if (strcmp(argv[i], "--score") == 0 ||
      strcmp(argv[i], "-s") == 0)
    {
      i++;
      if (i >= argc) 
      {
        cerr << "Error parsing score filename\n";
        return 1;
      }
      scoreFilenamePtr = argv[i];
    }
    else
    if (strcmp(argv[i], "--movie") == 0 ||
      strcmp(argv[i], "-m") == 0)
    {
      i++;
      if (i >= argc) 
      {
        cerr << "Error parsing movie filename\n";
        return 1;
      }
      gMovieDirectoryPtr = argv[i];
    }
    else
    if (strcmp(argv[i], "--config") == 0 ||
      strcmp(argv[i], "-c") == 0)
    {
      i++;
      if (i >= argc) 
      {
        cerr << "Error parsing config filename\n";
        return 1;
      }
      configFilenamePtr = argv[i];
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
      if (sscanf(argv[i], "%d", &j) == 0)
      {
        cerr << "Error parsing debug level\n";
        return 1;
      }
      gDebug = (DebugControl)j;
    }
    else
    if (strcmp(argv[i], "--timeLimit") == 0 ||
      strcmp(argv[i], "-t") == 0)
    {
      i++;
      if (i >= argc) 
      {
        cerr << "Error parsing debug level\n";
        return 1;
      }
      if (sscanf(argv[i], "%lf", &gTimeLimit) == 0)
      {
        cerr << "Error parsing debug level\n";
        return 1;
      }
    }
    else
    if (strcmp(argv[i], "--cameraRadius") == 0)
    {
      i++;
      if (i >= argc) 
      {
        cerr << "Error parsing camera radius\n";
        return 1;
      }
      if (sscanf(argv[i], "%lf", &gCameraRadius) == 0)
      {
        cerr << "Error parsing camera radius\n";
        return 1;
      }
    }
    else
    if (strcmp(argv[i], "--cameraCOI") == 0)
    {
      i++;
      if (i >= argc) 
      {
        cerr << "Error parsing camera COI x\n";
        return 1;
      }
      if (sscanf(argv[i], "%lf", &gCOIx) == 0)
      {
        cerr << "Error parsing camera COI x\n";
        return 1;
      }
      i++;
      if (i >= argc) 
      {
        cerr << "Error parsing camera COI y\n";
        return 1;
      }
      if (sscanf(argv[i], "%lf", &gCOIy) == 0)
      {
        cerr << "Error parsing camera COI y\n";
        return 1;
      }
      i++;
      if (i >= argc) 
      {
        cerr << "Error parsing camera COI z\n";
        return 1;
      }
      if (sscanf(argv[i], "%lf", &gCOIz) == 0)
      {
        cerr << "Error parsing camera COI z\n";
        return 1;
      }
    }
    else
    if (strcmp(argv[i], "--tracking") == 0 ||
      strcmp(argv[i], "-r") == 0)
    {
      i++;
      if (i >= argc) 
      {
        cerr << "Error parsing tracking flag\n";
        return 1;
      }
      if (sscanf(argv[i], "%d", &j) == 0)
      {
        cerr << "Error parsing tracking flag\n";
        return 1;
      }
      if (j) gTrackingFlag = true;
      else gTrackingFlag = false;
    }
    else
    if (strcmp(argv[i], "--help") == 0 ||
      strcmp(argv[i], "-h") == 0 ||
      strcmp(argv[i], "-?") == 0)
    {
      cout <<       
"\nObjective build " << __DATE__ << " " << __TIME__ << "\n" <<
"\nobjective produces a fitness score in score.tmp depending on the input data in\n\
genome.tmp and the model information in ModelData.txt which generally loads s\n\
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
-c filename, --config filename\n\
Reads filename rather than the default ModelData.tmp as the config data\n\n\
-s filename, --score filename\n\
Writes filename rather than the default score.tmp as the genome data\n\n\
-m dirname, --movie dirname\n\
Writes out each frame as a separate file in directory dirname\n\n\
-e n, --debug n\n\
Prints out a lot of extra debugging information on stderr if n is higher than 0.\n\
Suitable values of n are defined in DebugControl.h\n\n\
-r n, --tracking n\n\
If n is non-zero, camera tracks the walking model otherwise camera is fixed.\n\n\
--cameraRadius n\n\
Sets the viewing radius of the camera (approx).\n\n\
--cameraCOI x y z\n\
Sets the centre of interest of the camera - requires separate x y and z values.\n\n\
-t n, --timeLimit n\n\
Sets a time limit on the simulation. The simulation will pause after this limit\n\
in display mode. It has no effect in fitness only mode since the time limit\n\
will be set internally.\n\n\
-h, -?, --help\n\
Prints this message!\n\n\
When running:\n\n\
p - toggles pause (starts paused)\n\
s - steps a single time increment\n\
l - toggles slow mode (starts fast)\n\
esc - quits\n\n";
      return 1;
    }
    else
    {
      cerr << "Unrecognised option. Try 'objective --help' for more info\n";
      return 1;
    }
  }

  if (outputKineticsFlag) myFitness.SetKineticsFileName("OutputKinetics.dat");
  myFitness.SetGenomeFileName(genomeFilenamePtr);
  myFitness.SetConfigFileName(configFilenamePtr);
  
  if (displayFlag) 
  {
    InitOpenGL();
    if (gMovieDirectoryPtr)
    {
      char command[256];
      sprintf(command, "mkdir %s", gMovieDirectoryPtr);
      system(command);
    }
  }
  
  myFitness.ReadInitialisationData(displayFlag);
  
  if (displayFlag)
  {
    StartOpenGLLoop();  
  }
  else
  {
    FILE *out;                
    score = myFitness.CalculateFitness();
    out = fopen(scoreFilenamePtr, "wb");
    fwrite(&score, sizeof(double), 1, out);
    fclose(out);
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
  glutInitWindowSize(1200, 900);
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
  glutCreateWindow("DynaMechs Example");

  // lighting etc.
  GLfloat light_ambient[] = { 0.0, 0.0, 0.0, 1.0 };
  GLfloat light_diffuse[] = { 1.0, 1.0, 1.0, 1.0 };
  GLfloat light_specular[] = { 1.0, 1.0, 1.0, 1.0 };
//        light_position is NOT default value
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
  camera->setRadius(gCameraRadius);
  camera->setCOI(gCOIx, gCOIy, gCOIz);
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
  int i;
  char buffer[16];
  GLfloat v;
  
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
  
  glColor3f(0.5, 0.5, 1.0);
  for (i = -5; i <= 50; i += 1)
  {
    sprintf(buffer, "%d", i);
    GLOutputText((GLfloat)i + 5.0, 5, -0.1, buffer, 0.05, 1);
  }

  for (v = 0; v <= 55; v += 0.5)
  {
    glBegin(GL_LINES);
    glVertex3f(v, 5, 0);
    glVertex3f(v, 5, -0.02);
    glEnd();
  }

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

  camera->setPerspective(15.0, (GLfloat) w / (GLfloat) h, 1.0, 200.0);
  
  camera->setViewMat(view_mat);
  camera->applyView();
  
  gWidth = w;
  gHeight = h;
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
  int i;
  static double cameraCOIX = 0;
  bool newPicture = false;
  
  if (!paused_flag)
  {
    for (i = 0; i < kDisplaySkip; i++)
    {
      if (gSimulation->GetTime() >= gTimeLimit)
      {
        paused_flag = true;
        break;
      }
      gSimulation->UpdateSimulation();
      newPicture = true;
    }
    cout << gSimulation->GetTime() << "\n";
    cout.flush();
    if (gDebug == MainDebug)
    {
      cerr << "Idle\tLeftAnkleFlexor\t" <<
        gSimulation->GetForceList()->FindByName(kLeftAnkleFlexorName)->getForce() <<
	"\tRightAnkleFlexor\t" <<
        gSimulation->GetForceList()->FindByName(kRightAnkleFlexorName)->getForce() <<
	"\n";
    }
    if (slow_flag) sleep(1);
  } 
  else
  {
    if (step_flag)
    {
      step_flag = false;
      gSimulation->UpdateSimulation();
      newPicture = true;
      cout << gSimulation->GetTime() << "\n";
      cout.flush();
    }
  }

  if (gTrackingFlag && gSimulation->GetTime() > 0.0)
  {
    int linkIndex = gSimulation->GetRobot()->getLinkIndex(
        gSimulation->GetTorso());
    const dmABForKinStruct *theDmABForKinStruct = 
        gSimulation->GetRobot()->getForKinStruct(linkIndex);
    if (theDmABForKinStruct->p_ICS[XC] != cameraCOIX)
    {
      cameraCOIX = theDmABForKinStruct->p_ICS[XC];
      camera->setCOI(cameraCOIX, gCOIy, gCOIz);
      if (gDebug == MainDebug)
        cerr << "Idle\tcameraCOIX\t" << cameraCOIX << "\n";
    }

  }
  
  camera->update(mouse);
  camera->applyView();

  Display();
  
  if (newPicture && gMovieDirectoryPtr)
  {
    unsigned char *rgb = new unsigned char[gWidth * gHeight * 3];
    glReadBuffer(GL_FRONT);
    glPixelStorei(GL_PACK_ALIGNMENT, 1);
    glReadPixels(0, 0, gWidth, gHeight, GL_RGB, GL_UNSIGNED_BYTE, rgb);
    char pathname[256];
    sprintf(pathname, "%s/Frame%010.4lf.ppm", gMovieDirectoryPtr, gSimulation->GetTime());
    WritePPM(pathname, gWidth, gHeight, (unsigned char *)rgb);   
    delete [] rgb;
  }
  
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

// write some 3D text at specified location x, y, z
// plane = 0; draw in x = 0 plane
// plane = 1; draw in y = 0 plane
// plane = 2; draw in z = 0 plane  
void GLOutputText(GLfloat x, GLfloat y, GLfloat z, char *text, double size, int plane)
{
  char *p;
  assert(plane >=0 && plane <= 2);
  
  glPushMatrix();
  glTranslatef(x, y, z);
  // characters are roughly 100 units so scale accordingly 
  glScalef( size / 100, size / 100, size / 100);
  // with no rotation, text is in the z = 0 plane 
  if (plane == 0) glRotatef(90, 0, 1, 0); // x = 0 plane
  else if (plane == 1) glRotatef(90, 1, 0, 0); // y = 0 plane
  for (p = text; *p; p++)
    glutStrokeCharacter(GLUT_STROKE_MONO_ROMAN, *p);
  glPopMatrix();
}
