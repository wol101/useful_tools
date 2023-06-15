// Simulation.h - this simulation object is used to encapsulate
// a dynamechs simulation

#ifndef __SIMULATION_H__
#define __SIMULATION_H__

#include <gl.h>

class dmArticulation;
class dmIntegrator;
class ForceList;
struct dmGLMouse;
class dmGLPolarCamera_zup;
class ControllerList;
class LoggedForce;

class Simulation 
{
public:

	Simulation(void);
	~Simulation(void);

	void Load(char *filename);		 // load paramters from a dynamechs configuration file
	void UpdateSimulation(void);		 // called at each iteration through simulation

	void Reset(void);			 // reset most of the simulation parameters for a rerun
	
	// get hold of various variables

	dmArticulation *GetRobot(void) { return m_Robot; };
	dmIntegrator *GetIntegrator(void) { return m_Integrator; };
	double GetTime(void) { return m_SimulationTime; };
	double GetTimeIncrement(void) { return m_IDT; };
	ForceList *GetForceList(void) { return m_ForceList; };
	ControllerList *GetControllerList(void) { return m_ControllerList; };
	bool UseGraphics(void) { return m_UseGraphics; };
	bool GetControllerDataFromFile(void) { return m_ReadControllerDataFromFile; };

	// set some of the variables
	void SetGraphics(bool flag) { m_UseGraphics = flag; };
	void SetControllerDataFromFile(bool flag) { m_ReadControllerDataFromFile = flag;};

protected:
		
	// parts of the model
		
	dmArticulation *m_Robot;
	dmIntegrator *m_Integrator;

	// drawing is not implemented completely (forces don't draw for example)
	// so keep my own list of forces

	ForceList *m_ForceList;

	// also need a list of controllers

	ControllerList *m_ControllerList;

	// keep track of simulation time
	
	double m_SimulationTime;

	// simulation variables
	
	double m_IDT;

	// graphics control
	
	bool m_UseGraphics;

	// controller data control
	
	bool m_ReadControllerDataFromFile;
	
	// link state for reset
	double *m_qStore;
	double *m_qdStore;
	int m_numDOFs;
	
};



#endif						 //__SIMULATION_H__
