// VoxelObject.h - class to maintain a voxel based object
// generic but not terribly quick

#ifndef __VoxelObject_h__
#define __VoxelObject_h__

#include "Shapes.h"

struct Location
{
    int x;
    int y;
    int z;

    Location ()
    {
        Location(0, 0, 0);
    }
    Location (int inx, int iny, int inz)
    {
        x = inx;
        y = iny;
        z = inz;
    };
};

class VoxelObject
{
public:
    VoxelObject();
    VoxelObject(Vector inOrigin, int inNx, int Ny, int Nz,
                double inDelX, double inDelY, double inDelZ);
    ~VoxelObject();

    void AddTriangle(Triangle triangle);
    void DetectEdges(int x, int y, int z);
    Cuboid *GetCuboid(int x, int y, int z, unsigned char *status);
    bool TestIntersection(int x, int y, int z);

    const static unsigned char untested = 0;
    const static unsigned char edge = 1;
    const static unsigned char empty = 2;
    
protected:
    void SetDefaultValues();

    Vector	origin;
    int		nx;
    int		ny;
    int		nz;
    double	delX;
    double	delY;
    double	delZ;

    unsigned char *data;
    Triangle *triangleList;
    int numberOfTriangles;
    int triangleListSize;

    const static int allocationIncrement = 1000;
    
};



#endif
