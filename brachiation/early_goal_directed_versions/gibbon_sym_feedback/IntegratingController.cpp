// IntegratingController.cpp

// a simple integrator with multiple inputs and a single output

#include <assert.h>

#include "IntegratingController.h"

// default contsructor - sets all starting values

IntegratingController::IntegratingController(): Controller()
{
}

IntegratingController::IntegratingController(char *fileName):Controller(fileName)
{
}


// destructor - deallocates storage

IntegratingController::~IntegratingController()
{
}


// Calculate
//
// recalculate the output value and return it

double 
IntegratingController::Calculate(double /*time*/)
{
	assert(m_nParameters == mSensorList.size()); // sanity check

	double output = 0;

	for (int i = 0; i < m_nParameters; i++)
	{
		output += mSensorList[i]->GetValue() * m_Parameters[i];
	}
	
	output /= m_nParameters; // attempt at normalisation
	
	mIntegratingControllerOutput.SetValue(output);
	
	return output;
}

// AddSensor

// Add a sensor to the internal list of sensors

void 
IntegratingController::AddSensor(Sensor *theSensor)
{
	mSensorList.push_back(theSensor);
}