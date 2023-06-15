// Ladder.h - dynamechs object representing the ladder that is used for 
// climbing

#ifndef Ladder_h
#define Ladder_h

#include <gl.h>

class Ladder
{
   public:
      Ladder();
      ~Ladder();
      
      void SetDimensions(double x, double y, double z, double width, 
            double height, double spacing, double radius);            
      
      double GetDepth(double point[3], double surfacePosition[3], 
        double normal[3]);

      void InitialiseGraphics();
      void draw();
         
   protected:
      
      double m_X;
      double m_Y;
      double m_Z;
      double m_Width;
      double m_Height;
      double m_Spacing;
      double m_Radius;
      int m_Faces;
      
      bool m_GraphicsInitialised;
      GLuint m_UnitCylinder;  
};


#endif // Ladder_h
