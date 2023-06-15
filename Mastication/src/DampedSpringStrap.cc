// DampedSpringStrap - implementation of a damped spring strap force

#include "StrapForce.h"
#include "DampedSpringStrap.h"
#include "DebugControl.h"

// constructor

DampedSpringStrap::DampedSpringStrap()
{
    m_Damping = 0;
    m_SpringConstant = 0;
    m_UnloadedLength = 0;
}

// destructor
DampedSpringStrap::~DampedSpringStrap()
{
}

// update the tension epending on length and velocity
void DampedSpringStrap::UpdateTension()
{
    // m_Velocity is negative when muscle shortening
    m_Tension = (m_Length - m_UnloadedLength) * m_SpringConstant + m_Velocity * m_Damping;

    // stop any pushing
    if (m_Tension < 0) m_Tension = 0;
    
    if (gDebug == DampedSpringDebug)
        cerr << "DampedSpringStrap::UpdateTension\tm_Length\t" << m_Length << "\tm_UnloadedLength\t" << m_UnloadedLength
            << "\tm_SpringConstant\t" << m_SpringConstant << "\tm_Velocity\t" << m_Velocity
            << "\tm_Damping\t" << m_Damping << "\tm_Tension\t" << m_Tension << "\n";
}



