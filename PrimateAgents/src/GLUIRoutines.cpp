/*
 *  GLUIRoutines.cpp
 *  PrimateAgent
 *
 *  Created by Bill Sellers on Fri Jan 28 2005.
 *  Copyright (c) 2005 Bill Sellers. All rights reserved.
 *
 */

#ifdef USE_OPENGL

using namespace std;

#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <glut.h>
#include <glui.h>

#include "GLUIRoutines.h"
#include "Simulation.h"
#include "TIFFWrite.h"

static void myGlutKeyboard(unsigned char Key, int x, int y);
static void myGlutIdle( void );
static void myGlutMouse(int button, int button_state, int x, int y );
static void myGlutMotion(int x, int y );
static void myGlutReshape( int x, int y );
static void myGlutDisplay( void );
static void ButtonCallback( int control );
static void WritePPM(char *pathname, int gWidth, int gHeight, unsigned char *rgb);
static void WriteTIFF(char *pathname, int gWidth, int gHeight, unsigned char *rgb);   

extern Simulation g_Simulation;

static int gMainWindow; // stores the id of the main display window
static GLUI *gGLUIControlWindow;

static double g_Width = 1.0;
static double g_Height = 1.0;

// window colour
static int gWhiteBackground = false;

// window size
static GLfloat gDisplayWidth = 1.0;
static GLfloat gDisplayHeight = 1.0;

// control window separate?
bool gUseSeparateWindow = true;

// window origin
static int gWindowPositionX = 10;
static int gWindowPositionY = 20;

// buttons
static bool gRunFlag = false;
static bool gStepFlag = false;

// movie
static char gMovieDirectory[sizeof(GLUI_String)] = "movie";
static int gWriteMovie = false;
static int gUsePPM = false;

// UI ID defines
const int STEP_BUTTON = 1;
const int STOP_BUTTON = 2;
const int START_BUTTON = 3;
const int MOVIE_CHECKBOX = 11;

const int DEFAULT_TEXT_WIDTH = 40;


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
    int i;
    bool newPicture = false;
    time_t clock;
    struct tm *tm;

    /* According to the GLUT specification, the current window is
    undefined during an idle callback.  So we need to explicitly change
    it if necessary */
    if ( glutGetWindow() != gMainWindow )
        glutSetWindow(gMainWindow);

    if (g_Simulation.ShouldFinish())
    {
        gRunFlag = false;
        gStepFlag = false;
    }

    if (gRunFlag || gStepFlag)
    {
        if (g_Simulation.IsNightTime())
            for (i = 0; i < g_Simulation.GetNightTimeSpeedUp(); i++)
                g_Simulation.UpdateSimulation();
        else
            for (i = 0; i < g_Simulation.GetDayTimeSpeedUp(); i++)
            g_Simulation.UpdateSimulation();
            
        if (gStepFlag) gStepFlag = false;
        newPicture = true;
    }

    gGLUIControlWindow->sync_live();
    glutPostRedisplay();

    static unsigned int frameCount = 1;
    if (newPicture && gWriteMovie)
    {
        int tx, ty, tw, th;
        GLUI_Master.get_viewport_area( &tx, &ty, &tw, &th );
        unsigned char *rgb = new unsigned char[tw * th * 3];
        glReadBuffer(GL_FRONT);
        glPixelStorei(GL_PACK_ALIGNMENT, 1);
        glReadPixels(0, 0, tw, th, GL_RGB, GL_UNSIGNED_BYTE, rgb);
        char pathname[sizeof(gMovieDirectory) + 256];
        if (gUsePPM)
        {
            sprintf(pathname, "%s/Frame%05d.ppm", gMovieDirectory, frameCount);
            WritePPM(pathname, tw, th, (unsigned char *)rgb);
            frameCount++;
        }
        else
        {
            clock = (time_t)(g_Simulation.GetCurrentTime() + 0.5);
            tm = gmtime(&clock);
            sprintf(pathname, "%s/Frame_%04d-%02d-%02d_%02d.%02d.%02d.tif",
                    gMovieDirectory,
                    tm->tm_year + 1900,
                    tm->tm_mon + 1,
                    tm->tm_mday,
                    tm->tm_hour, tm->tm_min,
                    tm->tm_sec);
            
            WriteTIFF(pathname, tw, th, (unsigned char *)rgb);
        }
        delete [] rgb;
    }
    
    // enforce a control update
    // shouldn't be necessary but I'm not getting proper update of live controls on a Mac
    // but I get flickering on Linux and SGI - sigh!
#ifdef UPDATE_CONTROLS_ON_IDLE
    glutSetWindow(gGLUIControlWindow->get_glut_window_id());
    glutPostRedisplay();
#endif
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
    int xLimit, yLimit;
    GLfloat xScale;
    GLfloat yScale;
    if (gUseSeparateWindow)
    {
        // find limiting dimension
        xScale = gDisplayWidth * x;
        yScale = gDisplayHeight * y;
        if (xScale < yScale)
        {
            xLimit = x;
            yLimit = (int)(0.5 + xScale / gDisplayHeight);
        }
        else
        {
            yLimit = y;
            xLimit = (int)(0.5 + yScale / gDisplayWidth);
        }
        glViewport( 0, 0, xLimit, yLimit );
    }
    else
    {
        // need to do limiting dimension stuff if I want to use this
        int tx, ty, tw, th;
        GLUI_Master.get_viewport_area( &tx, &ty, &tw, &th );
        glViewport( tx, ty, tw, th );
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
    glutInitWindowSize( g_Simulation.GetWindowWidth(), g_Simulation.GetWindowHeight() );

    gMainWindow = glutCreateWindow( "Primate Agent Simulation" );

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

    /****************************************/
    /*          Flat Shading                */
    /****************************************/

    glShadeModel(GL_FLAT);

    /****************************************/
    /*         Here's the GLUI code         */
    /****************************************/

    /*** Create the side subwindow ***/
    if (gUseSeparateWindow)
    {
        gGLUIControlWindow = GLUI_Master.create_glui("Controls", 0,
                                                     gWindowPositionX + g_Simulation.GetWindowWidth() + 20, gWindowPositionY);
    }
    else
    {
        gGLUIControlWindow = GLUI_Master.create_glui_subwindow(gMainWindow, GLUI_SUBWINDOW_RIGHT);
    }


    /*** Add the view controls ***/

    // Animation Panel
    GLUI_Panel *animatePanel = gGLUIControlWindow->add_panel("Animate");
    gGLUIControlWindow->add_button_to_panel(animatePanel, "Start", START_BUTTON, ButtonCallback);
    gGLUIControlWindow->add_button_to_panel(animatePanel, "Stop", STOP_BUTTON, ButtonCallback);
    gGLUIControlWindow->add_button_to_panel(animatePanel, "Step", STEP_BUTTON, ButtonCallback);

    // Movie Panel
    GLUI_Panel *moviePanel = gGLUIControlWindow->add_panel("Movie");
    GLUI_EditText *directoryNameEditText = gGLUIControlWindow->add_edittext_to_panel(moviePanel, "Directory Name",
                                                                                     GLUI_EDITTEXT_TEXT, gMovieDirectory);
    directoryNameEditText->set_w(DEFAULT_TEXT_WIDTH);
    directoryNameEditText->set_alignment(GLUI_ALIGN_RIGHT);
    gGLUIControlWindow->add_checkbox_to_panel(moviePanel, "Record Movie", &gWriteMovie,
                                              MOVIE_CHECKBOX, ButtonCallback);
    gGLUIControlWindow->add_checkbox_to_panel(moviePanel, "PPM Output", &gUsePPM);
        
    // tell the gGLUIControlWindow window which the main window is
    gGLUIControlWindow->set_main_gfx_window(gMainWindow);

    /**** We register the idle callback with GLUI, *not* with GLUT ****/
    GLUI_Master.set_glutIdleFunc( myGlutIdle );

}

/***************************************** myGlutDisplay() *****************/

void myGlutDisplay( void )
{
    if (gWhiteBackground) glClearColor( 1.0, 1.0, 1.0, 1.0 );
    else glClearColor( .0, .0, .0, 1.0 );

    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    // Define the dimensions of the Orthographic Viewing Volume
    double vertMargin = 0.005 * g_Height;
    double horizMargin = 0.005 * g_Width;
    glOrtho(-horizMargin, g_Width + horizMargin,
            -vertMargin, g_Height + vertMargin,
            -1.0, 1.0);
    
    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity();


    g_Simulation.Draw();
    
    // draw the outline of the map area

    glDisable(GL_LIGHTING);
    glBegin(GL_LINES);
    glColor3f(1.0, 0.0, 0.0);
    glVertex2f(0.0, 0.0);
    glVertex2f(1.0, 0.0);
    glEnd();
    glBegin(GL_LINES);
    glColor3f(1.0, 0.0, 0.0);
    glVertex2f(1.0, 0.0);
    glVertex2f(1.0, 1.0);
    glEnd();
    glBegin(GL_LINES);
    glColor3f(1.0, 0.0, 0.0);
    glVertex2f(1.0, 1.0);
    glVertex2f(0.0, 1.0);
    glEnd();
    glBegin(GL_LINES);
    glColor3f(1.0, 0.0, 0.0);
    glVertex2f(0.0, 1.0);
    glVertex2f(0.0, 0.0);
    glEnd();
    glEnable(GL_LIGHTING);

    glFlush();
    

    glutSwapBuffers();
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

// write a PPM file (need to invert the y axis)
void WritePPM(char *pathname, int width, int height, unsigned char *rgb)
{
    FILE *out;
    int i;

    out = fopen(pathname, "wb");

    // need to invert write order
    fprintf(out, "P6\n%d %d\n255\n", width, height);
    for (i = height - 1; i >= 0; i--)
        fwrite(rgb + (i * width * 3), width * 3, 1, out);

    fclose(out);
}

// write a TIFF file
void WriteTIFF(char *pathname, int width, int height, unsigned char *rgb)
{
    TIFFWrite tiff;
    int i;

    tiff.initialiseImage(width, height, 72, 72, 3);
    // need to invert write order
    for (i = 0; i < height; i ++)
        tiff.copyRow(height - i - 1, rgb + (i * width * 3));

    tiff.writeToFile(pathname);
}


#endif



