// Controller Parent Class

// provides basic functionality

// can read itself from a file and the parameters can be set externally

// can calculate itself for any given time

#ifndef __CONTROLLER_H__
#define __CONTROLLER_H__

#include <dmObject.h>

class Controller:public dmObject { public:
	 Controller();
	Controller(char *fileName);
	~Controller();

        void SetName(const char * const prefix, const char * const suffix);
	void SetParameters(int n, double *p);
	int GetNumParameters() { return m_nParamaters; };
	double GetNthParameter(int n) { return m_Parameters[n]; };
	virtual double Calculate(double time) = 0;

      protected:

	int m_nParamaters;
	double *m_Parameters;
};

#endif						 // can calculate itself for any given timeameters can be set externallywoulde
__CONTROLLER_H__
