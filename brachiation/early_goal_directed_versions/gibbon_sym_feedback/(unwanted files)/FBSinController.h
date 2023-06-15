// FBSinController

// cyclic controller based on a series of sin functions with feedback

#ifndef __FBSinController_H__
#define __FBSinController_H__

#include "Controller.h"

class FBSinController:public Controller {

public:
	 FBSinController();
	FBSinController(char *fileName);
	~FBSinController();

	virtual double Calculate(double time);

	virtual void SetParameters(int n, double *p);
	virtual void SetFeedbackValues(int n, double *p);
	virtual void ChangeFeedbackValues(int start, int end, double *p);
	
protected:

	int	m_nFeedbackValues;
	double	*m_FeedbackValues;

};

#endif						 // cyclic controller based on a series of sin functionsdraw for example)dinates) log file
__FBSinController_H__
