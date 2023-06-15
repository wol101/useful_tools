// StepController

// controller that cycles through each controller value in list

#ifndef __StepController_H__
#define __StepController_H__

#include "Controller.h"

class StepController:public Controller 
{ 
public:
	 StepController();
	 StepController(double stepTime);
	StepController(char *fileName);
	~StepController();

	virtual double Calculate(double time);
	void SetStepTime(double time) { m_StepTime = time; };
	

protected:
	double m_StepTime;	
	
};

#endif		
__StepController_H__
