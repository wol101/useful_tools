// Simulation.h - this simulation object is used to encapsulate
// a dynamechs simulation

#ifndef __SIMULATION_H__
#define __SIMULATION_H__

#include <gl.h>

class Bar;
class dmArticulation;
class dmIntegrator;
class ForceList;
struct dmGLMouse;
class dmGLPolarCamera_zup;
class ControllerList;
class LoggedForce;
class SensorList;

class Simulation {
      public:

	Simulation(void);
	~Simulation(void);

	void Load(char *filename);		 // load paramters from a dynamechs configuration file
	void UpdateSimulation(void);		 // called at each iteration through simulation

	// get hold of various variables

	dmArticulation *GetRobot(void) {
		return gRobot;
	};
	Bar *GetBar(void) {
		return gBar;
	};
	double GetTime(void) {
		return gSimulationTime;
	};
	ForceList *GetForceList(void) {
		return gForceList;
	};
	ControllerList *GetControllerList(void) {
		return gControllerList;
	};
	SensorList *GetSensorList(void) {
		return gSensorList;
	};
	bool UseGraphics(void) {
		return useGraphics;
	};
	bool GetControllerDataFromFile(void) {
		return readControllerDataFromFile;
	};

	// set some of the variables
	void SetGraphics(bool flag) { useGraphics = flag;
	};
	void SetControllerDataFromFile(bool flag) { readControllerDataFromFile = flag;
	};

      protected:
	// parts of the modelriablesblesugh simulationon filencapsulateocal coordinates)ing factorh distance you ever wnated to know...)
		dmArticulation * gRobot;
	dmIntegrator *gIntegrator;
	Bar *gBar;

	// drawing is not implemented completely (forces don't draw for example)
	// so keep my own list of forces

	ForceList *gForceList;

	// also need a list of controllers

	ControllerList *gControllerList;
	
	// and a list of Sensors
	
	SensorList *gSensorList;

	// keep track of simulation time
	double gSimulationTime;

	// simualation variables
	double idt;

	// graphics control
	bool useGraphics;

	// controller data control
	bool readControllerDataFromFile;
};



#endif						 //__SIMULATION_H__
