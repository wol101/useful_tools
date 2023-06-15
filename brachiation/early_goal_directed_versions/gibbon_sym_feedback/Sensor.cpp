// sensor class - stores a double buffered value

#include "Sensor.h"
#include "SensorList.h"
#include "Simulation.h"

extern Simulation *gSimulation;

Sensor::Sensor() : dmObject()
{
	mCurrentValue = mLastValue = 0;
	
	gSimulation->GetSensorList()->AddSensor(this);
}