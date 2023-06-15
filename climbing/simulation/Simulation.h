// Simulation.h - this simulation object is used to encapsulate
// a dynamechs simulation

#ifndef __SIMULATION_H__
#define __SIMULATION_H__

#include "CPG.h"
#include "ForceList.h"
#include "Ladder.h"

class dmArticulation;
class dmIntegrator;
class ForceList;
class Torso;
class LeftThigh;
class RightThigh;
class LeftLeg;
class RightLeg;
class LeftFoot;
class RightFoot;
class LeftArm;
class RightArm;
class LeftForearm;
class RightForearm;
class LeftHand;
class RightHand;
class CPG;
class ForceList;
class Ladder;

class Simulation 
{
public:

  Simulation(void);
  ~Simulation(void);

  void LoadModel(const char *filename);  // load parameters from a dynamechs configuration file
  void UpdateSimulation(void);     // called at each iteration through simulation

  // get hold of various variables

  dmArticulation *GetRobot(void) { return m_Robot; };
  dmIntegrator *GetIntegrator(void) { return m_Integrator; };
  double GetTime(void) { return m_SimulationTime; };
  double GetTimeIncrement(void) { return m_IDT; };
  ForceList *GetForceList(void) { return m_ForceList; };
  bool UseGraphics(void) { return m_UseGraphics; };
  CPG * GetCPG() { return m_CPG; };
  Ladder * GetLadder() { return m_Ladder; };

  // set some of the variables
  void SetGraphics(bool flag) { m_UseGraphics = flag; };

  // get the model links
  Torso * GetTorso() { return m_Torso; };
  LeftThigh * GetLeftThigh() { return m_LeftThigh; };
  RightThigh * GetRightThigh() { return m_RightThigh; };
  LeftLeg * GetLeftLeg() { return m_LeftLeg; };
  RightLeg * GetRightLeg() { return m_RightLeg; };
  LeftFoot * GetLeftFoot() { return m_LeftFoot; };
  RightFoot * GetRightFoot() { return m_RightFoot; };
    
protected:
    
  // parts of the model
    
  dmArticulation *m_Robot;
  dmIntegrator *m_Integrator;
        
  Torso *m_Torso;
  LeftThigh *m_LeftThigh;
  RightThigh *m_RightThigh;
  LeftLeg *m_LeftLeg;
  RightLeg *m_RightLeg;
  LeftFoot *m_LeftFoot;
  RightFoot *m_RightFoot;
  LeftArm *m_LeftArm;
  RightArm *m_RightArm;
  LeftForearm *m_LeftForearm;
  RightForearm *m_RightForearm;
  LeftHand *m_LeftHand;
  RightHand *m_RightHand;

  // drawing is not implemented completely (forces don't draw for example)
  // so keep my own list of forces

  ForceList *m_ForceList;
  
  // Central Pattern Generator

  CPG *m_CPG;
  
  // Ladder
  
  Ladder *m_Ladder;

  // keep track of simulation time
  
  double m_SimulationTime; // current time
  double m_IDT; // step size

  // graphics control
  
  bool m_UseGraphics;
    
};



#endif //__SIMULATION_H__
