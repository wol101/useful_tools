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

#ifdef USE_OPENGL
#include <gl.h>
#endif

#include <dmEnvironment.hpp>

#include "ModifiedContactModel.h"
#include "DebugControl.h"

extern int gAxisFlag;        // puts axes on each element
extern float gAxisSize;        // size of axis elements
extern int gDrawForces;
extern float gForceLineScale; // multiply the force by this before drawing vector

//============================================================================
// class ModifiedContactModel : public dmForce
//============================================================================

//----------------------------------------------------------------------------
ModifiedContactModel::ModifiedContactModel():
  dmContactModel()
{
  m_drawValid = false;
}

//----------------------------------------------------------------------------
ModifiedContactModel::~ModifiedContactModel()
{
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
void ModifiedContactModel::computeForce(const dmABForKinStruct &val,
                                  SpatialVector f_contact)
{
  int j;
  
  dmContactModel::computeForce(val, f_contact);

  m_drawValid = true;
  
  if (gDebug == ContactDebug)
  {
    SpatialVector world;
    for (j = 0; j < 3; j++)
    {
      world[j] = // not sure this part is correct
        val.R_ICS[j][0] * f_contact[0] +
        val.R_ICS[j][1] * f_contact[1] +
        val.R_ICS[j][2] * f_contact[2];

      world[j + 3] = 
        val.R_ICS[j][0] * f_contact[0 + 3] +
        val.R_ICS[j][1] * f_contact[1 + 3] +
        val.R_ICS[j][2] * f_contact[2 + 3];
    }
    *gDebugStream << "ModifiedContactModel::computeForce" << "\t";
    *gDebugStream << m_name << "\tWorld\t";
    for (j = 0; j < 6; j++)
      *gDebugStream << world[j] << "\t";
    *gDebugStream << "Local\t";
    for (j = 0; j < 6; j++)
      *gDebugStream << f_contact[j] << "\t";
    *gDebugStream << "\n";
  }
}

// draw thyself!
// this isn't optimised yet - array calculations are done locally which is
// potentially very slow.

void ModifiedContactModel::draw() const
{
#ifdef USE_OPENGL    
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
        
        glEnable(GL_LIGHTING);
      }
      
      if (gDrawForces)
      {     
        glDisable(GL_LIGHTING);
        
        glBegin(GL_LINES);
        glColor3f(0.8, 0.8, 0.8);
        glVertex3f(m_world_contact_pos[i][0], 
            m_world_contact_pos[i][1], m_world_contact_pos[i][2]);
        glVertex3f(m_world_contact_pos[i][0] + m_world_force[i][3] * gForceLineScale, 
            m_world_contact_pos[i][1] + m_world_force[i][4] * gForceLineScale, 
            m_world_contact_pos[i][2] + m_world_force[i][5] * gForceLineScale);
        glEnd();

        glEnable(GL_LIGHTING);
      }
    }
  }
#endif
}
