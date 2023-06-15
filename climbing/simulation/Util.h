#ifndef __UTIL_H__
#define __UTIL_H__

#include <math.h>
#include <stdio.h>
#include <iostream.h>
#include <iomanip.h>
#include <fstream.h>

class Util {
   public:

   // INVERSION OF A MATRIX WITH GAUSS-JORDAN'S METHOD
   // modified from Numerical Recipes and fixed at 3 by 3
   // returns 0 if successful

   static int Gaussj3x3(double a[3][3]);

   // calculate cross product (vector product)
   inline static void CrossProduct3x1(const double *a, const double *b, double *c) 
   {
      c[0] = a[1] * b[2] - a[2] * b[1];
      c[1] = a[2] * b[0] - a[0] * b[2];
      c[2] = a[0] * b[1] - a[1] * b[0];
   };

   // calculate dot product (scalar product)
   inline static double DotProduct3x1(const double *a, const double *b)
   {
      return a[0] * b[0] + a[1] * b[1] + a[2] * b[2];
   };

   // calculate length of vector
   inline static double Magnitude3x1(const double *a) 
   {
      return sqrt(Square(a[0]) + Square(a[1]) + Square(a[2]));
   };

   // calculate distance between two points
   inline static double Distance3x1(const double *a, const double *b) 
   {
      return sqrt(Square(a[0] - b[0]) + Square(a[1] - b[1]) + Square(a[2] - b[2]));
   };
            
   // calculate unit vector
   inline static void Unit3x1(double *a) 
   {
      double len = sqrt(Square(a[0]) + Square(a[1]) + Square(a[2]));
      a[0] /= len;
      a[1] /= len;
      a[2] /= len;
   };

   // c = a + b vectors
   inline static void Add3x1(const double *a, const double *b, double *c) 
   {
      c[0] = a[0] + b[0];
      c[1] = a[1] + b[1];
      c[2] = a[2] + b[2];
   };

   // c = a - b vectors
   inline static void Subtract3x1(const double *a, const double *b, double *c) 
   {
      c[0] = a[0] - b[0];
      c[1] = a[1] - b[1];
      c[2] = a[2] - b[2];
   };

   // c = scalar * a 
   inline static void ScalarMultiply3x1(const double scalar, const double *a, double *c) 
   {
      c[0] = a[0] * scalar;
      c[1] = a[1] * scalar;
      c[2] = a[2] * scalar;
   };
   
   // b = a 
   inline static void Copy3x1(const double *a, double *b) 
   {
      b[0] = a[0];
      b[1] = a[1];
      b[2] = a[2];
   };
   
   static bool GetQuotedString(ifstream & in, char *string);
   static bool ReadParameter(ifstream & inp, char *m, int &i);
   static bool ReadParameter(ifstream & inp, char *m, double &i);
   static bool ReadParameter(ifstream & inp, char *m, char *i);
   static int my_system (char *command);
   static void MDHTransform(double a, double alpha, double d, double theta, 
      double *location);

   inline static bool OutRange(double v, double l, double h)
   {
      if (v < l)
      return true;
      if (v > h)
      return true;
      return false;
   }
   
   inline static double Square(double x)
   {
      return x * x;
   }
};



#endif                   // __UTIL_H__
