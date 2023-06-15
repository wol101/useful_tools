// StrapForce.cc

// this routine provides a force that acts on a number of bodies
// via a strap. The strap has an origin and insertion and is tethered
// by a number of Anchors.

#include <assert.h>
#include <dmLink.h>
#include <dmArticulation.h>

#include "StrapForce.h"
#include "Util.h"
#include "Simulation.h"
#include "DebugControl.h"

extern Simulation *gSimulation;

// constructor
StrapForce::StrapForce()
{
   m_Tension = 0;
   m_Length = 0;
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

   for (i = 0; i < theSize; i++)
   {
      ConvertLocalToWorldP(m_AnchorList[i].link, 
         m_AnchorList[i].localPosition, m_AnchorList[i].worldPosition);

      if (gDebug == StrapForceDebug)
         cerr << "StrapForce::UpdateWorldPositions\t" << m_name 
            << "\ti\t" << i << "\tm_AnchorList[i].localPosition\t"
            << m_AnchorList[i].localPosition[0] << "\t" 
            << m_AnchorList[i].localPosition[1] << "\t" 
            << m_AnchorList[i].localPosition[2] << "\t" 
            << "\tm_AnchorList[i].worldPosition\t"
            << m_AnchorList[i].worldPosition[0] << "\t" 
            << m_AnchorList[i].worldPosition[1] << "\t" 
            << m_AnchorList[i].worldPosition[2] << "\n"; 
   }
}
    
  
// calculate the length
double 
StrapForce::GetLength()
{
   unsigned int i;
   unsigned int theSize = m_AnchorList.size();
  
   assert(theSize >= 2);
  
   m_Length = 0;
   for (i = 1; i < theSize; i++)
   {
      m_Length += Util::Distance3x1(m_AnchorList[i - 1].worldPosition,
         m_AnchorList[i].worldPosition);
   }
  
   if (gDebug == StrapForceDebug)
      cerr << "StrapForce::GetLength\t" << m_name << "\tm_Length\t" <<
         m_Length << "\n";
         
   return m_Length;
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
      m->R_ICS[j][XC] * local[XC] +
      m->R_ICS[j][YC] * local[YC] +
      m->R_ICS[j][ZC] * local[ZC];
  }
}


// calculate the force vector at an Anchor

void 
StrapForce::GetAnchorForce(unsigned int anchor, CartesianVector force)
{
   unsigned int theSize = m_AnchorList.size();
   CartesianVector v;
   CartesianVector v1, v2;
  
   assert(anchor >= 0 && anchor < theSize);
  
   // check for origin
   if (anchor == 0)
   {
      // calculate the direction
      Util::Subtract3x1(m_AnchorList[anchor + 1].worldPosition, m_AnchorList[anchor].worldPosition, v);
      Util::Unit3x1(v);
      // multiply the unit direction vector by the tension
      Util::ScalarMultiply3x1(m_Tension, v, force);
      if (gDebug == StrapForceDebug)
         cerr << "StrapForce::GetAnchorForce\t" << m_name 
         << "\tanchor\t" << anchor
         << "\tforce\t" << force[0]
         << "\t" << force[1]
         << "\t" << force[2] << "\n";
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
         cerr << "StrapForce::GetAnchorForce\t" << m_name 
         << "\tanchor\t" << anchor
         << "\tforce\t" << force[0]
         << "\t" << force[1]
         << "\t" << force[2] << "\n";
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
      cerr << "StrapForce::GetAnchorForce\t" << m_name 
      << "\tanchor\t" << anchor
      << "\tforce\t" << force[0]
      << "\t" << force[1]
      << "\t" << force[2] << "\n";
   return;
}
  
