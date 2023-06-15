// IntegrativeController.h

// a simple integrator with multiple inputs and a single output

#pragma once

#include "Controller.h"
#include <vector>
#include "Sensor.h"

class IntegratingController: public Controller
{
public:
	
	IntegratingController();
	IntegratingController(char *fileName);
	~IntegratingController();
	
	virtual double Calculate(double time);
	virtual char *GetType() { return "IntegratingController"; };
	
	void AddSensor(Sensor *theSensor);
	
protected:
	
	Sensor	mIntegratingControllerOutput;
	std::vector <Sensor *> mSensorList;

};

