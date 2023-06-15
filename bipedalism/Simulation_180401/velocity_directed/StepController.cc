// StepController

// controller that cycles through each controller value in list

#include <math.h>
#include <assert.h>

#include "StepController.h"

StepController::StepController():
Controller()
{
}

StepController::StepController(double stepTime):
Controller()
{
  m_StepTime = stepTime;
}

StepController::StepController(char *fileName):Controller(fileName)
{
}

StepController::~StepController()
{
}

double StepController::Calculate(double time)
{
	double output;
	// work out where we are in the list
	
	int index = (int)(time / m_StepTime);
	double rem = time - ((double)index * m_StepTime);
	double interp = rem / m_StepTime;

	assert(m_nParamaters);
	index = index % m_nParamaters;
	// cerr << "index = " << index << "\n";
	if (index == m_nParamaters - 1)
		output = m_Parameters[index] + interp * (m_Parameters[0] - m_Parameters[index]);
	else 	
		output = m_Parameters[index] + interp * (m_Parameters[index + 1] - m_Parameters[index]);

	// cerr << "output = " << output << "\n";
	
	return output;
}		
