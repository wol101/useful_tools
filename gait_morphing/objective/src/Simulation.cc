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

#include <assert.h>

#include "ForceList.h"
#include "Segments.h"
#include "SegmentParameters.h"
#include "Util.h"
#include "DebugControl.h"

#include "DataFile.h"

#include "Simulation.h"

Simulation::Simulation()
{
  // set some variables
  m_SimulationTime = 0;
  m_Robot = 0;
  m_Integrator = 0;
  m_IDT = 0;
  m_UseGraphics = false;
  m_Torso = 0;
  m_LeftThigh = 0;
  m_RightThigh = 0;
  m_LeftLeg = 0;
  m_RightLeg = 0;
  m_LeftFoot = 0;
  m_RightFoot = 0;
  m_TorsoIndex = -1;
  m_LeftThighIndex = -1;
  m_RightThighIndex = -1;
  m_LeftLegIndex = -1;
  m_RightLegIndex = -1;
  m_LeftFootIndex = -1;
  m_RightFootIndex = -1;
  m_MechanicalEnergy = 0;
  m_MetabolicEnergy = 0;
  
  // and clear the global error flag
  gDynamechsError = 0;
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
void Simulation::LoadModel(DataFile &textFile)
{
  char terrainFileName[256];
  double a, b;
  
  // Read simulation timing information.
  textFile.RetrieveParameter("kIntegrationStepsize", &m_IDT);

  // ===========================================================================
  // Initialize DynaMechs environment - must occur before any linkage systems

  // the envoronment is a static (one copy) object
  dmEnvironment *environment = new dmEnvironment();

  // gravity
  CartesianVector gravity;
  textFile.RetrieveParameter("kGravityVector", 3, gravity);
  environment->setGravity(gravity);
  
  // terrain model
  bool flag = textFile.GetExitOnError();
  textFile.SetExitOnError(false);
  if (textFile.RetrieveQuotedStringParameter("kTerrainDataFilename", terrainFileName, 256) == false)
    environment->loadTerrainData(terrainFileName);
  textFile.SetExitOnError(flag);
    
  // ground characteristics
  textFile.RetrieveParameter("kGroundPlanarSpringConstant", &a);
  environment->setGroundPlanarSpringConstant(a);
  textFile.RetrieveParameter("kGroundNormalSpringConstant", &a);
  environment->setGroundNormalSpringConstant(a);
  textFile.RetrieveParameter("kGroundPlanarDamperConstant", &a);
  environment->setGroundPlanarDamperConstant(a);
  textFile.RetrieveParameter("kGroundNormalDamperConstant", &a);
  environment->setGroundNormalDamperConstant(a);
  textFile.RetrieveParameter("kGroundStaticFrictionCoeff", &a);
  textFile.RetrieveParameter("kGroundKineticFrictionCoeff", &b);
  environment->setFrictionCoeffs(a, b);
  
  if (UseGraphics() && environment->getTerrainFilename())
    environment->drawInit();
  dmEnvironment::setEnvironment(environment);

  // Initialize a DynaMechs linkage system
  
  // read the linkage globals
  ReadSegmentGlobals(textFile);
      
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
  
  // set the indices
  
  m_TorsoIndex = m_Robot->getLinkIndex(m_Torso);
  m_LeftThighIndex = m_Robot->getLinkIndex(m_LeftThigh);
  m_LeftLegIndex = m_Robot->getLinkIndex(m_LeftLeg);
  m_LeftFootIndex = m_Robot->getLinkIndex(m_LeftFoot);
  m_RightThighIndex = m_Robot->getLinkIndex(m_RightThigh);
  m_RightLegIndex = m_Robot->getLinkIndex(m_RightLeg);
  m_RightFootIndex = m_Robot->getLinkIndex(m_RightFoot);

  // read the crash ranges
  textFile.RetrieveParameter("kTorsoPositionRange", 6, (double *)m_TorsoPositionRange);
  textFile.RetrieveParameter("kTorsoVelocityRange", 6, (double *)m_TorsoVelocityRange);
}


//----------------------------------------------------------------------------
void Simulation::UpdateSimulation()
{
  // run the simulation
  m_Integrator->simulate(m_IDT);

  if (gDebug == SimulationDebug)
  {
      double a, alpha, d, theta;
      cerr << "Simulation::UpdateSimulation()\tm_SimulationTime\t" << m_SimulationTime << "\t";
      m_LeftThigh->getMDHParameters(&a, &alpha, &d, &theta);
      cerr << "LeftThighTheta\t" << theta << "\t";
      m_RightThigh->getMDHParameters(&a, &alpha, &d, &theta);
      cerr << "RightThighTheta\t" << theta << "\t";
      m_LeftLeg->getMDHParameters(&a, &alpha, &d, &theta);
      cerr << "LeftLegTheta\t" << theta << "\t";
      m_RightLeg->getMDHParameters(&a, &alpha, &d, &theta);
      cerr << "RightLegTheta\t" << theta << "\t";
      m_LeftFoot->getMDHParameters(&a, &alpha, &d, &theta);
      cerr << "LeftFootTheta\t" << theta << "\t";
      m_RightFoot->getMDHParameters(&a, &alpha, &d, &theta);
      cerr << "RightFootTheta\t" << theta << "\n";
  }
  
  // update the joints that need it
  m_LeftThigh->UpdateMidpoint();
  m_RightThigh->UpdateMidpoint();

  // update the time counter
  m_SimulationTime += m_IDT;
  
  // update the Central Pattern Generator
  m_CPG.Update(m_SimulationTime);
  
  // update the energy 
  
  m_MechanicalEnergy += m_CPG.GetLeftHipExtensor()->GetPower() * m_IDT;
  m_MechanicalEnergy += m_CPG.GetLeftHipFlexor()->GetPower() * m_IDT;
  m_MechanicalEnergy += m_CPG.GetRightHipExtensor()->GetPower() * m_IDT;
  m_MechanicalEnergy += m_CPG.GetRightHipFlexor()->GetPower() * m_IDT;
  m_MechanicalEnergy += m_CPG.GetLeftKneeExtensor()->GetPower() * m_IDT;
  m_MechanicalEnergy += m_CPG.GetLeftKneeFlexor()->GetPower() * m_IDT;
  m_MechanicalEnergy += m_CPG.GetRightKneeExtensor()->GetPower() * m_IDT;
  m_MechanicalEnergy += m_CPG.GetRightKneeFlexor()->GetPower() * m_IDT;
  m_MechanicalEnergy += m_CPG.GetLeftAnkleExtensor()->GetPower() * m_IDT;
  m_MechanicalEnergy += m_CPG.GetLeftAnkleFlexor()->GetPower() * m_IDT;
  m_MechanicalEnergy += m_CPG.GetRightAnkleExtensor()->GetPower() * m_IDT;
  m_MechanicalEnergy += m_CPG.GetRightAnkleFlexor()->GetPower() * m_IDT;
  
  m_MetabolicEnergy += m_CPG.GetLeftHipExtensor()->GetMetabolicPower() * m_IDT;
  m_MetabolicEnergy += m_CPG.GetLeftHipFlexor()->GetMetabolicPower() * m_IDT;
  m_MetabolicEnergy += m_CPG.GetRightHipExtensor()->GetMetabolicPower() * m_IDT;
  m_MetabolicEnergy += m_CPG.GetRightHipFlexor()->GetMetabolicPower() * m_IDT;
  m_MetabolicEnergy += m_CPG.GetLeftKneeExtensor()->GetMetabolicPower() * m_IDT;
  m_MetabolicEnergy += m_CPG.GetLeftKneeFlexor()->GetMetabolicPower() * m_IDT;
  m_MetabolicEnergy += m_CPG.GetRightKneeExtensor()->GetMetabolicPower() * m_IDT;
  m_MetabolicEnergy += m_CPG.GetRightKneeFlexor()->GetMetabolicPower() * m_IDT;
  m_MetabolicEnergy += m_CPG.GetLeftAnkleExtensor()->GetMetabolicPower() * m_IDT;
  m_MetabolicEnergy += m_CPG.GetLeftAnkleFlexor()->GetMetabolicPower() * m_IDT;
  m_MetabolicEnergy += m_CPG.GetRightAnkleExtensor()->GetMetabolicPower() * m_IDT;
  m_MetabolicEnergy += m_CPG.GetRightAnkleFlexor()->GetMetabolicPower() * m_IDT;
    
}

//----------------------------------------------------------------------------
bool Simulation::TestForCatastrophy()
{
  const dmABForKinStruct *theTorsoDmABForKinStruct = 
    m_Robot->getForKinStruct(m_TorsoIndex);
  SpatialVector torsoWorldV;
  CartesianVector torsoCGWorldPos;
  int i;
  
  // get torso CM world position and test against limits
  Util::ConvertLocalToWorldP(theTorsoDmABForKinStruct, kTorsoCG, torsoCGWorldPos);
  for (i = 0; i < 3; i++)
  {
      if (torsoCGWorldPos[i] < m_TorsoPositionRange[i][0] ||
          torsoCGWorldPos[i] > m_TorsoPositionRange[i][1])
          return true;
  }
    
  // get torso world velocity and test against limits
  Util::ConvertLocalToWorldV(theTorsoDmABForKinStruct, theTorsoDmABForKinStruct->v, 
    torsoWorldV);

  for (i = 0; i < 3; i++)
  {
      if (torsoWorldV[i] < m_TorsoVelocityRange[i][0] ||
          torsoWorldV[i] > m_TorsoVelocityRange[i][1])
          return true;
  }
          
  // check the gDynamechsError status
  if (gDynamechsError)
  {
    return true;
  }

  return false;
}


//----------------------------------------------------------------------------
double Simulation::CalculateInstantaneousFitness()
{
  const dmABForKinStruct *theTorsoDmABForKinStruct = 
    m_Robot->getForKinStruct(m_TorsoIndex);
  double xPosition = theTorsoDmABForKinStruct->p_ICS[0];
  
  return xPosition;
}

         


