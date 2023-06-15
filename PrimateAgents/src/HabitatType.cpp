/*
 *  HabitatType.cpp
 *  Primate Agent
 *
 *  Created by Bill Sellers on Wed Sep 22 2004.
 *  Copyright (c) 2004 Bill Sellers. All rights reserved.
 *
 */

using namespace std;

#include <stdlib.h>
#include <string.h>
#include <vector>
#include <iostream>

#ifdef USE_OPENGL
#include <glut.h>
#endif

#include "HabitatType.h"
#include "Simulation.h"
#include "Environment.h"
#include "Grid.h"
#include "DataFile.h"

#define THROWIFZERO(a) if ((a) == 0) throw 0
#define THROWIF(a) if ((a) != 0) throw 0

// construct from an XML node
HabitatType::HabitatType(xmlNodePtr cur)
{
    xmlChar *buf;
    char *ptrs[12];
    int i;

    try
    {
        THROWIFZERO(buf = xmlGetProp(cur, (const xmlChar *)"NAME"));
        m_Name = (char *)buf;
        xmlFree(buf);

        THROWIFZERO(buf = xmlGetProp(cur, (const xmlChar *)"VISIBILITY"));
        THROWIF(DataFile::ReturnTokens((char *)buf, ptrs, 12) != 12);
        for (i = 0; i < 12; i++) m_Visibility[i] = strtod(ptrs[i], 0);
        xmlFree(buf);

        THROWIFZERO(buf = xmlGetProp(cur, (const xmlChar *)"PRODUCTIVITY"));
        THROWIF(DataFile::ReturnTokens((char *)buf, ptrs, 12) != 12);
        for (i = 0; i < 12; i++) m_Productivity[i] = strtod(ptrs[i], 0);
        xmlFree(buf);

        THROWIFZERO(buf = xmlGetProp(cur, (const xmlChar *)"FORAGINGMODEL"));
        if (strcmp((char *)buf, "PROPORTION") == 0) m_ForagingModel = Proportion;
        else if (strcmp((char *)buf, "FIXEDRATE") == 0) m_ForagingModel = FixedRate;
        else throw(0);
        xmlFree(buf);

        THROWIFZERO(buf = xmlGetProp(cur, (const xmlChar *)"FORAGINGRATEFACTOR"));
        THROWIF(DataFile::ReturnTokens((char *)buf, ptrs, 12) != 12);
        for (i = 0; i < 12; i++) m_ForagingRateFactor[i] = strtod(ptrs[i], 0);
        xmlFree(buf);

        THROWIFZERO(buf = xmlGetProp(cur, (const xmlChar *)"MOVEFORAGINGRATEPENALTY"));
        THROWIF(DataFile::ReturnTokens((char *)buf, ptrs, 12) != 12);
        for (i = 0; i < 12; i++) m_MoveForagingRatePenalty[i] = strtod(ptrs[i], 0);
        xmlFree(buf);

        THROWIFZERO(buf = xmlGetProp(cur, (const xmlChar *)"RECOVERYRATE"));
        THROWIF(DataFile::ReturnTokens((char *)buf, ptrs, 12) != 12);
        for (i = 0; i < 12; i++) m_RecoveryRate[i] = strtod(ptrs[i], 0);
        xmlFree(buf);

        THROWIFZERO(buf = xmlGetProp(cur, (const xmlChar *)"MEANENERGYVALUE"));
        THROWIF(DataFile::ReturnTokens((char *)buf, ptrs, 12) != 12);
        for (i = 0; i < 12; i++) m_MeanEnergyValue[i] = strtod(ptrs[i], 0);
        xmlFree(buf);

#ifdef USE_OPENGL
        THROWIFZERO(buf = xmlGetProp(cur, (const xmlChar *)"MAPCOLOUR"));
        THROWIF(DataFile::ReturnTokens((char *)buf, ptrs, 3) != 3);
        m_R = strtod(ptrs[0], 0) / 255;
        m_G = strtod(ptrs[1], 0) / 255;
        m_B = strtod(ptrs[2], 0) / 255;
        xmlFree(buf);

        THROWIFZERO(buf = xmlGetProp(cur, (const xmlChar *)"FULLRESOURCE"));
        m_FullResource = strtod((char *)buf, 0);
        xmlFree(buf);
#endif

    }
    
    catch (int e)
    {
        cerr << "Error parsing HABITAT_TYPE " << m_Name << "\n";
        throw 0;
    }

    ResetTotals();
}



