// FacetedObject.h -- maintain and draw a faceted object

#ifndef FacetedObject_h
#define FacetedObject_h

#ifndef NO_GUI
#include <gl.h>
#endif

#include <map>
#include <vector>
#include "MyVertex.h"
#include "MyFace.h"
#include "VoxelObject.h"

const int maxVerticesPerFace = 256;

class FacetedObject
    {
    public:
        FacetedObject();
        ~FacetedObject();
        
        void SetName(const char *name);
        const char *GetName() 
        { 
            return mName; 
        };
        
        int AddVertex(MyVertex vertex);
        int GetNumVertices()
        {
            return (*mVertexList).size();
        };
        MyVertex *GetVertex(int i)
        {
            if (i < 0 || i >= (*mVertexList).size()) throw (0);
            return &(*mVertexList)[i];
        }
        
        // void AddFace(MyFace *face);
        void AddFace(MyVertex vertex[], int numVertices);
        int GetNumFaces()
        {
            return (*mFaceList).size();
        };
        MyFace *GetFace(int i)
        {
            if (i < 0 || i >= (*mFaceList).size()) throw (0);
            return (*mFaceList)[i];
        };
        
        void ClearLists();
        
        void Concatenate(FacetedObject *object, int keepNames);
        
        void Draw();
        
        void SetDrawClockwise(bool v)
        {
            mDrawClockwise = v;
        };
        void SetDrawAntiClockwise(bool v)
        {
            mDrawAntiClockwise = v;
        };
        
        void CalculateNormals();
        void ConvertGroupsToObjects();
        
        // static utilities
        static void ComputeFaceNormal(const MyVertex *v1, const MyVertex *v2, const MyVertex *v3, double normal[3]);
        
        
        // embedded sort routines
        void SortFacesByGroup();
        static int CompareFacesByGroup(const void *v1, const void *v2);
        
        // manipulation functions
        void Move(double x, double y, double z);
        void Scale(double x, double y, double z);
        void Mirror(bool x, bool y, bool z);
        void SwapAxes(int axis1, int axis2);
        void RotateAxes(int axis0, int axis1, int axis2);
        void RotateAxisAngle(double x, double y, double z, double angle);
        void RotateQuaternion(double w, double x, double y, double z);
        void ReverseWinding();
        
        // reporting functions
        void Stats();
        MyVertex* GetLimits();
        
        // utility
        void Triangulate();
        void Voxelize(double x, double y, double z, int outlineVoxels);
        void Isosurface(int threshold, int cubeBased);
        void MergeVertices(double tolerance);
        
        // friends
        friend void WriteVoxelFiles(const char *rawFilename, const char *textFilename, FacetedObject *obj, int bits);
        friend void WriteVoxelVTKFile(const char *vtkFilename, FacetedObject *obj, int binary_flag);
        friend void WriteVoxelDICOMFile(const char *dicomFilename, FacetedObject *obj);
        
    protected:
        
        std::vector<MyVertex> *mVertexList;
        std::vector<MyFace *> *mFaceList;
        bool mDrawClockwise;
        bool mDrawAntiClockwise;
        char *mName;
        MyVertex mLimits[2];
        
        std::map<MyVertex, int> mVertexMap;
        std::map<MyFace, int> mFaceMap;
        
        // voxel parts
        unsigned char *mVoxelData;
        MyVertex mVoxelOrigin;
        MyVertex mVoxelSize;
        Location mVoxelNumbers;
    };

#endif
