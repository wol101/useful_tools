// Ladder.cc - dynamechs object representing the ladder that is used for 
// climbing

#include <gl.h>
#include <math.h>

#include "Ladder.h"
#include "Util.h"
#include "LoadObj.h"

// intialise the ladder 
Ladder::Ladder()
{
   m_X = 0;
   m_Y = 0;
   m_Z = 0;
   m_Width = 1;
   m_Height = 10;
   m_Spacing = 0.5;
   m_Radius = 0.05;
   
   m_Faces = 32;
   m_GraphicsInitialised = false;
}

// delete the ladder
Ladder::~Ladder()
{
   if (m_GraphicsInitialised)
   {
      // delete the display list
      glDeleteLists(m_UnitCylinder, 1);
   }
}

// initialise the predrawing needed for rungs
void Ladder::InitialiseGraphics()
{ 
   // draw a unit cylinder
   m_UnitCylinder = glGenLists(1);
   glNewList(m_UnitCylinder, GL_COMPILE);

   glPolygonMode(GL_FRONT, GL_FILL);
   //glPolygonMode(GL_BACK, GL_LINE);
   glPolygonMode(GL_BACK, GL_FILL);

   // hardwired material properties

   SetGLColour(0, 157.0/255.0, 186.0/255.0,
      0.0, 0.9, 0.9,
      10);

   GLfloat inc = 2 * M_PI / (double)m_Faces;
   GLfloat normal[3];
   GLfloat y1, y2, z1, z2;
   int i;
   for (i = 0; i < m_Faces; i++)
   {
      y1 = cos((double)i * inc);
      z1 = sin((double)i * inc);
      y2 = cos((double)(i + 1) * inc);
      z2 = sin((double)(i + 1) * inc);
      glBegin(GL_POLYGON);
      //normal[1] = cos(((double)i + 0.5) * inc);
      //normal[2] = sin(((double)i + 0.5) * inc);
      normal[0] = 0;
      normal[1] = y1;
      normal[2] = z1;
      glNormal3fv(normal);
      glVertex3f(1, y1, z1);
      glVertex3f(0, y1, z1);
      glVertex3f(0, y2, z2);
      glVertex3f(1, y2, z2);
      glEnd();
   }

   // ends
   glBegin(GL_POLYGON);
   normal[0] = -1;
   normal[1] = 0;
   normal[2] = 0;
   glNormal3fv(normal);
   for (i = m_Faces - 1; i >= 0; i--)
   {
      y1 = cos((double)i * inc);
      z1 = sin((double)i * inc);
      glVertex3f(0, y1, z1);
   }
   glEnd();
   glBegin(GL_POLYGON);
   normal[0] = 1;
   normal[1] = 0;
   normal[2] = 0;
   glNormal3fv(normal);
   for (i = 0; i < m_Faces; i++)
   {
      y1 = cos((double)i * inc);
      z1 = sin((double)i * inc);
      glVertex3f(1, y1, z1);
   }
   glEnd();

   glEndList();
   m_GraphicsInitialised = true;
}

// set the various size parameters
void Ladder::SetDimensions(double x, double y, double z,
      double width, double height, double spacing, double radius)            
{
   m_X = x;
   m_Y = y;
   m_Z = z;
   m_Width = width;
   m_Height = height;
   m_Spacing = spacing;
   m_Radius = radius;
}

// draw the ladder
void Ladder::draw()
{
   double z = m_Z + m_Spacing / 2;
   
   glPushMatrix();
   
   while (z < m_Height)
   {
      glPushMatrix();
      glTranslatef(m_X, m_Y, z);
      glRotatef(90, 0, 0, 1);
      glScalef(m_Width, m_Radius, m_Radius);
      glCallList(m_UnitCylinder);
      z += m_Spacing;
      glPopMatrix();
   }
   glPopMatrix();
}

// get the rung penetration depth and the direction normal
// searches all the rungs...
// returns zero if no penetration
double Ladder::GetDepth(double point[3], 
  double surfacePosition[3], double normal[3])
{
   double z = m_Z + m_Spacing / 2;
   double distanceFromCentre;
   
   // check bounds
   
   if (point[0] < m_X - m_Radius || point[0] > m_X + m_Radius) return 0;
   if (point[1] < m_Y || point[1] > m_Y + m_Width) return 0;
   if (point[2] < m_Z || point[2] > m_Z + m_Height) return 0;
   
   normal[1] = 0;
   while (z < m_Height)
   {
      normal[0] = point[0] -  m_X;
      normal[2] = point[2] - z;
      distanceFromCentre = sqrt(Util::Square(normal[0]) + 
          Util::Square(normal[2]));
      if (distanceFromCentre <= m_Radius)
      {
         normal[0] /= distanceFromCentre;
         normal[2] /= distanceFromCentre;
         surfacePosition[0] = m_X + m_Radius * normal[0];
         surfacePosition[1] = point[1];
         surfacePosition[2] = z + m_Radius * normal[2];
         return m_Radius - distanceFromCentre;
      }
      
      z += m_Spacing;
   }
   return 0;
}      
      
      
