// Simulation.h - this simulation object is used to encapsulate
// a dynamechs simulation

#ifndef __SIMULATION_H__
#define __SIMULATION_H__

#include "CPG.h"
#include "ForceList.h"

class dmArticulation;
class dmIntegrator;
class dmEnvironment;
class ForceList;
class Torso;
class LeftThigh;
class RightThigh;
class LeftLeg;
class RightLeg;
class LeftFoot;
class RightFoot;
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
  Torso * GetTorso() { return m_Torso; };
  LeftThigh * GetLeftThigh() { return m_LeftThigh; };
  RightThigh * GetRightThigh() { return m_RightThigh; };
  LeftLeg * GetLeftLeg() { return m_LeftLeg; };
  RightLeg * GetRightLeg() { return m_RightLeg; };
  LeftFoot * GetLeftFoot() { return m_LeftFoot; };
  RightFoot * GetRightFoot() { return m_RightFoot; };
  
  // get the model indices
  int GetTorsoIndex() { return m_TorsoIndex; };
  int GetLeftThighIndex() { return m_LeftThighIndex; };
  int GetRightThighIndex() { return m_RightThighIndex; };
  int GetLeftLegIndex() { return m_LeftLegIndex; };
  int GetRightLegIndex() { return m_RightLegIndex; };
  int GetLeftFootIndex() { return m_LeftFootIndex; };
  int GetRightFootIndex() { return m_RightFootIndex; };
  
  // get the CPG
  CPG * GetCPG() { return &m_CPG; };
  
protected:
    
  // parts of the model
    
  dmArticulation *m_Robot;
  dmIntegrator *m_Integrator;
  dmEnvironment *m_Environment;
        
  Torso *m_Torso;
  LeftThigh *m_LeftThigh;
  RightThigh *m_RightThigh;
  LeftLeg *m_LeftLeg;
  RightLeg *m_RightLeg;
  LeftFoot *m_LeftFoot;
  RightFoot *m_RightFoot;

  int m_TorsoIndex;
  int m_LeftThighIndex;
  int m_RightThighIndex;
  int m_LeftLegIndex;
  int m_RightLegIndex;
  int m_LeftFootIndex;
  int m_RightFootIndex;

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
  double m_BMR;

  // graphics control
  
  bool m_UseGraphics;

  // range control
  double m_TorsoPositionRange[3][2];
  double m_TorsoVelocityRange[3][2];
  
  // FitnessType
  FitnessType m_FitnessType; 
};



#endif //__SIMULATION_H__
