// Shapes.cpp
// Some useful geometric shapes and entities

#include <float.h>
#include <assert.h>
#include "Shapes.h"

// Note origins are at a vertex

Triangle::Triangle()
{
}

Triangle::Triangle(Vector inOrigin, Vector inSide0, Vector inSide1)
{
    origin = inOrigin;
    side0 = inSide0;
    side1 = inSide1;
}

Vector Triangle::GetVertex(int i)
{
    switch (i)
    {
        case 0:
            return origin;
            break;

        case 1:
            return origin + side0;
            break;

        case 2:
            return origin + side1;
            break;
    }
    assert(false);
    return origin;
}

Box Triangle::GetBox()
{
    Vector minCorner(DBL_MAX, DBL_MAX, DBL_MAX);
    Vector maxCorner(-DBL_MAX, -DBL_MAX, -DBL_MAX);
    Vector v;
    int i;

    for (i = 0; i < 3; i++)
    {
        v = GetVertex(i);
        if (v.x < minCorner.x) minCorner.x = v.x;
        if (v.y < minCorner.y) minCorner.y = v.y;
        if (v.z < minCorner.z) minCorner.z = v.z;
        if (v.x > maxCorner.x) maxCorner.x = v.x;
        if (v.y > maxCorner.y) maxCorner.y = v.y;
        if (v.z > maxCorner.z) maxCorner.z = v.z;
    }

    return Box(minCorner, maxCorner.x - minCorner.x, maxCorner.y - minCorner.y,
               maxCorner.z - minCorner.z);
}

Line Triangle::GetEdge(int i)
{
    switch (i)
    {
        case 0:
            return Line(origin, side0);
            break;

        case 1:
            return Line(origin + side0, side1 - side0);
            break;

        case 2:
            return Line(origin + side1, -side1);
            break;
    }
    assert(false);
    return Line(origin, side0);
}

Rectangle::Rectangle()
{
}

Rectangle::Rectangle(Vector inOrigin, Vector inSide0, Vector inSide1)
{
    origin = inOrigin;
    side0 = inSide0;
    side1 = inSide1;
}

Vector Rectangle::GetVertex(int i)
{
    switch (i)
    {
        case 0:
            return origin;
            break;

        case 1:
            return origin + side0;
            break;

        case 2:
            return origin + side0 + side1;
            break;

        case 3:
            return origin + side1;
            break;
    }
    assert(false);
    return origin;
}

Cuboid::Cuboid()
{
}

Cuboid::Cuboid(Vector inOrigin, Vector inSide0, Vector inSide1, Vector inSide2)
{
    origin = inOrigin;
    side0 = inSide0;
    side1 = inSide1;
    side2 = inSide2;
}

Vector Cuboid::GetVertex(int i)
{
    switch (i)
    {
        case 0:
            return origin;
            break;

        case 1:
            return origin + side0;
            break;

        case 2:
            return origin + side0 + side1;
            break;

        case 3:
            return origin + side1;
            break;

        case 4:
            return origin + side2;
            break;

        case 5:
            return origin + side0 + side2;
            break;

        case 6:
            return origin + side0 + side1 + side2;
            break;

        case 7:
            return origin + side1 + side2;
            break;
    }
    assert(false);
    return origin;
}

Box Cuboid::GetBox()
{
    Vector minCorner(DBL_MAX, DBL_MAX, DBL_MAX);
    Vector maxCorner(-DBL_MAX, -DBL_MAX, -DBL_MAX);
    Vector v;
    int i;

    for (i = 0; i < 8; i++)
    {
        v = GetVertex(i);
        if (v.x < minCorner.x) minCorner.x = v.x;
        if (v.y < minCorner.y) minCorner.y = v.y;
        if (v.z < minCorner.z) minCorner.z = v.z;
        if (v.x > maxCorner.x) maxCorner.x = v.x;
        if (v.y > maxCorner.y) maxCorner.y = v.y;
        if (v.z > maxCorner.z) maxCorner.z = v.z;
    }

    return Box(minCorner, maxCorner.x - minCorner.x, maxCorner.y - minCorner.y,
               maxCorner.z - minCorner.z);
}

Line Cuboid::GetEdge(int i)
{
    switch (i)
    {
        case 0:
            return Line(origin, side0);
            break;

        case 1:
            return Line(origin + side0, side1);
            break;

        case 2:
            return Line(origin + side0 + side1, -side0);
            break;

        case 3:
            return Line(origin + side1, -side1);
            break;
            
        case 4:
            return Line(origin, side2);
            break;

        case 5:
            return Line(origin + side0, side2);
            break;

        case 6:
            return Line(origin + side0 + side1, side2);
            break;

        case 7:
            return Line(origin + side1, side2);
            break;
            
        case 8:
            return Line(origin + side2, side0);
            break;

        case 9:
            return Line(origin + side0 + side2, side1);
            break;

        case 10:
            return Line(origin + side0 + side1 + side2, -side0);
            break;

        case 11:
            return Line(origin + side1 + side2, -side1);
            break;
    }
    assert(false);
    return Line(origin, side0);
}

Rectangle Cuboid::GetFace(int i)
{
    switch (i)
    {
        case 0:
            return Rectangle(origin, side0, side1);
            break;

        case 1:
            return Rectangle(origin, side1, side2);
            break;

        case 2:
            return Rectangle(origin, side2, side0);
            break;

        case 3:
            return Rectangle(origin + side0 + side1 + side2, -side1, -side0);
            break;

        case 4:
            return Rectangle(origin + side0 + side1 + side2, -side2, -side1);
            break;

        case 5:
            return Rectangle(origin + side0 + side1 + side2, -side0, -side2);
            break;

    }
    assert(false);
    return Rectangle(origin, side0, side1);;
}

Box::Box(Vector inOrigin, double xLen, double yLen, double zLen)
{
    assert(xLen >= 0);
    assert(yLen >= 0);
    assert(zLen >= 0);
    
    origin = inOrigin;
    side0.x = xLen;
    side0.y = 0;
    side0.z = 0;
    side1.x = 0;
    side1.y = yLen;
    side1.z = 0;
    side2.x = 0;
    side2.y = 0;
    side2.z = zLen;
}

Line::Line()
{
}

Line::Line(Vector inPoint, Vector inDirection)
{
    point = inPoint;
    direction = inDirection;
}

Plane::Plane()
{
}

Plane::Plane(Vector inPoint, Vector inDirection0, Vector inDirection1)
{
    point = inPoint;
    direction0 = inDirection0;
    direction1 = inDirection1;
}





