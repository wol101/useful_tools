// GripModel - this class models a simple grip on the substrate.
// code modified from dmContactModel


#include <iostream.h>
#include <iomanip.h>
#include <fstream.h>
#include <assert.h>

#include <dm.h>

#include "Bar.h"
#include "Util.h"
#include "GripModel.h"
#include "Controller.h"
#include "ForceList.h"
#include "Simulation.h"
#include "LoggedForce.h"

extern Simulation *gSimulation;

//============================================================================
// class GripModel : public dmForce
//============================================================================

//----------------------------------------------------------------------------
GripModel::GripModel()
{
	m_reset_flag = true;
	m_num_contact_points = 0;
	m_contact_flag = 0;
	m_contact_pos = 0;
	m_initial_contact_pos = 0;
	m_reachDistance = 0.1;
	m_dampingFactor = 50;
	m_bar = 0;
	m_controller = 0;
	m_controlFactor = 1;

	gSimulation->GetForceList()->AddForce(this);	// add force to forcelist
}

GripModel::GripModel(Bar * bar, Controller * controller)
{
	m_reset_flag = true;
	m_num_contact_points = 0;
	m_contact_flag = 0;
	m_contact_pos = 0;
	m_initial_contact_pos = 0;
	m_reachDistance = 0.1;
	m_dampingFactor = 50;
	m_bar = bar;
	m_controller = controller;
	m_controlFactor = 1;

	gSimulation->GetForceList()->AddForce(this);	// add force to forcelist
}

//----------------------------------------------------------------------------
GripModel::~GripModel()
{
	if (m_num_contact_points)
	{
		delete[]m_contact_flag;

		delete[]m_contact_pos;
		delete[]m_initial_contact_pos;
	}
}

//----------------------------------------------------------------------------
//    Summary: set the list of contact points in local CS
// Parameters: num_contact_points - number of points to be initialized
//             contact_pts - Contact_Locations in local coordinate system
//    Returns: TRUE if operation is successful
//----------------------------------------------------------------------------
void GripModel::setContactPoints(unsigned int num_contact_points, CartesianVector * contact_pos)
{
	m_reset_flag = true;

	// delete any previous contact points
	if (m_num_contact_points)
	{
		m_num_contact_points = 0;
		delete[]m_contact_flag;
		delete[]m_contact_pos;
		delete[]m_initial_contact_pos;
	}

	if (num_contact_points)
	{
		m_num_contact_points = num_contact_points;

		m_contact_flag = new bool[m_num_contact_points];

		m_contact_pos = new CartesianVector[m_num_contact_points];
		m_initial_contact_pos = new CartesianVector[m_num_contact_points];

		for (unsigned int i = 0; i < m_num_contact_points; i++)
		{
			m_contact_flag[i] = false;

			m_contact_pos[i][0] = contact_pos[i][0];
			m_contact_pos[i][1] = contact_pos[i][1];
			m_contact_pos[i][2] = contact_pos[i][2];
		}
	}
}

//----------------------------------------------------------------------------
//    Summary: get the i-th contact point
// Parameters: 
//    Returns: TRUE if returned point is valid
//----------------------------------------------------------------------------
bool GripModel::getContactPoint(unsigned int index, CartesianVector pos) const
{
	if (index < m_num_contact_points)
	{
		pos[0] = m_contact_pos[index][0];
		pos[1] = m_contact_pos[index][1];
		pos[2] = m_contact_pos[index][2];
		return true;
	}

	return false;
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

void GripModel::computeForce(const dmABForKinStruct * val, SpatialVector f_contact)
{
	CartesianVector theNormal, current_pos;
	CartesianVector peC_pos, veC_pos, vnC_pos, fe, fn, nn;
	int j;
	double theDistance;
	double xa, ya, za;
	double xs, ys, zs;
	double gamma;
	bool inBar;
	double grip;

	for (j = 0; j < 6; j++)
	{
		f_contact[j] = 0.0;
	}

	for (unsigned int i = 0; i < m_num_contact_points; i++)
	{
		for (j = 0; j < 3; j++)		 // compute the contact pos.
		{				 // wrt ICS.

			current_pos[j] = val->p_ICS[j] +
				val->R_ICS[j][0] * m_contact_pos[i][0] +
				val->R_ICS[j][1] * m_contact_pos[i][1] +
				val->R_ICS[j][2] * m_contact_pos[i][2];
		}

		// get information on relation between contact point and bar
		inBar =
			m_bar->CalculateInformation(current_pos[0], current_pos[1],
						    current_pos[2], &theDistance, &xa, &ya,
						    &za, &gamma, &xs, &ys, &zs,
						    &(theNormal[0]), &(theNormal[1]),
						    &(theNormal[2]));

		if (
		    (m_contact_flag[i] == false && inBar) ||
		    (m_contact_flag[i] == true && theDistance < m_reachDistance + m_bar->GetRadius()))	// contact
		{
			// for scoring, tell the bar where we are
			m_bar->SetXContact(current_pos[0]);

			if (!m_contact_flag[i] || m_reset_flag)	// set initial contact Pos 
			{
				m_initial_contact_pos[i][0] = xs;
				m_initial_contact_pos[i][1] = ys;
				m_initial_contact_pos[i][2] = zs;
			}

			m_contact_flag[i] = true;

			// End-effector linear velocity and "spring" displacement wrt ICS.
			Util::CrossProduct3x1(&val->v[0], m_contact_pos[i], vnC_pos);
			// the cross product is used to calculate the linear velocity at the contact point that
			// is due to the rotational velocity of the body
			vnC_pos[0] += val->v[3];
			vnC_pos[1] += val->v[4];
			vnC_pos[2] += val->v[5];

			for (j = 0; j < 3; j++)	 // convert to ICS
			{
				veC_pos[j] = val->R_ICS[j][0] * vnC_pos[0] +
					val->R_ICS[j][1] * vnC_pos[1] +
					val->R_ICS[j][2] * vnC_pos[2];
				peC_pos[j] = current_pos[j] - m_initial_contact_pos[i][j];
			}
			// Calculate force based on displacement and velocity
			grip = ((m_controller->Calculate(gSimulation->GetTime()) + 1) / 2);
			assert(grip >= 0);
			// add in a tweak so that low grips become zero
			if (grip < 0.1)
				grip = 0;	 // grip range is 0 to 1
			grip *= m_controlFactor;

			for (j = 0; j < 3; j++)
			{
				fe[j] = -(veC_pos[j] * m_dampingFactor) - (peC_pos[j] * grip);
			}

			// Compute Contact Force at link CS
			for (j = 0; j < 3; j++)
			{
				fn[j] = val->R_ICS[0][j] * fe[0] +
					val->R_ICS[1][j] * fe[1] + val->R_ICS[2][j] * fe[2];
			}
			Util::CrossProduct3x1(m_contact_pos[i], fn, nn);

			// Accumulate for multiple contact points.
			for (j = 0; j < 3; j++)
			{
				f_contact[j] += nn[j];
				f_contact[j + 3] += fn[j];
			}

			if (logging() && i == 0)
			{

				(*mOutput) << "veC_pos\t" << veC_pos[0] << "\t" <<
					veC_pos[1] << "\t" << veC_pos[2] << "\t";
				(*mOutput) << "peC_pos\t" << peC_pos[0] << "\t" << peC_pos[1]
					<< "\t" << peC_pos[2] << "\t";
				(*mOutput) << "grip\t" << grip << "\t";

				(*mOutput) << "fe\t" << fe[0] << "\t" << fe[1] << "\t" <<
					fe[2] << "\n";
			}

		} else				 // no contact!
		{
			// Reset flags.
			m_contact_flag[i] = false;

			// Store last position
			m_initial_contact_pos[i][0] = current_pos[0];
			m_initial_contact_pos[i][1] = current_pos[1];
			m_initial_contact_pos[i][2] = current_pos[2];
		}
	}
	m_reset_flag = false;
}

// draw thyself!
void GripModel::draw() const
{

}
