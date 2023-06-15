// Intersection.cpp
// routines to test the intersection of standard entities

#include <math.h>
#include "Intersection.h"

bool Intersection(Cuboid cuboid, Triangle triangle)
{
    int i, j;

    // for speed, given that no intersection is the most common condition, check bounding box
    // overlap first
    if (Intersection(cuboid.GetBox(), triangle.GetBox())) return false;
    
    // first test whether vertices of triangle are within cube
    for (i = 0; i < 3; i++)
    {
        // if (Intersection(cuboid, triangle.GetVertex(i))) return true;
    }
    
    // then test whether any of the triangle edges intersect the cuboid faces

    for (i = 0; i < 3; i++)
    {
        for (j = 0; j < 6; j++)
        {
            if (Intersection(cuboid.GetFace(j), triangle.GetEdge(i))) return true;
        }
    }

    // finally test whether any of the cuboid edges intersects the triangle faces

    for (i = 0; i < 12; i++)
    {
        if (Intersection(triangle, cuboid.GetEdge(i))) return true;
    }

    return false;
}

bool Intersection(Cuboid cuboid, Vector P)
{
    // Using formula from Mathematica

    double x = P.x;
    double y = P.y;
    double z = P.z;
    double Ox = cuboid.origin.x;
    double Oy = cuboid.origin.y;
    double Oz = cuboid.origin.z;
    double Ax = cuboid.side0.x;
    double Ay = cuboid.side0.y;
    double Az = cuboid.side0.z;
    double Bx = cuboid.side1.x;
    double By = cuboid.side1.y;
    double Bz = cuboid.side1.z;
    double Cx = cuboid.side2.x;
    double Cy = cuboid.side2.y;
    double Cz = cuboid.side2.z;
    
    double da = (-(Az*By*Cx) + Ay*Bz*Cx + Az*Bx*Cy -
                 Ax*Bz*Cy - Ay*Bx*Cz + Ax*By*Cz);
    if (fabs(da) < 1e-30) return false;
    
    double db = (Az*By*Cx - Ay*Bz*Cx - Az*Bx*Cy +
                 Ax*Bz*Cy + Ay*Bx*Cz - Ax*By*Cz);
    if (fabs(db) < 1e-30) return false;

    double dc = (-(Az*By*Cx) + Ay*Bz*Cx + Az*Bx*Cy -
                 Ax*Bz*Cy - Ay*Bx*Cz + Ax*By*Cz);
    if (fabs(dc) < 1e-30) return false;

    double a = -((-(Bz*Cy*Ox) + By*Cz*Ox +
                  Bz*Cx*Oy - Bx*Cz*Oy - By*Cx*Oz +
                  Bx*Cy*Oz + Bz*Cy*x - By*Cz*x -
                  Bz*Cx*y + Bx*Cz*y + By*Cx*z - Bx*Cy*z
                  )/ da );

    double b = -((-(Az*Cy*Ox) + Ay*Cz*Ox +
                  Az*Cx*Oy - Ax*Cz*Oy - Ay*Cx*Oz +
                  Ax*Cy*Oz + Az*Cy*x - Ay*Cz*x -
                  Az*Cx*y + Ax*Cz*y + Ay*Cx*z - Ax*Cy*z
                  )/ db );

    double c = -((-(Az*By*Ox) + Ay*Bz*Ox +
                  Az*Bx*Oy - Ax*Bz*Oy - Ay*Bx*Oz +
                  Ax*By*Oz + Az*By*x - Ay*Bz*x -
                  Az*Bx*y + Ax*Bz*y + Ay*Bx*z - Ax*By*z
                  )/ dc );

    if (a >= 0 && a <= 1 && b >= 0 && b <= 1 && c >= 0 && c <= 1) return true;

    return false;
}

bool Intersection(Triangle triangle, Line edge)
{
    Plane facePlane(triangle.origin, triangle.side0, triangle.side1);
    double planeParameter0, planeParameter1, lineParameter;

    if (MeetingPoint(facePlane, edge, &planeParameter0, &planeParameter1, &lineParameter) == false) return false;

    if (planeParameter0 >= 0 && planeParameter1 >= 0 && planeParameter0 + planeParameter1 <= 1
        && lineParameter >= 0 && lineParameter <= 1) return true;

    return false;
}

bool Intersection(Rectangle rectangle, Line edge)
{
    Plane facePlane(rectangle.origin, rectangle.side0, rectangle.side1);
    double planeParameter0, planeParameter1, lineParameter;

    if (MeetingPoint(facePlane, edge, &planeParameter0, &planeParameter1, &lineParameter) == false) return false;

    if (planeParameter0 >= 0 && planeParameter1 >= 0 && planeParameter0 <= 1 && planeParameter1 <= 1
        && lineParameter >= 0 && lineParameter <= 1) return true;

    return false;
}

// This seems to work - it may need checking
bool Intersection(Box box1, Box box2)
{
    Vector b1_min = box1.origin;
    Vector b1_max = box1.GetVertex(6);
    Vector b2_min = box2.origin;
    Vector b2_max = box2.GetVertex(6);

    if (b1_max.x < b2_min.x) return true;
    if (b1_max.y < b2_min.y) return true;
    if (b1_max.z < b2_min.z) return true;
    if (b2_max.x < b1_min.x) return true;
    if (b2_max.y < b1_min.y) return true;
    if (b2_max.z < b1_min.z) return true;

    return false;
}

// calculate where a line and a plane meet
// returns false if they never meet (i.e. parallel)
bool MeetingPoint(Plane plane, Line line, double *planeParameter0, double *planeParameter1, double *lineParameter)
{
    // Using formula from Mathematica

    double olx = line.point.x;
    double oly = line.point.y;
    double olz = line.point.z;
    double lx = line.direction.x;
    double ly = line.direction.y;
    double lz = line.direction.z;
    double opx = plane.point.x;
    double opy = plane.point.y;
    double opz = plane.point.z;
    double p1x = plane.direction0.x;
    double p1y = plane.direction0.y;
    double p1z = plane.direction0.z;
    double p2x = plane.direction1.x;
    double p2y = plane.direction1.y;
    double p2z = plane.direction1.z;
    
    // calculate denominators
    double da = (-(lz*p1y*p2x) + ly*p1z*p2x +
                 lz*p1x*p2y - lx*p1z*p2y -
                 ly*p1x*p2z + lx*p1y*p2z);
    if (fabs(da) < 1e-30) return false;

    double db = (lz*p1y*p2x - ly*p1z*p2x - lz*p1x*p2y +
                 lx*p1z*p2y + ly*p1x*p2z - lx*p1y*p2z);
    if (fabs(db) < 1e-30) return false;

    double dc = (-(lz*p1y*p2x) + ly*p1z*p2x +
                 lz*p1x*p2y - lx*p1z*p2y -
                 ly*p1x*p2z + lx*p1y*p2z);
    if (fabs(dc) < 1e-30) return false;


    *lineParameter = -((-(olz*p1y*p2x) +
                        opz*p1y*p2x + oly*p1z*p2x -
                        opy*p1z*p2x + olz*p1x*p2y -
                        opz*p1x*p2y - olx*p1z*p2y +
                        opx*p1z*p2y - oly*p1x*p2z +
                        opy*p1x*p2z + olx*p1y*p2z -
                        opx*p1y*p2z) / (da));

    *planeParameter0 = -((-(lz*oly*p2x) + ly*olz*p2x +
                          lz*opy*p2x - ly*opz*p2x +
                          lz*olx*p2y - lx*olz*p2y -
                          lz*opx*p2y + lx*opz*p2y -
                          ly*olx*p2z + lx*oly*p2z +
                          ly*opx*p2z - lx*opy*p2z) / (db));

    *planeParameter1 = -((-(lz*oly*p1x) + ly*olz*p1x +
                          lz*opy*p1x - ly*opz*p1x +
                          lz*olx*p1y - lx*olz*p1y -
                          lz*opx*p1y + lx*opz*p1y -
                          ly*olx*p1z + lx*oly*p1z +
                          ly*opx*p1z - lx*opy*p1z) / (dc));

    return true;
}


