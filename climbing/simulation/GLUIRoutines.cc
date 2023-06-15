// GLUIRoutines.cc - routine for displaying the GLUT and GLUI windows

#include <stdio.h>
#include <unistd.h>
#include <iostream.h>
#include <limits.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <glut.h>
#include <glui.h>
#include <algebra3.h>

#include <dm.h>
#include <dmEnvironment.hpp>
#include <dmArticulation.hpp>

#include "GLUIRoutines.h"
#include "Simulation.h"
#include "ForceList.h"
#include "Segments.h"
#include "LadderContactModel.h"

// various globals

// Simulation global
extern Simulation *gSimulation;

extern float gTimeLimit;
extern int gWindowWidth;
extern int gWindowHeight;
      
int gAxisFlag = true;
float gAxisSize = 0.1;
int gDrawForces = true;
float gForceLineScale = 0.001; // multiply the force by this before drawing vector
int gDrawMuscles = true;
int gWhiteBackground = false;

// movie
static char gMovieDirectory[sizeof(GLUI_String)] = "movie";
static int gWriteMovie = false;

static int gDisplaySkip = 250; 
static int gOverlayFlag = false;

static int gTrackingFlag = false;
static float gCOIx = 5;
static float gCOIy = 5;
static float gCOIz = 0;
static float gCameraDistance = -10.0;
static float gFOV = 20;
static GLUI_Rotation *view_rot;
static int gBallDamping = true;
            
static float gCameraX;
static float gCameraY;
static float gCameraZ;

static int gWindowPositionX = 10;
static int gWindowPositionY = 20;

static int gMainWindow; // stores the id of the main display window
static int gStepFlag = false;
static int gSlowFlag = false;
static int gRunFlag = false;

static float gTime = 0;

static GLfloat light0_ambient[] =  {0.1f, 0.1f, 0.3f, 1.0f};
static GLfloat light0_diffuse[] =  {.6f, .6f, 1.0f, 1.0f};
static GLfloat light0_specular[] =  {.6f, .6f, 1.0f, 1.0f};
static GLfloat light0_position[] = {.5f, .5f, 1.0f, 0.0f};

static GLfloat light1_ambient[] =  {0.1f, 0.1f, 0.3f, 1.0f};
static GLfloat light1_diffuse[] =  {.9f, .6f, 0.0f, 1.0f};
static GLfloat light1_specular[] =  {.9f, .6f, 0.0f, 1.0f};
static GLfloat light1_position[] = {-1.0f, -1.0f, 1.0f, 0.0f};

static GLfloat lights_rotation[16] = {1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1 };

static float xy_aspect;
static GLUI *glui;
static float view_rotate[16] = { 1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1 };
static float scale = 1.0;

static void myGlutKeyboard(unsigned char Key, int x, int y);
static void myGlutIdle( void );
static void myGlutMouse(int button, int button_state, int x, int y );
static void myGlutMotion(int x, int y );
static void myGlutReshape( int x, int y );
static void myGlutDisplay( void );
static void WritePPM(char *pathname, int gWidth, int gHeight, unsigned char *rgb);   
static void GLOutputText(GLfloat x, GLfloat y, GLfloat z, char *text, double size, int plane);
static void ButtonCallback( int control );

// UI ID defines
const int STEP_BUTTON = 1;
const int STOP_BUTTON = 2;
const int START_BUTTON = 3;
const int BALL_RESET_BUTTON = 5;
const int MOVIE_CHECKBOX = 11;
const int BALL_DAMP_CHECKBOX = 12;


/**************************************** myGlutKeyboard() **********/

void myGlutKeyboard(unsigned char Key, int x, int y)
{
  switch(Key)
  {
  case 27: 
  case 'q':
    exit(0);
    break;
  };
  
  glutPostRedisplay();
}


/***************************************** myGlutIdle() ***********/

void myGlutIdle( void )
{
  /* According to the GLUT specification, the current window is 
     undefined during an idle callback.  So we need to explicitly change
     it if necessary */
  if ( glutGetWindow() != gMainWindow ) 
    glutSetWindow(gMainWindow);  

  GLUI_Master.sync_live_all();  

  int i;
  static double cameraCOIX = 0;
  bool newPicture = false;
  
  if (gRunFlag)
  {
    for (i = 0; i < gDisplaySkip; i++)
    {
      if (gSimulation->GetTime() >= gTimeLimit)
      {
        gRunFlag = false;
        break;
      }
      gSimulation->UpdateSimulation();
      newPicture = true;
    }
    if (gSlowFlag) usleep(100000); // 0.1 s
  } 
  else
  {
    if (gStepFlag)
    {
      gStepFlag = false;
      for (i = 0; i < gDisplaySkip; i++)
        gSimulation->UpdateSimulation();
      newPicture = true;
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
      gCOIx = cameraCOIX;
    }
  }
  
  myGlutDisplay();
  
  if (newPicture && gWriteMovie)
  {
    int tx, ty, tw, th;
    GLUI_Master.get_viewport_area( &tx, &ty, &tw, &th );
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


/***************************************** myGlutMouse() **********/

void myGlutMouse(int button, int button_state, int x, int y )
{
}


/***************************************** myGlutMotion() **********/

void myGlutMotion(int x, int y )
{
  glutPostRedisplay(); 
}

/**************************************** myGlutReshape() *************/

void myGlutReshape( int x, int y )
{
  int tx, ty, tw, th;
  GLUI_Master.get_viewport_area( &tx, &ty, &tw, &th );
  glViewport( tx, ty, tw, th );

  xy_aspect = (float)tw / (float)th;

  glutPostRedisplay();
}

/**************************************** StartGLUT() *************/

void StartGlut(void)
{
  /****************************************/
  /*   Initialize GLUT and create window  */
  /****************************************/

  glutInitDisplayMode( GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH );
  glutInitWindowPosition( gWindowPositionX, gWindowPositionY);
  glutInitWindowSize( gWindowWidth, gWindowHeight );
 
  gMainWindow = glutCreateWindow( "DynaMechs Simulation" );
  glutDisplayFunc( myGlutDisplay );
  GLUI_Master.set_glutReshapeFunc( myGlutReshape );  
  GLUI_Master.set_glutKeyboardFunc( myGlutKeyboard );
  GLUI_Master.set_glutSpecialFunc( NULL );
  GLUI_Master.set_glutMouseFunc( myGlutMouse );
  glutMotionFunc( myGlutMotion );

  /****************************************/
  /*       Set up OpenGL lights           */
  /****************************************/

  glEnable(GL_LIGHTING);
  glEnable(GL_NORMALIZE);

  glEnable(GL_LIGHT0);
  glLightfv(GL_LIGHT0, GL_AMBIENT, light0_ambient);
  glLightfv(GL_LIGHT0, GL_DIFFUSE, light0_diffuse);
  glLightfv(GL_LIGHT1, GL_SPECULAR, light0_specular);
  glLightfv(GL_LIGHT0, GL_POSITION, light0_position);

  glEnable(GL_LIGHT1);
  glLightfv(GL_LIGHT1, GL_AMBIENT, light1_ambient);
  glLightfv(GL_LIGHT1, GL_DIFFUSE, light1_diffuse);
  glLightfv(GL_LIGHT1, GL_SPECULAR, light1_specular);
  glLightfv(GL_LIGHT1, GL_POSITION, light1_position);

  /****************************************/
  /*          Enable z-buffering          */
  /****************************************/

  glEnable(GL_DEPTH_TEST);
  
  /****************************************/
  /*          Flat Shading                */
  /****************************************/

  glShadeModel(GL_FLAT);

  /****************************************/
  /*          Backface Cull               */
  /****************************************/

  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);
  
  /****************************************/
  /*         Here's the GLUI code         */
  /****************************************/
  
  /*** Create the side subwindow ***/
  glui = GLUI_Master.create_glui("Controls", 0, gWindowPositionX + gWindowWidth + 20, gWindowPositionY);
  
  /*** Add the view controls ***/
  
  // column 1
  
  // Camera Panel
  GLUI_Panel *cameraPanel = glui->add_panel("Camera");
  view_rot = 
        glui->add_rotation_to_panel(cameraPanel, "Rotate", view_rotate);
  view_rot->set_spin( 0 );
  glui->add_checkbox_to_panel(cameraPanel, "Damping", &gBallDamping, 
        BALL_DAMP_CHECKBOX, ButtonCallback);
  glui->add_button_to_panel(cameraPanel, "Reset", BALL_RESET_BUTTON, ButtonCallback);
  
  GLUI_Translation *dist = 
    glui->add_translation_to_panel(cameraPanel, 
        "Distance", GLUI_TRANSLATION_Z, &gCameraDistance );
  dist->set_speed( .1 );
  GLUI_Spinner *fovSpiner = glui->add_spinner_to_panel(cameraPanel, 
        "Field of View", GLUI_SPINNER_FLOAT, &gFOV);
  fovSpiner->set_float_limits(1.0, 170.0);
  
  // Centre of Interest Panel        
  GLUI_Panel *coiPanel = glui->add_panel("COI");
  GLUI_Translation *trans_x = 
    glui->add_translation_to_panel(coiPanel, 
        "X", GLUI_TRANSLATION_X, &gCOIx );
  trans_x->set_speed( .1 );
  GLUI_Translation *trans_y = 
    glui->add_translation_to_panel(coiPanel, 
        "Y", GLUI_TRANSLATION_Z, &gCOIy );
  trans_y->set_speed( .1 );
  GLUI_Translation *trans_z = 
    glui->add_translation_to_panel(coiPanel, 
        "Z", GLUI_TRANSLATION_Y, &gCOIz );
  trans_z->set_speed( .1 );
  glui->add_spinner_to_panel(coiPanel, 
        "COI X", GLUI_SPINNER_FLOAT, &gCOIx);
  glui->add_spinner_to_panel(coiPanel, 
        "COI Y", GLUI_SPINNER_FLOAT, &gCOIy);
  glui->add_spinner_to_panel(coiPanel, 
        "COI Z", GLUI_SPINNER_FLOAT, &gCOIz);
  

  // column 2
  glui->add_column( true );
  
  // Display Panel
  GLUI_Panel *displayPanel = glui->add_panel("Display");
  glui->add_checkbox_to_panel(displayPanel, "Tracking", &gTrackingFlag);
  glui->add_checkbox_to_panel(displayPanel, "Overlay", &gOverlayFlag);
  glui->add_checkbox_to_panel(displayPanel, "Draw Forces", &gDrawForces);
  glui->add_checkbox_to_panel(displayPanel, "Draw Muscles", &gDrawMuscles);
  glui->add_checkbox_to_panel(displayPanel, "Draw Axes", &gAxisFlag);
  glui->add_checkbox_to_panel(displayPanel, "White Background", &gWhiteBackground);
  GLUI_Spinner *forceScaleSpiner = glui->add_spinner_to_panel(displayPanel, 
        "Force Scale", GLUI_SPINNER_FLOAT, &gForceLineScale);
  forceScaleSpiner->set_float_limits(0.0, 0.1);
  GLUI_Spinner *axisSizeSpiner = glui->add_spinner_to_panel(displayPanel, 
        "Axis Size", GLUI_SPINNER_FLOAT, &gAxisSize);
  axisSizeSpiner->set_float_limits(0.0, 1.0);
  
  // Movie Panel
  GLUI_Panel *moviePanel = glui->add_panel("Movie");
  glui->add_edittext_to_panel(moviePanel, "Directory Name", 
        GLUI_EDITTEXT_TEXT, gMovieDirectory);
  glui->add_checkbox_to_panel(moviePanel, "Record Movie", &gWriteMovie, 
        MOVIE_CHECKBOX, ButtonCallback);

  // Animation Panel  
  GLUI_Panel *animatePanel = glui->add_panel("Animate");
  glui->add_button_to_panel(animatePanel, "Start", START_BUTTON, ButtonCallback);
  glui->add_button_to_panel(animatePanel, "Stop", STOP_BUTTON, ButtonCallback);
  glui->add_button_to_panel(animatePanel, "Step", STEP_BUTTON, ButtonCallback);
  glui->add_statictext_to_panel(animatePanel, "" );
  glui->add_checkbox_to_panel(animatePanel, "Slow", &gSlowFlag);
  GLUI_Spinner *frameSkipSpiner = glui->add_spinner_to_panel(animatePanel, 
        "Frame Skip", GLUI_SPINNER_INT, &gDisplaySkip);
  frameSkipSpiner->set_int_limits(1, INT_MAX);
  glui->add_spinner_to_panel(animatePanel, 
        "Time Limit", GLUI_SPINNER_FLOAT, &gTimeLimit);
  
  // Information Panel      
  GLUI_Panel *infoPanel = glui->add_panel("Info Only");
  glui->add_spinner_to_panel(infoPanel, 
        "Camera X", GLUI_SPINNER_FLOAT, &gCameraX);
  glui->add_spinner_to_panel(infoPanel, 
        "Camera Y", GLUI_SPINNER_FLOAT, &gCameraY);
  glui->add_spinner_to_panel(infoPanel, 
        "Camera Z", GLUI_SPINNER_FLOAT, &gCameraZ);
  glui->add_statictext_to_panel(infoPanel, "" );
  glui->add_spinner_to_panel(infoPanel, "Run Time", GLUI_SPINNER_FLOAT, &gTime);
  
  glui->add_statictext( "" );
  glui->add_statictext( "" );
        
  /****** A 'quit' button *****/
  glui->add_button( "Quit", 0,(GLUI_Update_CB)exit );
  
  /**** We register the idle callback with GLUI, *not* with GLUT ****/
  GLUI_Master.set_glutIdleFunc( myGlutIdle );

  /**** Regular GLUT main loop ****/
  
  // glutMainLoop();
}

/***************************************** myGlutDisplay() *****************/

void myGlutDisplay( void )
{
  static bool neverClearedFlag = true;
  
  if (gWhiteBackground) glClearColor( 1.0f, 1.0f, 1.0f, 1.0f );
  else glClearColor( .0f, .0f, .0f, 1.0f );
  
  if (neverClearedFlag) 
  {
     glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
     neverClearedFlag = false;
  }
  else
  {
     if (gOverlayFlag == false)
        glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
  }
     

  glMatrixMode( GL_PROJECTION );
  glLoadIdentity();
  gluPerspective(gFOV, xy_aspect, 0.1, 100);

  glMatrixMode( GL_MODELVIEW );

  glLoadIdentity();
  glMultMatrixf( lights_rotation );
  glLightfv(GL_LIGHT0, GL_POSITION, light0_position);
  
  glLoadIdentity();
  
  // note rotation - need to swap Y and Z because of different up vector
  // for the main view and the rotation widget
  mat4 myRotation(
     view_rotate[0], view_rotate[1], view_rotate[2], view_rotate[3],
     view_rotate[4], view_rotate[5], view_rotate[6], view_rotate[7],
     view_rotate[8], view_rotate[9], view_rotate[10], view_rotate[11],
     view_rotate[12], view_rotate[13], view_rotate[14], view_rotate[15]);

  vec4 myEye(0.0, 0.0, gCameraDistance, 1.0);
  vec3 myEyeTransformed = (vec3)(myRotation * myEye);  

  gCameraX = gCOIx + myEyeTransformed[0];
  gCameraY = gCOIy + myEyeTransformed[2];
  gCameraZ = gCOIz + myEyeTransformed[1];
  gluLookAt( gCameraX, gCameraY, gCameraZ, 
             gCOIx, gCOIy, gCOIz,
             0.0, 0.0, 1.0);
       
  glScalef( scale, scale, scale );

  gTime = gSimulation->GetTime();
  // ===============================================================

  (dmEnvironment::getEnvironment())->draw();
  gSimulation->GetForceList()->draw();
  gSimulation->GetCPG()->draw();
  
  glPushAttrib(GL_ALL_ATTRIB_BITS);
  gSimulation->GetLadder()->draw();
  glPopAttrib();

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
  char buffer[256];
  for (int i = -5; i <= 50; i += 1)
  {
    sprintf(buffer, "%d", i);
    GLOutputText((GLfloat)i + 5.0, 5, -0.1, buffer, 0.05, 1);
  }

  for (GLfloat v = 0; v <= 55; v += 0.5)
  {
    glBegin(GL_LINES);
    glVertex3f(v, 5, 0);
    glVertex3f(v, 5, -0.02);
    glEnd();
  }

  glEnable(GL_LIGHTING);

  glutSwapBuffers();
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

// button callback function
void ButtonCallback( int control )
{
   switch (control)
   {
      case STEP_BUTTON:
         gStepFlag = true;
         gRunFlag = false;
         break;
      case START_BUTTON:
         gRunFlag = true;
         break;
      case STOP_BUTTON:
         gRunFlag = false;
         break;
      case BALL_RESET_BUTTON:
         view_rot->reset();
         break;
      case BALL_DAMP_CHECKBOX:
         if (gBallDamping)
            view_rot->set_spin( 0 );
         else
            view_rot->set_spin( 1.0 );
         break;
      case MOVIE_CHECKBOX:
         if (gWriteMovie)
         {
            struct stat sb;
            int err = stat(gMovieDirectory, &sb);
            if (err) // file doesn't exist
            {
               char command[sizeof(gMovieDirectory) + 16];
               sprintf(command, "mkdir %s", gMovieDirectory);
               system(command);
               err = stat(gMovieDirectory, &sb);
            }
            if (err) // something wrong
            {
               gWriteMovie = false;
               break;
            }
            if ((sb.st_mode & S_IFDIR) == 0)
            {
               gWriteMovie = false;
               break;
            }
         }
         break;
   }
}

