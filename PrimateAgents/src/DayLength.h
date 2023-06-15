/*
 *  DayLength.h
 *  Primate Agent
 *
 *  Created by Bill Sellers on Wed Sep 22 2004.
 *  Copyright (c) 2004 Bill Sellers. All rights reserved.
 *
 */

#ifndef DayLength_h
#define DayLength_h

#include <libxml/xmlmemory.h>
#include <libxml/parser.h>

class DayLength
{
public:
    DayLength(xmlNodePtr cur);

    double GetHoursAtMonth(int month);

protected:
    double m_DayLengths[12];
};


#endif

