// SinController

// cyclic controller based on a series of sin functions

#ifndef __SINCONTROLLER_H__
#define __SINCONTROLLER_H__

#include "Controller.h"

class SinController:public Controller { public:
	 SinController();
	SinController(char *fileName);
	~SinController();

	virtual double Calculate(double time);

};

#endif		
__SINCONTROLLER_H__
