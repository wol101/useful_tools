/*
 *  Grid.h
 *  PrimateAgent
 *
 *  Created by Bill Sellers on Fri Jan 28 2005.
 *  Copyright (c) 2005 Bill Sellers. All rights reserved.
 *
 */

#ifndef Grid_h
#define Grid_h

#include <libxml/xmlmemory.h>
#include <libxml/parser.h>

class GridSquare;

struct GridData
{
    GridSquare* gridSquare;
    bool valid;
};

class Grid
{
public:
    Grid(xmlNodePtr cur);
    ~Grid();

    void SetMax(int x, int y);
    void SetOffset(int x, int y) { m_XOffset = x; m_YOffset = y; };
    int GetXMax() { return m_XMax; };
    int GetYMax() { return m_YMax; };
    double GetXSize() { return m_XSize; };
    double GetYSize() { return m_YSize; };
    double GetXLimit() { return m_XLimit; };
    double GetYLimit() { return m_YLimit; };
    double GetXOrigin() { return m_XOrigin; };
    double GetYOrigin() { return m_YOrigin; };
    GridData *Grid::GetGridDataPtr(int inx, int iny)
    {
        int x = inx - m_XOffset;
        int y = iny - m_YOffset;
        if (x < 0 || x >= m_XMax) return 0;
        if (y < 0 || y >= m_YMax) return 0;
        return &(m_GridData[x + y * m_XMax]);
    };
    GridData *Grid::GetGridDataPtr(double x, double y);
    void LimitRange(double *x, double *y)
    {
        if (*x < m_XOrigin) *x = m_XOrigin;
        else if (*x > m_XLimit) *x = m_XLimit;
        if (*y < m_YOrigin) *y = m_YOrigin;
        else if (*y > m_YLimit) *y = m_YLimit;
    };

#ifdef USE_OPENGL
    void Draw();
    void ConvertToGridCoordinates();
    double GetGridSquareDimensions(GLfloat *width, GLfloat *height);
#endif
    
protected:
    GridData *m_GridData;
    int m_XMax;
    int m_YMax;
    int m_XOffset;
    int m_YOffset;
    double m_XSize;
    double m_YSize;
    double m_XOrigin;
    double m_YOrigin;
    double m_XLimit;
    double m_YLimit;
};



#endif


