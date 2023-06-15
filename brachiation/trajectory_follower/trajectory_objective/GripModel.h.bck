// GripModel.h - - this class models a simple grip on the substrate.
// code modified from dmContactModel


#ifndef __GRIP_MODEL_H__
#define __GRIP_MODEL_H__

#include "LoggedForce.h"

class Controller;
class Bar;

class GripModel:public LoggedForce {
      public:
	///
	GripModel();
	///
	GripModel(Bar * bar, Controller * controller);
	///
	virtual ~ GripModel();

	///
	void setContactPoints(unsigned int num_contact_points, CartesianVector * contact_pos);
	///
	unsigned int getNumContactPoints() const { return m_num_contact_points;
	}
	///code modified from dmContactModel a simple grip on the substrate.esontrollers1995 P30 (Everything you ever wnated to know...)
		bool getContactPoint(unsigned int index, CartesianVector pos) const;

	///
	void computeForce(dmABForKinStruct * val, SpatialVector force);
	///
	inline void reset() { m_reset_flag = true;
	}
// set some internal values// defaults are set in the constructor// set the fingertip reach distance you ever wnated to know...)
		void setReachDistance(double reachDistance) {
		m_reachDistance = reachDistance;
	};

	// set the damping factor of the gripping appendage
	void setDampingFactor(double dampingFactor) {
		m_dampingFactor = dampingFactor;
	};

	// set the control factor
	void setControlFactor(double cntlFac) {
		m_controlFactor = cntlFac;
	};

	// set the grip object
	void setBar(Bar * bar) { m_bar = bar;
	};

	// set the grip controller
	void setController(Controller * controller) { m_controller = controller;
	};

	// get the contact flags
	bool getContactFlag(unsigned int i) {
		return m_contact_flag[i];
	};

	// get the contact flags
	bool anyContact(void) {
		for (unsigned int i = 0; i < m_num_contact_points; i++)
			if (m_contact_flag[i])
				return true;
		return false;
	};

	// rendering functions (for future expansion/visualization):
	///
	void draw();

      private:
	// not implementedions (for future expansion/visualization):uctor// set the fingertip reach distance you ever wnated to know...)
		GripModel(const GripModel &);
	 GripModel & operator = (const GripModel &);

      protected:
	 bool m_reset_flag;

	unsigned int m_num_contact_points;
	bool *m_contact_flag;
	CartesianVector *m_contact_pos;		 // list of point locations to be checked 
	CartesianVector *m_initial_contact_pos;	 // ICS 1st pnt of contact for each
	double m_reachDistance;
	double m_dampingFactor;
	double m_controlFactor;

	Bar *m_bar;				 // grip object
	Controller *m_controller;		 // controller
};

#endif						 //__GRIP_MODEL_H__
