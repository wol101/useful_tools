/*
 *  RangedVariable.cpp
 *  MonteCarlo
 *
 *  Created by Bill Sellers on Mon Mar 07 2005.
 *  Copyright (c) 2005 Bill Sellers. All rights reserved.
 *
 */

using namespace std;

#include "RangedVariable.h"
#include "Random.h"


RangedVariable::RangedVariable(char *name,
                               double lowerBound,
                               double upperBound,
                               double numberOfSDsInRange)
{
    m_name = name;
    m_lowerBound = lowerBound;
    m_upperBound = upperBound;
    m_gaussianSD = (upperBound - lowerBound) / numberOfSDsInRange;
    m_numberOfSDsInRange = numberOfSDsInRange;
    m_mean = (upperBound + lowerBound) / 2.0;
}

RangedVariable::~RangedVariable()
{
}

double RangedVariable::GetRandomValue()
{
    double r, v;
    do
    {
        r = RandomUnitGaussian();
        v = (r * m_gaussianSD) + m_mean;
    } while (v < m_lowerBound || v > m_upperBound);

    return v;
}



