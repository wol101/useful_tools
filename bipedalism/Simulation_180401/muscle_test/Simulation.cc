// Simulation.cpp - this simulation object is used to encapsulate
// a dynamechs simulation

// this is an encapsulated version of the main routine used in most of Scott McMillan's examples
// with various bits and pieces added and removed and moved.

// modified by wis to include a specific definintion of the model
// to allow me to customise things a bit more easily

#include <dm.h>					 // DynaMechs typedefs, globals, etc.
#include <dmSystem.h>				 // DynaMechs simulation code.
#include <dmEnvironment.h>
#include <dmArticulation.h>
#include <dmIntegrator.h>
#include <dmIntegEuler.h>

#include <dmu.h>

#include "LoadFile.h"
#include "ForceList.h"
#include "ControllerList.h"

#include "Simulation.h"

Simulation::Simulation()
{
	// set some variables
	gSimulationTime = 0;
	gForceList = 0;
	gRobot = 0;
	gIntegrator = 0;
	gControllerList = 0;
	idt = 0;
	useGraphics = true;
	readControllerDataFromFile = true;
}

//----------------------------------------------------------------------------
Simulation::~Simulation()
{
	// get rid of all those memory alloactions
	if (gForceList)
		delete gForceList;
	if (gIntegrator)
		delete gIntegrator;
	if (gRobot)
		delete gRobot;
	if (gControllerList)
		delete gControllerList;
}


//----------------------------------------------------------------------------
void Simulation::Load(char *filename)
{
	char env_flname[FILENAME_SIZE];
	char robot_flname[FILENAME_SIZE];


	// load robot stuff
	ifstream cfg_ptr;
	cfg_ptr.open(filename);

	// Read simulation timing information.
	readConfigParameterLabel(cfg_ptr, "Integration_Stepsize");
	cfg_ptr >> idt;

// ===========================================================================
// Initialize DynaMechs environment - must occur before any linkage systems

// the envoronment is a static (one copy) object

	readConfigParameterLabel(cfg_ptr, "Environment_Parameter_File");
	readFilename(cfg_ptr, env_flname);
	dmEnvironment *environment = dmuLoadFile_env(env_flname);
	if (UseGraphics())
		environment->drawInit();
	dmEnvironment::setEnvironment(environment);

// create the ForceList

	gForceList = new ForceList();

// create the controller list

	gControllerList = new ControllerList();

// ===========================================================================
// Initialize a DynaMechs linkage system
	readConfigParameterLabel(cfg_ptr, "Robot_Parameter_File");
	readFilename(cfg_ptr, robot_flname);

	dmSystem *sys = LoadFile(robot_flname);
	gRobot = dynamic_cast < dmArticulation * >(sys);

	gIntegrator = (dmIntegrator *) new dmIntegEuler();
	gIntegrator->setSystem(gRobot);
}


//----------------------------------------------------------------------------
void Simulation::UpdateSimulation()
{
	gIntegrator->simulate(idt);

	gSimulationTime += idt;
}
