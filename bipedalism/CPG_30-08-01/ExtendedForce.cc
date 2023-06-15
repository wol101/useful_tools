// ExtendedForce - a virtual subclass of dmForce 
// that attaches itself to the gSimulation force list

#include <dm.h>
#include <dmForce.hpp>

#include "Simulation.h"
#include "ExtendedForce.h"

extern Simulation *gSimulation;

ExtendedForce::ExtendedForce()
{
  gSimulation->GetForceList()->AddForce(this);  // add force to forcelist
}

ExtendedForce::~ExtendedForce()
{
}
