// SquareController

// cyclic controller based on a series of square functions

#ifndef __SquareController_H__
#define __SquareController_H__

#include "Controller.h"

class SquareController:public Controller { public:
	 SquareController();
	SquareController(char *fileName);
	~SquareController();

	virtual double Calculate(double time);
	double Square(double x);
};

#endif		
__SquareController_H__
