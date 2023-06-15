// StrapForce.cc

// this routine provides a force that acts on a number of bodies
// via a strap. The strap has an origin and insertion and is tethered
// by a number of Anchors.

#include <gl.h>
#include <assert.h>
#include <dmLink.hpp>
#include <dmArticulation.hpp>

#include "StrapForce.h"
#include "Util.h"
#include "Simulation.h"
#include "DebugControl.h"

extern Simulation *gSimulation;

extern int gDrawMuscles;

// constructor
StrapForce::StrapForce()
{
   m_Tension = 0;
   m_Length = 0; // flag 
   m_LastLength = 0;
   m_Velocity = 0;
   m_Valid = false;
}

// destructor
StrapForce::~StrapForce()
{
}

// set the number of anchor points (only allowed once)
void 
StrapForce::SetNumAnchors(unsigned int numAnchors)
{
   StrapAnchorLocation theAnchorLocation;
   unsigned int theSize = m_AnchorList.size();
   assert(theSize == 0);
   
   for (unsigned int i = 0; i < numAnchors; i++)
      m_AnchorList.push_back(theAnchorLocation);
}   

// assign values to the anchor points
// NB. does not set the world positions
// probably should only be called from StrapForceAnchor
void 
StrapForce::SetAnchor(dmLink *link,
   unsigned int anchorID, CartesianVector location)
{
   unsigned int theSize = m_AnchorList.size();
   assert(anchorID < theSize);
   
   Util::Copy3x1(location, m_AnchorList[anchorID].localPosition);
   m_AnchorList[anchorID].link = link;
}

// calculate the world positions for all the anchor locations
void 
StrapForce::UpdateWorldPositions()
{
   unsigned int i;
   unsigned int theSize = m_AnchorList.size();
   assert(theSize > 1);

   for (i = 0; i < theSize; i++)
   {
      ConvertLocalToWorldP(m_AnchorList[i].link, 
         m_AnchorList[i].localPosition, m_AnchorList[i].worldPosition);

      if (gDebug == StrapForceDebug)
      {
         cerr << "StrapForce::UpdateWorldPositions\t" << m_name <<
            "\tm_AnchorList[" << i << "].localPosition\t"
            << m_AnchorList[i].localPosition[0] << "\t" 
            << m_AnchorList[i].localPosition[1] << "\t" 
            << m_AnchorList[i].localPosition[2] <<  
            "\tm_AnchorList[" << i << "].worldPosition\t"
            << m_AnchorList[i].worldPosition[0] << "\t" 
            << m_AnchorList[i].worldPosition[1] << "\t" 
            << m_AnchorList[i].worldPosition[2] << "\n"; 
      }
   }
   
   if (m_Valid)
   {
     m_LastLength = m_Length;
     CalculateLength();
     m_Velocity = (m_Length - m_LastLength) / gSimulation->GetTimeIncrement();
   }
   else
   {
      // first time through
      m_Valid = true;
      CalculateLength();
      m_LastLength = m_Length;
      m_Velocity = 0;
   }
}
    
  
// calculate the length
void  
StrapForce::CalculateLength()
{
   unsigned int i;
   unsigned int theSize = m_AnchorList.size();
  
   assert(theSize >= 2);
   assert(m_Valid);
     
   m_Length = 0;
   for (i = 1; i < theSize; i++)
   {
      m_Length += Util::Distance3x1(m_AnchorList[i - 1].worldPosition,
         m_AnchorList[i].worldPosition);
   }
  
   if (gDebug == StrapForceDebug)
   {
      cerr << "StrapForce::CalculateLength\t" << m_name << "\tm_Length\t" <<
         m_Length << "\n";
   }
         
}
  
// convert a link local coordinate to a world coordinate

void 
StrapForce::ConvertLocalToWorldP(dmLink *link, 
    const CartesianVector local, CartesianVector world)
{
  // get positional data for the link
  unsigned int linkIndex = gSimulation->GetRobot()->getLinkIndex(link);
  const dmABForKinStruct *m = gSimulation->GetRobot()->getForKinStruct(linkIndex);
  
  for (int j = 0; j < 3; j++)
  {

    world[j] = m->p_ICS[j] +
      m->R_ICS[j][0] * local[0] +
      m->R_ICS[j][1] * local[1] +
      m->R_ICS[j][2] * local[2];
  }
}

// set the strap tension
void 
StrapForce::SetTension(double tension) 
{ 
   m_Tension = tension; 
   if (gDebug == StrapForceDebug)
   {
      cerr << "StrapForce::SetTension\t" << m_name 
         << "\tm_Tension\t" << m_Tension << "\n";
   }
}

// get the anchor location
void 
StrapForce::GetAnchorLocation(unsigned int anchor, 
   CartesianVector localPosition)
{
   unsigned int theSize = m_AnchorList.size();
   assert(anchor < theSize);
   
   Util::Copy3x1(m_AnchorList[anchor].localPosition, localPosition);
}

// get the anchor location (world coordinates)
void 
StrapForce::GetAnchorWorldLocation(unsigned int anchor, 
   CartesianVector worldPosition)
{
   unsigned int theSize = m_AnchorList.size();
   assert(anchor < theSize);
   
   Util::Copy3x1(m_AnchorList[anchor].worldPosition, worldPosition);
}

// set the anchor location
void 
StrapForce::SetAnchorLocation(unsigned int anchor, 
   CartesianVector localPosition)
{
   unsigned int theSize = m_AnchorList.size();
   assert(anchor < theSize);
   
   Util::Copy3x1(localPosition, m_AnchorList[anchor].localPosition);
}

// calculate the force vector at an Anchor (world coordinates)

void 
StrapForce::GetAnchorForce(unsigned int anchor, CartesianVector force)
{
   unsigned int theSize = m_AnchorList.size();
   CartesianVector v;
   CartesianVector v1, v2;
  
   assert(anchor < theSize);
   
   if (!m_Valid) UpdateWorldPositions();
  
   // check for origin
   if (anchor == 0)
   {
      // calculate the direction
      Util::Subtract3x1(m_AnchorList[anchor + 1].worldPosition, m_AnchorList[anchor].worldPosition, v);
      Util::Unit3x1(v);
      // multiply the unit direction vector by the tension
      Util::ScalarMultiply3x1(m_Tension, v, force);
      if (gDebug == StrapForceDebug)
      {
         cerr << "StrapForce::GetAnchorForce\t" << m_name 
            << "\tanchor\t" << anchor
            << "\tforce\t" << force[0]
            << "\t" << force[1]
            << "\t" << force[2] << "\n";
      }
      return;
   } 
    
   // check for insertion
   if (anchor == theSize - 1)
   {
      // calculate the direction
      Util::Subtract3x1(m_AnchorList[anchor - 1].worldPosition, m_AnchorList[anchor].worldPosition, v);
      Util::Unit3x1(v);
      // multiply the unit direction vector by the tension
      Util::ScalarMultiply3x1(m_Tension, v, force);
      if (gDebug == StrapForceDebug)
      {
         cerr << "StrapForce::GetAnchorForce\t" << m_name 
            << "\tanchor\t" << anchor
            << "\tforce\t" << force[0]
            << "\t" << force[1]
            << "\t" << force[2] << "\n";
      }
      return;
   }  
  
   // normal mid anchor
   // calculate the two pull vectors v1 and v2

   Util::Subtract3x1(m_AnchorList[anchor - 1].worldPosition, m_AnchorList[anchor].worldPosition, v1);
   Util::Unit3x1(v1);
   Util::Subtract3x1(m_AnchorList[anchor + 1].worldPosition, m_AnchorList[anchor].worldPosition, v2);
   Util::Unit3x1(v2);

   // and calculate actual force
   Util::Add3x1(v1, v2, v);
   Util::ScalarMultiply3x1(m_Tension, v, force);
   if (gDebug == StrapForceDebug)
   {
      cerr << "StrapForce::GetAnchorForce\t" << m_name 
         << "\tanchor\t" << anchor
         << "\tforce\t" << force[0]
         << "\t" << force[1]
         << "\t" << force[2] << "\n";
   }
   return;
}

// draw the strap force
void
StrapForce::Draw() 
{
   // draw strap force as a single line
   
   unsigned int theSize = m_AnchorList.size();
   if (theSize < 2) return;

   if (gDrawMuscles)
   {
      glDisable(GL_LIGHTING);

      // FIX ME
      // this should be implemented as polylines not single line segments
      // but it's a fairly minor inefficiency

      for (unsigned int i = 1; i < theSize; i++)
      {
         glBegin(GL_LINES);
         glColor3f(1.0, 0.0, 0.0);
         glVertex3f(m_AnchorList[i - 1].worldPosition[0], 
            m_AnchorList[i - 1].worldPosition[1], m_AnchorList[i - 1].worldPosition[2]);
         glVertex3f(m_AnchorList[i].worldPosition[0], 
            m_AnchorList[i].worldPosition[1], m_AnchorList[i].worldPosition[2]);
         glEnd();
      }

      glEnable(GL_LIGHTING);
   }

}
