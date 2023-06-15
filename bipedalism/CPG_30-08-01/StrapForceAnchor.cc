// StrapForceAnchor.cc - an anchor point for a strap force

#include <dmLink.hpp>

#include "StrapForceAnchor.h"
#include "StrapForce.h"
#include "Util.h"
#include "DebugControl.h"

// default constructor
StrapForceAnchor::StrapForceAnchor()
{
   m_StrapForce = 0;
   m_Anchor_ID = 0;
}

// default destructor
// doesn't do anything since m_StrapForce not owned by this object
StrapForceAnchor::~StrapForceAnchor()
{
}

// set the location of the anchor point in local link coordinates
// set the strap force and register this anchor point
void StrapForceAnchor::SetStrapForce(dmLink *link,
   CartesianVector location,
   StrapForce *strapForce, 
   unsigned int anchorID)
{
   m_Link = link;
   m_StrapForce = strapForce;
   m_Anchor_ID = anchorID;
   
   m_StrapForce->SetAnchor(m_Link, m_Anchor_ID, location);

   if (gDebug == StrapForceAnchorDebug)
   {
      cerr << "StrapForceAnchor::SetStrapForce\t" << m_name;
      cerr << "\tlink->getName()\t" << link->getName();
      cerr << "\tm_Anchor_ID\t" << m_Anchor_ID;
      cerr << "\tm_Location\t" << location[0];
      cerr << "\t" << location[1];
      cerr << "\t" << location[2] << "\n";
   }
}

// calculate the force as needed by the link this force is attached to
void StrapForceAnchor::computeForce(const dmABForKinStruct & val, SpatialVector f_contact)
{
   CartesianVector worldForce;
   CartesianVector localForce;
   CartesianVector localTorque;
   CartesianVector location;
   int j;
   
   // get the force from the StrapForce
   m_StrapForce->GetAnchorForce(m_Anchor_ID, worldForce);
   
   // convert to local coordinates
   for (j = 0; j < 3; j++)
   {
      localForce[j] = val.R_ICS[0][j] * worldForce[0] +
        val.R_ICS[1][j] * worldForce[1] + val.R_ICS[2][j] * worldForce[2];
   }
   
   // and local torque
   m_StrapForce->GetAnchorLocation(m_Anchor_ID, location);
   Util::CrossProduct3x1(location, localForce, localTorque);

   // and build the return force array
   for (j = 0; j < 3; j++)
   {
     f_contact[j] = localTorque[j];
     f_contact[j + 3] = localForce[j];
   }
   
   if (gDebug == StrapForceAnchorDebug)
   {
      cerr << "StrapForceAnchor::computeForce\t" << m_name;
      cerr << "\tlocalForce\t" << localForce[0];
      cerr << "\t" << localForce[1];
      cerr << "\t" << localForce[2];
      cerr << "\tworldForce\t" << worldForce[0];
      cerr << "\t" << worldForce[1];
      cerr << "\t" << worldForce[2] << "\n";
   }
}

