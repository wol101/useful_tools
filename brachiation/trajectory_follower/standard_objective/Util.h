#ifndef __UTIL_H__
#define __UTIL_H__

#include <math.h>

#define square(a) ((a) * (a))

class Util {
      public:

	// INVERSION OF A MATRIX WITH GAUSS-JORDAN'S METHOD
	// modified from Numerical Recipes and fixed at 3 by 3
	// returns 0 if successful

	static int Gaussj3x3(double a[3][3]);

	// calculate cross product (vector product)
	inline static void CrossProduct3x1(const double *a, const double *b, double *c) {
		c[0] = a[1] * b[2] - a[2] * b[1];
		c[1] = a[2] * b[0] - a[0] * b[2];
		c[2] = a[0] * b[1] - a[1] * b[0];
	};

	// calculate dot product (scalar product)
	inline static double DotProduct3x1(const double *a, const double *b) {
		return a[0] * b[0] + a[1] * b[1] + a[2] * b[2];
	};

	// calculate length of vector
	inline static double Magnitude3x1(const double *a) {
		return sqrt(square(a[0]) + square(a[1]) + square(a[2]));
	};

};

const int NAME_SIZE = 256;

bool GetQuotedString(ifstream & in, char *string);
inline bool OutRange(double v, double l, double h);
bool ReadParameter(ifstream & inp, char *m, int &i);
bool ReadParameter(ifstream & inp, char *m, double &i);

inline bool OutRange(double v, double l, double h)
{
	if (v < l)
		return true;
	if (v > h)
		return true;
	return false;
}


#endif						 // __UTIL_H__
