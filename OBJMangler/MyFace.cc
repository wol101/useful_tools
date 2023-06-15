// MyFace.cc - Face Object - necessary because faces can contain
// a variable number of vertices

#include <iostream>
#include <string.h>

#include "MyFace.h"

MyFace::MyFace()
{
    mNumVertices = 0;
    mVertexList = 0;
    mNormal[0] = 1;
    mNormal[1] = 0;
    mNormal[2] = 0;
    mGroup = 0;
    mObject = 0;
}

MyFace::MyFace(const MyFace &face)
{
    mNumVertices = face.mNumVertices;
    if (mNumVertices > 0)
    {
        mVertexList = new int[mNumVertices];
        for (int i = 0; i < mNumVertices; i++) mVertexList[i] = face.mVertexList[i];
    }
    else
    {
        mVertexList = 0;
    }
    
    SetNormal(face.mNormal[0], face.mNormal[1], face.mNormal[2]);
    mGroup = 0;
    mObject = 0;
    SetGroup(face.mGroup);
    SetObject(face.mObject);
}

MyFace::~MyFace()
{
    if (mVertexList) delete [] mVertexList;
    if (mGroup) delete [] mGroup;
    if (mObject) delete [] mObject;
}

void MyFace::SetNumVertices(int numVertices)
{
    if (numVertices != mNumVertices)
    {
        mNumVertices = numVertices;
        if (mVertexList) delete [] mVertexList;
        mVertexList = new int[mNumVertices];
    }
    if (mGroup) delete [] mGroup;
    if (mObject) delete [] mObject;
}

MyFace& MyFace::operator=(MyFace &in)
{
    SetNumVertices(in.GetNumVertices());
    for (int i = 0; i < mNumVertices; i++)
        mVertexList[i] = in.GetVertex(i);

    SetGroup(in.GetGroup());
    SetObject(in.GetObject());
    in.GetNormal(mNormal);

    return *this;
}

void MyFace::SetGroup(const char *string)
{
    if (string == 0)
    {
        if (mGroup)
        {
            delete [] mGroup;
            mGroup = 0;
        }
        return;
    }

    size_t l = strlen(string);
    if (mGroup)
    {
        if (l != strlen (mGroup))
        {
            delete [] mGroup;
            mGroup = new char[l + 1];
        }
    }
    else mGroup = new char[l + 1];
    strcpy(mGroup, string);
}

void MyFace::SetObject(const char *string)
{
    if (string == 0)
    {
        if (mObject)
        {
            delete [] mObject;
            mObject = 0;
        }
        return;
    }

    size_t l = strlen(string);
    if (mObject)
    {
        if (l != strlen (mObject))
        {
            delete [] mObject;
            mObject = new char[l + 1];
        }
    }
    else mObject = new char[l + 1];
    strcpy(mObject, string);
}

void MyFace::ReverseVertexOrder()
{
    int tmp;
    int *p1 = mVertexList;
    int *p2 = mVertexList + mNumVertices - 1;

    while (p1 < p2)
    {
        tmp = *p1;
        *p1 = *p2;
        *p2 = tmp;
        p1++;
        p2--;
    }
}

void MyFace::OffsetVertices(int offset)
{
    int i;
    for (i = 0; i < mNumVertices; i++) mVertexList[i] += offset;
}



