// MuscleModel - a simple linear muscle model


#include <dm.h>
#include <dmu.h>
#include <dmForce.h>
#include <dmArticulation.h>
#include <gl.h>

#include <assert.h>

#include "MuscleModel.h"
#include "Controller.h"
#include "Util.h"
#include "ForceList.h"
#include "Simulation.h"

extern Simulation *gSimulation;

//============================================================================
// class MuscleModel : public dmForce
//============================================================================

//----------------------------------------------------------------------------
MuscleModel::MuscleModel()
{
	m_reset_flag = true;
	m_dampingFactor = 50;
	m_controller = 0;
	m_otherBody = 0;
	m_minDistance = 0;
	m_springConstant = 0;
	m_controlFactor = 1;
	for (int i = 0; i < 3; i++)
	{
		m_attachmentLocal[i] = 0;
		m_attachmentOnOtherBody[i] = 0;
	}
	m_otherBody = 0;
	m_otherBodyName = 0;
	drawValid = false;

	gSimulation->GetForceList()->AddForce(this);	// add force to forcelist
}

MuscleModel::MuscleModel(Controller * controller,
			 CartesianVector attachmentLocal,
			 char *otherBodyName, CartesianVector attachmentOnOtherBody)
{
	m_reset_flag = true;
	m_dampingFactor = 50;
	m_controller = controller;
	m_minDistance = 0;
	m_springConstant = 0;
	m_controlFactor = 1;
	for (int i = 0; i < 3; i++)
	{
		m_attachmentLocal[i] = attachmentLocal[i];
		m_attachmentOnOtherBody[i] = attachmentOnOtherBody[i];
	}
	m_otherBodyName = new char[strlen(otherBodyName) + 1];
	strcpy(m_otherBodyName, otherBodyName);
	m_otherBody = 0;			 // need to delay this assigment - part may not exist yet
	drawValid = false;

	gSimulation->GetForceList()->AddForce(this);	// add force to forcelist
}

//----------------------------------------------------------------------------
MuscleModel::~MuscleModel()
{
	if (m_otherBodyName)
		delete[]m_otherBodyName;
}

//----------------------------------------------------------------------------
void MuscleModel::setOtherBodyName(char *otherBodyName)
{
	if (m_otherBodyName)
		delete[]m_otherBodyName;
	m_otherBodyName = new char[strlen(otherBodyName) + 1];
	strcpy(m_otherBodyName, otherBodyName);
	m_otherBody = 0;			 // need to delay this assigment - part may not exist yet
}

//----------------------------------------------------------------------------
//    Summary: Compute the external force due to contact with terrain stored in
//             a dmEnvironment object
// Parameters: val - struct containing position and orientation of the rigid
//                   body wrt the inertial CS, and the spatial velocity
//                   with respect to the body's CS.
//    Returns: f_contact - spatial contact force exerted on the body wrt to the
//                   body's CS
//----------------------------------------------------------------------------

// wis - note contents of f_contact are three axial torques followed by three 
// linear forces (or at least that's what they seem to be)

// this routine is triggered by a contact point within the bar volume
// the surface position is calculated and this is used to tether a damped spring
// whose strength is given by the grip strength variable obtained from the gibbon
// controller

void MuscleModel::computeForce(dmABForKinStruct * val, SpatialVector f_contact)
{
	int j;
	CartesianVector w_localOrigin;
	CartesianVector w_otherOrigin;
	CartesianVector localVelocity;
	CartesianVector otherVelocity;
	CartesianVector w_localVelocity;
	CartesianVector w_otherVelocity;
	const dmABForKinStruct *other_dmABForKinStruct;
	unsigned int linkIndex;
	CartesianVector distanceVector;
	CartesianVector normalisedDistanceVector;
	CartesianVector velocityVector;
	double speed;
	double theDistance;
	CartesianVector localForceVector;
	CartesianVector localTorque;
	double speedForce;
	double muscleForce;
	CartesianVector forceVector;
	double control;

	// check whether we need to resolve m_otherBody from it's name
	if (m_otherBody == 0)
		m_otherBody =
			(dmRigidBody *) dmuFindObject(m_otherBodyName, gSimulation->GetRobot());
	assert(m_otherBody);

	for (j = 0; j < 6; j++)
	{
		f_contact[j] = 0.0;
	}

	// magnitude of force depends on length and rate of change of length
	// get locations first

	// get positional data for the other link
	linkIndex = gSimulation->GetRobot()->getLinkIndex((dmLink *) m_otherBody);
	other_dmABForKinStruct = gSimulation->GetRobot()->getForKinStruct(linkIndex);

	// Compute local attachment point wrt ICS

	for (j = 0; j < 3; j++)
	{

		w_localOrigin[j] = val->p_ICS[j] +
			val->R_ICS[j][XC] * m_attachmentLocal[XC] +
			val->R_ICS[j][YC] * m_attachmentLocal[YC] +
			val->R_ICS[j][ZC] * m_attachmentLocal[ZC];
	}

	// and do the same for the attachment on the other body

	for (j = 0; j < 3; j++)
	{

		w_otherOrigin[j] = other_dmABForKinStruct->p_ICS[j] +
			other_dmABForKinStruct->R_ICS[j][XC] * m_attachmentOnOtherBody[XC] +
			other_dmABForKinStruct->R_ICS[j][YC] * m_attachmentOnOtherBody[YC] +
			other_dmABForKinStruct->R_ICS[j][ZC] * m_attachmentOnOtherBody[ZC];
	}

	// then velocities wrt ICS

	Util::CrossProduct3x1(&val->v[0], m_attachmentLocal, localVelocity);
	// the cross product is used to calculate the linear velocity at the contact point that
	// is due to the rotational velocity of the body
	localVelocity[XC] += val->v[3];
	localVelocity[YC] += val->v[4];
	localVelocity[ZC] += val->v[5];

	Util::CrossProduct3x1(&other_dmABForKinStruct->v[0],
			      m_attachmentOnOtherBody, otherVelocity);
	// the cross product is used to calculate the linear velocity at the contact point that
	// is due to the rotational velocity of the body
	otherVelocity[XC] += other_dmABForKinStruct->v[3];
	otherVelocity[YC] += other_dmABForKinStruct->v[4];
	otherVelocity[ZC] += other_dmABForKinStruct->v[5];

	// and rotate the velocites to world coordinates
	for (j = 0; j < 3; j++)
	{

		w_localVelocity[j] =
			val->R_ICS[j][XC] * localVelocity[XC] +
			val->R_ICS[j][YC] * localVelocity[YC] +
			val->R_ICS[j][ZC] * localVelocity[ZC];
	}

	// and do the same for the attachment on the other body

	for (j = 0; j < 3; j++)
	{

		w_otherVelocity[j] =
			other_dmABForKinStruct->R_ICS[j][XC] * otherVelocity[XC] +
			other_dmABForKinStruct->R_ICS[j][YC] * otherVelocity[YC] +
			other_dmABForKinStruct->R_ICS[j][ZC] * otherVelocity[ZC];
	}


	// and calculate the vectors from local to other
	for (j = 0; j < 3; j++)
	{
		distanceVector[j] = w_otherOrigin[j] - w_localOrigin[j];
		velocityVector[j] = w_otherVelocity[j] - w_localVelocity[j];
	}

	// find magnitude of velocity between the two points
	theDistance = Util::Magnitude3x1(distanceVector);
	for (j = 0; j < 3; j++)
		normalisedDistanceVector[j] = distanceVector[j] / theDistance;
	speed = Util::DotProduct3x1(normalisedDistanceVector, velocityVector);	// positive if moving apart

	// and calculate forces
	control = (m_controller->Calculate(gSimulation->GetTime()) + 1) / 2;
	// assert(control >= 0);
	// FIX ME - I'm fairly sure this shouldn't actually happed but it doess
	if (control < 0)
	{
		// cerr << "Control = " << control << " set to zero\n";
		control = 0;
	}

	speedForce = m_dampingFactor * speed;

	// can't seem to get damping to work (unstable) - fix it later
	// actually it's becuase I'm not calculating the relative velocity correctly

	muscleForce =
		control * m_controlFactor + m_springConstant * (theDistance -
								m_minDistance) + speedForce;
	if (muscleForce < 0)
		muscleForce = 0;		 // muscles can't push

	// and calculate instantaneous power (note can be negative if muscle stretching under load)
	m_power = muscleForce * speed * -1;

	// calculate force vector wrt ICS
	for (j = 0; j < 3; j++)
		forceVector[j] = normalisedDistanceVector[j] * muscleForce;

	// Compute Force wrt link CS
	for (j = 0; j < 3; j++)
	{
		localForceVector[j] = val->R_ICS[XC][j] * forceVector[XC] +
			val->R_ICS[YC][j] * forceVector[YC] + val->R_ICS[ZC][j] * forceVector[ZC];
	}
	// and local torque
	Util::CrossProduct3x1(m_attachmentLocal, localForceVector, localTorque);

	// and build the return force array
	for (j = 0; j < 3; j++)
	{
		f_contact[j] += localTorque[j];
		f_contact[j + 3] += localForceVector[j];
	}

	m_reset_flag = false;			 // not used

	if (logging())
	{
		(*mOutput) << "speed\t" << speed << "\ttheDistance\t" << theDistance << "\t";

		(*mOutput) << "distanceVector\t" << distanceVector[XC] << "\t" <<
			distanceVector[YC] << "\t" << distanceVector[ZC] << "\t";
		(*mOutput) << "w_localOrigin\t" << w_localOrigin[XC] << "\t" <<
			w_localOrigin[YC] << "\t" << w_localOrigin[ZC] << "\t";
		(*mOutput) << "w_otherOrigin\t" << w_otherOrigin[XC] << "\t" <<
			w_otherOrigin[YC] << "\t" << w_otherOrigin[ZC] << "\t";
		(*mOutput) << "localVelocity\t" << localVelocity[XC] << "\t" <<
			localVelocity[YC] << "\t" << localVelocity[ZC] << "\t";
		(*mOutput) << "otherVelocity\t" << otherVelocity[XC] << "\t" <<
			otherVelocity[YC] << "\t" << otherVelocity[ZC] << "\t";
		(*mOutput) << "w_localVelocity\t" << w_localVelocity[XC] << "\t" <<
			w_localVelocity[YC] << "\t" << w_localVelocity[ZC] << "\t";
		(*mOutput) << "w_otherVelocity\t" << w_otherVelocity[XC] << "\t" <<
			w_otherVelocity[YC] << "\t" << w_otherVelocity[ZC] << "\t";
		(*mOutput) << "forceVector\t" << forceVector[XC] << "\t" <<
			forceVector[YC] << "\t" << forceVector[ZC] << "\n";
	}
	// set the member variables so the origin and insertion can be used elsewhere
	for (j = 0; j < 3; j++)
	{
		m_w_localOrigin[j] = w_localOrigin[j];
		m_w_otherOrigin[j] = w_otherOrigin[j];
	}
	drawValid = true;
}

// draw thyself!
// this isn't optimised yet - array calculations are done locally which is
// potentially very slow.

void MuscleModel::draw()
{
	// draw muscle as a single line (using data stored in computeForce)

	if (drawValid)
	{
		glDisable(GL_LIGHTING);

		glBegin(GL_LINES);
		glColor3f(1.0, 0.0, 0.0);
		glVertex3f(m_w_localOrigin[0], m_w_localOrigin[1], m_w_localOrigin[2]);
		glVertex3f(m_w_otherOrigin[0], m_w_otherOrigin[1], m_w_otherOrigin[2]);
		glEnd();

		glEnable(GL_LIGHTING);
	}

}
