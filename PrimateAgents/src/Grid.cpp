/*
 *  Grid.cpp
 *  PrimateAgent
 *
 *  Created by Bill Sellers on Fri Jan 28 2005.
 *  Copyright (c) 2005 Bill Sellers. All rights reserved.
 *
 */

using namespace std;

#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <iostream>

#ifdef USE_OPENGL
#include <glut.h>
#endif

#include "Grid.h"
#include "GridSquare.h"

#define THROWIFZERO(a) if ((a) == 0) throw 0
#define THROWIF(a) if ((a) != 0) throw 0

#ifndef ABS
#define ABS(x) ((x)>=0?(x):-(x))
#endif /* ABS */
#ifndef MIN
#define	MIN(a,b) (((a)<(b))?(a):(b))
#endif /* MIN */
#ifndef MAX
#define	MAX(a,b) (((a)>(b))?(a):(b))
#endif /* MAX */

Grid::Grid(xmlNodePtr cur)
{
    xmlChar *buf;
    
    m_GridData = 0;
    m_XMax = 0;
    m_YMax = 0;
    m_XOffset = 0;
    m_YOffset = 0;
    m_XSize = 0;
    m_YSize = 0;
    m_XOrigin = 0;
    m_YOrigin = 0;
    m_XLimit = 0;
    m_YLimit = 0;
    
    try
    {
        THROWIFZERO(buf = xmlGetProp(cur, (const xmlChar *)"XSIZE"));
        m_XSize = strtod((char *)buf, 0);
        xmlFree(buf);

        THROWIFZERO(buf = xmlGetProp(cur, (const xmlChar *)"YSIZE"));
        m_YSize = strtod((char *)buf, 0);
        xmlFree(buf);

        THROWIFZERO(buf = xmlGetProp(cur, (const xmlChar *)"XORIGIN"));
        m_XOrigin = strtod((char *)buf, 0);
        xmlFree(buf);

        THROWIFZERO(buf = xmlGetProp(cur, (const xmlChar *)"YORIGIN"));
        m_YOrigin = strtod((char *)buf, 0);
        xmlFree(buf);
    }

    catch (int e)
    {
        cerr << "Error parsing GRID\n";
        throw 0;
    }
    
}


Grid::~Grid()
{
    if (m_GridData) delete [] m_GridData;
}

void Grid::SetMax(int x, int y)
{
    assert(x > 0 && y > 0);
    m_XMax = x;
    m_YMax = y;
    m_GridData = new GridData[m_XMax * m_YMax];
    m_XLimit = m_XOrigin + m_XSize * (double)m_XMax;
    m_YLimit = m_YOrigin + m_YSize * (double)m_YMax;
}

GridData *Grid::GetGridDataPtr(double x, double y)
{
    if (x < m_XOrigin || x >= m_XLimit) return 0;
    if (y < m_YOrigin || y >= m_YLimit) return 0;
    int ix = (int)((x - m_XOrigin) / m_XSize);
    int iy = (int)((y - m_YOrigin) / m_YSize);
    if (ix < 0) ix = 0;
    if (ix >= m_XMax) ix = m_XMax - 1;
    if (iy < 0) iy = 0;
    if (iy >= m_YMax) iy = m_XMax - 1;
    return &(m_GridData[ix + iy * m_XMax]);
};

#ifdef USE_OPENGL
void Grid::Draw()
{
    int x, y;
    GridData *gridData;
    GLfloat scale = 1.0 / (GLfloat)MAX(m_XMax, m_YMax);
    
    glMatrixMode( GL_MODELVIEW );
    for (y = 0; y < m_YMax; y++)
    {
        for (x = 0; x < m_XMax; x++)
        {
            gridData = &(m_GridData[x + y * m_XMax]);
            if (gridData->valid)
            {
                // need to move and scale so that the unit square is drawn in the right place
                glPushMatrix();
                glScalef(scale, scale, scale);
                glTranslatef(x, y, 0);
                gridData->gridSquare->Draw();
                glPopMatrix();
            }
        }
    }
}

// assume a unity square on entry
void Grid::ConvertToGridCoordinates()
{
    GLfloat scale = (GLfloat)(1 / MAX(m_XLimit - m_XOrigin, m_YLimit - m_YOrigin));
    glScalef(scale, scale, 1);
    glTranslatef(-(GLfloat)m_XOrigin, -(GLfloat)m_YOrigin, 0);
}

double Grid::GetGridSquareDimensions(GLfloat *width, GLfloat *height)
{
    GLfloat scale = (GLfloat)(1 / MAX(m_XLimit - m_XOrigin, m_YLimit - m_YOrigin));
    *width = m_XSize * scale;
    *height = m_YSize * scale;
    return scale;
}
#endif

    
    
    
