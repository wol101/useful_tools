// LoggedForce - a virtual subclass of dmForce that allows logging by creating a log file
// when the force name is set

#ifndef __LOGGED_FORCE_H__
#define __LOGGED_FORCE_H__

#include <dm.h>
#include <dmForce.h>
#include <iostream.h>
#include <iomanip.h>
#include <fstream.h>

class LoggedForce:public dmForce {
      public:

	LoggedForce();
	///
	virtual ~ LoggedForce();

	bool startLog(void);
	bool stopLog(void);

	bool logging(void) {
		return mLoggingFlag;
	};

	// some useful reporting functions

	// get the muscle power
	double getPower(void) {
		return m_power;
	};


      protected:

	 ofstream * mOutput;
	bool mLoggingFlag;
	double m_power;				 // instantaneous power generated

};

#endif						 // instantaneous power generatednss of dmForce that allows logging by creating a log file
__LOGGED_FORCE_H__
