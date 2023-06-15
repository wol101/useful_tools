// Controller Parent Class

// provides basic functionality

// can read itself from a file and the parameters can be set externally

// can calculate itself for any given time

#ifndef __CONTROLLER_H__
#define __CONTROLLER_H__

#include <dmObject.h>

class Controller:public dmObject 
{ 
public:
	Controller();
	Controller(char *fileName);
	~Controller();

	virtual void SetParameters(int n, double *p);
	virtual void ChangeParameters(int start, int end, double *p);
	virtual double Calculate(double time) = 0; // calculate the current value
	virtual char *GetType() = 0;

protected:

	int m_nParameters;
	double *m_Parameters;
};

#endif

__CONTROLLER_H__
