// MAMuscle - implementation of an Minetti & Alexander style
// muscle based on the StrapForce class

// Minetti & Alexander, J. theor Biol (1997) 186, 467-476

// Added extra terms to allow a parallel spring element

#include "StrapForce.h"
#include "MAMuscle.h"
#include "DebugControl.h"

// constructor

MAMuscle::MAMuscle()
{
    m_VMax = 0;
    m_F0 = 0;
    m_K = 0;
    m_Alpha = 0;
    m_SpringConstant = 0;
    m_UnloadedLength = 0;
}

// destructor
MAMuscle::~MAMuscle()
{
}

// set the proportion of muscle fibres that are active
// calculates the tension in the strap

void MAMuscle::SetAlpha(double alpha)
{
    if (alpha < 0) m_Alpha = 0;
    else
    {
        if (alpha > 1.0) m_Alpha = 1.0;
        else m_Alpha = alpha;
    }

    // m_Velocity is negative when muscle shortening
    // we need the sign the other way round
    double v = -m_Velocity;
    double fFull;

    if (v < 0)
    {
        fFull = m_F0 * (1.8 - 0.8 * ((m_VMax + v) / (m_VMax - (7.56 / m_K) * v)));
    }
    else
    {
        if (v < m_VMax)
        {
            fFull = m_F0 * (m_VMax - v) / (m_VMax + (v / m_K));
        }
        else
        {
            fFull = 0;
        }
    }

    // now set the tension as a proportion of fFull

    if (m_SpringConstant == 0) SetTension(m_Alpha * fFull);
    else SetTension((m_Alpha * fFull) + ((m_Length - m_UnloadedLength) * m_SpringConstant));

    if (gDebug == MAMuscleDebug)
    {
        *gDebugStream << "MAMuscle::SetAlpha\t" << m_name <<
        "\talpha\t" << alpha <<
        "\tm_Alpha\t" << m_Alpha <<
        "\tm_F0\t" << m_F0 <<
        "\tm_VMax\t" << m_VMax <<
        "\tm_Velocity\t" << m_Velocity <<
        "\tfFull\t" << fFull <<
        "\tm_Length\t" << m_Length <<
        "\tm_UnloadedLength\t" << m_UnloadedLength <<
        "\tm_SpringConstant\t" << m_SpringConstant <<
        "\tactiveTension\t" << m_Alpha * fFull <<
        "\tpassiveTension\t" << (m_Length - m_UnloadedLength) * m_SpringConstant <<
        "\n";
    }
}

// calculate the metabolic power of the muscle

double MAMuscle::GetMetabolicPower()
{
    // m_Velocity is negative when muscle shortening
    // we need the sign the other way round
    double relV = -m_Velocity / m_VMax;
    double relVSquared = relV * relV;
    double relVCubed = relVSquared * relV;

    double sigma = (0.054 + 0.506 * relV + 2.46 * relVSquared) /
        (1 - 1.13 * relV + 12.8 * relVSquared - 1.64 * relVCubed);

    if (gDebug == MAMuscleDebug)
    {
        *gDebugStream << "MAMuscle::GetMetabolicPower\t" << m_name <<
        "\tm_Alpha\t" << m_Alpha <<
        "\tm_F0\t" << m_F0 <<
        "\tm_VMax\t" << m_VMax <<
        "\tm_Velocity\t" << m_Velocity <<
        "\tsigma\t" << sigma <<
        "\tpower\t" << m_Alpha * m_F0 * m_VMax * sigma << "\n";
    }
    return (m_Alpha * m_F0 * m_VMax * sigma);
}



