// Heavily based on dmContactModel but modified to support force drawing
// and added some reporting functions

#ifndef ModifiedContactModel_h
#define ModifiedContactModel_h

#include <dm.h>
#include <ExtendedForce.h>

class ModifiedContactModel:public ExtendedForce
{
public:
  ModifiedContactModel();
  ~ModifiedContactModel();

  void setContactPoints (int num_contact_points, CartesianVector *contact_pos);
  int getNumContactPoints () const { return m_num_contact_points; };
  bool getContactPoint (int index, CartesianVector pos) const;

  void computeForce (const dmABForKinStruct & val, SpatialVector force);
  void reset () { m_reset_flag = true; };

  // check to see whether there is any contact (returns true if there is)
  bool AnyContact(void);

  // copy the last calculated contact force into the passed vector
  void GetContactForce(SpatialVector f);
    
  // rendering functions
  void draw() const;
  
private:
  bool m_reset_flag;

  int m_num_contact_points;
  bool *m_contact_flag;
  bool *m_sliding_flag;
  CartesianVector *m_contact_pos;            // list of point locations to be checked 
  CartesianVector *m_initial_contact_pos;    // ICS 1st pnt of contact for each
  
  // force storage
  SpatialVector m_contactForce;
  
  // drawing variables
  bool m_drawValid;
  CartesianVector *m_world_contact_pos;
  SpatialVector *m_world_force;
};

#endif
