// GLUIRoutines.cc - routine for displaying the GLUT and GLUI windows

#include <stdio.h>
#include <unistd.h>
#include <iostream>
#include <limits.h>
#include <sys/types.h>
#include <sys/stat.h>

using namespace std;

#include <glut.h>
#include <glui.h>
#include <algebra3.h>

#include "GLUIRoutines.h"
#include "FacetedObject.h"
#include "MyFace.h"

// various globals

extern FacetedObject *gDisplayObject;

static int gWindowWidth = 640;
static int gWindowHeight = 480;

static int gAxisFlag = true;
static int gAxisLabelFlag = false;
static float gAxisSize = 10.0;
static int gWhiteBackground = false;
static int gDrawClockwise = true;
static int gDrawAntiClockwise = false;
      
static float gCOIx = 0;
static float gCOIy = 0;
static float gCOIz = 0;
static float gCameraDistance = -100.0;
static float gFOV = 20;
static float gNear = 0.1;
static float gFar = 1000;
static GLUI_Rotation *gView_rot;
static int gBallDamping = true;
            
static float gCameraX;
static float gCameraY;
static float gCameraZ;

static int gWindowPositionX = 10;
static int gWindowPositionY = 20;

static int gMainWindow; // stores the id of the main display window

static GLfloat gLight0_ambient[] =  {0.1f, 0.1f, 0.3f, 1.0f};
static GLfloat gLight0_diffuse[] =  {.6f, .6f, 1.0f, 1.0f};
static GLfloat gLight0_specular[] =  {.6f, .6f, 1.0f, 1.0f};
//static GLfloat gLight0_position[] = {.5f, .5f, 1.0f, 0.0f};
static GLfloat gLight0_position[] = {50.0f, 50.0f, 100.0f, 0.0f};

static GLfloat gLight1_ambient[] =  {0.1f, 0.1f, 0.3f, 1.0f};
static GLfloat gLight1_diffuse[] =  {.9f, .6f, 0.0f, 1.0f};
static GLfloat gLight1_specular[] =  {.9f, .6f, 0.0f, 1.0f};
//static GLfloat gLight1_position[] = {-1.0f, -1.0f, 1.0f, 0.0f};
static GLfloat gLight1_position[] = {-100.0f, -100.0f, 100.0f, 0.0f};

static GLfloat gLights_rotation[16] = {1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1 };

static float gXY_aspect;
static GLUI *gGlui;
static float gView_rotate[16] = { 1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1 };
static float gScale = 1.0;

static void myGlutKeyboard(unsigned char Key, int x, int y);
static void myGlutIdle( void );
static void myGlutMouse(int button, int button_state, int x, int y );
static void myGlutMotion(int x, int y );
static void myGlutReshape( int x, int y );
static void myGlutDisplay( void );
static void ButtonCallback( int control );

static void GLOutputText(GLfloat x, GLfloat y, GLfloat z, const char *text, double size,
      GLfloat degrees, GLfloat xa, GLfloat ya, GLfloat za);
static void SetGLColour(GLfloat red, GLfloat green, GLfloat blue,
      GLfloat ambient, GLfloat diffuse, GLfloat specular,
      GLfloat specularPower);

// UI ID defines
const int BALL_RESET_BUTTON = 5;
const int COI_CENTRE_BUTTON = 6;
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

  myGlutDisplay();
  
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

  gXY_aspect = (float)tw / (float)th;

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
 
  gMainWindow = glutCreateWindow( "OBJMangler Window" );
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
  glLightfv(GL_LIGHT0, GL_AMBIENT, gLight0_ambient);
  glLightfv(GL_LIGHT0, GL_DIFFUSE, gLight0_diffuse);
  glLightfv(GL_LIGHT1, GL_SPECULAR, gLight0_specular);
  glLightfv(GL_LIGHT0, GL_POSITION, gLight0_position);

  glEnable(GL_LIGHT1);
  glLightfv(GL_LIGHT1, GL_AMBIENT, gLight1_ambient);
  glLightfv(GL_LIGHT1, GL_DIFFUSE, gLight1_diffuse);
  glLightfv(GL_LIGHT1, GL_SPECULAR, gLight1_specular);
  glLightfv(GL_LIGHT1, GL_POSITION, gLight1_position);

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
  gGlui = GLUI_Master.create_glui("Controls", 0, gWindowPositionX + gWindowWidth + 20, gWindowPositionY);
  
  /*** Add the view controls ***/
  
  // column 1
  
  // Camera Panel
  GLUI_Panel *cameraPanel = gGlui->add_panel("Camera");
  gView_rot = 
        gGlui->add_rotation_to_panel(cameraPanel, "Rotate", gView_rotate);
  gView_rot->set_spin( 0 );
  gGlui->add_checkbox_to_panel(cameraPanel, "Damping", &gBallDamping, 
        BALL_DAMP_CHECKBOX, ButtonCallback);
  gGlui->add_button_to_panel(cameraPanel, "Reset", BALL_RESET_BUTTON, ButtonCallback);
  
  GLUI_Translation *dist = 
    gGlui->add_translation_to_panel(cameraPanel, 
        "Distance", GLUI_TRANSLATION_Z, &gCameraDistance );
  dist->set_speed( .1 );
  GLUI_Spinner *fovSpiner = gGlui->add_spinner_to_panel(cameraPanel, 
        "Field of View", GLUI_SPINNER_FLOAT, &gFOV);
  fovSpiner->set_float_limits(0.1, 170.0);
  GLUI_Spinner *nearSpiner = gGlui->add_spinner_to_panel(cameraPanel, 
        "Near", GLUI_SPINNER_FLOAT, &gNear);
  nearSpiner->set_float_limits(0.1, 1000);
  GLUI_Spinner *farSpiner = gGlui->add_spinner_to_panel(cameraPanel, 
        "Far", GLUI_SPINNER_FLOAT, &gFar);
  farSpiner->set_float_limits(0.1, 1000);
  
  // Centre of Interest Panel        
  GLUI_Panel *coiPanel = gGlui->add_panel("COI");
  GLUI_Translation *trans_x = 
    gGlui->add_translation_to_panel(coiPanel, 
        "X", GLUI_TRANSLATION_X, &gCOIx );
  trans_x->set_speed( .1 );
  GLUI_Translation *trans_y = 
    gGlui->add_translation_to_panel(coiPanel, 
        "Y", GLUI_TRANSLATION_Z, &gCOIy );
  trans_y->set_speed( .1 );
  GLUI_Translation *trans_z = 
    gGlui->add_translation_to_panel(coiPanel, 
        "Z", GLUI_TRANSLATION_Y, &gCOIz );
  trans_z->set_speed( .1 );
  gGlui->add_spinner_to_panel(coiPanel, 
        "COI X", GLUI_SPINNER_FLOAT, &gCOIx);
  gGlui->add_spinner_to_panel(coiPanel, 
        "COI Y", GLUI_SPINNER_FLOAT, &gCOIy);
  gGlui->add_spinner_to_panel(coiPanel, 
        "COI Z", GLUI_SPINNER_FLOAT, &gCOIz);
  gGlui->add_button_to_panel(coiPanel, "Centre", COI_CENTRE_BUTTON, ButtonCallback);
  

  // column 2
  gGlui->add_column( true );
  
  // Display Panel
  GLUI_Panel *displayPanel = gGlui->add_panel("Display");
  gGlui->add_checkbox_to_panel(displayPanel, "White Background", &gWhiteBackground);
  gGlui->add_checkbox_to_panel(displayPanel, "Draw Clockwise", &gDrawClockwise);
  gGlui->add_checkbox_to_panel(displayPanel, "Draw Anti-Clockwise", &gDrawAntiClockwise);
  gGlui->add_checkbox_to_panel(displayPanel, "Draw Axes", &gAxisFlag);
  gGlui->add_checkbox_to_panel(displayPanel, "Label Axes", &gAxisLabelFlag);
  GLUI_Spinner *axisSizeSpiner = gGlui->add_spinner_to_panel(displayPanel, 
        "Axis Size", GLUI_SPINNER_FLOAT, &gAxisSize);
  axisSizeSpiner->set_float_limits(0.0, 100.0);
  
  // Information Panel      
  GLUI_Panel *infoPanel = gGlui->add_panel("Info Only");
  gGlui->add_spinner_to_panel(infoPanel, 
        "Camera X", GLUI_SPINNER_FLOAT, &gCameraX);
  gGlui->add_spinner_to_panel(infoPanel, 
        "Camera Y", GLUI_SPINNER_FLOAT, &gCameraY);
  gGlui->add_spinner_to_panel(infoPanel, 
        "Camera Z", GLUI_SPINNER_FLOAT, &gCameraZ);
  
  gGlui->add_statictext( "" );
  gGlui->add_statictext( "" );
        
  /****** A 'quit' button *****/
  gGlui->add_button( "Quit", 0,(GLUI_Update_CB)exit );
  
  /**** We register the idle callback with GLUI, *not* with GLUT ****/
  GLUI_Master.set_glutIdleFunc( myGlutIdle );

  /**** Regular GLUT main loop ****/
  
  // glutMainLoop();
}

/***************************************** myGlutDisplay() *****************/

void myGlutDisplay( void )
{
  if (gWhiteBackground) glClearColor( 1.0f, 1.0f, 1.0f, 1.0f );
  else glClearColor( .0f, .0f, .0f, 1.0f );
  
  glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

  glMatrixMode( GL_PROJECTION );
  glLoadIdentity();
  gluPerspective(gFOV, gXY_aspect, gNear, gFar);

  glMatrixMode( GL_MODELVIEW );

  glLoadIdentity();
  glMultMatrixf( gLights_rotation );
  glLightfv(GL_LIGHT0, GL_POSITION, gLight0_position);
  
  glLoadIdentity();
  
  // note rotation - need to swap Y and Z because of different up vector
  // for the main view and the rotation widget
  mat4 myRotation(
     gView_rotate[0], gView_rotate[1], gView_rotate[2], gView_rotate[3],
     gView_rotate[4], gView_rotate[5], gView_rotate[6], gView_rotate[7],
     gView_rotate[8], gView_rotate[9], gView_rotate[10], gView_rotate[11],
     gView_rotate[12], gView_rotate[13], gView_rotate[14], gView_rotate[15]);

  vec4 myEye(0.0, 0.0, gCameraDistance, 1.0);
  vec3 myEyeTransformed = (vec3)(myRotation * myEye);  

  gCameraX = gCOIx + myEyeTransformed[0];
  gCameraY = gCOIy + myEyeTransformed[2];
  gCameraZ = gCOIz + myEyeTransformed[1];
  gluLookAt( gCameraX, gCameraY, gCameraZ, 
             gCOIx, gCOIy, gCOIz,
             0.0, 0.0, 1.0);
       
  glScalef( gScale, gScale, gScale );
  
  // put my drawing here
  glPushAttrib(GL_ALL_ATTRIB_BITS);
   // hardwired material properties
  SetGLColour(233.0/255.0, 218.0/255.0, 201.0/255.0,
      0.2, 0.9, 0.3,
      20);
  gDisplayObject->SetDrawClockwise(gDrawClockwise);
  gDisplayObject->SetDrawAntiClockwise(gDrawAntiClockwise);
  gDisplayObject->Draw();
  glPopAttrib(); 

  // ===============================================================
  // draw axes
  glDisable(GL_LIGHTING);

  if (gAxisFlag)
  {
    glBegin(GL_LINES);
    glColor3f(1.0, 0.0, 0.0);
    glVertex3f(gAxisSize, 0.0, 0.0);
    glVertex3f(0.0, 0.0, 0.0);
    glEnd();
    
    glBegin(GL_LINES);
    glColor3f(0.0, 1.0, 0.0);
    glVertex3f(0.0, gAxisSize, 0.0);
    glVertex3f(0.0, 0.0, 0.0);
    glEnd();

    glBegin(GL_LINES);
    glColor3f(0.0, 0.0, 1.0);
    glVertex3f(0.0, 0.0, gAxisSize);
    glVertex3f(0.0, 0.0, 0.0);
    glEnd();
    

    if (gAxisLabelFlag)
    {
      glColor3f(1.0, 0.0, 0.0);
      GLOutputText(1.1 * gAxisSize, 0, 0, 
        "X", 0.1 * gAxisSize, 0, 1, 0, 0);
      GLOutputText(1.1 * gAxisSize, 0, 0, 
        "X", 0.1 * gAxisSize, 90, 1, 0, 0);
      GLOutputText(1.1 * gAxisSize, 0, 0, 
        "X", 0.1 * gAxisSize, 180, 1, 0, 0);
      GLOutputText(1.1 * gAxisSize, 0, 0, 
        "X", 0.1 * gAxisSize, 270, 1, 0, 0);

      glColor3f(0.0, 1.0, 0.0);
      GLOutputText(0, 1.1 * gAxisSize, 0, 
        "Y", 0.1 * gAxisSize, 0, 0, 1, 0);
      GLOutputText(0, 1.1 * gAxisSize, 0, 
        "Y", 0.1 * gAxisSize, 90, 0, 1, 0);
      GLOutputText(0, 1.1 * gAxisSize, 0, 
        "Y", 0.1 * gAxisSize, 180, 0, 1, 0);
      GLOutputText(0, 1.1 * gAxisSize, 0, 
        "Y", 0.1 * gAxisSize, 270, 0, 1, 0);

      glColor3f(0.0, 0.0, 1.0);
      GLOutputText(0, 0, 1.1 * gAxisSize, 
        "Z", 0.1 * gAxisSize, 0, 0, 0, 1);
      GLOutputText(0, 0, 1.1 * gAxisSize, 
        "Z", 0.1 * gAxisSize, 90, 0, 0, 1);
      GLOutputText(0, 0, 1.1 * gAxisSize, 
        "Z", 0.1 * gAxisSize, 180, 0, 0, 1);
      GLOutputText(0, 0, 1.1 * gAxisSize, 
        "Z", 0.1 * gAxisSize, 270, 0, 0, 1);
    }
  }
  
  glEnable(GL_LIGHTING);

  glutSwapBuffers();
}  



// button callback function
void ButtonCallback( int control )
{
   switch (control)
   {
      case BALL_RESET_BUTTON:
         gView_rot->reset();
         break;
      case BALL_DAMP_CHECKBOX:
         if (gBallDamping)
            gView_rot->set_spin( 0 );
         else
            gView_rot->set_spin( 1.0 );
         break;
      case COI_CENTRE_BUTTON:
         MyVertex* limits = gDisplayObject->GetLimits();
         gCOIx = (limits[0].x + limits[1].x) / 2;
         gCOIy = (limits[0].y + limits[1].y) / 2;
         gCOIz = (limits[0].z + limits[1].z) / 2;
         break;
   }
}

// write some 3D text at specified location x, y, z, and orientation
// degrees about (xa, ya, za). Note default plane is z = 0
void GLOutputText(GLfloat x, GLfloat y, GLfloat z, const char *text, double size,
      GLfloat degrees, GLfloat xa, GLfloat ya, GLfloat za)
{
  const char *p;
  
  glPushMatrix();
  glTranslatef(x, y, z);
  // characters are roughly 100 units so scale accordingly 
  glScalef( size / 100, size / 100, size / 100);
  // do rotation about axis
  if (degrees != 0) glRotatef(degrees, xa, ya, za);
  for (p = text; *p; p++)
    glutStrokeCharacter(GLUT_STROKE_MONO_ROMAN, *p);
  glPopMatrix();
}

// set some standard material colours
void SetGLColour(GLfloat red, GLfloat green, GLfloat blue,
      GLfloat ambient, GLfloat diffuse, GLfloat specular,
      GLfloat specularPower)
{
   int i;   
   GLfloat color[4];
   GLfloat color2[4];
   
   color[0] = red;
   color[1] = green;
   color[2] = blue;
   color[3] = 1.0;
   
   // for (i = 0; i < 4; i++) color2[i] = color[i] * emission;
   //glMaterialfv(GL_FRONT, GL_EMISSION, color2);
   
   for (i = 0; i < 4; i++) color2[i] = color[i] * ambient;
   glMaterialfv(GL_FRONT, GL_AMBIENT, color2);
   
   for (i = 0; i < 4; i++) color2[i] = color[i] * diffuse;
   glMaterialfv(GL_FRONT, GL_DIFFUSE, color2);
   
   for (i = 0; i < 4; i++) color2[i] = color[i] * specular;
   glMaterialfv(GL_FRONT, GL_SPECULAR, color2);

   glMaterialf(GL_FRONT, GL_SHININESS, specularPower);
}

