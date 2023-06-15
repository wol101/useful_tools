// MyFace.h - face object

#ifndef MyFace_h
#define MyFace_h

#include "MyVertex.h"

// including this automatically defines the boolean operators based on operator== and operator<
#include <utility>

class MyFace
{
public:
    MyFace();
    MyFace(const MyFace &face);
    ~MyFace();
    
    int GetVertex(int i)
    {
        if (i < 0 || i >= mNumVertices) throw (0);
        return mVertexList[i];
    };
    
    void SetVertex(int i, int v)
    {
        if (i < 0 || i >= mNumVertices) throw (0);
        mVertexList[i] = v;
    };
    
    int GetNumVertices()
    {
        return mNumVertices;
    };
    
    void SetNumVertices(int numVertices);
    
    void SetGroup(const char *string);
    void SetObject(const char *string);
    
    const char *GetGroup()
    {
        return mGroup;
    };
    
    const char *GetObject()
    {
        return mObject;
    };
    
    MyFace& operator=(MyFace &in);
    
    void SetNormal(double x, double y, double z)
    {
        mNormal[0] = x;
        mNormal[1] = y;
        mNormal[2] = z;
    };
    void GetNormal(double normal[3])
    {
        normal[0] = mNormal[0];
        normal[1] = mNormal[1];
        normal[2] = mNormal[2];
    };
    void GetNormal(float normal[3])
    {
        normal[0] = (float)mNormal[0];
        normal[1] = (float)mNormal[1];
        normal[2] = (float)mNormal[2];
    };
    
    void ReverseVertexOrder();
    void OffsetVertices(int offset);
    
    // operator< needed for map to work
    inline bool operator<(const MyFace &u) const
    {
        if (*this == u) return false;
        if (this->mNumVertices < u.mNumVertices) return true;
        if (this->mNumVertices > u.mNumVertices) return false;
        for (int i = 0; i < this->mNumVertices; i++)
        {
            if (this->mVertexList[i] < u.mVertexList[i]) return true;
            else if (this->mVertexList[i] > u.mVertexList[i]) return false;
        }
        return false;
    }
    
    inline bool operator==(const MyFace &u) const
    {
        if (this->mNumVertices != u.mNumVertices) return false;
        int i, j;
        for (i = 0; i < this->mNumVertices; i++)
        {
            if (this->mVertexList[0] == u.mVertexList[i])
            {
                for (j = 1; j < this->mNumVertices; j++)
                {
                    if (this->mVertexList[j] != u.mVertexList[(i + j) % this->mNumVertices]) return false;
                }
                return true;
            }
        }
        return false;
    }
    
    
protected:
    int mNumVertices;
    int *mVertexList;
    char *mGroup;
    char *mObject;
    double mNormal[3];
};

#endif
                         
