// Axis.h - draw a smart axis

#ifndef Axis_h
#define Axis_h

enum orientation
{
   top = 1,
   bottom = 2,
   left = 3,
   right = 4,
   centre = 5
};

struct Rect
{
  double left;
  double right;
  double bottom;
  double top;
};   
   
class Axis
{
public:
   static void Draw(Rect &theRect, double theMin, double theMax, 
      orientation orient, 
      double tickSize, double textWidth, double textHeight,
      char *label = 0, bool number = true, bool reverse = false);
   static char *ConvertToString(double v, int precision);
};

#endif
