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

#include <assert.h>

#include "LoadFile.h"
#include "ForceList.h"
#include "ControllerList.h"

#include "Simulation.h"

Simulation::Simulation()
{
	// set some variables
	m_SimulationTime = 0;
	m_ForceList = 0;
	m_Robot = 0;
	m_Integrator = 0;
	m_ControllerList = 0;
	m_IDT = 0;
	m_UseGraphics = true;
	m_ReadControllerDataFromFile = true;
	m_qStore = 0;
	m_qdStore = 0;		
}

//----------------------------------------------------------------------------
Simulation::~Simulation()
{
	// get rid of all those memory alloactions
	if (m_ForceList)
		delete m_ForceList;
	if (m_Integrator)
		delete m_Integrator;
	if (m_Robot)
		delete m_Robot;
	if (m_ControllerList)
		delete m_ControllerList;
	if (m_qStore)
		delete m_qStore;
	if (m_qdStore)
		delete m_qdStore;
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
	cfg_ptr >> m_IDT;

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

	m_ForceList = new ForceList();

// create the controller list

	m_ControllerList = new ControllerList();

// ===========================================================================
// Initialize a DynaMechs linkage system
	readConfigParameterLabel(cfg_ptr, "Robot_Parameter_File");
	readFilename(cfg_ptr, robot_flname);

	dmSystem *sys = LoadFile(robot_flname);
	m_Robot = dynamic_cast < dmArticulation * >(sys);

	m_Integrator = (dmIntegrator *) new dmIntegEuler();
	m_Integrator->setSystem(m_Robot);
	
// save a local copy of (some) the articulation parameters
	m_numDOFs = m_Robot->getNumDOFs();
	m_qStore = new double [m_numDOFs];
	m_qdStore = new double [m_numDOFs];
	m_Robot->getState(m_qStore, m_qdStore);
}


//----------------------------------------------------------------------------
void Simulation::UpdateSimulation()
{
	m_Integrator->simulate(m_IDT);

	m_SimulationTime += m_IDT;
}

//----------------------------------------------------------------------------
void Simulation::Reset()
{
	assert(m_Integrator);
	// reinitialise the model - NB fragile since it just resets the JointInput to zero
	m_Robot->setState(m_qStore, m_qdStore);
	double *jointInput = new double [m_numDOFs];
	::memset(jointInput, 0, sizeof(double) * m_numDOFs);
	m_Robot->setJointInput(jointInput);
	delete jointInput;
	
	// reset the simulation time
	m_SimulationTime = 0;
}

