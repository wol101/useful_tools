#include <math.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include <gl.h>

#include "Axis.h"
#include "StrokeFont.h"

void Axis::Draw(Rect &theRect, double theMin, double theMax, 
  orientation orient, double tickSize, double textWidth, double textHeight, 
  char *label, bool number, bool reverse)
{
   double   p10;
   double   range;
   double   subRange;
   double   ticks[10];
   double   guess;
   double   theTicks[10];
   double   theRange;
   int      i;
   int      nTicks;
   int      precision;
   char     *buf;
   
   assert(theMin != theMax);
   
   // calculate positions for tick marks
   
   range = theMax - theMin;
   
   // find power of ten that is smaller than the range
   
   p10 = floor(log(range) / log (10));
   subRange = pow(10, p10);
   
   // is this a reasonable value?
   
   guess = range / subRange;
   if (guess > 7) subRange *= 2;
   if (guess < 4 && guess > 1.6) subRange /= 2;
   if (guess <= 1.6) subRange /= 5;
   
   precision = -((int)floor(log(range) / log (10)));
   if (precision < 0) precision = 0;
   
   // now position of first tick
   
   if (theMin == 0) ticks[0] = 0;
   else
   {
      if (theMin > 0)
      {
         i = (int)(0.5 + theMin / subRange);
         ticks[0] = (double)i * subRange;
         if (ticks[0] < theMin) ticks[0] += subRange;
      }
      else
      {
         i = (int)(theMin / subRange - 0.5);
         ticks[0] = (double)i * subRange;
         if (ticks[0] < theMin) ticks[0] += subRange;
      }
   }      
   
   nTicks = 0;
   while (ticks[nTicks] <= theMax)      // rounding error could be a problem here
   {
      ticks[nTicks + 1] = ticks[nTicks] + subRange;
      nTicks++;
   }
   
   if (orient == left || orient == right)
   {
      theRange = theRect.top - theRect.bottom;
      for (i = 0; i < nTicks; i++)
      {
         if (reverse)
            theTicks[i] = theRect.top - ((theRect.bottom +
            ((((ticks[i] - theMin) / range) * theRange)))
            - theRect.bottom);
         else
            theTicks[i] = theRect.bottom +
            ((((ticks[i] - theMin) / range) * theRange));
      }
   }
   else
   {
      theRange = theRect.right - theRect.left;
      for (i = 0; i < nTicks; i++)
      {
         if (reverse)
            theTicks[i] = theRect.right - ((theRect.left +
            ((((ticks[i] - theMin) / range) * theRange)))
            - theRect.left);
         else
            theTicks[i] = theRect.left +
            ((((ticks[i] - theMin) / range) * theRange));
      }
   }
      
   switch (orient)
   {
      case right:
      {
         glBegin(GL_LINES);
         glVertex2f(theRect.right, theRect.top);
         glVertex2f(theRect.right, theRect.bottom);
         glEnd();
         for (i = 0; i < nTicks; i++)
         {
            glBegin(GL_LINES);
            glVertex2f(theRect.right + tickSize, theTicks[i]);
            glVertex2f(theRect.right, theTicks[i]);
            glEnd();
            if (number)
            {
                buf = ConvertToString(ticks[i], precision);
                StrokeString(buf, strlen(buf),
                    theRect.right + tickSize * 1.5, theTicks[i],
                    textWidth, textHeight, 0, 1);
            }
            if (label)
                StrokeString(label, strlen(label), 
                    theRect.right + tickSize * 1.5 + textWidth * 5,
                    (theRect.top - theRect.bottom) / 2, 
                    textWidth, textHeight, 2, 1, 1);
         }
         break;
      }
      
      case bottom:
      {
         glBegin(GL_LINES);
         glVertex2f(theRect.left, theRect.bottom);
         glVertex2f(theRect.right, theRect.bottom);
         glEnd();
         for (i = 0; i < nTicks; i++)
         {
            glBegin(GL_LINES);
            glVertex2f(theTicks[i], theRect.bottom - tickSize);
            glVertex2f(theTicks[i], theRect.bottom);
            glEnd();
            if (number)
            {
                buf = ConvertToString(ticks[i], precision);
                StrokeString(buf, strlen(buf),
                    theTicks[i], theRect.bottom - tickSize * 1.5,
                    textWidth, textHeight, 1, 2);
            }
            if (label)
                StrokeString(label, strlen(label), 
                    (theRect.right - theRect.left) / 2, 
                    theRect.bottom - tickSize * 1.5 - textHeight * 1.5,
                    textWidth, textHeight, 1, 2);
         }
         break;
      }
   
      case left:
      {
         glBegin(GL_LINES);
         glVertex2f(theRect.left, theRect.top);
         glVertex2f(theRect.left, theRect.bottom);
         glEnd();
         for (i = 0; i < nTicks; i++)
         {
            glBegin(GL_LINES);
            glVertex2f(theRect.left - tickSize, theTicks[i]);
            glVertex2f(theRect.left, theTicks[i]);
            glEnd();
            if (number)
            {
                buf = ConvertToString(ticks[i], precision);
                StrokeString(buf, strlen(buf),
                    theRect.left - tickSize, theTicks[i],
                    textWidth, textHeight, 2, 1);
            }
            if (label)
                StrokeString(label, strlen(label), 
                    theRect.left - tickSize - textWidth * 5,
                    (theRect.top - theRect.bottom) / 2, 
                    textWidth, textHeight, 0, 1, 1);
         }
         break;
      }
      case top:
      {
         glBegin(GL_LINES);
         glVertex2f(theRect.left, theRect.top);
         glVertex2f(theRect.right, theRect.top);
         glEnd();
         for (i = 0; i < nTicks; i++)
         {
            glBegin(GL_LINES);
            glVertex2f(theTicks[i], theRect.top + tickSize);
            glVertex2f(theTicks[i], theRect.top);
            glEnd();
            if (number)
            {
                buf = ConvertToString(ticks[i], precision);
                StrokeString(buf, strlen(buf),
                    theTicks[i], theRect.top + tickSize * 1.5,
                    textWidth, textHeight, 1, 0);
            }
            if (label)
                StrokeString(label, strlen(label), 
                    (theRect.right - theRect.left) / 2, 
                    theRect.top + tickSize * 1.5 + textHeight * 1.5,
                    textWidth, textHeight, 1, 0);
         }
         break;
      }   
      case centre:
      {
         assert(true); // this is an error condition
      }
   }
}

char *Axis::ConvertToString(double v, int precision)
{
    static char buffer[64];
    
    if (precision == 0) sprintf(buffer, "%.0f", v);
    else if (precision == 1) sprintf(buffer, "%.1f", v);
    else if (precision == 2) sprintf(buffer, "%.2f", v);
    else if (precision == 3) sprintf(buffer, "%.3f", v);
    else if (precision == 4) sprintf(buffer, "%.4f", v);
    else if (precision == 5) sprintf(buffer, "%.5f", v);
    else if (precision == 6) sprintf(buffer, "%.6f", v);
    else sprintf(buffer, "%f", v);
    
    return buffer;
}
