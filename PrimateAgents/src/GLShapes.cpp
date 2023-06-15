/*
 *  GLShapes.cpp
 *  PrimateAgent
 *
 *  Created by Bill Sellers on Fri Jan 28 2005.
 *  Copyright (c) 2005 Bill Sellers. All rights reserved.
 *
 */

#ifdef USE_OPENGL

#include <glut.h>

#include "GLShapes.h"

void DrawRectangle(GLfloat x, GLfloat y, GLfloat width, GLfloat height, GLfloat r, GLfloat g, GLfloat b)
{
    glDisable(GL_LIGHTING);
    glBegin(GL_POLYGON);
    glColor3f(r, g, b);
    glVertex2f(x, y);
    glVertex2f(x + width, y);
    glVertex2f(x + width, y + height);
    glVertex2f(x, y + height);
    glEnd();
    glEnable(GL_LIGHTING);
}

void DrawCross(GLfloat x, GLfloat y, GLfloat width, GLfloat height, GLfloat r, GLfloat g, GLfloat b)
{
    glDisable(GL_LIGHTING);
    glBegin(GL_LINES);
    glColor3f(r, g, b);
    glVertex2f(x + width / 2, y);
    glVertex2f(x + width / 2, y + height);
    glEnd();
    glBegin(GL_LINES);
    glColor3f(r, g, b);
    glVertex2f(x, y + height / 2);
    glVertex2f(x + width, y + height / 2);
    glEnd();
    glEnable(GL_LIGHTING);
}

void GLOutputText(GLfloat x, GLfloat y, GLfloat width, GLfloat height, GLfloat r, GLfloat g, GLfloat b, const char *text)
{
    const char *p;
    glDisable(GL_LIGHTING);
    glColor3f(r, g, b);
    glPushMatrix();
    glTranslatef(x, y, 0);
    // characters are roughly 100 units so gScale accordingly
    glScalef( width / 100, height / 100, 1);
    for (p = text; *p; p++)
        glutStrokeCharacter(GLUT_STROKE_MONO_ROMAN, *p);
    glPopMatrix();
}

#endif


