// Heavily based on dmContactModel but modified to support force drawing
// and added some reporting functions

#ifndef ModifiedContactModel_h
#define ModifiedContactModel_h

#include <dm.h>
#include <dmContactModel.hpp>

class ModifiedContactModel:public dmContactModel
{
public:
  ModifiedContactModel();
  ~ModifiedContactModel();

  void computeForce (const dmABForKinStruct & val, SpatialVector force);
  void draw() const;
  
protected:

  // drawing variables
  bool m_drawValid;
  
};

#endif
