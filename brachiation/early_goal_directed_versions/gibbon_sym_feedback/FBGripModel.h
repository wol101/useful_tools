// FBGripModel.h - - this class models a simple grip on the substrate.
// code modified from dmContactModel


#ifndef __FBGRIP_MODEL_H__
#define __FBGRIP_MODEL_H__

#include "LoggedForce.h"
#include "Sensor.h"

class Controller;
class Bar;

class FBGripModel:public LoggedForce 
{
      public:
	
	FBGripModel();
	
	FBGripModel(Bar * bar, Controller * controller);
	
	virtual ~ FBGripModel();

	void setContactPoints(unsigned int num_contact_points, CartesianVector * contact_pos);
	
	unsigned int getNumContactPoints() const { return m_num_contact_points;}
	
	bool getContactPoint(unsigned int index, CartesianVector pos) const;

	void computeForce(dmABForKinStruct * val, SpatialVector force);
	
	inline void reset() { m_reset_flag = true;}
	
	// set some internal values
	// defaults are set in the constructor
	
	// set the fingertip reach distance
	void setReachDistance(double reachDistance) { m_reachDistance = reachDistance; };

	// set the damping factor of the gripping appendage
	void setDampingFactor(double dampingFactor) { m_dampingFactor = dampingFactor; };

	// set the control factor
	void setControlFactor(double cntlFac) { m_controlFactor = cntlFac; };

	// set the control factor
	void setMaxForce(double maxForce) { m_maxForce = maxForce;};

	// set the grip object
	void setBar(Bar * bar) { m_bar = bar; };

	// set the grip controller
	void setController(Controller * controller) { m_controller = controller; };

	// get the contact flags
	bool getContactFlag(unsigned int i) { return m_contact_flag[i]; };

	// get the contact flags
	bool anyContact(void) 
	{
		for (unsigned int i = 0; i < m_num_contact_points; i++)
			if (m_contact_flag[i])
				return true;
		return false;
	};

	// rendering functions (for future expansion/visualization):
	
	void draw();

      private:
	// not implemented
	FBGripModel(const FBGripModel &);
	FBGripModel & operator = (const FBGripModel &);

      protected:
	 bool m_reset_flag;

	unsigned int m_num_contact_points;
	bool *m_contact_flag;
	CartesianVector *m_contact_pos;		 // list of point locations to be checked 
	CartesianVector *m_initial_contact_pos;	 // ICS 1st pnt of contact for each
	double m_reachDistance;
	double m_dampingFactor;
	double m_controlFactor;
	double m_maxForce;

	Bar *m_bar;				 // grip object
	Controller *m_controller;		 // controller
	
	// sensors
	Sensor m_XSensor;			// grip reaction force sensors (local coordinates)
	Sensor m_YSensor;
	Sensor m_ZSensor;
};

#endif						 //__FBGRIP_MODEL_H__
