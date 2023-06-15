// customised version of dmLoadfile_dm30 to cope with my extra classes

/*****************************************************************************
 * Copyright 1999, Scott McMillan
 *****************************************************************************
 *     File: dmLoadfile_dm30.cpp
 *   Author: Scott McMillan
 *  Created: 27 July 1999
 *  Summary: load v3.0 configuration files and piece together a complete
 *           dmSystem for simulation. 
 *****************************************************************************/

#include <dm.h>

#ifdef WIN32
#include <windows.h>
#endif

#include <gl.h>

#include <dmObject.h>
#include <dmSystem.h>
#include <dmArticulation.h>
#include <dmLink.h>
#include <dmZScrewTxLink.h>
#include <dmMDHLink.h>
#include <dmRevoluteLink.h>
#include <dmPrismaticLink.h>
#include <dmSphericalLink.h>
#include <dmMobileBaseLink.h>
#include <dmActuator.h>
#include <dmRevDCMotor.h>

#include <dmContactModel.h>

#include <dmLoadFile.h>
#include <glLoadModels.h>

#include "LoadFile.h"
#include "SinController.h"
#include "SquareController.h"
#include "MuscleModel.h"
#include "Simulation.h"

// external globals

extern Simulation *gSimulation;

// local globals

int line_num30_ext;

const int NAME_SIZE = 256;
char object_name_ext[NAME_SIZE];

// wis - added function prototypes for internal funcitons
dmSystem *dmLoadFile_dm30_ext(ifstream & cfg_ptr);
void setRigidBodyParameters30_ext(dmRigidBody * body, ifstream & cfg_ptr);
void parseBranch30_ext(ifstream & cfg_ptr, dmArticulation * articulation, dmLink * parent);
dmArticulation *loadArticulation30_ext(ifstream & cfg_ptr);
void readForces(dmRigidBody * body, ifstream & cfg_ptr);
Controller *readController(ifstream & cfg_ptr);
void readMuscleModel(dmRigidBody * body, ifstream & cfg_ptr);
void getGraphicsModel_ext(ifstream & cfg_ptr, dmLink * link);

// wis - external routines (in dmLoadfile_dm30)
bool getStringParameter30(ifstream & cfg_ptr, const char *label, char *name);
void getGraphicsModel_ext(ifstream & cfg_ptr, dmLink * link);
void setContactParameters30(dmRigidBody * body, ifstream & cfg_ptr);
void setJointFriction30(dmLink * link, ifstream & cfg_ptr);
void setMDHParameters30(dmMDHLink * link, ifstream & cfg_ptr);
void setRevDCMotorParameters30(dmRevDCMotor * actuator, ifstream & cfg_ptr);
void setActuator30(dmRevoluteLink * link, ifstream & cfg_ptr);
void setSphericalLinkParameters30(dmSphericalLink * link, ifstream & cfg_ptr);
void setMobileBaseParameters30(dmMobileBaseLink * ref, ifstream & cfg_ptr);


//----------------------------------------------------------------------------
dmSystem *LoadFile(char *filename)
{
	dmSystem *robot = NULL;

	ifstream cfg_ptr(filename);
	if (!cfg_ptr)
	{
		cerr << "Unable to open robot configuration file" << endl;
		exit(7);
	}

	char line[256] = "\0";			 // initialize the line to empty
	cfg_ptr.getline(line, 256);

	// Right now I support to formats and use the initial comment string to
	// determine which format the remainder of the file is in.

	// WIN32 and GCC 2.91.66 (RedHat 6 - Mandrake) have a problem with
	// initializing strtok.  So the following is necessary
	strtok("", "");

	if (strcmp(line, "# DynaMechs V 3.0 ascii extended") == 0)
	{
		robot = dmLoadFile_dm30_ext(cfg_ptr);
	} else if (strcmp(line, "# DynaMechs V 3.0 ascii") == 0)
	{
		robot = dmLoadFile_dm30(cfg_ptr);
	} else if (strcmp(line, "# DynaMechs V 2.1 ascii") == 0)
	{
		robot = dmLoadFile_dm21(cfg_ptr);
	} else if (strcmp(line, "# DynaMechs V 2.0.3 ascii") == 0)
	{
		robot = dmLoadFile_dm203(cfg_ptr);
	} else
	{
		cerr << "dmLoadFile_dm() error: unknown format on line 1"
			<< ": \"" << line << "\"" << endl;
	}

	cfg_ptr.close();
	return robot;
}


//----------------------------------------------------------------------------
void setRigidBodyParameters30_ext(dmRigidBody * body, ifstream & cfg_ptr)
{
	register int i;

// dynamic properties:
	double mass;
	CartesianTensor I_bar;
	CartesianVector cg_pos;

	readConfigParameterLabel(cfg_ptr, "Mass");
	cfg_ptr >> mass;
	readConfigParameterLabel(cfg_ptr, "Inertia");
	for (i = 0; i < 3; i++)
	{
		cfg_ptr >> I_bar[i][0] >> I_bar[i][1] >> I_bar[i][2];
	}
	readConfigParameterLabel(cfg_ptr, "Center_of_Gravity");
	cfg_ptr >> cg_pos[0] >> cg_pos[1] >> cg_pos[2];

	body->setInertiaParameters(mass, I_bar, cg_pos);

#ifdef HYDRODYNAMICS
// hydrodynamic properties:
	double volume;
	SpatialTensor I_added_mass;
	CartesianVector cb_pos;
	int drag_axis;
	double cyl_min;
	double cyl_max;
	double cyl_radius;
	double C_d;

	readConfigParameterLabel(cfg_ptr, "Volume");
	cfg_ptr >> volume;

	readConfigParameterLabel(cfg_ptr, "Added_Mass");
	for (i = 0; i < 6; i++)
		for (j = 0; j < 6; j++)
			cfg_ptr >> I_added_mass[i][j];

	readConfigParameterLabel(cfg_ptr, "Center_of_Buoyancy");
	cfg_ptr >> cb_pos[0] >> cb_pos[1] >> cb_pos[2];

// drag parameters
	readConfigParameterLabel(cfg_ptr, "Drag_Axis");
	cfg_ptr >> drag_axis;

	readConfigParameterLabel(cfg_ptr, "Cylinder_Bounds");
	cfg_ptr >> cyl_min >> cyl_max;

	readConfigParameterLabel(cfg_ptr, "Cylinder_Radius");
	cfg_ptr >> cyl_radius;

	readConfigParameterLabel(cfg_ptr, "Drag_Coefficient");
	cfg_ptr >> C_d;

	body->setHydrodynamicParameters(volume, I_added_mass, cb_pos,
					drag_axis, cyl_min, cyl_max, cyl_radius, C_d);
#endif

	// wis - this is obviously a good place to add forces

	readForces(body, cfg_ptr);


}


//----------------------------------------------------------------------------
void parseBranch30_ext(ifstream & cfg_ptr, dmArticulation * articulation, dmLink * parent)
{
	char *tok;

	dmLink *last_link = parent;

	for (;;)
	{
		// get next token
		tok = getNextToken(cfg_ptr, line_num30_ext);

		// allocate a link and pass stream reference
		if (strcmp(tok, "Branch") == 0)
		{
//           if (last_link == NULL)
//           {
//              cerr << "dmLoadfile_dm::parseBranch error: cannot branch "
//                   << "immediately inside a branch." << endl;
//              exit(4);
//           }
			parseToBlockBegin(cfg_ptr, line_num30_ext);

			parseBranch30_ext(cfg_ptr, articulation, last_link);
		} else if (strcmp(tok, "MobileBaseLink") == 0)
		{
			if (last_link != NULL)
			{
				cerr <<
					"dmLoadfile_dm30::parseBranch30_ext warning: mobile base "
					<< "link should only be used for first link." << endl;
			}

			parseToBlockBegin(cfg_ptr, line_num30_ext);

			dmMobileBaseLink *link = new dmMobileBaseLink();

			if (getStringParameter30(cfg_ptr, "Name", object_name_ext))
			{
				link->setName(object_name_ext);
			}

			getGraphicsModel_ext(cfg_ptr, link);

			setRigidBodyParameters30_ext(link, cfg_ptr);
			setMobileBaseParameters30(link, cfg_ptr);

			parseToBlockEnd(cfg_ptr, line_num30_ext);

			articulation->addLink(link, last_link);
			last_link = (dmLink *) link;
		} else if (strcmp(tok, "RevoluteLink") == 0)
		{
			parseToBlockBegin(cfg_ptr, line_num30_ext);

			dmRevoluteLink *link = new dmRevoluteLink();

			if (getStringParameter30(cfg_ptr, "Name", object_name_ext))
			{
				link->setName(object_name_ext);
			}

			getGraphicsModel_ext(cfg_ptr, link);

			setRigidBodyParameters30_ext(link, cfg_ptr);
			setMDHParameters30(link, cfg_ptr);
			setActuator30(link, cfg_ptr);

			parseToBlockEnd(cfg_ptr, line_num30_ext);

			articulation->addLink(link, last_link);
			last_link = (dmLink *) link;
		} else if (strcmp(tok, "PrismaticLink") == 0)
		{
			parseToBlockBegin(cfg_ptr, line_num30_ext);

			dmPrismaticLink *link = new dmPrismaticLink();

			if (getStringParameter30(cfg_ptr, "Name", object_name_ext))
			{
				link->setName(object_name_ext);
			}

			getGraphicsModel_ext(cfg_ptr, link);

			setRigidBodyParameters30_ext(link, cfg_ptr);
			setMDHParameters30(link, cfg_ptr);
			setJointFriction30(link, cfg_ptr);

			parseToBlockEnd(cfg_ptr, line_num30_ext);

			articulation->addLink(link, last_link);
			last_link = link;
		} else if (strcmp(tok, "SphericalLink") == 0)
		{
			parseToBlockBegin(cfg_ptr, line_num30_ext);

			dmSphericalLink *link = new dmSphericalLink();

			if (getStringParameter30(cfg_ptr, "Name", object_name_ext))
			{
				link->setName(object_name_ext);
			}

			getGraphicsModel_ext(cfg_ptr, link);

			setRigidBodyParameters30_ext(link, cfg_ptr);
			setSphericalLinkParameters30(link, cfg_ptr);

			parseToBlockEnd(cfg_ptr, line_num30_ext);

			articulation->addLink(link, last_link);
			last_link = link;
		} else if ((strcmp(tok, "ZScrewTxLink") == 0) || (strcmp(tok, "ZScrewLinkTx") == 0))	// latter is deprecated
		{
			parseToBlockBegin(cfg_ptr, line_num30_ext);

			bool got_name = getStringParameter30(cfg_ptr, "Name", object_name_ext);

			double d, theta;

			// Get Inboard to Chain base transformation info.
			readConfigParameterLabel(cfg_ptr, "ZScrew_Parameters");
			cfg_ptr >> d >> theta;

			dmZScrewTxLink *link = new dmZScrewTxLink(d, theta);
			if (got_name)
			{
				link->setName(object_name_ext);
			}
			parseToBlockEnd(cfg_ptr, line_num30_ext);

			articulation->addLink(link, last_link);
			last_link = link;
		} else if (tok[0] == BLOCK_END_CHAR)
		{
			break;
		} else
		{
			cerr << "Error: Invalid Joint_Type: " << tok
				<< ", line " << line_num30_ext << endl;
			exit(3);
		}
	}
}

//----------------------------------------------------------------------------
dmArticulation *loadArticulation30_ext(ifstream & cfg_ptr)
{
	dmArticulation *robot = new dmArticulation();;
	if (robot == NULL)
	{
		cerr << "dmLoadFile_dm30::loadArticulation30_ext error: "
			<< "unable to allocate dmArticulation" << endl;
		cfg_ptr.close();
		exit(4);
	}

	parseToBlockBegin(cfg_ptr, line_num30_ext);

	// label the system object
	char sys_name[NAME_SIZE];

	if (getStringParameter30(cfg_ptr, "Name", sys_name))
	{
		robot->setName(sys_name);
	}

	char object_name_ext[256];
	if (getStringParameter30(cfg_ptr, "Graphics_Model", object_name_ext) &&
	    (object_name_ext[0] != '\0'))
	{
		cerr << "in here " << object_name_ext << endl;
		GLuint *dlist = new GLuint;
		cerr << "in here" << endl;
		*dlist = glLoadModel(object_name_ext);
		cerr << "in here" << endl;
		robot->setUserData((void *) dlist);
	}
	// ================== NEW Get Reference System info. =====================
	CartesianVector pos;
	Quaternion quat;

	readConfigParameterLabel(cfg_ptr, "Position");
	cfg_ptr >> pos[0] >> pos[1] >> pos[2];
	readConfigParameterLabel(cfg_ptr, "Orientation_Quat");
	cfg_ptr >> quat[0] >> quat[1] >> quat[2] >> quat[3];

	robot->setRefSystem(quat, pos);
	//========================================================================

	// there have to be links
	parseBranch30_ext(cfg_ptr, robot, NULL);

	return robot;
}

//----------------------------------------------------------------------------
dmSystem *dmLoadFile_dm30_ext(ifstream & cfg_ptr)
{
	line_num30_ext = 1;
	bool system_flag = true;

	dmSystem *robot = NULL;

	do
	{
		char *tok = getNextToken(cfg_ptr, line_num30_ext);

		if (strcmp(tok, "Articulation") == 0)
		{
			robot = loadArticulation30_ext(cfg_ptr);
			system_flag = false;
		}
//    else if (strcmp(tok, "SerialChain") == 0)
//    {
//       robot = loadSerialChain30(cfg_ptr);
//       system_flag = false;
//    }
//    else if (strcmp(tok, "ClosedArticulation") == 0)
//    {
//       robot = loadClosedArticulation30(cfg_ptr);
//       system_flag = false;
//    }
		else
		{
			cerr << "dmLoadFile_dm30 error: unknown token on line " <<
				line_num30_ext << ": " << tok << endl;
		}
	}
	while (system_flag);

	return robot;
}

//----------------------------------------------------------------------------
void readForces(dmRigidBody * body, ifstream & cfg_ptr)
{
	char buffer[NAME_SIZE] = "";

	readConfigParameterLabel(cfg_ptr, "Force_Objects");

	// read list of force objects

	while (strcmp(buffer, "End_Of_Force_Objects") != 0)
	{
		cfg_ptr >> buffer;

		if (strcmp(buffer, "ContactModel") == 0)
			setContactParameters30(body, cfg_ptr);

		if (strcmp(buffer, "MuscleModel") == 0)
			readMuscleModel(body, cfg_ptr);

	}
}

//----------------------------------------------------------------------------
Controller *readController(ifstream & cfg_ptr)
{
	char buffer[NAME_SIZE] = "";
	char controllerName[NAME_SIZE] = "";
	char fileName[NAME_SIZE] = "";
	Controller *controller;

/*
	SinController
	Name					"ControllerName"
	Controller_File				"FileName"
or
	SquareController
	Name					"ControllerName"
	Controller_File				"FileName"
*/
	cfg_ptr >> buffer;

	if (strcmp(buffer, "SinController") == 0)
	{
		getStringParameter30(cfg_ptr, "Name", controllerName);
		getStringParameter30(cfg_ptr, "Controller_File", fileName);
		if (gSimulation->GetControllerDataFromFile())
			controller = new SinController(fileName);
		else
			controller = new SinController();
		controller->setName(controllerName);
		return controller;
	}

	if (strcmp(buffer, "SquareController") == 0)
	{
		getStringParameter30(cfg_ptr, "Name", controllerName);
		getStringParameter30(cfg_ptr, "Controller_File", fileName);
		if (gSimulation->GetControllerDataFromFile())
			controller = new SquareController(fileName);
		else
			controller = new SquareController();
		controller->setName(controllerName);
		return controller;
	}

	return 0;
}

//----------------------------------------------------------------------------
void readMuscleModel(dmRigidBody * body, ifstream & cfg_ptr)
{
	char muscleName[NAME_SIZE];
	double minLength;
	double dampingFactor;
	double controlFactor;
	double springConstant;
	Controller *controller;
	MuscleModel *muscleModel;
	CartesianVector localAttach;
	CartesianVector otherAttach;
	char otherBodyName[NAME_SIZE];

/*
	Name						"PartName"
    Local_Attachment			xx xx xx
	Other_Body_Attachment		xx xx xx
	Other_Body_Name				"BodyName"
	Min_Muscle_Length			xx
	Damping_Factor				xx
	Control_Factor              xx
	Spring_Constant				xx
	<insert controller stuff here>
*/
	getStringParameter30(cfg_ptr, "Name", muscleName);

	readConfigParameterLabel(cfg_ptr, "Local_Attachment");
	cfg_ptr >> localAttach[0] >> localAttach[1] >> localAttach[2];

	readConfigParameterLabel(cfg_ptr, "Other_Body_Attachment");
	cfg_ptr >> otherAttach[0] >> otherAttach[1] >> otherAttach[2];

	getStringParameter30(cfg_ptr, "Other_Body_Name", otherBodyName);

	readConfigParameterLabel(cfg_ptr, "Min_Muscle_Length");
	cfg_ptr >> minLength;
	readConfigParameterLabel(cfg_ptr, "Damping_Factor");
	cfg_ptr >> dampingFactor;
	readConfigParameterLabel(cfg_ptr, "Control_Factor");
	cfg_ptr >> controlFactor;
	readConfigParameterLabel(cfg_ptr, "Spring_Constant");
	cfg_ptr >> springConstant;

	controller = readController(cfg_ptr);

	muscleModel = new MuscleModel(controller, localAttach, otherBodyName, otherAttach);
	muscleModel->setMinMuscleLength(minLength);
	muscleModel->setDampingFactor(dampingFactor);
	muscleModel->setControlFactor(controlFactor);
	muscleModel->setSpringConstant(springConstant);
	muscleModel->setName(muscleName);

	body->addForce(muscleModel);
}

//----------------------------------------------------------------------------
void getGraphicsModel_ext(ifstream & cfg_ptr, dmLink * link)
{
	if (getStringParameter30(cfg_ptr, "Graphics_Model", object_name_ext))
	{
		if (gSimulation->UseGraphics())
		{
			GLuint *dlist = new GLuint;
			*dlist = glLoadModel(object_name_ext);
			link->setUserData((void *) dlist);
		}
	}
}
