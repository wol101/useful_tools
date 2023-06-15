// Bar - class defining a bar that can be used for grasping

#include <math.h>
#include <glLoadModels.h>
#include <gl.h>
#include "Bar.h"

#define RADTODEG (180 / M_PI)

// constructor
Bar::Bar(double xStart, double yStart, double zStart,
	 double xEnd, double yEnd, double zEnd, double radius)
{
	// line equation is (x0 +gamma l, y0 + gamma m, z0 + gamma n)
	// taken from Lyon 1995 P30 (Everything you ever wanted to know...)

	m_x0 = xStart;
	m_y0 = yStart;
	m_z0 = zStart;
	m_l = xEnd - xStart;
	m_m = yEnd - yStart;
	m_n = zEnd - zStart;
	m_radius = radius;

	// preinitialise transformations

	m_xs = sqrt(m_l * m_l + m_m * m_m + m_n * m_n);
	m_yr = RADTODEG * atan2(m_n, m_l);
	m_zr = RADTODEG * atan2(m_m, m_l);

	m_model = 0;
	m_XContactPoint = 0;
}

Bar::Bar(char *fileName)
{
	double xStart, yStart, zStart;
	double xEnd, yEnd, zEnd;
	double radius;
	ifstream inFile(fileName);

	inFile >> xStart >> yStart >> zStart >> xEnd >> yEnd >> zEnd >> radius;
	inFile.close();

	// line equation is (x0 +gamma l, y0 + gamma m, z0 + gamma n)
	// taken from Lyon 1995 P30 (Everything you ever wanted to know...)

	m_x0 = xStart;
	m_y0 = yStart;
	m_z0 = zStart;
	m_l = xEnd - xStart;
	m_m = yEnd - yStart;
	m_n = zEnd - zStart;
	m_radius = radius;

	// preinitialise transformations

	m_xs = sqrt(m_l * m_l + m_m * m_m + m_n * m_n);
	m_yr = RADTODEG * atan2(m_n, m_l);
	m_zr = RADTODEG * atan2(m_m, m_l);

	m_model = 0;
	m_XContactPoint = 0;
}

// initialise graphics from .xan filename
void Bar::InitGraphics(char *filename)
{
	// load up file containing graphic
	m_model = dmGLLoadFile_xan(filename);
}

// find distance from axis to point
double Bar::DistanceFromAxis(double xp, double yp, double zp)
{
	double gamma;
	double distance;

	gamma = -((m_x0 - xp) * m_l + (m_y0 - yp) * m_m + (m_z0 - zp) * m_n) /
		(m_l * m_l + m_m * m_m + m_n * m_n);

	distance = sqrt(pow(m_x0 - xp + gamma * m_l, 2) +
			pow(m_y0 - yp + gamma * m_m, 2) + pow(m_z0 - zp + gamma * m_n, 2));

	return distance;
}

// find distance from axis and return coordinates of the closest point on the
// axis and gamma (how far along bar)
double
	Bar::DistanceFromAxis(double xp, double yp, double zp,
			      double *xa, double *ya, double *za, double *gamma)
{
	double distance;

	*gamma = -((m_x0 - xp) * m_l + (m_y0 - yp) * m_m + (m_z0 - zp) * m_n) /
		(m_l * m_l + m_m * m_m + m_n * m_n);

	distance = sqrt(pow(m_x0 - xp + *gamma * m_l, 2) +
			pow(m_y0 - yp + *gamma * m_m, 2) + pow(m_z0 - zp + *gamma * m_n, 2));

	*xa = m_x0 + *gamma * m_l;
	*ya = m_y0 + *gamma * m_m;
	*za = m_z0 + *gamma * m_n;

	return distance;
}

// check to see whether a point is within the bar (flat ends assumed)
bool Bar::PointInBar(double xp, double yp, double zp)
{
	double distance;
	double xa, ya, za;
	double gamma;

	distance = DistanceFromAxis(xp, yp, zp, &xa, &ya, &za, &gamma);

	if (gamma < 0 || gamma > 1)
		return false;

	if (distance > m_radius)
		return false;

	return true;
}

// find point on surface where line through perpendicular point on bar would
// intersect with surface
void Bar::PointOnSurface(double xp, double yp, double zp, double *xs, double *ys, double *zs)
{
	double distance;
	double xa, ya, za;
	double gamma;
	double stretch;

	distance = DistanceFromAxis(xp, yp, zp, &xa, &ya, &za, &gamma);

	stretch = m_radius / distance;
	*xs = xa + stretch * (xp - xa);
	*ys = ya + stretch * (yp - ya);
	*zs = za + stretch * (zp - za);
}

// find just about all the information you could want
bool
	Bar::CalculateInformation(double xp, double yp, double zp,
				  double *distance,
				  double *xa, double *ya, double *za, double *gamma,
				  double *xs, double *ys, double *zs,
				  double *xn, double *yn, double *zn)
{
	// distance information

	*distance = DistanceFromAxis(xp, yp, zp, xa, ya, za, gamma);

	// calculate normal vector
	*xn = (xp - *xa) / *distance;
	*yn = (yp - *ya) / *distance;
	*zn = (zp - *za) / *distance;

	// point on surface

	*xs = *xa + m_radius * *xn;
	*ys = *ya + m_radius * *yn;
	*zs = *za + m_radius * *zn;


	// check for point within bar (flat ends assumed)

	if (*gamma < 0 || *gamma > 1)
		return false;

	if (*distance > m_radius)
		return false;

	return true;
}

void Bar::draw()
{
	// save transformation
	glPushMatrix();

	// tansform in reverse order
	glTranslatef(m_x0, m_y0, m_z0);		 // lastly translate
	glRotatef(m_zr, 0, 0, 1);		 // then about z
	glRotatef(m_yr, 0, 1, 0);		 // rotate about y first
	glScalef(m_xs, m_radius, m_radius);

	glCallList(m_model);

	// restore transformation
	glPopMatrix();
}
