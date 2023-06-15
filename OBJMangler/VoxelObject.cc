// VoxelObject.h - class to maintain a voxel based object
// generic but not terribly quick

#include <string.h>
#include <vector>

#include "VoxelObject.h"
#include "Intersection.h"

VoxelObject::VoxelObject()
{
    SetDefaultValues();
}

VoxelObject::VoxelObject(Vector inOrigin, int inNx, int inNy, int inNz,
            double inDelX, double inDelY, double inDelZ)
{
    SetDefaultValues();

    origin = inOrigin;
    nx = inNx;
    ny = inNy;
    nz = inNz;
    delX = inDelX;
    delY = inDelY;
    delZ = inDelZ;
    data = new unsigned char[nx * ny * nz];
    memset(data, (int)untested, nx * ny * nz);
}

VoxelObject::~VoxelObject()
{
    if (data) delete [] data;
    if (triangleList) delete [] triangleList;
}

void VoxelObject::SetDefaultValues()
{
    nx = 0;
    ny = 0;
    nz = 0;
    delX = 0;
    delY = 0;
    delZ = 0;
    data = 0;
    triangleList = 0;
    numberOfTriangles = 0;
    triangleListSize = 0;
}

void VoxelObject::AddTriangle(Triangle triangle)
{
    int i;
    if (numberOfTriangles >= triangleListSize)
    {
        triangleListSize += allocationIncrement;
        Triangle *newTriangleList = new Triangle[triangleListSize];
        if (triangleList)
        {
            for (i = 0; i < numberOfTriangles; i++)
                newTriangleList[i] = triangleList[i];
            delete [] triangleList;
        }
        triangleList = newTriangleList;
    }
    triangleList[numberOfTriangles] = triangle;
    numberOfTriangles++;
}

/*
void VoxelObject::DetectEdges(int x, int y, int z)
{
    static unsigned char *statusPtr; // made static to minimise stack usage
    static int i;

    // check for out of range first
    if (x < 0 || x >= nx) return;
    if (y < 0 || y >= ny) return;
    if (z < 0 || z >= nz) return;

    // get current status of this voxel
    statusPtr = data + (x + y * nx + z * nx * ny);

    // only do something if it has not been tested before
    if (*statusPtr != untested) return;

    // test to see if a triangle intersects the voxel
    Cuboid cuboid(Vector(origin.x + x * delX, origin.y + y * delY, origin.z + z * delZ),
                  Vector(delX, 0, 0), Vector(0, delY, 0), Vector(0, 0, delZ));
    *statusPtr = empty;
    for (i = 0; i < numberOfTriangles; i++)
    {
        if (Intersection(cuboid, triangleList[i]))
        {
            *statusPtr = edge;
            return;
        }
    }

    // now test neighboring voxels
    DetectEdges(x + 1, y, z);
    DetectEdges(x, y + 1, z);
    DetectEdges(x, y, z + 1);
    DetectEdges(x - 1, y, z);
    DetectEdges(x, y - 1, z);
    DetectEdges(x, y, z - 1);
}
*/

void VoxelObject::DetectEdges(int x, int y, int z)
{
    std::vector<Location> pursuitList;
    Location location(x, y, z);

    if (location.x >= 0 && location.x < nx &&
        location.y >= 0 && location.y < ny &&
        location.z >= 0 && location.z < nz && 
        data[location.x + location.y * nx + location.z * nx * ny] == untested)
    pursuitList.push_back(location);

    while (pursuitList.size())
    {
        // get and delete the last item from the list
        location = pursuitList.back();
        pursuitList.pop_back();

        // check to see whether it intersects with an edge
        if (TestIntersection(location.x, location.y, location.z))
        {
            data[location.x + location.y * nx + location.z * nx * ny] = edge;
        }
        else
        {
            data[location.x + location.y * nx + location.z * nx * ny] = empty;

            // now check neigbours
            if (location.x < nx - 1 &&
                data[(location.x + 1) + location.y * nx + location.z * nx * ny] == untested)
                pursuitList.push_back(Location(location.x + 1, location.y, location.z));
            if (location.y < ny - 1 &&
                data[location.x + (location.y + 1) * nx + location.z * nx * ny] == untested)
                pursuitList.push_back(Location(location.x, location.y + 1, location.z));
            if (location.z < nz - 1 &&
                data[location.x + location.y * nx + (location.z + 1) * nx * ny] == untested)
                pursuitList.push_back(Location(location.x, location.y, location.z + 1));
            if (location.x > 0 &&
                data[(location.x - 1) + location.y * nx + location.z * nx * ny] == untested)
                pursuitList.push_back(Location(location.x - 1, location.y, location.z));
            if (location.y > 0 &&
                data[location.x + (location.y - 1) * nx + location.z * nx * ny] == untested)
                pursuitList.push_back(Location(location.x, location.y - 1, location.z));
            if (location.z > 0 &&
                data[location.x + location.y * nx + (location.z - 1) * nx * ny] == untested)
                pursuitList.push_back(Location(location.x, location.y, location.z - 1));
        }
    }
}

// test for a triangle voxel intersection
bool VoxelObject::TestIntersection(int x, int y, int z)
{
    int i;
    Cuboid cuboid(Vector(origin.x + x * delX, origin.y + y * delY,
                         origin.z + z * delZ),
                  Vector(delX, 0, 0), Vector(0, delY, 0), Vector(0, 0, delZ));
    
    for (i = 0; i < numberOfTriangles; i++)
    {
        if (Intersection(cuboid, triangleList[i]))
        {
            return true;
        }
    }
    return false;
}

// return a cuboid created out of a voxel
Cuboid *VoxelObject::GetCuboid(int x, int y, int z, unsigned char *status)
{
    // check for out of range first
    if (x < 0 && x >= nx) return 0;
    if (y < 0 && y >= ny) return 0;
    if (z < 0 && z >= nz) return 0;
    
    // create the new cuboid
    Cuboid *cuboid = new Cuboid(Vector(origin.x + x * delX,
                                       origin.y + y * delY, origin.z + z * delZ),
                                Vector(delX, 0, 0), Vector(0, delY, 0),
                                Vector(0, 0, delZ));

    *status = data[x + y * nx + z * nx * ny];

    return cuboid;
}

