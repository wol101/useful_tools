// sensor class - stores a double buffered value

#pragma once

#include <dmObject.h>

class Sensor: public dmObject
{
public:
	
				Sensor();
				~Sensor() {};
		
	void			SetValue(double inValue) { mCurrentValue = inValue; } ;
	double			GetValue() { return mLastValue; };
	void			Bump() { mLastValue = mCurrentValue; };
		
protected:
	
	double			mCurrentValue;
	double			mLastValue;
};

