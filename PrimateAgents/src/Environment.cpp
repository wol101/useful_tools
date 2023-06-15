/*
 *  Environment.cpp
 *  PrimateAgent
 *
 *  Created by Bill Sellers on Fri Jan 28 2005.
 *  Copyright (c) 2005 Bill Sellers. All rights reserved.
 *
 */

using namespace std;

#include <limits.h>
#include <assert.h>
#include <iostream>

#ifdef USE_OPENGL
#include <glut.h>
#endif

#include "Environment.h"
#include "DataFile.h"
#include "DayLength.h"
#include "HabitatType.h"
#include "GridSquare.h"

#ifndef ABS
#define ABS(x) ((x)>=0?(x):-(x))
#endif /* ABS */
#ifndef MIN
#define	MIN(a,b) (((a)<(b))?(a):(b))
#endif /* MIN */
#ifndef MAX
#define	MAX(a,b) (((a)>(b))?(a):(b))
#endif /* MAX */

// constructor

Environment::Environment()
{
    m_Grid = 0;
    m_DayLength = 0;
}

// destructor

Environment::~Environment()
{
    // delete the contents of the pointers stored in the lists
    map<string, HabitatType *>::const_iterator iter;
    for (iter=m_HabitatTypeList.begin(); iter != m_HabitatTypeList.end(); iter++) delete iter->second;
    unsigned int index;
    for (index = 0; index < m_GridSquareList.size(); index++) delete m_GridSquareList[index];
    if (m_Grid) delete m_Grid;
    if (m_DayLength) delete m_DayLength;
}


// initialise from an XML file
// returns 0 on success

void Environment::ParseEnvironment(xmlNodePtr cur)
{
    bool err = false;
    HabitatType *habitatTypePtr;
    GridSquare *gridSquarePtr;    

    try
    {
        // now parse the elements in the node

        cur = cur->xmlChildrenNode;
        while (cur != NULL)
        {
            if ((!xmlStrcmp(cur->name, (const xmlChar *)"DAY_LENGTH")))
            {
                m_DayLength = new DayLength(cur);
            }
            else if ((!xmlStrcmp(cur->name, (const xmlChar *)"HABITAT_TYPE")))
            {
                habitatTypePtr = new HabitatType(cur);
                m_HabitatTypeList[habitatTypePtr->GetName()] = habitatTypePtr;
            }
            else if ((!xmlStrcmp(cur->name, (const xmlChar *)"GRID")))
            {
                m_Grid = new Grid(cur);
            }
            else if ((!xmlStrcmp(cur->name, (const xmlChar *)"GRID_SQUARE")))
            {
                gridSquarePtr = new GridSquare(cur);
                m_GridSquareList.push_back(gridSquarePtr);
            }
            cur = cur->next;
        }
    }

    catch(...)
    {
        throw 1;
    }

    // fill in the internal lists
    unsigned int index;
    int minYGrid = INT_MAX;
    int minXGrid = INT_MAX;
    int maxYGrid = INT_MIN;
    int maxXGrid = INT_MIN;
    int yGrid, xGrid;
    GridData *gridDataPtr;
    double x, y;
    for (index = 0; index < m_GridSquareList.size(); index++)
    {
        yGrid = m_GridSquareList[index]->GetYGrid();
        minYGrid = MIN(minYGrid, yGrid);
        maxYGrid = MAX(maxYGrid, yGrid);
        xGrid = m_GridSquareList[index]->GetXGrid();
        minXGrid = MIN(minXGrid, xGrid);
        maxXGrid = MAX(maxXGrid, xGrid);

        if (m_GridSquareList[index]->IsSleepingSite())
            m_SleepingSiteList.push_back(m_GridSquareList[index]);
        if (m_GridSquareList[index]->IsCliffRefuge())
            m_CliffRefugeList.push_back(m_GridSquareList[index]);
        if (m_GridSquareList[index]->IsOtherRefuge())
            m_OtherRefugeList.push_back(m_GridSquareList[index]);
        if (m_GridSquareList[index]->IsWaterPresent())
            m_WaterPresentList.push_back(m_GridSquareList[index]);

    }
    m_Grid->SetMax(1 + maxXGrid - minXGrid, 1 + maxYGrid - minYGrid);
    m_Grid->SetOffset(minXGrid, minYGrid);
    for (index = 0; index < m_GridSquareList.size(); index++)
    {
        yGrid = m_GridSquareList[index]->GetYGrid();
        xGrid = m_GridSquareList[index]->GetXGrid();
        gridDataPtr = m_Grid->GetGridDataPtr(xGrid, yGrid);
        assert(gridDataPtr);
        if (gridDataPtr->valid)
        {
            cerr << "Duplicate GRID_SQUARE at " << xGrid << " " << yGrid << "\n";
            err = true;
        }
        gridDataPtr->valid = true;
        gridDataPtr->gridSquare = m_GridSquareList[index];

        x = m_Grid->GetXOrigin() + ((double)(xGrid - minXGrid) + 0.5) * m_Grid->GetXSize();
        y = m_Grid->GetYOrigin() + ((double)(yGrid - minYGrid) + 0.5) * m_Grid->GetYSize();
        m_GridSquareList[index]->SetCentre(x, y);
    }

    if (err) throw 1;
        
    return;
}

#ifdef USE_OPENGL
void Environment::Draw()
{
    m_Grid->Draw();
}
#endif

