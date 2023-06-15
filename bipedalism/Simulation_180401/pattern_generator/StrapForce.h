// StrapForce.h

// this routine provides a force that acts on a number of bodies
// via a strap. The strap has an origin and insertion and is tethered
// by a number of pulleys.

#ifndef StrapForce_h
#define StrapForce_h

#include <vector>
#include <dmObject.h>
class dmLink;

struct StrapAnchorLocation
{
  CartesianVector localPosition;
  CartesianVector worldPosition;
  dmLink * link;
};

class StrapForce :dmObject
{
  public: 
      
      StrapForce();
      ~StrapForce();
      
      void SetNumAnchors(unsigned int numAnchors);
      void SetAnchor(dmLink *link, unsigned int anchorID, 
         CartesianVector location);
            
      double GetLength();
      double GetTension() { return m_Tension; };
      void SetTension(double tension) { m_Tension = tension; };
      
      void GetAnchorForce(unsigned int anchor, CartesianVector force);
      
      void UpdateWorldPositions();
            
  protected:
      
      double                          m_Tension;
      double                          m_Length;
      std::vector<StrapAnchorLocation> m_AnchorList;      

      void ConvertLocalToWorldP(dmLink *link, 
          const CartesianVector local, CartesianVector world);
};
  
#endif // StrapForce_h
