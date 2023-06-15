// FBMuscleModel - a simple linear muscle model


#ifndef __FBMUSCLE_MODEL_H__
#define __FBMUSCLE_MODEL_H__

#include "LoggedForce.h"
#include "Sensor.h"

class Controller;
class dmRigidBody;

class FBMuscleModel:public LoggedForce 
{ 
public:
	FBMuscleModel();
	
	FBMuscleModel(Controller * controller, CartesianVector attachmentLocal,
		    char *otherBodyName, CartesianVector attachmentOnOtherBody);
	
	virtual ~ FBMuscleModel();
	
	void computeForce(dmABForKinStruct * val, SpatialVector force);
	
	inline void reset() { m_reset_flag = true; };

	// set some internal values
	// defaults are set in the constructor
	
	// set the damping factor
	void setDampingFactor(double dampingFactor) { m_dampingFactor = dampingFactor; };

	// set the min length
	void setMinMuscleLength(double minLength) { m_minDistance = minLength; };

	// set the spring constant
	void setSpringConstant(double spring) { m_springConstant = spring; };

	// set the control factor
	void setControlFactor(double cntlFac) { m_controlFactor = cntlFac;};

	// set the control factor
	void setMaxForce(double maxForce) { m_maxForce = maxForce;};

	// set the other rigid body connected
	void setOtherBody(dmRigidBody * otherBody) { m_otherBody = otherBody; };

	// set the other rigid body connected by name
	void setOtherBodyName(char *otherBodyName);

	// set the point on other rigid body connected (in other body local coordinates)
	void setAttachmentOnOtherBody(CartesianVector attachmentOnOtherBody)
	{ 
		for (int i = 0; i < 3; i++)
			m_attachmentOnOtherBody[i] = attachmentOnOtherBody[i];
	};

	// set the local attachment point
	void setAttachmentLocal(CartesianVector attachmentLocal)
	{ 
		for (int i = 0; i < 3; i++)
			m_attachmentLocal[i] = attachmentLocal[i];
	};

	// set the muscle controller
	void setController(Controller * controller) { m_controller = controller; };

	// rendering functions (for future expansion/visualization):
	
	void draw();

private:
	// not implemented
	FBMuscleModel(const FBMuscleModel &);
	FBMuscleModel & operator = (const FBMuscleModel &);

protected:
	 bool m_reset_flag;

	double m_dampingFactor;
	double m_springConstant;
	CartesianVector m_attachmentLocal;
	CartesianVector m_attachmentOnOtherBody;
	Controller *m_controller;		 // controller
	dmRigidBody *m_otherBody;
	char *m_otherBodyName;
	double m_minDistance;			 // minimum muscle length for force generation
	double m_controlFactor;
	double m_maxForce;

	// used for drawing
	bool drawValid;
	CartesianVector m_w_localOrigin;
	CartesianVector m_w_otherOrigin;
	
	// Sensors
	Sensor m_LengthSensor;
	Sensor m_ForceSensor;
};

#endif						 //__FBMUSCLE_MODEL_H__
