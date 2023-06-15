/*
 *  Key.cpp
 *  PrimateAgent
 *
 *  Created by Bill Sellers on Sun Feb 06 2005.
 *  Copyright (c) 2005 Bill Sellers. All rights reserved.
 *
 */

#ifdef USE_OPENGL

using namespace std;

#include <glut.h>

#include "GLShapes.h"

#include "Key.h"
#include "Simulation.h"
#include "Environment.h"
#include "HabitatType.h"

#ifndef ABS
#define ABS(x) ((x)>=0?(x):-(x))
#endif /* ABS */
#ifndef MIN
#define	MIN(a,b) (((a)<(b))?(a):(b))
#endif /* MIN */
#ifndef MAX
#define	MAX(a,b) (((a)>(b))?(a):(b))
#endif /* MAX */

extern Simulation g_Simulation;

Key::Key()
{
    m_TextSize = 0;
    m_XOrigin = 0;
    m_YOrigin = 0;
    m_Width = 0;
    m_Height = 0;
    m_FirstTime = true;
}

Key::~Key()
{
    unsigned int index;
    for (index = 0; index < m_KeyDataList.size(); index++)
        delete m_KeyDataList[index];
}

void Key::Draw()
{
    char text[64];
    GLfloat x, y;
    GLfloat r, g, b;
    KeyData *keyData;
    int i;
    GLfloat margin = 0.1 * MIN(m_Width, m_Height);
    static char *months[] = {"January", "February", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December"};

    if (m_FirstTime)
    {
        m_FirstTime = false;
        map<string, HabitatType *> *habitatTypeList = g_Simulation.GetEnvironment()->GetHabitatTypeList();
        map<string, HabitatType *>::const_iterator iter;
        for (iter = habitatTypeList->begin(); iter != habitatTypeList->end(); iter++)
        {
            keyData = new KeyData();
            keyData->name = iter->first;
            iter->second->GetColour(&r, &g, &b);
            keyData->r = r;
            keyData->g = g;
            keyData->b = b;
            m_KeyDataList.push_back(keyData);
        }
    }

    // get size of grid
    GLfloat w, h;
    g_Simulation.GetEnvironment()->GetGrid()->GetGridSquareDimensions(&w, &h);
    GLfloat textOffset = (h - m_TextSize) / 2;
    
    // first of all work out the number of lines
    int linesForTime = 2;
    int linesForSummary = 4;
    int linesForHabitatTypes = m_KeyDataList.size();

    GLfloat lineHeight = (m_Height - 2 * margin) / (GLfloat)(linesForTime + linesForSummary + linesForHabitatTypes + 2);
    
    // draw the time
    glPolygonMode(GL_FRONT, GL_FILL);
    if (g_Simulation.IsNightTime())
    {
        r = g = b = 0.0;
    }
    else
    {
        r = g = b = 1.0;
    }
    y = m_YOrigin + m_Height - h - margin;
    x = m_XOrigin + margin;
    glPolygonMode(GL_FRONT, GL_FILL);
    DrawRectangle(x, y, w, h, r, g, b);
    glPolygonMode(GL_FRONT, GL_LINE);
    DrawRectangle(x, y, w, h, 1, 0, 0);
    sprintf(text, "Date: %s %d", months[g_Simulation.GetMonth() ],
            g_Simulation.GetDay() + 1);
    GLOutputText(x + w * 1.5, y + textOffset, m_TextSize, m_TextSize, 1, 1, 1, text);
    y -= lineHeight;
    int seconds = (int)g_Simulation.GetTimeOfDay();
    int minutes = seconds / 60;
    int hours = minutes / 60;
    sprintf(text, "Time: %02d:%02d:%02d", hours, minutes % 60, seconds % 60);
    GLOutputText(x + w * 1.5, y + textOffset, m_TextSize, m_TextSize, 1, 1, 1, text);

    // draw the summary
    // foraging, socialising, resting, moving
    y -= lineHeight;
    sprintf(text, "Foraging: %d", g_Simulation.GetNumberOfAgentsForaging());
    GLOutputText(x + w * 1.5, y + textOffset, m_TextSize, m_TextSize, 1, 1, 1, text);
    y -= lineHeight;
    sprintf(text, "Socialising: %d", g_Simulation.GetNumberOfAgentsSocialising());
    GLOutputText(x + w * 1.5, y + textOffset, m_TextSize, m_TextSize, 1, 1, 1, text);
    y -= lineHeight;
    sprintf(text, "Resting: %d", g_Simulation.GetNumberOfAgentsResting());
    GLOutputText(x + w * 1.5, y + textOffset, m_TextSize, m_TextSize, 1, 1, 1, text);
    y -= lineHeight;
    sprintf(text, "Moving: %d", g_Simulation.GetNumberOfAgentsMoving());
    GLOutputText(x + w * 1.5, y + textOffset, m_TextSize, m_TextSize, 1, 1, 1, text);
    

    // draw the key
    y = m_YOrigin + margin + linesForHabitatTypes * lineHeight;
    for (i = 0; i < linesForHabitatTypes; i++)
    {
        y -= lineHeight;
        keyData = m_KeyDataList[i];
        glPolygonMode(GL_FRONT, GL_FILL);
        DrawRectangle(x, y, w, h, keyData->r, keyData->g, keyData->b);
        glPolygonMode(GL_FRONT, GL_LINE);
        DrawRectangle(x, y, w, h, 1, 0, 0);
        GLOutputText(x + w * 1.5, y + textOffset, m_TextSize, m_TextSize, 1, 1, 1, keyData->name.c_str());
    }

    // draw a frame
    r = g = b = 1.0;
    glPolygonMode(GL_FRONT, GL_LINE);
    DrawRectangle(m_XOrigin, m_YOrigin, m_Width, m_Height, r, g, b);
    
}



#endif


