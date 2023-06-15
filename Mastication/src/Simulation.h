// Simulation.h - this simulation object is used to encapsulate
// a dynamechs simulation

#ifndef __SIMULATION_H__
#define __SIMULATION_H__

#include "CPG.h"
#include "ForceList.h"
#include "DampedSpringStrap.h"

class dmArticulation;
class dmIntegrator;
class dmEnvironment;
class ForceList;
class Mandible;
class Skull;
class DataFile;

enum FitnessType
{
    DistanceTravelled = 0,
    KinematicMatch = 1
};

class Simulation 
{
public:

  Simulation(void);
  ~Simulation(void);

  void LoadModel(DataFile &textFile);  // load parameters from a dynamechs configuration file
  void UpdateSimulation(void);     // called at each iteration through simulation

  // get hold of various variables

  dmArticulation *GetRobot(void) { return m_Robot; };
  dmIntegrator *GetIntegrator(void) { return m_Integrator; };
  double GetTime(void) { return m_SimulationTime; };
  double GetTimeIncrement(void) { return m_IDT; };
  ForceList *GetForceList(void) { return &m_ForceList; };
  bool UseGraphics(void) { return m_UseGraphics; };
  double GetMechanicalEnergy(void) { return m_MechanicalEnergy; };
  double GetMetabolicEnergy(void) { return m_MetabolicEnergy; };
  
  // fitness related values
  
  bool TestForCatastrophy();
  double CalculateInstantaneousFitness();
  FitnessType GetFitnessType() { return m_FitnessType; };

  // set some of the variables
  void SetGraphics(bool flag) { m_UseGraphics = flag; };
  void SetFitnessType(FitnessType f) { m_FitnessType = f; };

  // get the model links
  Mandible * GetMandible() { return m_Mandible; };
  Skull * GetSkull() { return m_Skull; };
  
  // get the model indices
  int GetMandibleIndex() { return m_MandibleIndex; };
  int GetSkullIndex() { return m_SkullIndex; };
  
  // get the CPG
  CPG * GetCPG() { return &m_CPG; };

  // get the springs
  DampedSpringStrap * GetTooth() { return &m_Tooth; };
  DampedSpringStrap * GetLeftTMJ() { return &m_LeftTMJ; };
  DampedSpringStrap * GetRightTMJ() { return &m_RightTMJ; };
  
protected:
    
  // parts of the model
    
  dmArticulation *m_Robot;
  dmIntegrator *m_Integrator;
  dmEnvironment *m_Environment;
        
  Mandible *m_Mandible;
  Skull *m_Skull;

  int m_MandibleIndex;
  int m_SkullIndex;

  // drawing is not implemented completely (forces don't draw for example)
  // so keep my own list of forces

  ForceList m_ForceList;
  
  // Central Pattern Generator

  CPG m_CPG;

  // keep track of simulation time
  
  double m_SimulationTime; // current time
  double m_IDT; // step size
  
  // and calculated energy
  
  double m_MechanicalEnergy;
  double m_MetabolicEnergy;

  // graphics control
  
  bool m_UseGraphics;

  // range control
  double m_MandiblePositionRange[3][2];
  double m_MandibleVelocityRange[3][2];
  
  // FitnessType
  FitnessType m_FitnessType;

  // Springs
  DampedSpringStrap m_Tooth;
  DampedSpringStrap m_LeftTMJ;
  DampedSpringStrap m_RightTMJ;
  
};



#endif //__SIMULATION_H__
