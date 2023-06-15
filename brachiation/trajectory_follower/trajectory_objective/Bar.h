// Bar - class defining a bar that can be used for grasping

#ifndef __BAR_H__
#define __BAR_H__

#include <gl.h>

class Bar {
      public:

	// constructor
	Bar(double xStart, double yStart, double zStart,
	    double xEnd, double yEnd, double zEnd, double radius);

	 Bar(char *filename);

	// initialise graphics

	void InitGraphics(char *filename);

	// find distance from axis to point
	double DistanceFromAxis(double xp, double yp, double zp);

	// find distance from axis and return coordinates of the closest point on the
	// axis and gamma (how far along bar)
	double DistanceFromAxis(double xp, double yp, double zp,
				double *xa, double *ya, double *za, double *gamma);

	// check to see whether a point is within the bar (flat ends assumed)
	bool PointInBar(double xp, double yp, double zp);

	// find point on surface where line through perpendicular point on bar would
	// intersect with surface
	void PointOnSurface(double xp, double yp, double zp, double *xs, double *ys, double *zs);

	// find just about all the information you could want
	bool CalculateInformation(double xp, double yp, double zp,
				  double *distance,
				  double *xa, double *ya, double *za,
				  double *gamma, double *xs, double *ys,
				  double *zs, double *xn, double *yn, double *zn);

	double GetRadius() { return m_radius;
	};
	void GetStart(double *x, double *y, double *z) {
		*x = m_x0;
		*y = m_y0;
		*z = m_z0;
	};
	void GetEnd(double *x, double *y, double *z) {
		*x = m_x0 + m_l;
		*y = m_y0 + m_m;
		*z = m_z0 + m_n;
	};

	// for scoring
	double GetXContact(void) {
		return m_XContactPoint;
	};
	void SetXContact(double x) {
		m_XContactPoint = x;
	};

	// for visualisation

	void draw();

      protected:
// line equation is (x0 +gamma l, y0 + gamma m, z0 + gamma n)// taken from Lyon 1995 P30 (Everything you ever wnated to know...)
		double m_x0;
	double m_y0;
	double m_z0;
	double m_l;
	double m_m;
	double m_n;

	double m_radius;

	// for drawing

	GLint m_model;
	// tranformation values
	GLfloat m_xs;				 // scale
	GLfloat m_yr, m_zr;			 // rotation

	double m_XContactPoint;

};

#endif						 // rotationation values P30 (Everything you ever wnated to know...)bar woulde
__BAR_H__
