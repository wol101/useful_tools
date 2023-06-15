// Simulation.cpp - this simulation object is used to encapsulate
// a dynamechs simulation

// this is an encapsulated version of the main routine used in most of Scott McMillan's examples
// with various bits and pieces added and removed and moved.

// modified by wis to include a specific definintion of the model
// to allow me to customise things a bit more easily

#include <dm.h>           // DynaMechs typedefs, globals, etc.
#include <dmSystem.hpp>         // DynaMechs simulation code.
#include <dmEnvironment.hpp>
#include <dmArticulation.hpp>
#include <dmIntegrator.hpp>
#include <dmIntegEuler.hpp>
#include <dmu.h>

#include <assert.h>

#include "ForceList.h"
#include "Segments.h"
#include "ParameterFile.h"

#include "Simulation.h"

Simulation::Simulation()
{
  // set some variables
  m_SimulationTime = 0;
  m_Robot = 0;
  m_Integrator = 0;
  m_IDT = 0;
  m_UseGraphics = true;
  m_Torso = 0;
  m_LeftThigh = 0;
  m_RightThigh = 0;
  m_LeftLeg = 0;
  m_RightLeg = 0;
  m_LeftFoot = 0;
  m_RightFoot = 0;
  
  m_CPG.Initialise();
}

//----------------------------------------------------------------------------
Simulation::~Simulation()
{
  // get rid of all those memory alloactions
  if (m_Integrator)
    delete m_Integrator;
  if (m_Robot)
    delete m_Robot;
  if (m_Torso)
    delete m_Torso;
  if (m_LeftThigh)
    delete m_LeftThigh;
  if (m_RightThigh)
    delete m_RightThigh;
   if (m_LeftLeg)
    delete m_LeftLeg;
  if (m_RightLeg)
    delete m_RightLeg;
  if (m_LeftFoot)
    delete m_LeftFoot;
  if (m_RightFoot)
    delete m_RightFoot;
}


//----------------------------------------------------------------------------
void Simulation::LoadModel(const char *filename)
{
  char terrainFileName[256];
  double a, b;
  
  // load up the model file into memory
  ParameterFile myFile;
  myFile.SetExitOnError(true);
  myFile.ReadFile(filename);

  // Read simulation timing information.
  myFile.RetrieveParameter("kIntegrationStepsize", &m_IDT);

  // ===========================================================================
  // Initialize DynaMechs environment - must occur before any linkage systems

  // the envoronment is a static (one copy) object
  dmEnvironment *environment = new dmEnvironment();

  // gravity
  CartesianVector gravity;
  myFile.RetrieveParameter("kGravityVector", 3, gravity);
  environment->setGravity(gravity);
  
  // terrain model
  myFile.RetrieveQuotedStringParameter("kTerrainDataFilename", terrainFileName, 256);
  environment->loadTerrainData(terrainFileName);
  
  // ground characteristics
  myFile.RetrieveParameter("kGroundPlanarSpringConstant", &a);
  environment->setGroundPlanarSpringConstant(a);
  myFile.RetrieveParameter("kGroundNormalSpringConstant", &a);
  environment->setGroundNormalSpringConstant(a);
  myFile.RetrieveParameter("kGroundPlanarDamperConstant", &a);
  environment->setGroundPlanarDamperConstant(a);
  myFile.RetrieveParameter("kGroundNormalDamperConstant", &a);
  environment->setGroundNormalDamperConstant(a);
  myFile.RetrieveParameter("kGroundStaticFrictionCoeff", &a);
  myFile.RetrieveParameter("kGroundKineticFrictionCoeff", &b);
  environment->setFrictionCoeffs(a, b);
  
  if (UseGraphics())
    environment->drawInit();
  dmEnvironment::setEnvironment(environment);

  // Initialize a DynaMechs linkage system
  
  // read the linkage globals
  ReadSegmentGlobals(myFile);
      
  m_Robot = new dmArticulation();
  m_Robot->setName("BipedalSimulation");
  CartesianVector pos = {0.0, 0.0, 0.0};
  Quaternion quat = {0.0, 0.0, 0.0, 1.0};
  m_Robot->setRefSystem(quat, pos);
  
  m_Torso = new Torso();
  m_Robot->addLink(m_Torso, 0);
      
  m_LeftThigh = new LeftThigh();
  m_Robot->addLink(m_LeftThigh, m_Torso);

  m_LeftLeg = new LeftLeg();
  m_Robot->addLink(m_LeftLeg, m_LeftThigh);

  m_LeftFoot = new LeftFoot();
  m_Robot->addLink(m_LeftFoot, m_LeftLeg);

  m_RightThigh = new RightThigh();
  m_Robot->addLink(m_RightThigh, m_Torso);

  m_RightLeg = new RightLeg();
  m_Robot->addLink(m_RightLeg, m_RightThigh);

  m_RightFoot = new RightFoot();
  m_Robot->addLink(m_RightFoot, m_RightLeg);

  m_Integrator = (dmIntegrator *) new dmIntegEuler();
  m_Integrator->addSystem(m_Robot);
  
}


//----------------------------------------------------------------------------
void Simulation::UpdateSimulation()
{
  // run the simulation
  m_Integrator->simulate(m_IDT);
  
  // update the joints that need it
  m_LeftThigh->UpdateMidpoint();
  m_RightThigh->UpdateMidpoint();

  // update the time counter
  m_SimulationTime += m_IDT;
  
  // update the Central Pattern Generator
  m_CPG.Update(m_SimulationTime);
  
}


