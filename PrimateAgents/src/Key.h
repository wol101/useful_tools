/*
 *  Key.h
 *  PrimateAgent
 *
 *  Created by Bill Sellers on Sun Feb 06 2005.
 *  Copyright (c) 2005 Bill Sellers. All rights reserved.
 *
 */

#ifndef Key_h
#define Key_h

#include <string>
#include <vector>

struct KeyData
{
    string name;
    GLfloat r;
    GLfloat g;
    GLfloat b;
};

class Key
{
public:
    Key();
    ~Key();

    void SetSize(GLfloat xOrigin, GLfloat yOrigin, GLfloat width, GLfloat height)
    {
        m_XOrigin = xOrigin; m_YOrigin = yOrigin;
        m_Width = width; m_Height = height;
    };
    void SetTextSize(GLfloat textSize) { m_TextSize = textSize; };

    void Draw();

protected:

    GLfloat m_TextSize;
    GLfloat m_XOrigin;
    GLfloat m_YOrigin;
    GLfloat m_Width;
    GLfloat m_Height;

    bool m_FirstTime;
    vector<KeyData *> m_KeyDataList;
    
};


#endif


