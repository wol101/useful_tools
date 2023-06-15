// Shapes.h - a collection of useful shape classes

#ifndef __Shapes_h__
#define __Shapes_h__

#include "mymath.h"

// Parametric line
class Line
{
public:
    Line();
    Line(Vector inPoint, Vector inDirection);

    Vector point;
    Vector direction;
};

// Parametric plane
class Plane
{
public:
    Plane();
    Plane(Vector inPoint, Vector inDirection0, Vector inDirection1);

    Vector point;
    Vector direction0;
    Vector direction1;
};

class Box;
class Rectangle;

// 3D arbitrary cuboid
class Cuboid
{
public:
    Cuboid();
    Cuboid(Vector inOrigin, Vector inSide0, Vector inSide1, Vector inSide2);
    Vector GetVertex(int i);
    Line GetEdge(int i);
    Rectangle GetFace(int i);
    Box GetBox();

    Vector origin;
    Vector side0;
    Vector side1;
    Vector side2;
};

// cuboid with faces parallel to the 2 axis planes
class Box: public Cuboid
{
public:
    Box(Vector inOrigin, double xLen, double yLen, double zLen);
};

// 2D arbitrary triangle
class Triangle
{
public:
    Triangle();
    Triangle(Vector inOrigin, Vector inSide0, Vector inSide1);
    Vector GetVertex(int i);
    Line GetEdge(int i);
    Box GetBox();

    Vector origin;
    Vector side0;
    Vector side1;
};

// 2D arbitrary rectangle
class Rectangle
{
public:
    Rectangle();
    Rectangle(Vector inOrigin, Vector inSide0, Vector inSide1);
    Vector GetVertex(int i);

    Vector origin;
    Vector side0;
    Vector side1;
};


#endif 
