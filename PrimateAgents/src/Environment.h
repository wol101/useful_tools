/*
 *  Environment.h
 *  PrimateAgent
 *
 *  Created by Bill Sellers on Fri Jan 28 2005.
 *  Copyright (c) 2005 Bill Sellers. All rights reserved.
 *
 */

#ifndef Environment_h
#define Environment_h

#include <vector>
#include <map>
#include <string>
#include <libxml/xmlmemory.h>
#include <libxml/parser.h>

#include "Grid.h"

class DayLength;
class HabitatType;
class GridSquare;

class Environment
{
public:
    Environment();
    ~Environment();

    void ParseEnvironment(xmlNodePtr cur);

    vector<GridSquare *> *GetSleepingSiteList() { return &m_SleepingSiteList; };
    vector<GridSquare *> *GetWaterPresentList() { return &m_WaterPresentList; };
    map<string, HabitatType *> *GetHabitatTypeList() { return &m_HabitatTypeList; };
    vector<GridSquare *> *GetGridSquareList() { return &m_GridSquareList; };
    DayLength *GetDayLength() { return m_DayLength; };
    Grid *GetGrid() { return m_Grid; };

#ifdef USE_OPENGL
    void Draw();
#endif
    
protected:

    // primary data lists
    DayLength *m_DayLength;
    map<string, HabitatType *> m_HabitatTypeList;
    vector<GridSquare *> m_GridSquareList;

    // secondary lists for fast access
    Grid *m_Grid;
    vector<GridSquare *> m_SleepingSiteList;
    vector<GridSquare *> m_CliffRefugeList;
    vector<GridSquare *> m_OtherRefugeList;
    vector<GridSquare *> m_WaterPresentList;
    
};



#endif



