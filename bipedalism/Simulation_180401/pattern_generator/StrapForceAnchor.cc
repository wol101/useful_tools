// StrapForceAnchor.cc - an anchor point for a strap force

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
   Util::Copy3x1(location, m_Location);
   m_StrapForce = strapForce;
   m_Anchor_ID = anchorID;
   
   m_StrapForce->SetAnchor(m_Link, m_Anchor_ID, m_Location);
}

// calculate the force as needed by the link this force is attached to
void StrapForceAnchor::computeForce(dmABForKinStruct * val, SpatialVector f_contact)
{
   CartesianVector worldForce;
   CartesianVector localForce;
   CartesianVector localTorque;
   int j;
   
   // get the force from the StrapForce
   m_StrapForce->GetAnchorForce(m_Anchor_ID, worldForce);
   
   // convert to local coordinates
   for (j = 0; j < 3; j++)
   {
      localForce[j] = val->R_ICS[0][j] * worldForce[0] +
        val->R_ICS[1][j] * worldForce[1] + val->R_ICS[2][j] * worldForce[2];
   }
   
   // and local torque
   Util::CrossProduct3x1(m_Location, localForce, localTorque);

   // and build the return force array
   for (j = 0; j < 3; j++)
   {
     f_contact[j] += localTorque[j];
     f_contact[j + 3] += localForce[j];
   }
   
   // log the force as required
   if (logging())
   {
      (*mOutput) << "localForce\t" << localForce[0];
      (*mOutput) << "\t" << localForce[1];
      (*mOutput) << "\t" << localForce[2];
      (*mOutput) << "\tworldForce\t" << worldForce[0];
      (*mOutput) << "\t" << worldForce[1];
      (*mOutput) << "\t" << worldForce[2] << "\n";
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

