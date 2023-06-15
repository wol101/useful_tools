// FacetedObject.cc -- maintain and draw a faceted object

#include <stdio.h>
#include <iostream>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <float.h>
#include <vector>
#include <map>

#ifndef NO_GUI
#include <gl.h>
#endif

#include <ANN/ANN.h>

using namespace std;

#include "FacetedObject.h"
#include "MyFace.h"
#include "VoxelObject.h"
#include "sort/sort.h"
#include "MarchingCubes.h"

#define MAX(a,b) ((a) > (b) ? (a) : (b))
#define MIN(a,b) ((a) < (b) ? (a) : (b))
#define ABS(a) ((a) >= 0 ? (a) : -(a))
#define ODD(n) ((n) & 1)

// create object
FacetedObject::FacetedObject()
{
    mDrawClockwise = true;
    mDrawAntiClockwise = false;
    mName = 0;
    mVoxelData = 0;
    mVertexList = new std::vector<MyVertex>();
    mFaceList = new std::vector<MyFace *>();
}

// destroy object
FacetedObject::~FacetedObject()
{
    delete mVertexList;
    delete mFaceList;
    if (mName) delete [] mName;
    if (mVoxelData) delete [] mVoxelData;
}

// clear the vertex and face lists
void FacetedObject::ClearLists()
{
    (*mVertexList).clear();
    for (int i = 0; i < (*mFaceList).size(); i++) delete (*mFaceList)[i];
    (*mFaceList).clear();
    mVertexMap.clear();
    mFaceMap.clear();
}

void FacetedObject::SetName(const char *name)
{
    if (mName) delete [] mName;
    int l = strlen(name);
    mName = new char[l + 1];
    strcpy(mName, name);
}

void FacetedObject::Draw()
{
#ifndef NO_GUI
    int i, j;
    int nVertices;
    GLfloat normal[3];
    MyVertex *vPtr;

    glPolygonMode(GL_FRONT, GL_FILL);
    glPolygonMode(GL_BACK, GL_LINE);
    // glPolygonMode(GL_BACK, GL_FILL);

    for (i = 0; i < (*mFaceList).size(); i++)
    {

        if (mDrawClockwise)
        {
            nVertices = (*mFaceList)[i]->GetNumVertices();
            if (nVertices < 3) throw (0);

            glBegin(GL_POLYGON);
            (*mFaceList)[i]->GetNormal(normal);
            glNormal3fv(normal);

            for (j = 0; j < nVertices; j++)
            {
                vPtr = &(*mVertexList)[(*mFaceList)[i]->GetVertex(j)];
                glVertex3f(vPtr->x, vPtr->y, vPtr->z);
            }
            glEnd();
        }

        if (mDrawAntiClockwise)
        {
            nVertices = (*mFaceList)[i]->GetNumVertices();
            if (nVertices < 3) throw (0);

            glBegin(GL_POLYGON);
            (*mFaceList)[i]->GetNormal(normal);
            glNormal3fv(normal);

            for (j = nVertices - 1; j >= 0; j--)
            {
                vPtr = &(*mVertexList)[(*mFaceList)[i]->GetVertex(j)];
                glVertex3f(vPtr->x, vPtr->y, vPtr->z);
            }
            glEnd();
        }
    }
#endif
}

// sort the faces list by group
void FacetedObject::SortFacesByGroup()
{
    int error;

    // using mergesort because list is likely to be almost competelely
    // sorted and also because it does not swap equal values
    // FIX ME - doesn't work
    cerr << "FacetedObject::SortFacesByGroup not currently working\n";
    //error = mergesort((*mFaceList), (*mFaceList).size(), sizeof(MyFace *), CompareFacesByGroup);

    if (error)
    {
        cerr << "Error in FacetedObject::SortFacesByGroup\n";
        exit(1);
    }
}

// comparison for SortFacesByGroup
int FacetedObject::CompareFacesByGroup(const void *v1, const void *v2)
{
    MyFace **f1 = (MyFace **)v1;
    MyFace **f2 = (MyFace **)v2;

    return strcmp((*f1)->GetGroup(), (*f2)->GetGroup());
}

// utility to calculate a face normal
void FacetedObject::ComputeFaceNormal(const MyVertex *v1, const MyVertex *v2, const MyVertex *v3, double normal[3])
{
    double a[3], b[3];

    // calculate in plane vectors
    a[0] = v2->x - v1->x;
    a[1] = v2->y - v1->y;
    a[2] = v2->z - v1->z;
    b[0] = v3->x - v1->x;
    b[1] = v3->y - v1->y;
    b[2] = v3->z - v1->z;

    // cross(a, b, normal);
    normal[0] = a[1] * b[2] - a[2] * b[1];
    normal[1] = a[2] * b[0] - a[0] * b[2];
    normal[2] = a[0] * b[1] - a[1] * b[0];

    // normalize(normal);
    double norm = sqrt(normal[0] * normal[0] +
                       normal[1] * normal[1] +
                       normal[2] * normal[2]);

    if (norm > 0.0)
    {
        normal[0] /= norm;
        normal[1] /= norm;
        normal[2] /= norm;
    }
}

// Calculate all the normals
void FacetedObject::CalculateNormals()
{
    int i;
    MyFace *face;
    double normal[3];

    for (i = 0; i < (*mFaceList).size(); i++)
    {
        face = (*mFaceList)[i];

        if (face->GetNumVertices() > 2)
        {
            ComputeFaceNormal(&(*mVertexList)[face->GetVertex(0)],
                              &(*mVertexList)[face->GetVertex(1)],
                              &(*mVertexList)[face->GetVertex(2)],
                              normal);
            face->SetNormal(normal[0], normal[1], normal[2]);
        }
    }
}

// Convert groups to objects
void FacetedObject::ConvertGroupsToObjects()
{
    int i;
    MyFace *face;

    for (i = 0; i < (*mFaceList).size(); i++)
    {
        face = (*mFaceList)[i];
        face->SetObject(face->GetGroup());
        face->SetGroup(0);
    }
}

// concatenate another faceted object
void FacetedObject::Concatenate(FacetedObject *object, int keepNames)
{
    int i, j;
    MyFace *face;
    MyVertex vertex[maxVerticesPerFace];
    int numVertices;
    for (i = 0; i < object->GetNumFaces(); i++)
    {
        face = object->GetFace(i);
        numVertices = face->GetNumVertices();
        if (numVertices > maxVerticesPerFace)
        {
            std::cerr << "Error in FacetedObject::Concatenate\n";
            exit(1);
        }
        for (j = 0; j < numVertices; j++) vertex[j] = *object->GetVertex(face->GetVertex(j));
        AddFace(vertex, numVertices);
    }
    CalculateNormals();
}

// move the object
void FacetedObject::Move(double x, double y, double z)
{
    int i;

    for (i = 0; i < (*mVertexList).size(); i++)
    {
        (*mVertexList)[i].x += x;
        (*mVertexList)[i].y += y;
        (*mVertexList)[i].z += z;
    }
}

// scale the object
void FacetedObject::Scale(double x, double y, double z)
{
    int i;

    for (i = 0; i < (*mVertexList).size(); i++)
    {
        (*mVertexList)[i].x *= x;
        (*mVertexList)[i].y *= y;
        (*mVertexList)[i].z *= z;
    }
}

// swap axes of the object
void FacetedObject::SwapAxes(int axis1, int axis2)
{
    int i;
    double t;
    if (axis1 < 0 || axis1 > 2 || 
            axis2 < 0 || axis2 > 2)
    {
        cerr << "FacetedObject::SwapAxes axis out of range\n";
        exit(1);
    }
    #define SWAP(a,b) {t = a; a = b; b = t;}
    
    for (i = 0; i < (*mVertexList).size(); i++)
    {
        if (axis1 == 0 && axis2 == 1) SWAP((*mVertexList)[i].x, (*mVertexList)[i].y)
        if (axis1 == 0 && axis2 == 2) SWAP((*mVertexList)[i].x, (*mVertexList)[i].z)
        if (axis1 == 1 && axis2 == 2) SWAP((*mVertexList)[i].y, (*mVertexList)[i].z)
        if (axis1 == 1 && axis2 == 0) SWAP((*mVertexList)[i].y, (*mVertexList)[i].x)
        if (axis1 == 2 && axis2 == 0) SWAP((*mVertexList)[i].z, (*mVertexList)[i].x)
        if (axis1 == 2 && axis2 == 1) SWAP((*mVertexList)[i].z, (*mVertexList)[i].y)
    }
}

// rotate axes of the object
void FacetedObject::RotateAxes(int axis0, int axis1, int axis2)
{
    int i;
    double t0, t1, t2;
    if (axis0 < 0 || axis0 > 2 || 
            axis1 < 0 || axis1 > 2 || 
            axis2 < 0 || axis2 > 2)
    {
        cerr << "FacetedObject::RotateAxes axis out of range\n";
        exit(1);
    }
    
    for (i = 0; i < (*mVertexList).size(); i++)
    {
        if (axis0 == 0) t0 = (*mVertexList)[i].x;
        if (axis0 == 1) t0 = (*mVertexList)[i].y;
        if (axis0 == 2) t0 = (*mVertexList)[i].z;
        if (axis1 == 0) t1 = (*mVertexList)[i].x;
        if (axis1 == 1) t1 = (*mVertexList)[i].y;
        if (axis1 == 2) t1 = (*mVertexList)[i].z;
        if (axis2 == 0) t2 = (*mVertexList)[i].x;
        if (axis2 == 1) t2 = (*mVertexList)[i].y;
        if (axis2 == 2) t2 = (*mVertexList)[i].z;
        (*mVertexList)[i].x = t0;
        (*mVertexList)[i].y = t1;
        (*mVertexList)[i].z = t2;
    }
}

// apply an axis-angle rotation
// note that angle is in degrees
void FacetedObject::RotateAxisAngle(double x, double y, double z, double angle)
{    
    Quaternion rot = MakeQFromAxis(x, y, z, DegreesToRadians(angle));
    Vector v;
    int i;

    for (i = 0; i < (*mVertexList).size(); i++)
    {
        v = QVRotate(rot, Vector((*mVertexList)[i].x, (*mVertexList)[i].y, (*mVertexList)[i].z));
        (*mVertexList)[i].x = v.x;
        (*mVertexList)[i].y = v.y;
        (*mVertexList)[i].z = v.z;
    }
    
}

// apply a quaternion rotation
// this is the same argument order as used in ODE
void FacetedObject::RotateQuaternion(double w, double x, double y, double z)
{    
    Quaternion rot;
    rot.n = w;
    rot.v.x = x;
    rot.v.y = y;
    rot.v.z = z;
    Vector v;
    int i;

    for (i = 0; i < (*mVertexList).size(); i++)
    {
        v = QVRotate(rot, Vector((*mVertexList)[i].x, (*mVertexList)[i].y, (*mVertexList)[i].z));
        (*mVertexList)[i].x = v.x;
        (*mVertexList)[i].y = v.y;
        (*mVertexList)[i].z = v.z;
    }
    
}


// mirror the object
void FacetedObject::Mirror(bool x, bool y, bool z)
{
    int i;
    int actions = 0;

    if (x)
    {
        for (i = 0; i < (*mVertexList).size(); i++) (*mVertexList)[i].x =  -(*mVertexList)[i].x;
        actions++;
    }

    if (y)
    {
        for (i = 0; i < (*mVertexList).size(); i++) (*mVertexList)[i].y =  -(*mVertexList)[i].y;
        actions++;
    }

    if (z)
    {
        for (i = 0; i < (*mVertexList).size(); i++) (*mVertexList)[i].z =  -(*mVertexList)[i].z;
        actions++;
    }

    // if we have an odd number of mirroring operations we need to
    // reverse the order that the faces are defined
    if (ODD(actions))
    {
        for (i = 0; i < (*mFaceList).size(); i++) (*mFaceList)[i]->ReverseVertexOrder();
    }

    // if we've done anything then we need to recalculate the normals
    if (actions) CalculateNormals();
}

// reverse the face winding
void FacetedObject::ReverseWinding()
{
    for (int i = 0; i < (*mFaceList).size(); i++) (*mFaceList)[i]->ReverseVertexOrder();
    CalculateNormals();
}

// calculate limits
MyVertex* FacetedObject::GetLimits()
{
    int i;
    MyVertex min(DBL_MAX, DBL_MAX, DBL_MAX);
    MyVertex max(-DBL_MAX, -DBL_MAX, -DBL_MAX);
    
    for (i = 0; i < (*mVertexList).size(); i++)
    {
        min.x = MIN(min.x, (*mVertexList)[i].x);
        min.y = MIN(min.y, (*mVertexList)[i].y);
        min.z = MIN(min.z, (*mVertexList)[i].z);
        max.x = MAX(max.x, (*mVertexList)[i].x);
        max.y = MAX(max.y, (*mVertexList)[i].y);
        max.z = MAX(max.z, (*mVertexList)[i].z);
    }
    
    mLimits[0] = min;
    mLimits[1] = max;
    
    return mLimits;
}

// print out stats on the object
void FacetedObject::Stats()
{
    int i;
    MyVertex min(DBL_MAX, DBL_MAX, DBL_MAX);
    MyVertex max(-DBL_MAX, -DBL_MAX, -DBL_MAX);

    cout << "FacettedObject: " << mName << "\n";
    cout << "Vertices: " << (*mVertexList).size() << "\n";
    cout << "Faces: " << (*mFaceList).size() << "\n";

    for (i = 0; i < (*mVertexList).size(); i++)
    {
        min.x = MIN(min.x, (*mVertexList)[i].x);
        min.y = MIN(min.y, (*mVertexList)[i].y);
        min.z = MIN(min.z, (*mVertexList)[i].z);
        max.x = MAX(max.x, (*mVertexList)[i].x);
        max.y = MAX(max.y, (*mVertexList)[i].y);
        max.z = MAX(max.z, (*mVertexList)[i].z);
    }
    cout << "Vertex extents " << min.x << ", " << min.y << ", " << min.z <<
        " to " << max.x << ", " << max.y << ", " << max.z << "\n";

    cout.flush();
}

// simple routine to convert convex polygons to triangles
void FacetedObject::Triangulate()
{
    int i;
    int numVertices;
    int v0, v1, v2;
    std::vector<MyFace *> *newFaceList = new std::vector<MyFace *>();
    MyFace *face;

    if ((*mFaceList).size() == 0) return; // nothing to do

    // do the triangulation
    for (i = 0; i < (*mFaceList).size(); i++)
    {
        numVertices = (*mFaceList)[i]->GetNumVertices();
        if (numVertices == 3)
        {
            (*newFaceList).push_back((*mFaceList)[i]);
         }
        else
        {
            if (numVertices > 3)
            {
                v0 = 0;
                for (v2 = 2; v2 < numVertices; v2++)
                {
                    face = new MyFace();
                    face->SetNumVertices(3);
                    v1 = v2 - 1;
                    face->SetVertex(0, (*mFaceList)[i]->GetVertex(v0));
                    face->SetVertex(1, (*mFaceList)[i]->GetVertex(v1));
                    face->SetVertex(2, (*mFaceList)[i]->GetVertex(v2));
                    (*newFaceList).push_back(face);
                }
                delete (*mFaceList)[i];
            }
        }
    }

    // and change over the face list
    delete mFaceList;
    mFaceList = newFaceList;
}

#ifdef USE_ODE
#include <ode/ode.h>
// very basic collision detection
void nearCallback (void *data, dGeomID o1, dGeomID o2)
{
    const int maxContacts = 1; // I only need one
    dContactGeom contacts[maxContacts];   
    int numContacts = dCollide (o1, o2, maxContacts, contacts, sizeof(dContactGeom));
    *(int *)data += numContacts;
}

// convert to a voxel version - optionally producing an outline
void FacetedObject::Voxelize(double x, double y, double z, int outlineVoxels)
{
    int i, j;
    // first get extents of object
    MyVertex min(DBL_MAX, DBL_MAX, DBL_MAX);
    MyVertex max(-DBL_MAX, -DBL_MAX, -DBL_MAX);
    int nx, ny, nz;
    int ix, iy, iz;
    MyVertex *v0;
    MyVertex v[4];
    int vertexStride;
    int vertexCount;
    int indexCount;
    int triStride;
    int collisionData;
    Rectangle rectangle;
    int index;
    
    for (i = 0; i < (*mVertexList).size(); i++)
    {
        min.x = MIN(min.x, (*mVertexList)[i].x);
        min.y = MIN(min.y, (*mVertexList)[i].y);
        min.z = MIN(min.z, (*mVertexList)[i].z);
        max.x = MAX(max.x, (*mVertexList)[i].x);
        max.y = MAX(max.y, (*mVertexList)[i].y);
        max.z = MAX(max.z, (*mVertexList)[i].z);
    }

    // Calculate a suitable bounding box

    min.x -= 2 * x;
    min.y -= 2 * y;
    min.z -= 2 * z;
    max.x += 2 * x;
    max.y += 2 * y;
    max.z += 2 * z;

    
    // calculate the number of voxels

    nx = (int)((max.x - min.x) / x) + 1;
    ny = (int)((max.y - min.y) / y) + 1;
    nz = (int)((max.z - min.z) / z) + 1;

    // voxel origin is centred on the first voxel
    mVoxelOrigin.x = min.x + x/2;
    mVoxelOrigin.y = min.y + y/2;
    mVoxelOrigin.z = min.z + z/2;
    mVoxelSize.x = x;
    mVoxelSize.y = y;
    mVoxelSize.z = z;
    mVoxelNumbers.x = nx;
    mVoxelNumbers.y = ny;
    mVoxelNumbers.z = nz;
    
    // initialise the ODE world
    dInitODE();
    dWorldID worldID = dWorldCreate();
    dSpaceID spaceID = dSimpleSpaceCreate(0);
    dJointGroupID contactGroup = dJointGroupCreate(0);
    
    // create the collision box
    dGeomID box = dCreateBox (spaceID, x, y, z);
    dGeomSetBody (box, 0);
    
    // create the collision trimesh
    Triangulate(); // must be triangles
    dTriMeshDataID triMeshDataID = dGeomTriMeshDataCreate();
    double *vertices = new double[(*mFaceList).size() * 3 * 3];
    int *indices = new int[(*mFaceList).size() * 3];
    index = 0;
    for (i = 0; i < (*mFaceList).size(); i++)
    {
        for (j = 0; j < 3; j++)
        {
            indices[i * 3 + j] = index;
            index++;
            v0 = &(*mVertexList)[(*mFaceList)[i]->GetVertex(j)];
            vertices[i * 9 + j * 3 + 0] = v0->x;
            vertices[i * 9 + j * 3 + 1] = v0->y;
            vertices[i * 9 + j * 3 + 2] = v0->z;
        }
    }
    vertexStride = sizeof(double) * 3;
    vertexCount = (*mFaceList).size() * 3;
    triStride = sizeof(int) * 3;
    indexCount = (*mFaceList).size() * 3;
    dGeomTriMeshDataBuildDouble (triMeshDataID, vertices, vertexStride, vertexCount,
                           indices, indexCount, triStride);
    dGeomID trimesh=  dCreateTriMesh (spaceID, triMeshDataID, 0, 0, 0);
    dGeomSetBody (trimesh, 0);
    
    // create voxel map
    const static unsigned char untested = 255;
    const static unsigned char listed = 254;
    const static unsigned char edge = 127;
    const static unsigned char empty = 0;
    if (mVoxelData) delete [] mVoxelData;
    mVoxelData = new unsigned char[nx * ny * nz];
    memset(mVoxelData, untested, nx * ny * nz);
    
    std::vector<Location> pursuitList;
    Location location(0, 0, 0);
    pursuitList.push_back(location);
    
    while (pursuitList.size())
    {
        // get and delete the last item from the list
        location = pursuitList.back();
        pursuitList.pop_back();
        
        // check to see whether it intersects with an edge
        collisionData = 0;
        dGeomSetPosition (box, min.x + location.x * x + x/2, min.y + location.y * y + y/2, min.z + location.z * z + z/2);
        dSpaceCollide (spaceID, &collisionData, nearCallback);
        
        if (collisionData)
        {
            mVoxelData[location.x + location.y * nx + location.z * nx * ny] = edge;
        }
        else
        {
            mVoxelData[location.x + location.y * nx + location.z * nx * ny] = empty;
            
            // now check neigbours
            if (location.x < nx - 1)
                if (mVoxelData[(location.x + 1) + location.y * nx + location.z * nx * ny] == untested)
                {
                    mVoxelData[(location.x + 1) + location.y * nx + location.z * nx * ny] = listed;
                    pursuitList.push_back(Location(location.x + 1, location.y, location.z));
                }
            if (location.y < ny - 1)
                if (mVoxelData[location.x + (location.y + 1) * nx + location.z * nx * ny] == untested)
                {
                    mVoxelData[location.x + (location.y + 1) * nx + location.z * nx * ny] = listed;
                    pursuitList.push_back(Location(location.x, location.y + 1, location.z));
                }
            if (location.z < nz - 1)
                if (mVoxelData[location.x + location.y * nx + (location.z + 1) * nx * ny] == untested)
                {
                    mVoxelData[location.x + location.y * nx + (location.z + 1) * nx * ny] = listed;
                    pursuitList.push_back(Location(location.x, location.y, location.z + 1));
                }
            if (location.x > 0)
                if (mVoxelData[(location.x - 1) + location.y * nx + location.z * nx * ny] == untested)
                {
                    mVoxelData[(location.x - 1) + location.y * nx + location.z * nx * ny] = listed;
                    pursuitList.push_back(Location(location.x - 1, location.y, location.z));
                }
            if (location.y > 0)
                if (mVoxelData[location.x + (location.y - 1) * nx + location.z * nx * ny] == untested)
                {
                    mVoxelData[location.x + (location.y - 1) * nx + location.z * nx * ny] = listed;
                    pursuitList.push_back(Location(location.x, location.y - 1, location.z));
                }
            if (location.z > 0)
                if (mVoxelData[location.x + location.y * nx + (location.z - 1) * nx * ny] == untested)
                {
                    mVoxelData[location.x + location.y * nx + (location.z - 1) * nx * ny] = listed;
                    pursuitList.push_back(Location(location.x, location.y, location.z - 1));
                }
        }
    }
    
    // clear out the old list of triangles
    ClearLists();
    
    if (outlineVoxels)
    {
        // cycle through the voxels collecting the ones marked as edges
        // and add those to the new list of faces
        Vector inSide0(x, 0, 0);
        Vector inSide1(0, y, 0); 
        Vector inSide2(0, 0, z);
        Vector origin;
        for (ix = 0; ix < nx; ix++)
        {
            for (iy = 0; iy < ny; iy++)
            {
                for (iz = 0; iz < nz; iz++)
                {
                    if (mVoxelData[ix + iy * nx + iz * nx * ny] == edge)
                    {
                        origin.x = min.x + ix * x;
                        origin.y = min.y + iy * y;
                        origin.z = min.z + iz * z;
                        Cuboid cuboid = Cuboid(origin, inSide0, inSide1, inSide2);
                        for (i = 0; i < 6; i++)
                        {
                            rectangle = cuboid.GetFace(i);
                            v[0].x = rectangle.origin.x;
                            v[0].y = rectangle.origin.y;
                            v[0].z = rectangle.origin.z;
                            v[1].x = rectangle.origin.x + rectangle.side0.x;
                            v[1].y = rectangle.origin.y + rectangle.side0.y;
                            v[1].z = rectangle.origin.z + rectangle.side0.z;
                            v[2].x = rectangle.origin.x + rectangle.side0.x + rectangle.side1.x;
                            v[2].y = rectangle.origin.y + rectangle.side0.y + rectangle.side1.y;
                            v[2].z = rectangle.origin.z + rectangle.side0.z + rectangle.side1.z;
                            v[3].x = rectangle.origin.x + rectangle.side1.x;
                            v[3].y = rectangle.origin.y + rectangle.side1.y;
                            v[3].z = rectangle.origin.z + rectangle.side1.z;
                            AddFace(v, 4);
                        }
                    }
                }
            }
        }
        
        
        CalculateNormals();
    }
    
    delete [] vertices;
    delete [] indices;
    dGeomTriMeshDataDestroy(triMeshDataID);
    dJointGroupDestroy(contactGroup);
    dSpaceDestroy(spaceID);
    dWorldDestroy(worldID);
    dCloseODE();
    
}

#else
// convert to a voxel version
void FacetedObject::Voxelize(double x, double y, double z)
{
    int i;
    // first get extents of object
    MyVertex min = {DBL_MAX, DBL_MAX, DBL_MAX};
    MyVertex max = {-DBL_MAX, -DBL_MAX, -DBL_MAX};
    int nx, ny, nz;
    int ix, iy, iz;
    MyVertex *v0, *v1, *v2;
    MyVertex v[4];
    Cuboid *cuboid;
    unsigned char status;
    Rectangle rectangle;
    
    for (i = 0; i < (*mVertexList).size(); i++)
    {
        min.x = MIN(min.x, (*mVertexList)[i]->x);
        min.y = MIN(min.y, (*mVertexList)[i]->y);
        min.z = MIN(min.z, (*mVertexList)[i]->z);
        max.x = MAX(max.x, (*mVertexList)[i]->x);
        max.y = MAX(max.y, (*mVertexList)[i]->y);
        max.z = MAX(max.z, (*mVertexList)[i]->z);
    }
    
    // Calculate a suitable bounding box
    
    min.x -= 2 * x;
    min.y -= 2 * y;
    min.z -= 2 * z;
    max.x += 2 * x;
    max.y += 2 * y;
    max.z += 2 * z;
    
    // calculate the number of voxels
    
    nx = (int)((max.x - min.x) / x) + 1;
    ny = (int)((max.y - min.y) / y) + 1;
    nz = (int)((max.z - min.z) / z) + 1;
    
    // Create a new VoxelObject
    VoxelObject voxelObject(Vector(min.x, min.y, min.z), nx, ny, nz, x, y, z);
    
    // add the triangles
    for (i = 0; i < (*mFaceList).size(); i++)
    {
        v0 = (*mVertexList)[(*mFaceList)[i]->GetVertex(0)];
        v1 = (*mVertexList)[(*mFaceList)[i]->GetVertex(1)];
        v2 = (*mVertexList)[(*mFaceList)[i]->GetVertex(2)];
        voxelObject.AddTriangle(Triangle(Vector(v0->x, v0->y, v0->z),
                                         Vector(v1->x - v0->x, v1->y - v0->y, v1->z - v0->z),
                                         Vector(v2->x - v0->x, v2->y - v0->y, v2->z - v0->z)));
    }
    
    // detect the exterior edges
    voxelObject.DetectEdges(0, 0, 0);
    
    // clear out the old list of triangles
    ClearLists();
    
    // cycle through the voxels collecting the ones marked as edges
    // and add those to the new list of faces
    for (ix = 0; ix < nx; ix++)
    {
        for (iy = 0; iy < ny; iy++)
        {
            for (iz = 0; iz < nz; iz++)
            {
                cuboid = voxelObject.GetCuboid(ix, iy, iz, &status);
                if (cuboid)
                {
                    if (status == VoxelObject::edge)
                    {
                        for (i = 0; i < 6; i++)
                        {
                            rectangle = cuboid->GetFace(i);
                            v[0].x = rectangle.origin.x;
                            v[0].y = rectangle.origin.y;
                            v[0].z = rectangle.origin.z;
                            v[1].x = rectangle.origin.x + rectangle.side0.x;
                            v[1].y = rectangle.origin.y + rectangle.side0.y;
                            v[1].z = rectangle.origin.z + rectangle.side0.z;
                            v[2].x = rectangle.origin.x + rectangle.side0.x + rectangle.side1.x;
                            v[2].y = rectangle.origin.y + rectangle.side0.y + rectangle.side1.y;
                            v[2].z = rectangle.origin.z + rectangle.side0.z + rectangle.side1.z;
                            v[3].x = rectangle.origin.x + rectangle.side1.x;
                            v[3].y = rectangle.origin.y + rectangle.side1.y;
                            v[3].z = rectangle.origin.z + rectangle.side1.z;
                            AddFace(v, 4);
                        }
                    }
                    delete cuboid;
                }
            }
        }
    }
    CalculateNormals();
}
#endif
    
// add a face to the object
// adds vertices
void FacetedObject::AddFace(MyVertex vertex[], int numVertices)
{
    int i;
    int *vertexIndices = new int[numVertices];
    MyFace *face = new MyFace();

    for (i = 0; i < numVertices; i++)
        vertexIndices[i] = AddVertex(vertex[i]);

    face->SetNumVertices(numVertices);
    for (i = 0; i < numVertices; i++)
        face->SetVertex(i, vertexIndices[i]);
    delete [] vertexIndices;

    // try to match existing face
    std::map<MyFace, int>::const_iterator iter = mFaceMap.find(*face);
    if (iter != mFaceMap.end())
    {
        delete face;
        return;    
    }
    (*mFaceList).push_back(face);
    mFaceMap[*face] = (*mFaceList).size() - 1;
}

// add a vertex to an object
int FacetedObject::AddVertex(MyVertex vertex)
{
    // try to match existing vertex
    std::map<MyVertex, int>::const_iterator iter = mVertexMap.find(vertex);
    if (iter != mVertexMap.end()) 
    {
        return iter->second;
    }

    (*mVertexList).push_back(vertex);
    mVertexMap[vertex] = (*mVertexList).size() - 1;
    return (*mVertexList).size() - 1;
}

// Calculate an isosurface using an isolevel
// normals appear to be clockwise
// isolevel is the threshold value
// cubeBased  - set to 0 to use cube rather than tetrahedron based surfacing
void FacetedObject::Isosurface(int isolevel, int cubeBased)
{
    GRIDCELL cell;
    int nTriangles;
    TRIANGLE triangles[5];
    MyVertex triVert[3];
    int i, j, ix, iy, iz;
    
    if (mVoxelData == 0)
    {
        cerr << "Error in FacetedObject::Isosurface - no voxel data\n";
        exit(1);
    }
    
    ClearLists();
    
    // construct GRIDCELLs from the voxel data
    for (iz = 1; iz < mVoxelNumbers.z; iz++)
    {
        for (iy = 1; iy < mVoxelNumbers.y; iy++)
        {
            for (ix = 1; ix < mVoxelNumbers.x; ix++)
            {
                
                cell.p[0].x = mVoxelOrigin.x + (ix - 1) * mVoxelSize.x;
                cell.p[0].y = mVoxelOrigin.y + (iy - 1) * mVoxelSize.y;
                cell.p[0].z = mVoxelOrigin.z + (iz - 1) * mVoxelSize.z;
                cell.p[1].x = mVoxelOrigin.x + (ix    ) * mVoxelSize.x;
                cell.p[1].y = mVoxelOrigin.y + (iy - 1) * mVoxelSize.y;
                cell.p[1].z = mVoxelOrigin.z + (iz - 1) * mVoxelSize.z;
                cell.p[2].x = mVoxelOrigin.x + (ix    ) * mVoxelSize.x;
                cell.p[2].y = mVoxelOrigin.y + (iy    ) * mVoxelSize.y;
                cell.p[2].z = mVoxelOrigin.z + (iz - 1) * mVoxelSize.z;
                cell.p[3].x = mVoxelOrigin.x + (ix - 1) * mVoxelSize.x;
                cell.p[3].y = mVoxelOrigin.y + (iy    ) * mVoxelSize.y;
                cell.p[3].z = mVoxelOrigin.z + (iz - 1) * mVoxelSize.z;
                cell.p[4].x = mVoxelOrigin.x + (ix - 1) * mVoxelSize.x;
                cell.p[4].y = mVoxelOrigin.y + (iy - 1) * mVoxelSize.y;
                cell.p[4].z = mVoxelOrigin.z + (iz    ) * mVoxelSize.z;
                cell.p[5].x = mVoxelOrigin.x + (ix    ) * mVoxelSize.x;
                cell.p[5].y = mVoxelOrigin.y + (iy - 1) * mVoxelSize.y;
                cell.p[5].z = mVoxelOrigin.z + (iz    ) * mVoxelSize.z;
                cell.p[6].x = mVoxelOrigin.x + (ix    ) * mVoxelSize.x;
                cell.p[6].y = mVoxelOrigin.y + (iy    ) * mVoxelSize.y;
                cell.p[6].z = mVoxelOrigin.z + (iz    ) * mVoxelSize.z;
                cell.p[7].x = mVoxelOrigin.x + (ix - 1) * mVoxelSize.x;
                cell.p[7].y = mVoxelOrigin.y + (iy    ) * mVoxelSize.y;
                cell.p[7].z = mVoxelOrigin.z + (iz    ) * mVoxelSize.z;
                
                cell.val[0] = mVoxelData[(ix - 1) + (iy - 1) * mVoxelNumbers.x + (iz - 1) * mVoxelNumbers.x * mVoxelNumbers.y];
                cell.val[1] = mVoxelData[(ix    ) + (iy - 1) * mVoxelNumbers.x + (iz - 1) * mVoxelNumbers.x * mVoxelNumbers.y];
                cell.val[2] = mVoxelData[(ix    ) + (iy    ) * mVoxelNumbers.x + (iz - 1) * mVoxelNumbers.x * mVoxelNumbers.y];
                cell.val[3] = mVoxelData[(ix - 1) + (iy    ) * mVoxelNumbers.x + (iz - 1) * mVoxelNumbers.x * mVoxelNumbers.y];
                cell.val[4] = mVoxelData[(ix - 1) + (iy - 1) * mVoxelNumbers.x + (iz    ) * mVoxelNumbers.x * mVoxelNumbers.y];
                cell.val[5] = mVoxelData[(ix    ) + (iy - 1) * mVoxelNumbers.x + (iz    ) * mVoxelNumbers.x * mVoxelNumbers.y];
                cell.val[6] = mVoxelData[(ix    ) + (iy    ) * mVoxelNumbers.x + (iz    ) * mVoxelNumbers.x * mVoxelNumbers.y];
                cell.val[7] = mVoxelData[(ix - 1) + (iy    ) * mVoxelNumbers.x + (iz    ) * mVoxelNumbers.x * mVoxelNumbers.y];
                
                if (cubeBased)
                {
                    nTriangles = Polygonise(cell, isolevel, triangles);
                    
                    for (i = 0; i <  nTriangles; i++)
                    {
                        triVert[0].x = triangles[i].p[0].x;
                        triVert[0].y = triangles[i].p[0].y;
                        triVert[0].z = triangles[i].p[0].z;
                        triVert[1].x = triangles[i].p[1].x;
                        triVert[1].y = triangles[i].p[1].y;
                        triVert[1].z = triangles[i].p[1].z;
                        triVert[2].x = triangles[i].p[2].x;
                        triVert[2].y = triangles[i].p[2].y;
                        triVert[2].z = triangles[i].p[2].z;
                        AddFace(triVert, 3);
                    }
                }
                else // use the smoother but slower and more triangles tetrahedron based version
                {
                    for (j = 0; j < 6; j++)
                    {
                        switch(j)
                        {
                            case 0:
                                nTriangles = PolygoniseTri(cell, isolevel, triangles,0,2,3,7);
                                break;
                            case 1:
                                nTriangles = PolygoniseTri(cell, isolevel, triangles,0,2,6,7);
                                break;
                            case 2:
                                nTriangles = PolygoniseTri(cell, isolevel, triangles,0,4,6,7);
                                break;
                            case 3:
                                nTriangles = PolygoniseTri(cell, isolevel, triangles,0,6,1,2);
                                break;
                            case 4:
                                nTriangles = PolygoniseTri(cell, isolevel, triangles,0,6,1,4);
                                break;
                            case 5:
                                nTriangles = PolygoniseTri(cell, isolevel, triangles,5,6,1,4);
                                break;
                        }
                        for (i = 0; i <  nTriangles; i++)
                        {
                            triVert[0].x = triangles[i].p[0].x;
                            triVert[0].y = triangles[i].p[0].y;
                            triVert[0].z = triangles[i].p[0].z;
                            triVert[1].x = triangles[i].p[1].x;
                            triVert[1].y = triangles[i].p[1].y;
                            triVert[1].z = triangles[i].p[1].z;
                            triVert[2].x = triangles[i].p[2].x;
                            triVert[2].y = triangles[i].p[2].y;
                            triVert[2].z = triangles[i].p[2].z;
                            AddFace(triVert, 3);
                        }
                    }                        
                    
                }
            }
        }
    }
    
    CalculateNormals();
}

struct FaceInfo
{
    MyFace *face;
    int vertex;
};

// Merge close or identical vertices
void FacetedObject::MergeVertices(double tolerance)
{
    int nPts;                           // actual number of data points 
    int maxPts;                         // maximu number of data points (for allocation)
    ANNpointArray dataPts;              // data points 
    ANNpoint queryPt;                   // query point 
    ANNidxArray nnIdx;                  // near neighbor indices 
    ANNdistArray dists;                 // near neighbor distances 
    ANNkd_tree* kdTree;                 // search structure 
    
    int dim = 3;                // x, y and z
    
    nPts = maxPts = (*mVertexList).size();
    int i, j, k, l;
    int count;
    
    queryPt = annAllocPt(dim);          // allocate query point 
    dataPts = annAllocPts(maxPts, dim); // allocate data points 
    nnIdx = new ANNidx[maxPts];         // allocate near neigh indices 
    dists = new ANNdist[maxPts];        // allocate near neighbor dists 
    
    for (i = 0; i < (*mVertexList).size(); i++)
    {
        dataPts[i] =  &((*mVertexList)[i].x);
    }
    kdTree = new ANNkd_tree(            // build search structure 
                            dataPts,    // the data points 
                            nPts,       // number of points 
                            dim);       // dimension of space 
    
    ANNdist sqRad = tolerance * tolerance;
    
    // create an empty vertex list to add values to
    std::vector<MyVertex> *newVertexList = new std::vector<MyVertex>();
    const unsigned char untested = 0;
    const unsigned char replaced = 1;
    unsigned char *controlList = new unsigned char [(*mVertexList).size()];
    memset(controlList, untested, (*mVertexList).size());
    
    FaceInfo faceInfo;
    std::multimap<int, FaceInfo> faceMultiMap;
    for (j = 0; j < (*mFaceList).size(); j++)
    {
        for (k = 0; k < (*mFaceList)[j]->GetNumVertices(); k++)
        {
            faceInfo.face = (*mFaceList)[j];
            faceInfo.vertex = k;
            faceMultiMap.insert(std::pair<int, FaceInfo>((*mFaceList)[j]->GetVertex(k), faceInfo));
        }        
    }
        
    int newIndex = 0;
    bool foundInFace;
    for (i = 0; i < (*mVertexList).size(); i++)
    {
        if (controlList[i] == untested)
        {
            // set the search point
            queryPt = &((*mVertexList)[i].x);

            count = kdTree->annkFRSearch( 
                                         queryPt,       // query point 
                                         sqRad,         // squared radius 
                                         maxPts,        // number of near neighbors to return 
                                         nnIdx,        // nearest neighbor array (modified) 
                                         dists,         // dist to near neighbors (modified) 
                                         0.0);          // error bound 
            
            if (count > 0)
            {
                foundInFace = false;
                for (j = 0; j < count; j++)
                {
                    controlList[nnIdx[j]] = replaced;
                    
                    std::pair<std::multimap<int, FaceInfo>::iterator, std::multimap<int, FaceInfo>::iterator> ppp;
                    ppp = faceMultiMap.equal_range(nnIdx[j]); // return range that matches
                    for (std::multimap<int, FaceInfo>::iterator it2 = ppp.first; it2 != ppp.second; ++it2)
                    {
                        it2->second.face->SetVertex(it2->second.vertex, newIndex);
                        foundInFace = true;
                    }
                }
                
                if (foundInFace)
                {
                    (*newVertexList).push_back((*mVertexList)[i]);
                    newIndex++;
                }
                else
                {
                    std::cerr << "Warning in MergeVertices - unused vertex found\n";
                }
            }
            else
            {
                std::cerr << "Error in MergeVertices - queryPt not found\n";
                exit(1);
            }
        }
    }
    
    std::cerr << "Reduced vertices from " << (*mVertexList).size() << " to " << newIndex << "\n";
    //std::cerr << __LINE__ << "\n";
    delete mVertexList;
    //std::cerr << __LINE__ << "\n";
    delete [] controlList;
    mVertexList = newVertexList;

    
    // there are problems with some of this deallocation
    //std::cerr << __LINE__ << "\n";
    //annDeallocPts(dataPts);
    //std::cerr << __LINE__ << "\n";
    //annDeallocPt(queryPt);
    //std::cerr << __LINE__ << "\n";
    delete [] nnIdx;  
    //std::cerr << __LINE__ << "\n";
    delete [] dists; 
    //std::cerr << __LINE__ << "\n";
    delete kdTree; 
    //std::cerr << __LINE__ << "\n";
    // annClose(); // done with ANN 
        
}






