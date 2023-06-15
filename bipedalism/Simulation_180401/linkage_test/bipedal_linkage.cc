// bipedal_linkage.cc based heavily on the pendulum.cpp test program
// this just drops a passive linkage model to check everything is hooked up
// correctly

/*****************************************************************************
 * Copyright 1999, Scott McMillan
 *****************************************************************************
 *     File: pendulum.cpp
 *   Author: Scott McMillan
 *  Project: DynaMechs 3.0
 *  Created: 29 July 1999
 *  Summary: Test the static base case for DynaMechs 3.0
 *      $Id: bipedal_linkage.cc,v 1.1.1.1 2001/05/11 16:12:51 ws Exp $ 
 *****************************************************************************/

#include <glut.h>

#include <dmGLTime.h>
#include <dmGLMouse.h>
#include <dmGLPolarCamera_zup.h>

#include <dm.h>					 // DynaMechs typedefs, globals, etc.
#include <dmSystem.h>				 // DynaMechs simulation code.
#include <dmArticulation.h>
#include <dmLink.h>
#include <dmEnvironment.h>
#include <dmIntegRK4.h>

#include <dmu.h>

dmGLMouse *mouse;
dmGLPolarCamera_zup *camera;
GLfloat view_mat[4][4];

double idt;
double sim_time = 0.0;
double rtime = 0.0;
int paused_flag = true;
bool step_flag = false;

dmSystem *G_robot;
dmIntegRK4 *G_integrator;

dmGLTimespec tv, last_tv;

int render_rate;
int render_count = 0;
int timer_count = 0;
int motion_plan_rate;				 // fixed rate of 100Hz
int motion_plan_count = 0;			 // counter for motion planning updates

double cmd_direction = 0.0;
double cmd_speed = 0.0;

//----------------------------------------------------------------------------
void myinit(void)
{
	GLfloat light_ambient[] = { 0.0, 0.0, 0.0, 1.0 };
	GLfloat light_diffuse[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat light_specular[] = { 1.0, 1.0, 1.0, 1.0 };
//     light_position is NOT default value
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
}

//----------------------------------------------------------------------------
void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();

	// ===============================================================
	(dmEnvironment::getEnvironment())->draw();

	glPushAttrib(GL_ALL_ATTRIB_BITS);
	G_robot->draw();
	glPopAttrib();
	// ===============================================================

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
void myReshape(int w, int h)
{
	glViewport(0, 0, w, h);
	mouse->win_size_x = w;
	mouse->win_size_y = h;

	//if (w <= h) 
	//    glOrtho (-2.5, 2.5, -2.5*(GLfloat)h/(GLfloat)w, 
	//        2.5*(GLfloat)h/(GLfloat)w, -10.0, 10.0);
	//else 
	//    glOrtho (-2.5*(GLfloat)w/(GLfloat)h, 
	//        2.5*(GLfloat)w/(GLfloat)h, -2.5, 2.5, -10.0, 10.0);

	camera->setPerspective(45.0, (GLfloat) w / (GLfloat) h, 1.0, 200.0);

	//glMatrixMode (GL_MODELVIEW);
	//glLoadIdentity();
	//glTranslatef(0,0,-10.0);

	camera->setViewMat(view_mat);
	camera->applyView();
}

//----------------------------------------------------------------------------
void processKeyboard(unsigned char key, int, int)
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
void processSpecialKeys(int key, int, int)
{
	switch (key)
	{
	case GLUT_KEY_LEFT:
		cmd_direction += 5.0;
		if (cmd_direction > 180.0)
			cmd_direction -= 360.0;
		break;
	case GLUT_KEY_RIGHT:
		cmd_direction -= 5.0;
		if (cmd_direction < -180.0)
			cmd_direction += 360.0;
		break;
	case GLUT_KEY_UP:
		cmd_speed += 0.01f;
		if (cmd_speed > 0.25f)
			cmd_speed = 0.25f;
		break;
	case GLUT_KEY_DOWN:
		cmd_speed -= 0.01f;
		if (cmd_speed < 0.0)
			cmd_speed = 0.0;
		break;
	}
}


//----------------------------------------------------------------------------
void updateSim()
{

	if (!paused_flag)
	{
		for (int i = 0; i < render_rate; i++)
		{
			G_integrator->simulate(idt);
			sim_time += idt;
		}
	}
	else
	{
		if (step_flag)
		{
			step_flag = false;
			G_integrator->simulate(idt);
			sim_time += idt;
		}
	}

	camera->update(mouse);
	camera->applyView();

	display();

	// compute render rate
	timer_count++;
	dmglGetSysTime(&tv);
	double elapsed_time = ((double) tv.tv_sec - last_tv.tv_sec) +
		(1.0e-9 * ((double) tv.tv_nsec - last_tv.tv_nsec));

	if (elapsed_time > 2.5)
	{
		rtime += elapsed_time;
		cerr << "time/real_time: " << sim_time << '/' << rtime
			<< "  frame_rate: " << (double) timer_count / elapsed_time << endl;

		timer_count = 0;
		last_tv.tv_sec = tv.tv_sec;
		last_tv.tv_nsec = tv.tv_nsec;
	}
}

//----------------------------------------------------------------------------
int main(int argc, char **argv)
{
	int i, j;

	glutInit(&argc, argv);

	//=========================
	char *filename = "Biped.cfg";
	if (argc > 1)
	{
		filename = argv[1];
	}

	glutInitWindowSize(640, 480);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutCreateWindow("DynaMechs Example");

	myinit();
	mouse = dmGLMouse::dmInitGLMouse();

	for (i = 0; i < 4; i++)
	{
		for (j = 0; j < 4; j++)
		{
			view_mat[i][j] = 0.0;
		}
		view_mat[i][i] = 1.0;
	}
	view_mat[3][2] = -10.0;
	camera = new dmGLPolarCamera_zup();
	camera->setRadius(30.0);
	camera->setCOI(10.0, 10.0, 5.0);
	camera->setTranslationScale(0.02f);

	// load robot stuff
	ifstream cfg_ptr;
	cfg_ptr.open(filename);

	// Read simulation timing information.
	readConfigParameterLabel(cfg_ptr, "Integration_Stepsize");
	cfg_ptr >> idt;
	if (idt <= 0.0)
	{
		cerr << "main error: invalid integration stepsize: " << idt << endl;
		exit(3);
	}
	motion_plan_rate = (int) (0.5 + 0.01 / idt);	// fixed rate of 100Hz

	readConfigParameterLabel(cfg_ptr, "Display_Update_Rate");
	cfg_ptr >> render_rate;
	if (render_rate < 1)
		render_rate = 1;

// ===========================================================================
// Initialize DynaMechs environment - must occur before any linkage systems
	char env_flname[FILENAME_SIZE];
	readConfigParameterLabel(cfg_ptr, "Environment_Parameter_File");
	readFilename(cfg_ptr, env_flname);
	dmEnvironment *environment = dmuLoadFile_env(env_flname);
	environment->drawInit();
	dmEnvironment::setEnvironment(environment);

// ===========================================================================
// Initialize a DynaMechs linkage system
	char robot_flname[FILENAME_SIZE];
	readConfigParameterLabel(cfg_ptr, "Robot_Parameter_File");
	readFilename(cfg_ptr, robot_flname);

	G_robot = dmuLoadFile_dm(robot_flname);

	G_integrator = new dmIntegRK4();
	G_integrator->setSystem(G_robot);

	glutReshapeFunc(myReshape);
	glutKeyboardFunc(processKeyboard);
	glutSpecialFunc(processSpecialKeys);
	glutDisplayFunc(display);
	glutIdleFunc(updateSim);

	dmglGetSysTime(&last_tv);

	cout << endl;
	cout << "p - toggles dynamic simulation" << endl;
	cout << "Use mouse to rotate/move/zoom the camera" << endl << endl;

	glutMainLoop();
	return 0;				 /* ANSI C requires main to return int. */
}
