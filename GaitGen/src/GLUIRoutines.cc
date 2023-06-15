// GLUIRoutines.cc - routine for displaying the GLUT and GLUI windows

#ifdef USE_OPENGL

#include <stdio.h>
#include <unistd.h>
#include <iostream>
#include <fstream>
#include <limits.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>

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
#include "DebugControl.h"

// external functions
int ReadModel();
void WriteModel();
void OutputProgramState(ostream &out);
void OutputKinetics();

// various globals

// Simulation global
extern Simulation *gSimulation;

extern float gTimeLimit;
extern float gEnergyLimit;

extern int gWindowWidth;
extern int gWindowHeight;

extern bool gFinishedFlag;
extern double gOutputStateAt;
extern char *gKineticsFilenamePtr;

extern bool gUseSeparateWindow;

extern int gDisplaySkip;
    
int gAxisFlag = true;
float gAxisSize = 0.1;
int gDrawForces = true;
float gForceLineScale = 0.001; // multiply the force by this before drawing vector
int gDrawMuscles = true;
int gDrawBonesFlag = true;
int gWhiteBackground = false;

// movie
static char gMovieDirectory[sizeof(GLUI_String)] = "movie";
static int gWriteMovie = false;

static int gOverlayFlag = false;

static int gTrackingFlag = true;
static float gCOIx = 0;
static float gCOIy = 0;
static float gCOIz = 0;
static float gCameraDistance = -15.0;
static float gFOV = 15;
static GLUI_Rotation *gViewRotationControl;
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

static GLfloat gLight0Ambient[] =  {0.1f, 0.1f, 0.3f, 1.0f};
static GLfloat gLight0Diffuse[] =  {.6f, .6f, 1.0f, 1.0f};
static GLfloat gLight0Specular[] =  {.6f, .6f, 1.0f, 1.0f};
static GLfloat gLight0Position[] = {.5f, .5f, 1.0f, 0.0f};

static GLfloat gLight1Ambient[] =  {0.1f, 0.1f, 0.3f, 1.0f};
static GLfloat gLight1Diffuse[] =  {.9f, .6f, 0.0f, 1.0f};
static GLfloat gLight1Specular[] =  {.9f, .6f, 0.0f, 1.0f};
static GLfloat gLight1Position[] = {-1.0f, -1.0f, 1.0f, 0.0f};

static GLfloat gLightsRotation[16] = {1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1 };

static float gXYAspect;
static GLUI *gGLUIControlWindow;
static float gViewRotation[16] = { 1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1 };
static float gScale = 1.0;

// info panel
static GLUI_EditText *gGLUIEditTextCameraX;
static GLUI_EditText *gGLUIEditTextCameraY;
static GLUI_EditText *gGLUIEditTextCameraZ;
static GLUI_EditText *gGLUIEditTextSimulationTime;
static GLUI_EditText *gGLUIEditTextMechanicalEnergy;
static GLUI_EditText *gGLUIEditTextMetabolicEnergy;
static GLUI_EditText *gGLUIEditTextFitness;

// debug panel
static int gDebugListItem;
static int gDebugWrite = false;

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
const int DEBUG_CHECKBOX = 21;

const int DEFAULT_TEXT_WIDTH = 30;  


/**************************************** myGlutKeyboard() **********/

void myGlutKeyboard(unsigned char Key, int x, int y)
{
    switch(Key)
    {
        case 27:
        case 'q':
            exit(0);
            break;
    }

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

    int i;
    static double cameraCOIX = 0;
    bool newPicture = false;
    int debugStore;

    // before we do anything else, check that we have a model

    if (gFinishedFlag)
    {
        if (ReadModel() == 0)
        {
            gFinishedFlag = false;
            usleep(100000); // slight pause on read failure
#ifdef USE_SOCKETS
            gRunFlag = true;
#endif
        }
        else
        {
            glutPostRedisplay();
            return;
        }
    }

    if (gRunFlag)
    {
        for (i = 0; i < gDisplaySkip; i++)
        {
            if (gSimulation->GetTime() >= gTimeLimit ||
                gSimulation->GetMetabolicEnergy() >= gEnergyLimit)
            {
                gFinishedFlag = true;
                gRunFlag = false;
                break;
            }
            if (gDebug != NoDebug && i != (gDisplaySkip - 1))
            {
                debugStore = gDebug;
                gDebug = NoDebug;
                gSimulation->UpdateSimulation();
                gDebug = debugStore;
            }
            else gSimulation->UpdateSimulation();

            if (gKineticsFilenamePtr) OutputKinetics();
            newPicture = true;

            if (gSimulation->TestForCatastrophy())
            {
                gFinishedFlag = true;
                gRunFlag = false;
                break;
            }

            if (gOutputStateAt >= 0)
            {
                if (gSimulation->GetTime() >= gOutputStateAt)
                {
                    ofstream out("ModelState.txt");
                    OutputProgramState(out);
                    out.close();
                    gOutputStateAt = -1.0;
                }
            }

        }

        if (gSlowFlag) usleep(100000); // 0.1 s
    }
    else
    {
        if (gStepFlag)
        {
            gStepFlag = false;
            for (i = 0; i < gDisplaySkip; i++)
            {
                if (gDebug != NoDebug && i != (gDisplaySkip - 1))
                {
                    debugStore = gDebug;
                    gDebug = NoDebug;
                    gSimulation->UpdateSimulation();
                    gDebug = debugStore;
                }
                else gSimulation->UpdateSimulation();
                if (gKineticsFilenamePtr) OutputKinetics();
            }
            newPicture = true;
        }

    }

    // update the live variables
    if (gTrackingFlag && gSimulation->GetTime() > 0.0)
    {
        const dmABForKinStruct *theDmABForKinStruct =
        gSimulation->GetRobot()->getForKinStruct(gSimulation->GetTorsoIndex());
        if (theDmABForKinStruct->p_ICS[0] != cameraCOIX)
        {
            cameraCOIX = theDmABForKinStruct->p_ICS[0];
            gCOIx = cameraCOIX;
        }
    }

    // set the info values by hand
    gGLUIEditTextCameraX->set_float_val(gCameraX);
    gGLUIEditTextCameraY->set_float_val(gCameraY);
    gGLUIEditTextCameraZ->set_float_val(gCameraZ);
    gGLUIEditTextSimulationTime->set_float_val((float)gSimulation->GetTime());
    gGLUIEditTextMechanicalEnergy->set_float_val((float)gSimulation->GetMechanicalEnergy());
    gGLUIEditTextMetabolicEnergy->set_float_val((float)gSimulation->GetMetabolicEnergy());
    gGLUIEditTextFitness->set_float_val((float)gSimulation->CalculateInstantaneousFitness());

    gGLUIControlWindow->sync_live();
    glutPostRedisplay();

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

    // enforce a control update
    // shouldn't be necessary but I'm not getting proper redraw on a Mac
    glutSetWindow(gGLUIControlWindow->get_glut_window_id());
    glutPostRedisplay();

    if (gFinishedFlag)
    {
        WriteModel();
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
    if (gUseSeparateWindow)
    {
        glViewport( 0, 0, x, y );
        gXYAspect = (float)x / (float)y;
    }
    else
    {
        int tx, ty, tw, th;
        GLUI_Master.get_viewport_area( &tx, &ty, &tw, &th );
        glViewport( tx, ty, tw, th );
        gXYAspect = (float)tw / (float)th;
    }

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
    glutMotionFunc( myGlutMotion );
    GLUI_Master.set_glutReshapeFunc( myGlutReshape );
    GLUI_Master.set_glutKeyboardFunc( myGlutKeyboard );
    GLUI_Master.set_glutSpecialFunc( NULL );
    GLUI_Master.set_glutMouseFunc( myGlutMouse );

    /****************************************/
    /*       Set up OpenGL lights           */
    /****************************************/

    glEnable(GL_LIGHTING);
    glEnable(GL_NORMALIZE);

    glEnable(GL_LIGHT0);
    glLightfv(GL_LIGHT0, GL_AMBIENT, gLight0Ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, gLight0Diffuse);
    glLightfv(GL_LIGHT1, GL_SPECULAR, gLight0Specular);
    glLightfv(GL_LIGHT0, GL_POSITION, gLight0Position);

    glEnable(GL_LIGHT1);
    glLightfv(GL_LIGHT1, GL_AMBIENT, gLight1Ambient);
    glLightfv(GL_LIGHT1, GL_DIFFUSE, gLight1Diffuse);
    glLightfv(GL_LIGHT1, GL_SPECULAR, gLight1Specular);
    glLightfv(GL_LIGHT1, GL_POSITION, gLight1Position);

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
    if (gUseSeparateWindow)
    {
        gGLUIControlWindow = GLUI_Master.create_glui("Controls", 0,
                                                     gWindowPositionX + gWindowWidth + 20, gWindowPositionY);
    }
    else
    {
        gGLUIControlWindow = GLUI_Master.create_glui_subwindow(gMainWindow, GLUI_SUBWINDOW_RIGHT);
    }


    /*** Add the view controls ***/

    // column 1

    // Camera Panel
    GLUI_Panel *cameraPanel = gGLUIControlWindow->add_panel("Camera");
    gViewRotationControl =
        gGLUIControlWindow->add_rotation_to_panel(cameraPanel, "Rotate", gViewRotation);
    gViewRotationControl->set_spin( 0 );
    GLUI_Checkbox *dampingCheckBox = gGLUIControlWindow->add_checkbox_to_panel(cameraPanel, "Damping", &gBallDamping,
                                                                               BALL_DAMP_CHECKBOX, ButtonCallback);
    GLUI_Button *resetButton = gGLUIControlWindow->add_button_to_panel(cameraPanel, "Reset", BALL_RESET_BUTTON, ButtonCallback);

#ifdef USE_TRANSLATION_WIDGETS
    GLUI_Translation *dist =
        gGLUIControlWindow->add_translation_to_panel(cameraPanel,
                                                     "Distance", GLUI_TRANSLATION_Z, &gCameraDistance );
    dist->set_speed( .1 );
#endif
    GLUI_Spinner *cameraDistanceSpinner = gGLUIControlWindow->add_spinner_to_panel(cameraPanel,
                                                                                   "Distance", GLUI_SPINNER_FLOAT, &gCameraDistance);
    cameraDistanceSpinner->set_w(DEFAULT_TEXT_WIDTH);
    cameraDistanceSpinner->set_alignment(GLUI_ALIGN_RIGHT);
    GLUI_Spinner *fovSpiner = gGLUIControlWindow->add_spinner_to_panel(cameraPanel,
                                                                       "FoV", GLUI_SPINNER_FLOAT, &gFOV);
    fovSpiner->set_float_limits(1.0, 170.0);
    fovSpiner->set_w(DEFAULT_TEXT_WIDTH);
    fovSpiner->set_alignment(GLUI_ALIGN_RIGHT);

    // Centre of Interest Panel
    GLUI_Panel *coiPanel = gGLUIControlWindow->add_panel("COI");
#ifdef USE_TRANSLATION_WIDGETS
    GLUI_Translation *trans_x =
        gGLUIControlWindow->add_translation_to_panel(coiPanel,
                                                     "X", GLUI_TRANSLATION_X, &gCOIx );
    trans_x->set_speed( .1 );
    GLUI_Translation *trans_y =
        gGLUIControlWindow->add_translation_to_panel(coiPanel,
                                                     "Y", GLUI_TRANSLATION_Z, &gCOIy );
    trans_y->set_speed( .1 );
    GLUI_Translation *trans_z =
        gGLUIControlWindow->add_translation_to_panel(coiPanel,
                                                     "Z", GLUI_TRANSLATION_Y, &gCOIz );
    trans_z->set_speed( .1 );
#endif
    GLUI_Spinner *coiXSpinner = gGLUIControlWindow->add_spinner_to_panel(coiPanel,
                                                                         "COI X", GLUI_SPINNER_FLOAT, &gCOIx);
    coiXSpinner->set_w(DEFAULT_TEXT_WIDTH);
    coiXSpinner->set_alignment(GLUI_ALIGN_RIGHT);
    GLUI_Spinner *coiYSpinner = gGLUIControlWindow->add_spinner_to_panel(coiPanel,
                                                                         "COI Y", GLUI_SPINNER_FLOAT, &gCOIy);
    coiYSpinner->set_w(DEFAULT_TEXT_WIDTH);
    coiYSpinner->set_alignment(GLUI_ALIGN_RIGHT);
    GLUI_Spinner *coiZSpinner = gGLUIControlWindow->add_spinner_to_panel(coiPanel,
                                                                         "COI Z", GLUI_SPINNER_FLOAT, &gCOIz);
    coiZSpinner->set_w(DEFAULT_TEXT_WIDTH);
    coiZSpinner->set_alignment(GLUI_ALIGN_RIGHT);


#if defined USE_TRANSLATION_WIDGETS
    // column 2
    gGLUIControlWindow->add_column( true );
#endif

    // Display Panel
    GLUI_Panel *displayPanel = gGLUIControlWindow->add_panel("Display");
    gGLUIControlWindow->add_checkbox_to_panel(displayPanel, "Tracking", &gTrackingFlag);
    gGLUIControlWindow->add_checkbox_to_panel(displayPanel, "Overlay", &gOverlayFlag);
    gGLUIControlWindow->add_checkbox_to_panel(displayPanel, "Draw Forces", &gDrawForces);
    gGLUIControlWindow->add_checkbox_to_panel(displayPanel, "Draw Muscles", &gDrawMuscles);
    gGLUIControlWindow->add_checkbox_to_panel(displayPanel, "Draw Bones", &gDrawBonesFlag);
    gGLUIControlWindow->add_checkbox_to_panel(displayPanel, "Draw Axes", &gAxisFlag);
    gGLUIControlWindow->add_checkbox_to_panel(displayPanel, "White Background", &gWhiteBackground);
    GLUI_Spinner *forceScaleSpiner = gGLUIControlWindow->add_spinner_to_panel(displayPanel,
                                                                              "Force Scale", GLUI_SPINNER_FLOAT, &gForceLineScale);
    forceScaleSpiner->set_float_limits(0.0, 0.1);
    forceScaleSpiner->set_w(DEFAULT_TEXT_WIDTH);
    forceScaleSpiner->set_alignment(GLUI_ALIGN_RIGHT);
    GLUI_Spinner *axisSizeSpiner = gGLUIControlWindow->add_spinner_to_panel(displayPanel,
                                                                            "Axis Size", GLUI_SPINNER_FLOAT, &gAxisSize);
    axisSizeSpiner->set_float_limits(0.0, 1.0);
    axisSizeSpiner->set_w(DEFAULT_TEXT_WIDTH);
    axisSizeSpiner->set_alignment(GLUI_ALIGN_RIGHT);

#if ! defined USE_TRANSLATION_WIDGETS && ! defined(USE_SOCKETS)
    // column 2
    gGLUIControlWindow->add_column( true );
#endif

#if ! defined(USE_SOCKETS)
    // Movie Panel
    GLUI_Panel *moviePanel = gGLUIControlWindow->add_panel("Movie");
    GLUI_EditText *directoryNameEditText = gGLUIControlWindow->add_edittext_to_panel(moviePanel, "Directory Name",
                                                                                     GLUI_EDITTEXT_TEXT, gMovieDirectory);
    directoryNameEditText->set_w(DEFAULT_TEXT_WIDTH);
    directoryNameEditText->set_alignment(GLUI_ALIGN_RIGHT);
    gGLUIControlWindow->add_checkbox_to_panel(moviePanel, "Record Movie", &gWriteMovie,
                                              MOVIE_CHECKBOX, ButtonCallback);

    // Animation Panel
    GLUI_Panel *animatePanel = gGLUIControlWindow->add_panel("Animate");
    gGLUIControlWindow->add_button_to_panel(animatePanel, "Start", START_BUTTON, ButtonCallback);
    gGLUIControlWindow->add_button_to_panel(animatePanel, "Stop", STOP_BUTTON, ButtonCallback);
    gGLUIControlWindow->add_button_to_panel(animatePanel, "Step", STEP_BUTTON, ButtonCallback);
    gGLUIControlWindow->add_statictext_to_panel(animatePanel, "" );
    gGLUIControlWindow->add_checkbox_to_panel(animatePanel, "Slow", &gSlowFlag);
    GLUI_Spinner *frameSkipSpiner = gGLUIControlWindow->add_spinner_to_panel(animatePanel,
                                                                             "Frame Skip", GLUI_SPINNER_INT, &gDisplaySkip);
    frameSkipSpiner->set_int_limits(1, INT_MAX);
    frameSkipSpiner->set_w(DEFAULT_TEXT_WIDTH);
    frameSkipSpiner->set_alignment(GLUI_ALIGN_RIGHT);
    GLUI_Spinner *timeLimitSpinner = gGLUIControlWindow->add_spinner_to_panel(animatePanel,
                                                                              "Time Limit", GLUI_SPINNER_FLOAT, &gTimeLimit);
    timeLimitSpinner->set_w(DEFAULT_TEXT_WIDTH);
    timeLimitSpinner->set_alignment(GLUI_ALIGN_RIGHT);
    GLUI_Spinner *energyLimitSpinner = gGLUIControlWindow->add_spinner_to_panel(animatePanel,
                                                                                "Energy Limit", GLUI_SPINNER_FLOAT, &gEnergyLimit);
    energyLimitSpinner->set_w(DEFAULT_TEXT_WIDTH);
    energyLimitSpinner->set_alignment(GLUI_ALIGN_RIGHT);
#endif

    // Information Panel
    GLUI_Panel *infoPanel = gGLUIControlWindow->add_panel("Info Only");
    gGLUIEditTextCameraX = gGLUIControlWindow->add_edittext_to_panel(infoPanel,
                                                                     "Camera X", GLUI_EDITTEXT_FLOAT);
    gGLUIEditTextCameraX->set_w(DEFAULT_TEXT_WIDTH);
    gGLUIEditTextCameraX->set_alignment(GLUI_ALIGN_RIGHT);
    gGLUIEditTextCameraY = gGLUIControlWindow->add_edittext_to_panel(infoPanel,
                                                                     "Camera Y", GLUI_EDITTEXT_FLOAT);
    gGLUIEditTextCameraY->set_w(DEFAULT_TEXT_WIDTH);
    gGLUIEditTextCameraY->set_alignment(GLUI_ALIGN_RIGHT);
    gGLUIEditTextCameraZ = gGLUIControlWindow->add_edittext_to_panel(infoPanel,
                                                                     "Camera Z", GLUI_EDITTEXT_FLOAT);
    gGLUIEditTextCameraZ->set_w(DEFAULT_TEXT_WIDTH);
    gGLUIEditTextCameraZ->set_alignment(GLUI_ALIGN_RIGHT);
    gGLUIControlWindow->add_statictext_to_panel(infoPanel, "" );
    gGLUIEditTextSimulationTime = gGLUIControlWindow->add_edittext_to_panel(infoPanel, "Sim. Time", GLUI_EDITTEXT_FLOAT);
    gGLUIEditTextSimulationTime->set_w(DEFAULT_TEXT_WIDTH);
    gGLUIEditTextSimulationTime->set_alignment(GLUI_ALIGN_RIGHT);
    gGLUIEditTextMechanicalEnergy = gGLUIControlWindow->add_edittext_to_panel(infoPanel, "Mech. Energy", GLUI_EDITTEXT_FLOAT);
    gGLUIEditTextMechanicalEnergy->set_w(DEFAULT_TEXT_WIDTH);
    gGLUIEditTextMechanicalEnergy->set_alignment(GLUI_ALIGN_RIGHT);
    gGLUIEditTextMetabolicEnergy = gGLUIControlWindow->add_edittext_to_panel(infoPanel, "Met. Energy", GLUI_EDITTEXT_FLOAT);
    gGLUIEditTextMetabolicEnergy->set_w(DEFAULT_TEXT_WIDTH);
    gGLUIEditTextMetabolicEnergy->set_alignment(GLUI_ALIGN_RIGHT);
    gGLUIEditTextFitness = gGLUIControlWindow->add_edittext_to_panel(infoPanel, "Fitness", GLUI_EDITTEXT_FLOAT);
    gGLUIEditTextFitness->set_w(DEFAULT_TEXT_WIDTH);
    gGLUIEditTextFitness->set_alignment(GLUI_ALIGN_RIGHT);

    // debug panel
    if (gDebug == GLUIDebug)
    {
        GLUI_Panel *debugPanel = gGLUIControlWindow->add_panel("Debug");
        GLUI_Listbox *debugListBox =
            gGLUIControlWindow->add_listbox_to_panel
            (debugPanel, "Level", &gDebugListItem);
        for (int i = 0; i < 13; i++)
            debugListBox->add_item(i , (char *)gDebugLabels[i]);
        gGLUIControlWindow->add_checkbox_to_panel(debugPanel, "Debug to File", &gDebugWrite,
                                                  DEBUG_CHECKBOX, ButtonCallback);
    }

#ifdef QUIT_BUTTON_REQUIRED
    gGLUIControlWindow->add_statictext( "" );

    /****** A 'quit' button *****/
    gGLUIControlWindow->add_button( "Quit", 0,(GLUI_Update_CB)exit );
#endif

    // tell the gGLUIControlWindow window which the main window is
    gGLUIControlWindow->set_main_gfx_window(gMainWindow);

    /**** We register the idle callback with GLUI, *not* with GLUT ****/
    GLUI_Master.set_glutIdleFunc( myGlutIdle );

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
    gluPerspective(gFOV, gXYAspect, 0.01, 1000);

    glMatrixMode( GL_MODELVIEW );

    glLoadIdentity();
    glMultMatrixf( gLightsRotation );
    glLightfv(GL_LIGHT0, GL_POSITION, gLight0Position);

    glLoadIdentity();

    // note rotation - need to swap Y and Z because of different up vector
    // for the main view and the rotation widget
    mat4 myRotation(
                    gViewRotation[0], gViewRotation[1], gViewRotation[2], gViewRotation[3],
                    gViewRotation[4], gViewRotation[5], gViewRotation[6], gViewRotation[7],
                    gViewRotation[8], gViewRotation[9], gViewRotation[10], gViewRotation[11],
                    gViewRotation[12], gViewRotation[13], gViewRotation[14], gViewRotation[15]);

    vec4 myEye(0.0, 0.0, gCameraDistance, 1.0);
    vec3 myEyeTransformed = (vec3)(myRotation * myEye);

    gCameraX = gCOIx + myEyeTransformed[0];
    gCameraY = gCOIy + myEyeTransformed[2];
    gCameraZ = gCOIz + myEyeTransformed[1];
    gluLookAt( gCameraX, gCameraY, gCameraZ,
               gCOIx, gCOIy, gCOIz,
               0.0, 0.0, 1.0);

    glScalef( gScale, gScale, gScale );

    // ===============================================================

    if (gFinishedFlag == false)
    {
        if ((dmEnvironment::getEnvironment())->getTerrainFilename())
            (dmEnvironment::getEnvironment())->draw();
        gSimulation->GetForceList()->draw();
        gSimulation->GetCPG()->draw();

        glPushAttrib(GL_ALL_ATTRIB_BITS);
        gSimulation->GetRobot()->draw();
        glPopAttrib();
    }

    // ===============================================================
    // draw axes
    glDisable(GL_LIGHTING);

    glBegin(GL_LINES);
    glColor3f(1.0, 0.0, 0.0);
    glVertex3f(1.0, 0.0, 0.0);
    glVertex3f(0.0, 0.0, 0.0);
    glEnd();

    glBegin(GL_LINES);
    glColor3f(0.0, 1.0, 0.0);
    glVertex3f(0.0, 1.0, 0.0);
    glVertex3f(0.0, 0.0, 0.0);
    glEnd();

    glBegin(GL_LINES);
    glColor3f(0.0, 0.0, 1.0);
    glVertex3f(0.0, 0.0, 1.0);
    glVertex3f(0.0, 0.0, 0.0);
    glEnd();

    // draw ruler
    glColor3f(0.5, 0.5, 1.0);
    char buffer[256];
    GLfloat v;
    for (v = -100; v <= 100; v += 1)
    {
        sprintf(buffer, "%.1f", v);
        GLOutputText(v, 0, -0.1, buffer, 0.05, 1);
    }

    for (v = -100; v <= 100; v += 0.5)
    {
        glBegin(GL_LINES);
        glVertex3f(v, 0, 0);
        glVertex3f(v, 0, -0.02);
        glEnd();
    }
    glBegin(GL_LINES);
    glVertex3f(-100, 0, 0);
    glVertex3f(100, 0, 0);
    glEnd();

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
    // characters are roughly 100 units so gScale accordingly
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
            gViewRotationControl->reset();
            break;
        case BALL_DAMP_CHECKBOX:
            if (gBallDamping)
                gViewRotationControl->set_spin( 0 );
            else
                gViewRotationControl->set_spin( 1.0 );
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
        case DEBUG_CHECKBOX:
        {
            if (gDebugWrite)
            {
                char filename[256];
                time_t theTime = time(0);
                struct tm *theLocalTime = localtime(&theTime);
                sprintf(filename, "%s_%04d-%02d-%02d_%02d.%02d.%02d.log",
                        gDebugLabels[gDebugListItem],
                        theLocalTime->tm_year + 1900, theLocalTime->tm_mon + 1,
                        theLocalTime->tm_mday,
                        theLocalTime->tm_hour, theLocalTime->tm_min,
                        theLocalTime->tm_sec);
                ofstream *file = new ofstream();
                file->open(filename, ofstream::out | ofstream::app);
                gDebugStream = file;
                gDebug = gDebugListItem;
            }
            else
            {
                ofstream *file = dynamic_cast<ofstream *>(gDebugStream);
                if (file != (ostream *)&cerr && file != 0)
                {
                    file->close();
                    delete file;
                }
                gDebugStream = &cerr;
                gDebug = 0; // we don't want to debug to stderr
            }
        }
            break;
    }
}

#endif // USE_OPENGL
