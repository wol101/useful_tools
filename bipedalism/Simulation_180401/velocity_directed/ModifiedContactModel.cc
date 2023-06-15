// wis - had to modify this to allow the right sort of reporting for my
// fitness function

/*****************************************************************************
 * Copyright 1999, Scott McMillan
 *****************************************************************************
 *     File: ModifiedContactModel.cpp
 *   Author: Scott McMillan
 *  Project: DynaMechs 3.0
 *  Summary: Class declaration for contact modelling (contacts with terrain).
 *         : Used to be class EndEffector, but now is a component of RigidBody
 *         : so that contact forces on each body can be computed.
 *****************************************************************************/

#include <dm.h>
#include <dmForce.h>
#include <dmEnvironment.h>

#include "ForceList.h"
#include "Simulation.h"
#include "ModifiedContactModel.h"

extern Simulation *gSimulation;
extern int gDebug;
extern bool gAxisFlag;        // puts axes on each element
extern float gAxisSize;        // size of axis elements

double kForceLineScale = 0.0001; // multiply the force by this before drawing vector

//============================================================================
// class ModifiedContactModel : public dmForce
//============================================================================

//----------------------------------------------------------------------------
ModifiedContactModel::ModifiedContactModel():
  LoggedForce(),
  m_reset_flag(true),
  m_num_contact_points(0),
  m_contact_flag(NULL), 
  m_sliding_flag(NULL), 
  m_contact_pos(NULL), 
  m_initial_contact_pos(NULL)
{
  m_drawValid = false;
  m_world_contact_pos = 0;
  m_world_force = 0;
  gSimulation->GetForceList()->AddForce(this);  // add force to forcelist
}

//----------------------------------------------------------------------------
ModifiedContactModel::~ModifiedContactModel()
{
  if (m_num_contact_points)
  {
    delete[]m_contact_flag;
    delete[]m_sliding_flag;

    delete[]m_contact_pos;
    delete[]m_initial_contact_pos;
    
    delete [] m_world_contact_pos;
    delete [] m_world_force;
  }
}

//----------------------------------------------------------------------------
//    Summary: set the list of contact points in local CS
// Parameters: num_contact_points - number of points to be initialized
//             contact_pts - Contact_Locations in local coordinate system
//    Returns: TRUE if operation is successful
//----------------------------------------------------------------------------
void
ModifiedContactModel::setContactPoints(unsigned int num_contact_points,
           CartesianVector * contact_pos)
{
  m_reset_flag = true;

  // delete any previous contact points
  if (m_num_contact_points)
  {
    m_num_contact_points = 0;
    delete [] m_contact_flag;
    delete [] m_sliding_flag;
    delete [] m_contact_pos;
    delete [] m_initial_contact_pos;
    delete [] m_world_contact_pos;
    delete [] m_world_force;
  }

  if (num_contact_points)
  {
    m_num_contact_points = num_contact_points;

    /* FIXME - need to error check this allocation */
    m_contact_flag = new bool[m_num_contact_points];
    m_sliding_flag = new bool[m_num_contact_points];

    m_contact_pos = new CartesianVector[m_num_contact_points];
    m_initial_contact_pos = new CartesianVector[m_num_contact_points];
    m_world_contact_pos = new CartesianVector[m_num_contact_points];
    m_world_force = new SpatialVector[m_num_contact_points];

    for (unsigned int i = 0; i < m_num_contact_points; i++)
    {
      m_contact_flag[i] = false;
      m_sliding_flag[i] = false;

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
bool ModifiedContactModel::getContactPoint(unsigned int index, CartesianVector pos) const
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
void ModifiedContactModel::computeForce(dmABForKinStruct * val, SpatialVector f_contact)
{
  register int j;
  double ground_elevation;

  m_drawValid = true;

  for (j = 0; j < 6; j++)
  {
    f_contact[j] = 0.0;
  }
  if (dmEnvironment::getEnvironment() == NULL)
  {
    return;
  }

  for (unsigned int i = 0; i < m_num_contact_points; i++)
  {
    for (j = 0; j < 3; j++)     // compute the contact pos.
    {         // wrt ICS.

      current_pos[j] = val->p_ICS[j] +
        val->R_ICS[j][XC] * m_contact_pos[i][XC] +
        val->R_ICS[j][YC] * m_contact_pos[i][YC] +
        val->R_ICS[j][ZC] * m_contact_pos[i][ZC];

      // store for use drawing
      m_world_contact_pos[i][j] = current_pos[j];
    }
        
    ground_elevation =
      (dmEnvironment::getEnvironment())->getGroundElevation(current_pos, normal);

    if (current_pos[ZC] > ground_elevation)  // No contact
    {
      // Reset flags.
      m_contact_flag[i] = false;
      m_sliding_flag[i] = false;

      // Store last position
      m_initial_contact_pos[i][XC] = current_pos[XC];
      m_initial_contact_pos[i][YC] = current_pos[YC];
      m_initial_contact_pos[i][ZC] = current_pos[ZC];
      
      // calculate world values for drawing
      for (j = 0; j < 6; j++)
        m_world_force[i][j] = 0;
      
    } 
    else         // Contact!
    {
      if (!m_contact_flag[i] || m_reset_flag)  // set initial contact Pos.
      {
#ifdef DEBUG
        cout << "Contact " << flush;
#endif
        m_initial_contact_pos[i][XC] = current_pos[XC];
        m_initial_contact_pos[i][YC] = current_pos[YC];
        m_initial_contact_pos[i][ZC] = ground_elevation;
      }

      m_contact_flag[i] = true;

      // End-effector linear velocity and "spring" displacement wrt ICS.
      crossproduct(&val->v[0], m_contact_pos[i], vnC_pos);
      vnC_pos[XC] += val->v[3];
      vnC_pos[YC] += val->v[4];
      vnC_pos[ZC] += val->v[5];

      for (j = 0; j < 3; j++)
      {
        veC_pos[j] = val->R_ICS[j][XC] * vnC_pos[XC] +
          val->R_ICS[j][YC] * vnC_pos[YC] +
          val->R_ICS[j][ZC] * vnC_pos[ZC];
        peC_pos[j] = current_pos[j] - m_initial_contact_pos[i][j];
      }

      // Magnitudes of normal components of velocity and delta position.
      vtemp = veC_pos[XC] * normal[XC] +
        veC_pos[YC] * normal[YC] + veC_pos[ZC] * normal[ZC];
      ptemp = peC_pos[XC] * normal[XC] +
        peC_pos[YC] * normal[YC] + peC_pos[ZC] * normal[ZC];

      // Magnitude of Normal force.
      fe_normal_mag_damping = -(dmEnvironment::getEnvironment())->
        getGroundNormalDamperConstant() * vtemp;
      fe_normal_mag_spring = -(dmEnvironment::getEnvironment())->
          getGroundNormalSpringConstant() * ptemp;

      if (fe_normal_mag_spring < 0.0) 
        fe_normal_mag_spring = 0.0; // spring force can't suck into ground
      
      fe_normal_mag = fe_normal_mag_damping + fe_normal_mag_spring;

      for (j = 0; j < 3; j++)
      {
        fe_normal[j] = normal[j] * fe_normal_mag;
      }

      // Planar forces for sticking contact.
      for (j = 0; j < 3; j++)
      {
        v_planar[j] = veC_pos[j] - normal[j] * vtemp;
        p_planar[j] = peC_pos[j] - normal[j] * ptemp;
        fe_planar[j] =
          -(dmEnvironment::getEnvironment())->
          getGroundPlanarDamperConstant() * v_planar[j] -
          (dmEnvironment::getEnvironment())->
          getGroundPlanarSpringConstant() * p_planar[j];
      }
      fe_planar_mag = sqrt(fe_planar[XC] * fe_planar[XC] +
               fe_planar[YC] * fe_planar[YC] +
               fe_planar[ZC] * fe_planar[ZC]);

      // Check to see whether it should start sticking.
/*        if (m_sliding_flag[i])
      {
        // if sliding, is it going slow enough to stick.
        //double speedSquared = (v_planar[XC]*v_planar[XC] + 
        //                      v_planar[YC]*v_planar[YC] + 
        //                      v_planar[ZC]*v_planar[ZC]);

        // Stick if speed less than a threshhold or
        // static planar force is less than the kinetic one.
        if (   //(speedSquared < 0.005) || 
             (fe_planar_mag < (fe_normal_mag *
                   (dmEnvironment::getEnvironment
                    ())->
                   getGroundKineticFrictionCoeff
                   ())))
        {
          m_sliding_flag[i] = false;
        }
      }
      // Check to see whether it should start sliding.
      else
      {
        if (fe_planar_mag > (fe_normal_mag *
                 (dmEnvironment::getEnvironment
                  ())->getGroundStaticFrictionCoeff()))
        {
          //slippage!
          m_sliding_flag[i] = true;
        }
      }

      // if sliding recompute a smaller planar force
      if (m_sliding_flag[i])
      {
        temp = (fe_normal_mag / fe_planar_mag) *
          (dmEnvironment::getEnvironment())->
          getGroundKineticFrictionCoeff();
        fe_planar[XC] *= temp;
        fe_planar[YC] *= temp;
        fe_planar[ZC] *= temp;

        m_initial_contact_pos[i][XC] = current_pos[XC];
        m_initial_contact_pos[i][YC] = current_pos[YC];
        m_initial_contact_pos[i][ZC] = ground_elevation;
      }*/
      // Add normal and planar forces.
      for (j = 0; j < 3; j++)
      {
        fe[j] = fe_normal[j] + fe_planar[j];
      }

      // Compute Contact Force at link CS
      for (j = 0; j < 3; j++)
      {
        fn[j] = val->R_ICS[XC][j] * fe[XC] +
          val->R_ICS[YC][j] * fe[YC] + val->R_ICS[ZC][j] * fe[ZC];
      }
      crossproduct(m_contact_pos[i], fn, nn);

      // Accumulate for multiple contact points.
      for (j = 0; j < 3; j++)
      {
        f_contact[j] += nn[j];
        f_contact[j + 3] += fn[j];
        m_contactForce[j] += nn[j];
        m_contactForce[j + 3] += fn[j];
      }
      
      // calculate world values for drawing
      for (j = 0; j < 3; j++)
      {
        m_world_force[i][j] = // not sure this part is correct
          val->R_ICS[j][XC] * nn[XC] +
          val->R_ICS[j][YC] * nn[YC] +
          val->R_ICS[j][ZC] * nn[ZC];

        m_world_force[i][j + 3] = 
          val->R_ICS[j][XC] * fn[XC] +
          val->R_ICS[j][YC] * fn[YC] +
          val->R_ICS[j][ZC] * fn[ZC];
      }
      
    }
    
  }
  
  if (gDebug > 0)
  {
    SpatialVector world;
      for (j = 0; j < 3; j++)
      {
        world[j] = // not sure this part is correct
          val->R_ICS[j][XC] * m_contactForce[XC] +
          val->R_ICS[j][YC] * m_contactForce[YC] +
          val->R_ICS[j][ZC] * m_contactForce[ZC];

        world[j + 3] = 
          val->R_ICS[j][XC] * m_contactForce[XC + 3] +
          val->R_ICS[j][YC] * m_contactForce[YC + 3] +
          val->R_ICS[j][ZC] * m_contactForce[ZC + 3];
      }
    cerr << "ModifiedContactModel::computeForce" << "\t";
    cerr << m_name << "\tWorld\t";
    for (j = 0; j < 6; j++)
      cerr << world[j] << "\t";
    cerr << "Local\t";
    for (j = 0; j < 6; j++)
      cerr << m_contactForce[j] << "\t";
    cerr << "\n";
  }

  m_reset_flag = false;
}

bool ModifiedContactModel::AnyContact(void)
{
  for (int i = 0; i < m_num_contact_points; i++)
  {
    if (m_contact_flag[i]) return true;
  }
  
  return false;
}

void ModifiedContactModel::GetContactForce(SpatialVector f)
{
  for (int i = 0; i < 6; i++)
  {
    f[i] = m_contactForce[i];
  }
}

// draw thyself!
// this isn't optimised yet - array calculations are done locally which is
// potentially very slow.

void ModifiedContactModel::draw()
{
  if (m_drawValid)
  {
    for (unsigned int i = 0; i < m_num_contact_points; i++)
    {
      if (gAxisFlag)
      {
        glDisable(GL_LIGHTING);

        glBegin(GL_LINES);
        glColor3f(1.0, 0.0, 0.0);
        glVertex3f(gAxisSize + m_world_contact_pos[i][0], 
            m_world_contact_pos[i][1], m_world_contact_pos[i][2]);
        glVertex3f(m_world_contact_pos[i][0], 
            m_world_contact_pos[i][1], m_world_contact_pos[i][2]);
        glEnd();

        glBegin(GL_LINES);
        glColor3f(0.0, 1.0, 0.0);
        glVertex3f(m_world_contact_pos[i][0], gAxisSize +
            m_world_contact_pos[i][1], m_world_contact_pos[i][2]);
        glVertex3f(m_world_contact_pos[i][0], 
            m_world_contact_pos[i][1], m_world_contact_pos[i][2]);
        glEnd();

        glBegin(GL_LINES);
        glColor3f(0.0, 0.0, 1.0);
        glVertex3f(m_world_contact_pos[i][0], 
            m_world_contact_pos[i][1], gAxisSize + m_world_contact_pos[i][2]);
        glVertex3f(m_world_contact_pos[i][0], 
            m_world_contact_pos[i][1], m_world_contact_pos[i][2]);
        glEnd();

        glBegin(GL_LINES);
        glColor3f(0.8, 0.8, 0.8);
        glVertex3f(m_world_contact_pos[i][0], 
            m_world_contact_pos[i][1], m_world_contact_pos[i][2]);
        glVertex3f(m_world_contact_pos[i][0] + m_world_force[i][3] * kForceLineScale, 
            m_world_contact_pos[i][1] + m_world_force[i][4] * kForceLineScale, 
            m_world_contact_pos[i][2] + m_world_force[i][5] * kForceLineScale);
        glEnd();

        glEnable(GL_LIGHTING);
      }
    }
  }
}
