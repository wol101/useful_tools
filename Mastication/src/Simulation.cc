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
  m_Environment = 0;
  m_IDT = 0;
  m_UseGraphics = false;
  m_Mandible = 0;
  m_Skull = 0;
  m_MandibleIndex = -1;
  m_SkullIndex = -1;
  m_MechanicalEnergy = 0;
  m_MetabolicEnergy = 0;
  m_FitnessType = DistanceTravelled;
  
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
  if (m_Environment)
    delete m_Environment;
  if (m_Mandible)
    delete m_Mandible;
  if (m_Skull)
    delete m_Skull;
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

  m_Environment = new dmEnvironment();

  // gravity
  CartesianVector gravity;
  textFile.RetrieveParameter("kGravityVector", 3, gravity);
  m_Environment->setGravity(gravity);
  
  // terrain model
  bool flag = textFile.GetExitOnError();
  textFile.SetExitOnError(false);
  if (textFile.RetrieveQuotedStringParameter("kTerrainDataFilename", terrainFileName, 256) == false)
    m_Environment->loadTerrainData(terrainFileName);
  textFile.SetExitOnError(flag);
    
  // ground characteristics
  textFile.RetrieveParameter("kGroundPlanarSpringConstant", &a);
  m_Environment->setGroundPlanarSpringConstant(a);
  textFile.RetrieveParameter("kGroundNormalSpringConstant", &a);
  m_Environment->setGroundNormalSpringConstant(a);
  textFile.RetrieveParameter("kGroundPlanarDamperConstant", &a);
  m_Environment->setGroundPlanarDamperConstant(a);
  textFile.RetrieveParameter("kGroundNormalDamperConstant", &a);
  m_Environment->setGroundNormalDamperConstant(a);
  textFile.RetrieveParameter("kGroundStaticFrictionCoeff", &a);
  textFile.RetrieveParameter("kGroundKineticFrictionCoeff", &b);
  m_Environment->setFrictionCoeffs(a, b);
  
  if (UseGraphics() && m_Environment->getTerrainFilename())
    m_Environment->drawInit();
  dmEnvironment::setEnvironment(m_Environment);

  // Initialize a DynaMechs linkage system
  
  // read the linkage globals
  ReadSegmentGlobals(textFile);
      
  m_Robot = new dmArticulation();
  m_Robot->setName("BipedalSimulation");
  CartesianVector pos = {0.0, 0.0, 0.0};
  Quaternion quat = {0.0, 0.0, 0.0, 1.0};
  m_Robot->setRefSystem(quat, pos);
  
  m_Skull = new Skull();
  m_Robot->addLink(m_Skull, 0);

  m_Mandible = new Mandible();
  m_Robot->addLink(m_Mandible, m_Skull);
      
  m_Integrator = (dmIntegrator *) new dmIntegEuler();
  m_Integrator->addSystem(m_Robot);
  
  // set the indices
  
  m_MandibleIndex = m_Robot->getLinkIndex(m_Mandible);
  m_SkullIndex = m_Robot->getLinkIndex(m_Skull);

  // read the crash ranges
  textFile.RetrieveParameter("kMandiblePositionRange", 6, (double *)m_MandiblePositionRange);
  textFile.RetrieveParameter("kMandibleVelocityRange", 6, (double *)m_MandibleVelocityRange);
}


//----------------------------------------------------------------------------
void Simulation::UpdateSimulation()
{
  // run the simulation
  m_Integrator->simulate(m_IDT);

  // fix the skull position - doesn't work properly - use ground stabilisers instead
  // m_Skull->setState(kSkullPosition, kSkullVelocity);

  // update the springs;
  m_Tooth.UpdateWorldPositions();
  m_Tooth.UpdateTension();
  m_LeftTMJ.UpdateWorldPositions();
  m_LeftTMJ.UpdateTension();
  m_RightTMJ.UpdateWorldPositions();
  m_RightTMJ.UpdateTension();

  // update the time counter
  m_SimulationTime += m_IDT;
  
  // update the Central Pattern Generator
  m_CPG.Update(m_SimulationTime);
  
  // update the energy 

  m_MechanicalEnergy += m_CPG.GetLeftTemporalis()->GetPower() * m_IDT;
  //m_MetabolicEnergy += m_CPG.GetLeftTemporalis()->GetMetabolicPower() * m_IDT;

  m_MechanicalEnergy += m_CPG.GetLeftMasseter()->GetPower() * m_IDT;
  //m_MetabolicEnergy += m_CPG.GetLeftMasseter()->GetMetabolicPower() * m_IDT;

  m_MechanicalEnergy += m_CPG.GetLeftMedialPterygoid()->GetPower() * m_IDT;
  //m_MetabolicEnergy += m_CPG.GetLeftMedialPterygoid()->GetMetabolicPower() * m_IDT;

  m_MechanicalEnergy += m_CPG.GetLeftLateralPterygoid()->GetPower() * m_IDT;
  //m_MetabolicEnergy += m_CPG.GetLeftLateralPterygoid()->GetMetabolicPower() * m_IDT;

  m_MechanicalEnergy += m_CPG.GetRightTemporalis()->GetPower() * m_IDT;
  //m_MetabolicEnergy += m_CPG.GetRightTemporalis()->GetMetabolicPower() * m_IDT;

  m_MechanicalEnergy += m_CPG.GetRightMasseter()->GetPower() * m_IDT;
  //m_MetabolicEnergy += m_CPG.GetRightMasseter()->GetMetabolicPower() * m_IDT;

  m_MechanicalEnergy += m_CPG.GetRightMedialPterygoid()->GetPower() * m_IDT;
  //m_MetabolicEnergy += m_CPG.GetRightMedialPterygoid()->GetMetabolicPower() * m_IDT;

  m_MechanicalEnergy += m_CPG.GetRightLateralPterygoid()->GetPower() * m_IDT;
  //m_MetabolicEnergy += m_CPG.GetRightLateralPterygoid()->GetMetabolicPower() * m_IDT;

  // at appropriate times output the state
  static double nextReportingTime = 1;
  if (m_SimulationTime + 3 * m_IDT >= nextReportingTime)
  {
      std::cout << m_SimulationTime;
      CartesianVector force;
      m_Tooth.GetAnchorForce(1, force);
      std::cout << "\ttooth\t" << force[0] << "\t" << force[1] << "\t" << force[2];
      m_LeftTMJ.GetAnchorForce(1, force);
      std::cout << "\tLeftTMJ\t" << force[0] << "\t" << force[1] << "\t" << force[2];
      m_RightTMJ.GetAnchorForce(1, force);
      std::cout << "\tRightTMJ\t" << force[0] << "\t" << force[1] << "\t" << force[2] << "\n";
      nextReportingTime += 1.0;
      std::cout.flush();
  }
}

//----------------------------------------------------------------------------
bool Simulation::TestForCatastrophy()
{
  const dmABForKinStruct *theMandibleDmABForKinStruct = 
    m_Robot->getForKinStruct(m_MandibleIndex);
  SpatialVector mandibleWorldV;
  CartesianVector mandibleCGWorldPos;
  int i;
  
  // get mandible CM world position and test against limits
  Util::ConvertLocalToWorldP(theMandibleDmABForKinStruct, kMandibleCG, mandibleCGWorldPos);
  for (i = 0; i < 3; i++)
  {
      if (mandibleCGWorldPos[i] < m_MandiblePositionRange[i][0] ||
          mandibleCGWorldPos[i] > m_MandiblePositionRange[i][1])
          return true;
  }
    
  // get mandible world velocity and test against limits
  Util::ConvertLocalToWorldV(theMandibleDmABForKinStruct, theMandibleDmABForKinStruct->v, 
    mandibleWorldV);

  for (i = 0; i < 3; i++)
  {
      if (mandibleWorldV[i] < m_MandibleVelocityRange[i][0] ||
          mandibleWorldV[i] > m_MandibleVelocityRange[i][1])
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
  const dmABForKinStruct *theDmABForKinStruct;
  double q[7], qd[6];
  int i;
  double error;
  double positionWeight = 1.0;
  double velocityWeight = 0.01;
  
  switch (m_FitnessType)
  {
    case DistanceTravelled:
    {  
      theDmABForKinStruct = m_Robot->getForKinStruct(m_MandibleIndex);
      return theDmABForKinStruct->p_ICS[0];
    }
    
    case KinematicMatch:
    {
      error = 0;
      m_Mandible->getState(q, qd);
      for (i = 0; i < 7; i++) error += positionWeight * fabs(q[i] - kMandiblePositionTarget[i]);
      for (i = 0; i < 6; i++) error += velocityWeight * fabs(qd[i] - kMandibleVelocityTarget[i]);
      return -error;        
    }      
  }
  return 0;  
}

         


