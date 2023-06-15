/*
 *  RangedVariable.h
 *  MonteCarlo
 *
 *  Created by Bill Sellers on Mon Mar 07 2005.
 *  Copyright (c) 2005 Bill Sellers. All rights reserved.
 *
 */

#ifndef RangedVariable_h
#define RangedVariable_h

#include <string>

class RangedVariable
{
public:
    RangedVariable(char *theName,
                   double lowerBound,
                   double upperBound,
                   double numberOfSDsInRange);
    ~RangedVariable();

    double GetRandomValue();
    string &GetName() { return m_name; };

protected:
    
    string	m_name;
    double	m_lowerBound;
    double  m_upperBound;
    double  m_gaussianSD;
    double  m_numberOfSDsInRange;
    double  m_mean;
};


#endif

