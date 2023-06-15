// MAMuscle - implementation of an Minetti & Alexander style 
// muscle based on the StrapForce class

// Minetti & Alexander, J. theor Biol (1997) 186, 467-476

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
}

// destructor
MAMuscle::~MAMuscle()
{
}

// set the proportion of muscle fibres that are active
// calculates the tension in the strap

void MAMuscle::SetAlpha(double alpha)
{
   assert(alpha >=0 && alpha <= 1.0);
   m_Alpha = alpha;
   
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
   
   SetTension(m_Alpha * fFull);
   
   if (gDebug == MAMuscleDebug)
   {
      cerr << "MAMuscle::SetAlpha\t" << m_name <<
      "\tm_Alpha\t" << m_Alpha <<
      "\tm_F0\t" << m_F0 <<
      "\tm_VMax\t" << m_VMax <<
      "\tm_Velocity\t" << m_Velocity <<
      "\tfFull\t" << fFull <<
      "\ttension\t" << m_Alpha * fFull << "\n";
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
      cerr << "MAMuscle::GetMetabolicPower\t" << m_name <<
      "\tm_Alpha\t" << m_Alpha <<
      "\tm_F0\t" << m_F0 <<
      "\tm_VMax\t" << m_VMax <<
      "\tm_Velocity\t" << m_Velocity <<
      "\tsigma\t" << sigma <<
      "\tpower\t" << m_Alpha * m_F0 * m_VMax * sigma << "\n";
   }
   return (m_Alpha * m_F0 * m_VMax * sigma);
}


