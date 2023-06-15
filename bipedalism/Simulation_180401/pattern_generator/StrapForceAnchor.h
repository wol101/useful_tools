// StrapForceAnchor.h - an anchor point for a strap force

#ifndef StrapForceAnchor_h
#define StrapForceAnchor_h

#include "LoggedForce.h"

class StrapForce;
class dmLink;

class StrapForceAnchor: public LoggedForce
{
   public:
   
      StrapForceAnchor();
      ~StrapForceAnchor();
      
      void SetStrapForce(dmLink *link, CartesianVector location,
         StrapForce *strapForce, unsigned int anchorID);
      
      // required funcitons
      void reset() {};
      void computeForce(dmABForKinStruct * val, SpatialVector force);
      
   protected:
   
      dmLink            *m_Link;
      StrapForce        *m_StrapForce;
      unsigned int      m_Anchor_ID;
      CartesianVector   m_Location;
};


#endif // StrapForceAnchor_h
