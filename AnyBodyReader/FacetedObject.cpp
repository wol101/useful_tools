/*
 *  FacettedObject.cpp
 *  GaitSymODE
 *
 *  Created by Bill Sellers on 13/09/2005.
 *  Copyright 2005 Bill Sellers. All rights reserved.
 *
 */

#include <stdio.h>
#include <iostream>
#include <fstream>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <float.h>
#include <vector>
#include <string>
#include <sstream>

#include "FacetedObject.h"
#include "Face.h"
#include "DataFile.h"
#include "AnyFunTransform3D.h"

#define SQUARE(a) ((a) * (a))
#define MAX(a,b) ((a) > (b) ? (a) : (b))
#define MIN(a,b) ((a) < (b) ? (a) : (b))
#define ABS(a) ((a) >= 0 ? (a) : -(a))
#define ODD(n) ((n) & 1)

extern int gDebug;

// create object
FacetedObject::FacetedObject()
{
    mNumVertices = 0;
    mNumVerticesAllocated = 0;
    mNumFaces = 0;
    mNumFacesAllocated = 0;
    mVertexList = 0;
    mFaceList = 0;
    mDrawClockwise = true;
    m_VertexMatchTolerance = 0.0000000001;
    m_UseRelativeOBJ = false;
    
}

// destroy object
FacetedObject::~FacetedObject()
{
    ClearLists();
}

// parse an OBJ file to a FacetedObject
// returns true on error
bool FacetedObject::ParseOBJFile(const char *filename)
{
    std::string newFilename(filename);
    unsigned int p = newFilename.rfind("/");
    if (p == std::string::npos) p = -1;
    m_Name = newFilename.substr(p + 1);
    unsigned int q = m_Name.rfind(".");
    if (q == std::string::npos) newFilename += ".obj";
    else m_Name = m_Name.substr(0, p - 1);
    
    DataFile theFile;
    if (theFile.ReadFile(filename)) return true;

    return (ParseOBJFile(theFile));
}
    
// parse an OBJ file to a FacetedObject
// returns true on error
bool FacetedObject::ParseOBJFile(DataFile &theFile)
{
    const int kBufferSize = 64000;
    char *line = new char[kBufferSize];
    char *buffer = new char[kBufferSize];
    char **tokens = new char *[kBufferSize];
    int numTokens;
    int count = 0;
    std::vector<Vertex *> vertexList;
    std::vector<Face *> faceList;
    Vertex *vertex;
    Face *face;
    int i;
    Vertex min = {DBL_MAX, DBL_MAX, DBL_MAX};
    Vertex max = {-DBL_MAX, -DBL_MAX, -DBL_MAX};
    
    // read the file
    while (theFile.ReadNextLine(line, kBufferSize, true, '#', '\\') == false)
    {
        strcpy(buffer, line);
        count++;
        numTokens = DataFile::ReturnTokens(buffer, tokens, kBufferSize);
        
        // vertices
        if (strcmp(tokens[0], "v") == 0)
        {
            if (numTokens > 3)
            {
                vertex = new Vertex();
                vertex->x = atof(tokens[1]);
                vertex->y = atof(tokens[2]);
                vertex->z = atof(tokens[3]);
                vertexList.push_back(vertex);
                
                if (gDebug)
                {
                    min.x = MIN(min.x, vertex->x);
                    min.y = MIN(min.y, vertex->y);
                    min.z = MIN(min.z, vertex->z);
                    max.x = MAX(max.x, vertex->x);
                    max.y = MAX(max.y, vertex->y);
                    max.z = MAX(max.z, vertex->z);
                }
            }
        }
        
        // faces
        if (strcmp(tokens[0], "f") == 0)
        {
            if (numTokens > 3)
            {
                face = new Face();
                face->SetNumVertices(numTokens - 1);
                // note obj files start at 1 not zero
                for (i = 1; i < numTokens; i++)
                    face->SetVertex(i - 1, atoi(tokens[i]) - 1);
                faceList.push_back(face);
            }
        }
    }
    
    if (gDebug)
        std::cerr << "ParseOBJFile:\tmin.x\t" << min.x << "\tmax.x\t" << max.x <<
            "\tmin.y\t" << min.y << "\tmax.y\t" << max.y <<
            "\tmin.z\t" << min.z << "\tmax.z\t" << max.z << "\n";
    
    // fill out the display object
    ClearLists();
    for (i = 0; i < (int)vertexList.size(); i++)
    {
        AddVertex(*vertexList[i]);
    }
    
    for (i = 0; i < (int)faceList.size(); i++)
    {
        AddFace(faceList[i]);
    }
    
    // calculate normals
    CalculateNormals();
    
    // clear memory
    for (i = 0; i < (int)vertexList.size(); i++)
        delete vertexList[i];
    
    delete [] line;
    delete [] buffer;
    delete [] tokens;
    
    return false;
}

// parse an ANYSURF file to a FacetedObject
bool FacetedObject::ParseANYSURFFile(const char *filename)
{
    std::string newFilename(filename);
    unsigned int p = newFilename.rfind("/");
    if (p == std::string::npos) p = -1;
    m_Name = newFilename.substr(p + 1);
    unsigned int q = m_Name.rfind(".");
    if (q == std::string::npos) newFilename += ".anysurf";
    else m_Name = m_Name.substr(0, p - 1);
    
    const int kBufferSize = 64000;
    DataFile theFile;
    char *line = new char[kBufferSize];
    char *buffer = new char[kBufferSize];
    char **tokens = new char *[kBufferSize];
    int numTokens;
    std::vector<Vertex *> vertexList;
    std::vector<Face *> faceList;
    Vertex *vertex;
    Face *face;
    int i, j;
    Vertex min = {DBL_MAX, DBL_MAX, DBL_MAX};
    Vertex max = {-DBL_MAX, -DBL_MAX, -DBL_MAX};
    
    theFile.SetExitOnError(true);
    theFile.ReadFile(newFilename.c_str());
    
    theFile.ReadNextLine(line, kBufferSize);
    strcpy(buffer, line);
    numTokens = DataFile::ReturnTokens(buffer, tokens, kBufferSize);
    if (numTokens != 1)
    {
        std::cerr << "Wrong number of tokens 1\n";
        return true;
    }
    if (strcmp(tokens[0], "ANYSURF") != 0)
    {
        std::cerr << "Not an ANYSURF file\n";
        return true;
    }
    
    theFile.ReadNextLine(line, kBufferSize);
    strcpy(buffer, line);
    numTokens = DataFile::ReturnTokens(buffer, tokens, kBufferSize);
    if (numTokens != 2)
    {
        std::cerr << "Wrong number of tokens 2\n";
        return true;
    }
    if (strcmp(tokens[0], "FORMAT") != 0)
    {
        std::cerr << "FORMAT line not found\n";
        return true;
    }
    if (strcmp(tokens[1], "1") != 0)
    {
        std::cerr << "Only FORMAT 1 supported\n";
        return true;
    }
    theFile.ReadNextLine(line, kBufferSize);
    strcpy(buffer, line);
    numTokens = DataFile::ReturnTokens(buffer, tokens, kBufferSize);
    if (numTokens != 2)
    {
        std::cerr << "Wrong number of tokens 3\n";
        return true;
    }
    int numVertices = strtol(tokens[0], 0, 10);
    int numFaces = strtol(tokens[1], 0, 10);
    if (gDebug) 
        std::cerr << "numVertices = " << numVertices
            << "numFaces = " << numFaces << "\n";
    
    for (i = 0; i < numVertices; i++)
    {
        theFile.ReadNextLine(line, kBufferSize);
        strcpy(buffer, line);
        numTokens = DataFile::ReturnTokens(buffer, tokens, kBufferSize);
        if (numTokens > 3)
        {
            vertex = new Vertex();
            vertex->x = atof(tokens[0]);
            vertex->y = atof(tokens[1]);
            vertex->z = atof(tokens[2]);
            vertexList.push_back(vertex);
            
            if (gDebug)
            {
                min.x = MIN(min.x, vertex->x);
                min.y = MIN(min.y, vertex->y);
                min.z = MIN(min.z, vertex->z);
                max.x = MAX(max.x, vertex->x);
                max.y = MAX(max.y, vertex->y);
                max.z = MAX(max.z, vertex->z);
            }
        }
    }
    
    for (i = 0; i < numFaces; i++)
    {
        theFile.ReadNextLine(line, kBufferSize);
        strcpy(buffer, line);
        numTokens = DataFile::ReturnTokens(buffer, tokens, kBufferSize);
        if (numTokens > 3)
        {
            face = new Face();
            face->SetNumVertices(3); // assume always 3 vertices per face
            for (j = 0; j < 3; j++)
                face->SetVertex(j, atoi(tokens[j]));
            faceList.push_back(face);
        }
    }
    
    
    if (gDebug)
        std::cerr << "ParseANYFACEFile:\tmin.x\t" << min.x << "\tmax.x\t" << max.x <<
            "\tmin.y\t" << min.y << "\tmax.y\t" << max.y <<
            "\tmin.z\t" << min.z << "\tmax.z\t" << max.z << "\n";
    
    // fill out the display object
    ClearLists();
    for (i = 0; i < (int)vertexList.size(); i++)
    {
        AddVertex(*vertexList[i]);
    }
    
    for (i = 0; i < (int)faceList.size(); i++)
    {
        AddFace(faceList[i]);
    }
    
    // calculate normals
    CalculateNormals();
    
    // clear memory
    for (i = 0; i < (int)vertexList.size(); i++)
        delete vertexList[i];
    
    delete [] line;
    delete [] buffer;
    delete [] tokens;
    
    return false;
}

void FacetedObject::WriteOBJFile(const char *filename)
{
    std::ostringstream out;
    WriteOBJFile(out);
    std::ofstream f;
    f.open(filename);
    f << out.str();
    f.close();
}

// Write a FacetedObject out as a OBJ
void FacetedObject::WriteOBJFile(std::ostringstream &out)
{
    int i, j;
    Vertex *vPtr;
    static unsigned long counter = 0;
    
    out.precision(7); // should be plenty
    
    out << "o " << m_Name << "\n";
    counter++;
    
    if (m_UseRelativeOBJ)
    {
        // write out the vertices, faces, groups and objects
        // this is the relative version - inefficient but allows concatenation of objects
        for (i = 0; i < this->GetNumFaces(); i++)
        {
            for (j = 0; j < this->GetFace(i)->GetNumVertices(); j++)
            {
                vPtr = mVertexList[mFaceList[i]->GetVertex(j)];
                out << "v " << vPtr->x << " " << vPtr->y << " " << vPtr->z << "\n";
            }
                            
            out << "f ";
            for (j = 0; j < this->GetFace(i)->GetNumVertices(); j++)
            {
                if (j == this->GetFace(i)->GetNumVertices() - 1)
                    out << j - this->GetFace(i)->GetNumVertices() << "\n";
                else
                    out << j - this->GetFace(i)->GetNumVertices() << " ";
            }
        }
    }
    else
    {
        for (i = 0; i < this->GetNumVertices(); i++)
        {
            vPtr = this->GetVertex(i);
            out << "v " << vPtr->x << " " << vPtr->y << " " << vPtr->z << "\n";
        }
        
        for (i = 0; i < this->GetNumFaces(); i++)
        {
            out << "f ";
            for (j = 0; j < this->GetFace(i)->GetNumVertices(); j++)
            {
                // note this files vertex list start at 1 not zero
                if (j == this->GetFace(i)->GetNumVertices() - 1)
                    out << this->GetFace(i)->GetVertex(j) + 1 << "\n";
                else
                    out << this->GetFace(i)->GetVertex(j) + 1 << " ";
            }
        }
    }
}

// clear the vertex and face lists
void FacetedObject::ClearLists()
{
    int i;
    if (mVertexList)
    {
        for (i = 0; i < mNumVertices; i++) delete mVertexList[i];
        delete [] mVertexList;
        mVertexList = 0;
        mNumVertices = 0;
        mNumVerticesAllocated = 0;
    }
    if (mFaceList)
    {
        for (i = 0; i < mNumFaces; i++) delete mFaceList[i];
        delete [] mFaceList;
        mFaceList = 0;
        mNumFaces = 0;
        mNumFacesAllocated = 0;
    }
}


// utility to calculate a face normal
void FacetedObject::ComputeFaceNormal(const Vertex *v1,
                                      const Vertex *v2, const Vertex *v3, double normal[3])
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
    Face *face;
    double normal[3];

    for (i = 0; i < mNumFaces; i++)
    {
        face = mFaceList[i];

        if (face->GetNumVertices() > 2)
        {
            ComputeFaceNormal(mVertexList[face->GetVertex(0)],
                              mVertexList[face->GetVertex(1)],
                              mVertexList[face->GetVertex(2)],
                              normal);
            face->SetNormal(normal[0], normal[1], normal[2]);
        }
    }
}

// concatenate another faceted object
void FacetedObject::Concatenate(FacetedObject *object)
{
    int i;
    Vertex **oldVertexList = mVertexList;
    Face **oldFaceList = mFaceList;
    int oldNumVertices = mNumVertices;
    int oldNumFaces = mNumFaces;
    int objectNumVertices = object->GetNumVertices();
    int objectNumFaces = object->GetNumFaces();

    // copy over old lists
    mNumVertices = oldNumVertices + objectNumVertices;
    mVertexList = new Vertex *[mNumVertices];
    for (i = 0; i < oldNumVertices; i++) mVertexList[i] = oldVertexList[i];
    if (oldVertexList) delete [] oldVertexList;

    mNumFaces = oldNumFaces + objectNumFaces;
    mFaceList = new Face *[mNumFaces];
    for (i = 0; i < oldNumFaces; i++) mFaceList[i] = oldFaceList[i];
    if (oldFaceList) delete [] oldFaceList;

    // copy vertices
    for (i = oldNumVertices; i < mNumVertices; i++)
    {
        mVertexList[i] = new Vertex();
        *mVertexList[i] = *object->GetVertex(i - oldNumVertices);
    }

    // copy faces offsetting vertex numbers
    for (i = oldNumFaces; i < mNumFaces; i++)
    {
        mFaceList[i] = new Face();
        *mFaceList[i] = *object->GetFace(i - oldNumFaces);
        mFaceList[i]->OffsetVertices(oldNumVertices);
    }
}

// move the object
void FacetedObject::Move(double x, double y, double z)
{
    int i;

    for (i = 0; i < mNumVertices; i++)
    {
        mVertexList[i]->x += x;
        mVertexList[i]->y += y;
        mVertexList[i]->z += z;
    }
}

// scale the object
void FacetedObject::Scale(double x, double y, double z)
{
    int i;

    for (i = 0; i < mNumVertices; i++)
    {
        mVertexList[i]->x *= x;
        mVertexList[i]->y *= y;
        mVertexList[i]->z *= z;
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
        std::cerr << "FacetedObject::SwapAxes axis out of range\n";
        exit(1);
    }
    #define SWAP(a,b) {t = a; a = b; b = t;}
    
    for (i = 0; i < mNumVertices; i++)
    {
        if (axis1 == 0 && axis2 == 1) SWAP(mVertexList[i]->x, mVertexList[i]->y)
        if (axis1 == 0 && axis2 == 2) SWAP(mVertexList[i]->x, mVertexList[i]->z)
        if (axis1 == 1 && axis2 == 2) SWAP(mVertexList[i]->y, mVertexList[i]->z)
        if (axis1 == 1 && axis2 == 0) SWAP(mVertexList[i]->y, mVertexList[i]->x)
        if (axis1 == 2 && axis2 == 0) SWAP(mVertexList[i]->z, mVertexList[i]->x)
        if (axis1 == 2 && axis2 == 1) SWAP(mVertexList[i]->z, mVertexList[i]->y)
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
        std::cerr << "FacetedObject::RotateAxes axis out of range\n";
        exit(1);
    }
    
    for (i = 0; i < mNumVertices; i++)
    {
        if (axis0 == 0) t0 = mVertexList[i]->x;
        if (axis0 == 1) t0 = mVertexList[i]->y;
        if (axis0 == 2) t0 = mVertexList[i]->z;
        if (axis1 == 0) t1 = mVertexList[i]->x;
        if (axis1 == 1) t1 = mVertexList[i]->y;
        if (axis1 == 2) t1 = mVertexList[i]->z;
        if (axis2 == 0) t2 = mVertexList[i]->x;
        if (axis2 == 1) t2 = mVertexList[i]->y;
        if (axis2 == 2) t2 = mVertexList[i]->z;
        mVertexList[i]->x = t0;
        mVertexList[i]->y = t1;
        mVertexList[i]->z = t2;
    }
}

// mirror the object
void FacetedObject::Mirror(bool x, bool y, bool z)
{
    int i;
    int actions = 0;

    if (x)
    {
        for (i = 0; i < mNumVertices; i++) mVertexList[i]->x =  -mVertexList[i]->x;
        actions++;
    }

    if (y)
    {
        for (i = 0; i < mNumVertices; i++) mVertexList[i]->y =  -mVertexList[i]->y;
        actions++;
    }

    if (z)
    {
        for (i = 0; i < mNumVertices; i++) mVertexList[i]->z =  -mVertexList[i]->z;
        actions++;
    }

    // if we have an odd number of mirroring operations we need to
    // reverse the order that the faces are defined
    if (ODD(actions))
    {
        for (i = 0; i < mNumFaces; i++) mFaceList[i]->ReverseVertexOrder();
    }

    // if we've done anything then we need to recalculate the normals
    if (actions) CalculateNormals();
}

// print out stats on the object
void FacetedObject::Stats()
{
    int i;
    Vertex min = {DBL_MAX, DBL_MAX, DBL_MAX};
    Vertex max = {-DBL_MAX, -DBL_MAX, -DBL_MAX};

    std::cout << "FacettedObject: " << m_Name << "\n";
    std::cout << "Vertices: " << mNumVertices << "\n";
    std::cout << "Faces: " << mNumFaces << "\n";

    for (i = 0; i < mNumVertices; i++)
    {
        min.x = MIN(min.x, mVertexList[i]->x);
        min.y = MIN(min.y, mVertexList[i]->y);
        min.z = MIN(min.z, mVertexList[i]->z);
        max.x = MAX(max.x, mVertexList[i]->x);
        max.y = MAX(max.y, mVertexList[i]->y);
        max.z = MAX(max.z, mVertexList[i]->z);
    }
    std::cout << "Vertex extents (" << min.x << ", " << min.y << ", " << min.z <<
        ") to (" << max.x << ", " << max.y << ", " << max.z << ")\n";

    std::cout.flush();
}

// simple routine to convert convex polygons to triangles
void FacetedObject::Triangulate()
{
    int i;
    int numVertices;
    int numNewFaces = 0;
    int currentNewFace = 0;
    int v0, v1, v2;
    Face **newFaceList;

    if (mNumFaces == 0) return; // nothing to do

    // count the number of faces after triangulation
    for (i = 0; i < mNumFaces; i++)
    {
        numVertices = mFaceList[i]->GetNumVertices();
        if (numVertices >= 3) numNewFaces += (numVertices - 2);
    }
    if (numNewFaces == mNumFaces) return; // nothing to do

    if (numNewFaces == 0) // no triangular faces
    {
        for (i = 0; i < mNumFaces; i++) delete mFaceList[i];
        delete [] mFaceList;
        mFaceList = 0;
        mNumFaces = 0;
        return;
    }

    // allocate the new faces
    newFaceList = new Face *[numNewFaces];
    for (i = 0; i < numNewFaces; i++) newFaceList[i] = new Face();

    // do the triangulation
    for (i = 0; i < mNumFaces; i++)
    {
        numVertices = mFaceList[i]->GetNumVertices();
        if (numVertices == 3)
        {
            *newFaceList[currentNewFace] = *mFaceList[i];
            currentNewFace++;
        }
        else
        {
            if (numVertices > 3)
            {
                v0 = 0;
                for (v2 = 2; v2 < numVertices; v2++)
                {
                    v1 = v2 - 1;
                    newFaceList[currentNewFace]->SetNumVertices(3);
                    newFaceList[currentNewFace]->SetVertex(0, mFaceList[i]->GetVertex(v0));
                    newFaceList[currentNewFace]->SetVertex(1, mFaceList[i]->GetVertex(v1));
                    newFaceList[currentNewFace]->SetVertex(2, mFaceList[i]->GetVertex(v2));
                    currentNewFace++;
                }
            }
        }
    }

    // and change over the face list
    for (i = 0; i < mNumFaces; i++) delete mFaceList[i];
    delete [] mFaceList;
    mFaceList = newFaceList;
    mNumFaces = numNewFaces;
}

// add a face to the object
// adds vertices
void FacetedObject::AddFace(Vertex vertex[], int numVertices, bool fast)
{
    int i;
    int *vertexIndices = new int[numVertices];
    Face *face = new Face();

    for (i = 0; i < numVertices; i++)
        vertexIndices[i] = AddVertex(vertex[i], fast);

    face->SetNumVertices(numVertices);
    for (i = 0; i < numVertices; i++)
        face->SetVertex(i, vertexIndices[i]);

    AddFace(face);
    delete [] vertexIndices;
}

// add a face to the object
// assumes all vertex indices are correct
// takes over ownership of the pointer
void FacetedObject::AddFace(Face *face)
{
    int i;
    if (mNumFaces >= mNumFacesAllocated)
    {
        mNumFacesAllocated += kAllocationIncrement;
        Face **newFaceList = new Face *[mNumFacesAllocated];
        if (mFaceList)
        {
            for (i = 0; i < mNumFaces; i++)
                newFaceList[i] = mFaceList[i];
            delete [] mFaceList;
        }
        mFaceList = newFaceList;
    }
    mFaceList[mNumFaces] = face;
    mNumFaces++;
}

// add a vertex to an object
// if fast is true it doesn' try to match with existing vertices
int FacetedObject::AddVertex(Vertex vertex, bool fast)
{
    int i;
    // try to match existing vertex
    if (fast == false)
    {
        for (i = 0; i < mNumVertices; i++)
        {
            if (fabs(vertex.x - mVertexList[i]->x) < m_VertexMatchTolerance &&
                fabs(vertex.y - mVertexList[i]->y) < m_VertexMatchTolerance &&
                fabs(vertex.z - mVertexList[i]->z) < m_VertexMatchTolerance ) return i;
        }
    }

    Vertex *newVertex = new Vertex();
    *newVertex = vertex;
    if (mNumVertices >= mNumVerticesAllocated)
    {
        mNumVerticesAllocated += kAllocationIncrement;
        Vertex **newVertexList = new Vertex *[mNumVerticesAllocated];
        if (mVertexList)
        {
            for (i = 0; i < mNumVertices; i++)
                newVertexList[i] = mVertexList[i];
            delete [] mVertexList;
        }
        mVertexList = newVertexList;
    }
    mVertexList[mNumVertices] = newVertex;
    mNumVertices++;
    return mNumVertices - 1;
}

// apply a function the object
void FacetedObject::ApplyAnyFunction(AnyFunTransform3D *function)
{
    int i;
    pgd::Vector v;
    
    for (i = 0; i < mNumVertices; i++)
    {
        v.x = mVertexList[i]->x;
        v.y = mVertexList[i]->y;
        v.z = mVertexList[i]->z;
        function->Calculate(&v);
        mVertexList[i]->x = v.x;
        mVertexList[i]->y = v.y;
        mVertexList[i]->z = v.z;
    }
}

// apply a function the object
void FacetedObject::ApplyMatrix3x3(pgd::Matrix3x3 *matrix)
{
    int i;
    pgd::Vector v;
    
    for (i = 0; i < mNumVertices; i++)
    {
        v.x = mVertexList[i]->x;
        v.y = mVertexList[i]->y;
        v.z = mVertexList[i]->z;
        v = *matrix * v;
        mVertexList[i]->x = v.x;
        mVertexList[i]->y = v.y;
        mVertexList[i]->z = v.z;
    }
}

