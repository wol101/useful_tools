// ExtendedForce - a virtual subclass of dmForce 
// that attaches itself to the gSimulation force list

#ifndef ExtendedForce_h
#define ExtendedForce_h

#include <dm.h>
#include <dmForce.hpp>
#include <iostream.h>
#include <iomanip.h>
#include <fstream.h>

class ExtendedForce:public dmForce 
{
  public:

  ExtendedForce();
  virtual ~ExtendedForce();

protected:

};

#endif  // ExtendedForce_h

