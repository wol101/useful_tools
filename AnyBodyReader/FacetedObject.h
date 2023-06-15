/*
 *  FacettedObject.h
 *  GaitSymODE
 *
 *  Created by Bill Sellers on 13/09/2005.
 *  Copyright 2005 Bill Sellers. All rights reserved.
 *
 */

#ifndef FacetedObject_h
#define FacetedObject_h

#include "PGDMath.h"

class Vertex;
class Face;
class DataFile;
class ostringstream;
class AnyFunTransform3D;

class FacetedObject
{
        public:
        FacetedObject();
        virtual ~FacetedObject();
        
        bool ParseOBJFile(const char *filename);
        bool ParseOBJFile(DataFile &theFile);
        bool ParseANYSURFFile(const char *filename);
        
        virtual void WriteOBJFile(const char *filename);
        virtual void WriteOBJFile(std::ostringstream &out);
    
        int AddVertex(Vertex vertex, bool fast = true);
        int GetNumVertices()
        {
            return mNumVertices;
        };
        Vertex *GetVertex(int i)
        {
            if (i < 0 || i >= mNumVertices) throw (__LINE__);
            return mVertexList[i];
        }
    
        void AddFace(Face *face);
        void AddFace(Vertex vertex[], int numVertices, bool fast = true);
        int GetNumFaces()
        {
            return mNumFaces;
        };
        Face *GetFace(int i)
        {
            if (i < 0 || i >= mNumFaces) throw (__LINE__);
            return mFaceList[i];
        };
    
        void ClearLists();
        void Concatenate(FacetedObject *object);
                
        void CalculateNormals();
    
        // static utilities
        static void ComputeFaceNormal(const Vertex *v1, 
            const Vertex *v2, const Vertex *v3, double normal[3]);
    
        // manipulation functions
        void Move(double x, double y, double z);
        void Scale(double x, double y, double z);
        void Mirror(bool x, bool y, bool z);
        void SwapAxes(int axis1, int axis2);
        void RotateAxes(int axis0, int axis1, int axis2);
        void ApplyAnyFunction(AnyFunTransform3D *function);
        void ApplyMatrix3x3(pgd::Matrix3x3 *matrix);
    
        // reporting functions
        void Stats();
        std::string GetName() { return m_Name; };
        
        // utility
        void Triangulate();
            
    protected:
                    
        int mNumVertices;
        int mNumVerticesAllocated;
        Vertex **mVertexList;
        int mNumFaces;
        int mNumFacesAllocated;
        Face **mFaceList;
        bool mDrawClockwise;
        double m_VertexMatchTolerance;
        bool m_UseRelativeOBJ;
        std::string m_Name;
            
        static const int kAllocationIncrement = 1000;
    };

#endif
