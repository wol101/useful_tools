// Intersection.h
// routines to test the intersection of standard entities

#ifndef __Intersection_h__
#define __Intersection_h__

#include "Shapes.h"

bool Intersection(Cuboid cuboid, Triangle triangle);
bool Intersection(Cuboid cuboid, Vector P);
bool Intersection(Triangle triangle, Line edge);
bool Intersection(Rectangle rectangle, Line edge);
bool MeetingPoint(Plane plane, Line line, double *planeParameter0, double *planeParameter1, double *lineParameter);
bool Intersection(Box box1, Box box2);

#endif
