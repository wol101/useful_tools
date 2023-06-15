// StrapForce.h

// this routine provides a force that acts on a number of bodies
// via a strap. The strap has an origin and insertion and is tethered
// by a number of pulleys.

#ifndef StrapForce_h
#define StrapForce_h

#include <vector>
#include <dmObject.hpp>
class dmLink;

struct StrapAnchorLocation
{
  CartesianVector localPosition;
  CartesianVector worldPosition;
  dmLink * link;
};

class StrapForce: public dmObject
{
  public: 
      
      StrapForce();
      ~StrapForce();
      
      void SetNumAnchors(unsigned int numAnchors);
            
      double GetLength() { return m_Length; };
      double GetTension() { return m_Tension; };
      double GetVelocity() { return m_Velocity; };
      double GetPower() { return -(m_Velocity * m_Tension); }; // need negative as this is the power required
      void SetTension(double tension);
      void SetAnchor(dmLink *link, unsigned int anchorID, 
         CartesianVector location);
      void SetAnchorLocation(unsigned int anchorID, 
         CartesianVector location);
      
      void GetAnchorForce(unsigned int anchor, CartesianVector force);
      void GetAnchorLocation(unsigned int anchor, CartesianVector localPosition);
      void GetAnchorWorldLocation(unsigned int anchor, CartesianVector worldPosition);
      
      void UpdateWorldPositions();
      
      void Draw();
                  
  protected:
      
      bool                             m_Valid;
      double                           m_Tension;
      double                           m_Length;
      std::vector<StrapAnchorLocation> m_AnchorList;
      double                           m_LastLength;
      double                           m_Velocity;      

      void ConvertLocalToWorldP(dmLink *link, 
          const CartesianVector local, CartesianVector world);
      void CalculateLength();
};
  
#endif // StrapForce_h
