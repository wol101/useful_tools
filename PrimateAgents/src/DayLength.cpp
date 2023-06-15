/*
 *  DayLength.cpp
 *  Primate Agent
 *
 *  Created by Bill Sellers on Wed Sep 22 2004.
 *  Copyright (c) 2004 Bill Sellers. All rights reserved.
 *
 */

using namespace std;

#include <stdlib.h>
#include <vector>
#include <iostream>

#include "DayLength.h"
#include "DataFile.h"

#define THROWIFZERO(a) if ((a) == 0) throw 0
#define THROWIF(a) if ((a) != 0) throw 0

// construct from an XML node
DayLength::DayLength(xmlNodePtr cur)
{
    xmlChar *buf;
    char *ptrs[12];
    int i;

    try
    {
        THROWIFZERO(buf = xmlGetProp(cur, (const xmlChar *)"VALUE"));
        THROWIF(DataFile::ReturnTokens((char *)buf, ptrs, 12) != 12);
        for (i = 0; i < 12; i++) m_DayLengths[i] = strtod(ptrs[i], 0);
    }

    catch (int e)
    {
        cerr << "Error parsing DAY_LENGTH\n";
        throw 0;
    }
    
}

// retrieve a value for a given month
double DayLength::GetHoursAtMonth(int month)
{
    return m_DayLengths[month % 12];
}





