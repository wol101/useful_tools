#include <stdio.h>
#include <iostream.h>
#include <iomanip.h>
#include <fstream.h>
#include <assert.h>
#include <unistd.h>

#include <glut.h>

#include <dm.h>
#include <dmu.h>
#include <dmGLMouse.hpp>
#include <dmGLPolarCamera_zup.hpp>
#include <dmEnvironment.hpp>
#include <dmArticulation.hpp>

#include "Simulation.h"
#include "ForceList.h"
#include "Segments.h"
#include "SegmentParameters.h"
#include "ExtendedForce.h"
#include "ModifiedContactModel.h"
#include "MAMuscle.h"

#define DEBUG_EXTERN
#include "DebugControl.h"

// Energy variables
const double kTensionPowerCost = 0.001; // Watts per Newton
const double kPositiveWorkCost = 1.0; // multiplier
const double kNegativeWorkCost = 0.5; // multiplier

// Simulation global
Simulation *gSimulation;

// viewing variables
dmGLMouse *mouse;
dmGLPolarCamera_zup *camera;
GLfloat view_mat[4][4];
bool paused_flag = true;
bool step_flag = false;
bool slow_flag = false;
// set kDisplaySkip to 1 to display every frame
// 250 seems approx real time and 200 gives 25 fps at 0.0002 kIntegrationStepsize
int kDisplaySkip = 250; 

// 10 seconds when maximising speed 
// 30 when maximising economy     
double gTimeLimit = 50;
double gEnergyLimit = 0; // set to <= 0 to ignore

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

// kinetics file
char * gKineticsFilenamePtr = 0;

// summary flag
bool gSummaryFlag = false;
    
// axes
bool gAxisFlag = true;
double gAxisSize = 0.1;

void InitOpenGL(void);            // initialise the OpenGL environment
void StartOpenGLLoop();           // start the OpenGL loop
static void Display(void);        // the OpenGL display loop
static void Reshape(int w, int h);      // reshape the OpenGL view
static void ProcessKeyboard(unsigned char key, int, int); // the OpenGL keyboard loop
static void ProcessSpecialKeys(int key, int, int);  // the OpenGL special key loop
static void Idle(void);           // called by Open GL idle routine
static void WritePPM(char *pathname, int gWidth, int gHeight, unsigned char *rgb);   
static void GLOutputText(GLfloat x, GLfloat y, GLfloat z, char *text, double size, int plane);
static double CalculateFitness();
static void OutputKinetics();
static void ConvertLocalToWorldP(const dmABForKinStruct *m, 
   const CartesianVector local, CartesianVector world);
static void ConvertLocalToWorldV(const dmABForKinStruct *m, 
   const SpatialVector local, SpatialVector world);
static double CalculateEnergyCost(MAMuscle *strapForce);

int main(int argc, char ** argv)
{
  char *genomeFilenamePtr = "genome.tmp";
  char *configFilenamePtr = "ModelData.txt";
  char *scoreFilenamePtr = "score.tmp";
  bool displayFlag = false;
  double score;
  int i, j;
  
  // debugging control - set to zero for no debugging output
  gDebug = NoDebug;

  // do some simple stuff with command line arguments
  
  for (i = 1; i < argc; i++)
  {
    if (strcmp(argv[i], "--kinetics") == 0 ||
      strcmp(argv[i], "-k") == 0)
    {
      i++;
      if (i >= argc) 
      {
        cerr << "Error parsing kinetics filename\n";
        return 1;
      }
      gKineticsFilenamePtr = argv[i];
    }
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
    if (strcmp(argv[i], "--summary") == 0 ||
      strcmp(argv[i], "-u") == 0)
    {
      gSummaryFlag = true;
    }
    else
    if (strcmp(argv[i], "--timeLimit") == 0 ||
      strcmp(argv[i], "-t") == 0)
    {
      i++;
      if (i >= argc) 
      {
        cerr << "Error parsing time limit\n";
        return 1;
      }
      if (sscanf(argv[i], "%lf", &gTimeLimit) == 0)
      {
        cerr << "Error parsing time limit\n";
        return 1;
      }
    }
    else
    if (strcmp(argv[i], "--energyLimit") == 0 ||
      strcmp(argv[i], "-y") == 0)
    {
      i++;
      if (i >= argc) 
      {
        cerr << "Error parsing energy limit\n";
        return 1;
      }
      if (sscanf(argv[i], "%lf", &gEnergyLimit) == 0)
      {
        cerr << "Error parsing energy limit\n";
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
      cerr <<       
"\nObjective build " << __DATE__ << " " << __TIME__ << "\n" <<
"\nobjective produces a fitness score in score.tmp depending on the input data in\n\
genome.tmp and the model information in ModelData.txt which generally loads s\n\
number of .xan files).\n\n\
There are a number of command line options available:\n\n\
-d, --display\n\
Switches to interactive mode. No score.tmp file is produced and an animation of\n\
the model is produced in a new window. The animation is started and stopped with\n\
the (p) key, stepped with the (s) key, toggled between slow and fast with the\n\
(l) key, and quit with the (esc) key\n\n\
-k filename, --kinetics filename\n\
Creates a kinetics file containing kinetic data for all the\n\
segments in the model\n\n\
-g filename, --genome filename\n\
Reads filename rather than the default genome.tmp as the genome data\n\n\
-c filename, --config filename\n\
Reads filename rather than the default ModelData.tmp as the config data\n\n\
-s filename, --score filename\n\
Writes filename rather than the default score.tmp as the genome data\n\n\
-m dirname, --movie dirname\n\
Writes out each frame as a separate file in directory dirname\n\n\
-u, --summary\n\
Writes out summary information at end of fitness run.\n\n\
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
in display mode or stop in fitness mode.\n\n\
-y n, --energyLimit n\n\
Sets an energy limit on the simulation. The simulation will stop after this\n\
limit in fitness mode. No effect in display mode. If <= 0 then no limit!\n\n\
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
  
  // create the simulation object
  gSimulation = new Simulation();
  gSimulation->SetGraphics(displayFlag);
  gSimulation->LoadModel(configFilenamePtr);
  
  // load the genome
  gSimulation->GetCPG()->ReadGenome(genomeFilenamePtr);
  
  if (displayFlag)
  {
    StartOpenGLLoop();  
  }
  else
  {
    FILE *out;                
    score = CalculateFitness();
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
  //glutInitWindowSize(1200, 900);
  glutInitWindowSize(640, 480);
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
  gSimulation->GetCPG()->draw();
  
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
    cerr << gSimulation->GetTime() << "\n";
    cerr.flush();
    if (slow_flag) usleep(100000); // 0.1 s
  } 
  else
  {
    if (step_flag)
    {
      step_flag = false;
      gSimulation->UpdateSimulation();
      newPicture = true;
      cerr << gSimulation->GetTime() << "\n";
      cerr.flush();
    }
  }

  if (gTrackingFlag && gSimulation->GetTime() > 0.0)
  {
    int linkIndex = gSimulation->GetRobot()->getLinkIndex(
        gSimulation->GetTorso());
    const dmABForKinStruct *theDmABForKinStruct = 
        gSimulation->GetRobot()->getForKinStruct(linkIndex);
    if (theDmABForKinStruct->p_ICS[0] != cameraCOIX)
    {
      cameraCOIX = theDmABForKinStruct->p_ICS[0];
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
    // gcc gives a warning with %lf - probably not necessary anyway
    // sprintf(pathname, "%s/Frame%010.4lf.ppm", gMovieDirectoryPtr, gSimulation->GetTime());
    sprintf(pathname, "%s/Frame%010.4f.ppm", gMovieDirectoryPtr, gSimulation->GetTime());
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

double
CalculateFitness()
{
  dmLink *myLink;
  unsigned int torsoIndex;
  const dmABForKinStruct *theTorsoDmABForKinStruct;
  SpatialVector torsoWorldV;
  
  // get the indices of torso
  myLink = (dmLink *) dmuFindObject(kTorsoPartName, gSimulation->GetRobot());
  assert(myLink);
  torsoIndex = gSimulation->GetRobot()->getLinkIndex(myLink);
    
  // get some pointers for later
  theTorsoDmABForKinStruct = gSimulation->GetRobot()->getForKinStruct(torsoIndex);

  // and run for required amount of time
  double score = 0;
  int runCount = 0;
  double energy = 0;
  bool successFlag = true;
  
  // 10 seconds when maximising speed 
  // 30 when maximising economy     
  while (gSimulation->GetTime() < gTimeLimit)
  {

    gSimulation->UpdateSimulation();
        
    // use torso position as a sanity check
    if (theTorsoDmABForKinStruct->p_ICS[2] < 0.8 || 
      theTorsoDmABForKinStruct->p_ICS[2] > 1.6) 
    {
      successFlag = false;
      break;
    }
    
    // get torso world velocity and use as sanity check
    ConvertLocalToWorldV(theTorsoDmABForKinStruct, theTorsoDmABForKinStruct->v, 
      torsoWorldV);
    if (torsoWorldV[3] < -1.0 || torsoWorldV[3] > 10.0)
    {
      successFlag = false;
      break;
    }
    
    // calculate energy driving system
    
    energy += CalculateEnergyCost(gSimulation->GetCPG()->GetLeftHipExtensor());
    energy += CalculateEnergyCost(gSimulation->GetCPG()->GetLeftHipFlexor());
    energy += CalculateEnergyCost(gSimulation->GetCPG()->GetRightHipExtensor());
    energy += CalculateEnergyCost(gSimulation->GetCPG()->GetRightHipFlexor());
    energy += CalculateEnergyCost(gSimulation->GetCPG()->GetLeftKneeExtensor());
    energy += CalculateEnergyCost(gSimulation->GetCPG()->GetLeftKneeFlexor());
    energy += CalculateEnergyCost(gSimulation->GetCPG()->GetRightKneeExtensor());
    energy += CalculateEnergyCost(gSimulation->GetCPG()->GetRightKneeFlexor());
    energy += CalculateEnergyCost(gSimulation->GetCPG()->GetLeftAnkleExtensor());
    energy += CalculateEnergyCost(gSimulation->GetCPG()->GetLeftAnkleFlexor());
    energy += CalculateEnergyCost(gSimulation->GetCPG()->GetRightAnkleExtensor());
    energy += CalculateEnergyCost(gSimulation->GetCPG()->GetRightAnkleFlexor());
    
    // adding this criterion makes the distance proportional to efficiency rather
    // than speed
    if (gEnergyLimit > 0)
    {
      if (energy >= gEnergyLimit) break;
    }

    if (gDebug == FitnessDebug) 
      cerr << "CalculateFitness\tgSimulation->GetTime()\t" << gSimulation->GetTime() << 
        "\ttheTorsoDmABForKinStruct->p_ICS[0]\t" << theTorsoDmABForKinStruct->p_ICS[0] << 
        "\tenergy\t" << energy << "\n";

    runCount++;
    if (gKineticsFilenamePtr) OutputKinetics();
  }
  score = theTorsoDmABForKinStruct->p_ICS[0] - 5;
  if (gDebug == FitnessDebug) 
    cerr << "CalculateFitness\tscore\t" << score << 
        "\tenergy\t" << energy << "\n";
  
  if (gSummaryFlag) 
    cerr << "Simulation Time: " << gSimulation->GetTime() << 
      " Distance: " << theTorsoDmABForKinStruct->p_ICS[0] - 5 << 
      " Energy: " << energy << "\n";
  
  return score;
}

// function to produce a file of link kinetics

void 
OutputKinetics()
{
  unsigned int myNumLinks = gSimulation->GetRobot()->getNumLinks();
  const dmABForKinStruct *myDmABForKinStruct;
  unsigned int i;
  int j, k;
  static bool firstTimeFlag = true;
  static ofstream outputKineticsFile;
  
  if (gKineticsFilenamePtr == 0) return;
  
  // first time through output the column headings
  if (firstTimeFlag)
  {
    outputKineticsFile.open(gKineticsFilenamePtr);
    
    outputKineticsFile << "time\t";
    for (i = 0; i < myNumLinks; i++)
    {
      outputKineticsFile << 
        gSimulation->GetRobot()->getLink(i)->getName() 
        << "_X\t";
      outputKineticsFile << 
        gSimulation->GetRobot()->getLink(i)->getName() 
        << "_Y\t";
      outputKineticsFile << 
        gSimulation->GetRobot()->getLink(i)->getName() 
        << "_Z\t";
      outputKineticsFile << 
        gSimulation->GetRobot()->getLink(i)->getName() 
        << "_R00\t";
      outputKineticsFile << 
        gSimulation->GetRobot()->getLink(i)->getName() 
        << "_R01\t";
      outputKineticsFile << 
        gSimulation->GetRobot()->getLink(i)->getName() 
        << "_R02\t";
      outputKineticsFile << 
        gSimulation->GetRobot()->getLink(i)->getName() 
        << "_R10\t";
      outputKineticsFile << 
        gSimulation->GetRobot()->getLink(i)->getName() 
        << "_R11\t";
      outputKineticsFile << 
        gSimulation->GetRobot()->getLink(i)->getName() 
        << "_R12\t";
      outputKineticsFile << 
        gSimulation->GetRobot()->getLink(i)->getName() 
        << "_r20\t";
      outputKineticsFile << 
        gSimulation->GetRobot()->getLink(i)->getName() 
        << "_R21\t";
      outputKineticsFile << 
        gSimulation->GetRobot()->getLink(i)->getName() 
        << "_R22";
      if (i != myNumLinks - 1) outputKineticsFile << "\t";
      else outputKineticsFile << "\n";    
    }
    firstTimeFlag = false;
  }     
  
  // start by outputting the simulation time
  outputKineticsFile << gSimulation->GetTime() << "\t";
  
  for (i = 0; i < myNumLinks; i++)
  {
    // output the position
    myDmABForKinStruct = gSimulation->GetRobot()->getForKinStruct(i);
    outputKineticsFile << myDmABForKinStruct->p_ICS[0] << "\t"
      << myDmABForKinStruct->p_ICS[1] << "\t"
      << myDmABForKinStruct->p_ICS[2] << "\t";
    // then the orientation
    for (j = 0; j < 3; j++)
    {
      for (k = 0; k < 3; k++)
      {
        outputKineticsFile << myDmABForKinStruct->R_ICS[j][k];
        if (k != 2) outputKineticsFile << "\t";
      }
      if (j != 2) outputKineticsFile << "\t";
    }
    if (i != myNumLinks - 1) outputKineticsFile << "\t";
    else outputKineticsFile << "\n";    
  }
}

  
// convert a link local coordinate to a world coordinate

void 
ConvertLocalToWorldP(const dmABForKinStruct *m, const CartesianVector local, CartesianVector world)
{
  for (int j = 0; j < 3; j++)
  {

    world[j] = m->p_ICS[j] +
      m->R_ICS[j][0] * local[0] +
      m->R_ICS[j][1] * local[1] +
      m->R_ICS[j][2] * local[2];
  }
}

// convert a link local coordinate to a world coordinate
// don't know if this is actually corect for the rotations (are they
// global or local?)

void 
ConvertLocalToWorldV(const dmABForKinStruct *m, const SpatialVector local, SpatialVector world)
{
  for (int j = 0; j < 3; j++)
  {
    world[j] = 
      m->R_ICS[j][0] * local[0] +
      m->R_ICS[j][1] * local[1] +
      m->R_ICS[j][2] * local[2];

    world[j + 3] = 
      m->R_ICS[j][0] * local[0 + 3] +
      m->R_ICS[j][1] * local[1 + 3] +
      m->R_ICS[j][2] * local[2 + 3];
  }
}

// calulate the energy cost from a StrapForce

double
CalculateEnergyCost(MAMuscle *strapForce)
{
/* old StrapForce version
   double tension = strapForce->GetTension();
   double tensionEnergy = tension * gSimulation->GetTimeIncrement() * kTensionPowerCost;
   double lastLength = strapForce->GetLastLength();
   double delLength = strapForce->GetLength() - lastLength;
   double positiveWork = 0;
   double negativeWork = 0;

   assert(tension >= 0);
   
   if (delLength < 0) // positive work
      positiveWork += (-delLength) * tension * kPositiveWorkCost;
   else // negative work
      negativeWork += delLength * tension * kNegativeWorkCost;

   double energy = tensionEnergy + positiveWork + negativeWork;

   if (gDebug == FitnessDebug) 
      cerr << "CalculateEnergyCost\t" << 
         "\tstrapForce->getName()\t" << strapForce->getName() <<
         "\ttension\t" << tension << "\ttensionEnergy\t" << tensionEnergy <<
         "\tlastLength\t" << lastLength << "\tdelLength\t" << delLength <<
         "\tpositiveWork\t" << positiveWork << "\tnegativeWork\t" << negativeWork << 
         "\tenergy\t" << energy << "\n";

   return energy;
 */ 
   return (strapForce->GetMetabolicPower() * gSimulation->GetTimeIncrement());
}
