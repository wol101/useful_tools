#include <stdio.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <time.h>
#include <float.h>
#include <string>
#include <sstream>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>

#ifndef NO_GUI
#include <glut.h>
#endif

using namespace std;

#include "GLUIRoutines.h"
#include "FacetedObject.h"
#include "MyFace.h"
#include "MyVertex.h"
#include "TextFile.h"
#include "FacetedObjectList.h"
#include "Shapes.h"
#include "mymath.h"
#include "l3ds.h"
#include "Dicom.h"

#define gDebugDeclare
#include "Debug.h"

#define MAX(a,b) ((a) > (b) ? (a) : (b))
#define MIN(a,b) ((a) < (b) ? (a) : (b))
#define ABS(a) ((a) >= 0 ? (a) : -(a))
#define ODD(n) ((n) & 1)

double MyVertex::epsilon = 1e-8;

FacetedObject *gDisplayObject = 0;
FacetedObjectList gObjectList;
bool gVerboseFlag = false;

char *gSub1 = 0;
char *gSub2 = 0;

void ExecuteInstructionFile(const char *filename);
void ParseOBJFile(const char *filename, FacetedObject *obj);
void ParseANYSURFFile(const char *filename, FacetedObject *obj);
void Parse3DSFile(const char *filename, FacetedObject *obj);
void ParsePointCloudFile(const char *filename, double size, FacetedObject *obj);
void Parse2DPolylineFile(const char *filename, FacetedObject *obj);
void WriteOBJFile(const char *filename, FacetedObject *obj, int precision = 7);
void WriteVoxelFiles(const char *rawFilename, const char *textFilename, FacetedObject *obj, int bits);
void WriteVoxelVTKFile(const char *vtkFilename, FacetedObject *obj, int binary_flag);
void WriteVoxelDICOMFile(const char *dicomFilename, FacetedObject *obj);
void ParseError(char **tokens, int numTokens, int count, const char *message);
int JustTheFileName(const char *input, char *output, int len);


int main(int argc, char ** argv)
{
    char *instructionFile = 0;
    char *objFile = 0;
    char *threeDSFile = 0;
    char *anysurfFile = 0;
    char *pcFile = 0;
    char *twoDPolylineFile = 0;
    int i;
    int size = 1;

#ifndef NO_GUI
    glutInit(&argc, argv);
#endif

    // parse the command line
    try
    {
        for (i = 1; i < argc; i++)
        {
            // do something with arguments

            if (strcmp(argv[i], "--instructionFile") == 0 ||
                strcmp(argv[i], "-p") == 0)
            {
                i++;
                if (i >= argc) throw(1);
                instructionFile = argv[i];
            }

            else
                if (strcmp(argv[i], "--objFile") == 0 ||
                    strcmp(argv[i], "-j") == 0)
                {
                    i++;
                    if (i >= argc) throw(1);
                    objFile = argv[i];
                }

            else
                if (strcmp(argv[i], "--3dsFile") == 0 ||
                    strcmp(argv[i], "-3") == 0)
                {
                    i++;
                    if (i >= argc) throw(1);
                    threeDSFile = argv[i];
                }

            else
                if (strcmp(argv[i], "--anysurfFile") == 0 ||
                    strcmp(argv[i], "-a") == 0)
                {
                    i++;
                    if (i >= argc) throw(1);
                    anysurfFile = argv[i];
                }

            else
                if (strcmp(argv[i], "--pcFile") == 0 ||
                    strcmp(argv[i], "-c") == 0)
                {
                    i++;
                    if (i >= argc) throw(1);
                    pcFile = argv[i];
                }
            
            else
                if (strcmp(argv[i], "--2DPolylineFile") == 0 ||
                    strcmp(argv[i], "-2p") == 0)
                {
                    i++;
                    if (i >= argc) throw(1);
                    twoDPolylineFile = argv[i];
                }
            
            else
                if (strcmp(argv[i], "--size") == 0 ||
                    strcmp(argv[i], "-s") == 0)
                {
                    i++;
                    if (i >= argc) throw(1);
                    size = strtol(argv[i], 0, 10);
                }

            else
                if (strcmp(argv[i], "--debug") == 0 ||
                    strcmp(argv[i], "-e") == 0)
                {
                    i++;
                    if (i >= argc) throw(1);
                    gDebug = atoi(argv[i]);
                }

            else
                if (strcmp(argv[i], "--substitute") == 0 ||
                    strcmp(argv[i], "-u") == 0)
                {
                    i++;
                    if (i >= argc) throw(1);
                    gSub1 = argv[i];
                    i++;
                    if (i >= argc) throw(1);
                    gSub2 = argv[i];
                }
            
            else
                if (strcmp(argv[i], "--verbose") == 0 ||
                    strcmp(argv[i], "-v") == 0)
                {
                    gVerboseFlag = true;
                }

            else
            {
                throw(1);
            }
        }
        if (instructionFile == 0 && 
            objFile == 0 && 
            threeDSFile == 0 && 
            anysurfFile == 0 && 
            pcFile == 0 && 
            twoDPolylineFile == 0) throw(2);
    }

    // catch argument errors
    catch (...)
    {
        cerr << "\nOBJMangler program\n";
        cerr << "Build " << __DATE__ << " " << __TIME__ << "\n";
        cerr << "Supported options:\n\n";
        cerr << "(-p) --instructionFile instruction_file_name\n";
        cerr << "(-j) --objFile obj_file_name\n";
        cerr << "(-3) --3dsFile 3ds_file_name\n";
        cerr << "(-a) --anysurfFile anysurf_file_name\n";
        cerr << "(-c) --pcFile point_cloud_file_name\n";
        cerr << "(-2p) --2DPolylineFile 2d_polyline_file_name\n";
        cerr << "(-s) --size size\n";
        cerr << "(-u) --substitute string1 string2\n";
        cerr << "(-v) --verbose\n";
        cerr << "(-e) --debug n\n";
        cerr << "\nNote:\n";
        cerr << "Short form is shown in brackets.\n";
        cerr << "\nAvailable Commands\n\n";
        cerr <<
        "read in a new FacetedObject from a .obj file\n"
        "ReadOBJFile FacetedObjectName Filename\n"
        "\n"
        "read in a new FacetedObject from a .anysurf file\n"
        "ReadANYSURFFile FacetedObjectName Filename\n"
        "\n"
        "read in a new FacetedObject from a .3ds file\n"
        "Read3DSFile FacetedObjectName Filename\n"
        "\n"
        "read in a new FacetedObject from a .asc point cloud file\n"
        "ReadPointCloudFile FacetedObjectName Filename size\n"
        "\n"
        "write a FacetedObject to a .obj file. Precision is number of decimal points to use (optional)\n"
        "WriteOBJFile FacetedObjectName Filename precision\n"
        "\n"
        "write a Voxelated FacetedObject to a raw .dat file plus descriptive .txt file\n"
        "WriteVoxelFiles FacetedObjectName RawFilename TextFilename\n"
        "WriteVoxelFiles FacetedObjectName RawFilename TextFilename bits\n"
        "\n"
        "write a Voxelated FacetedObject to a .vtk file\n"
        "set BinaryFlag to zero if ascii file wanted\n"
        "WriteVoxelVTKFile FacetedObjectName VTKFilename BinaryFlag\n"
        "\n"
        "write a Voxelated FacetedObject to a DICOM file\n"
        "WriteVoxelDICOMFile FacetedObjectName DICOMFilename\n"
        "\n"
        "convert groups to objects\n"
        "ConvertGroupsToObjects FacetedObjectName\n"
        "\n"
        "Sort by groups\n"
        "SortByGroup FacetedObjectName\n"
        "\n"
        "Reverse Face Winding\n"
        "ReverseWinding FacetedObjectName\n"
        "\n"
        "Mirror about x=0, y=0 and z=0 planes\n"
        "set axis to non-zero to flip\n"
        "Mirror FacetedObjectName x y z\n"
        "\n"
        "Swap pair of axes\n"
        "x = 0, y = 1, z = 2\n"
        "SwapAxes FacetedObjectName axis1 axis2\n"
        "\n"
        "Rotate (as in free swap) axes\n"
        "x = 0, y = 1, z = 2\n"
        "RotateAxes FacetedObjectName axis0 axis1 axis2\n"
        "\n"
        "Apply an axis-angle rotation\n"
        "note angle is in degrees\n"
        "RotateAxisAngle FacetedObjectName x y z angle\n"
        "\n"
        "Apply a quaternion rotation\n"
        "note argument order is the same as ODE\n"
        "RotateQuaternion FacetedObjectName w x y z\n"
        "\n"
        "Move\n"
        "Move FacetedObjectName x y z\n"
        "\n"
        "Scale\n"
        "Scale FacetedObjectName x y z\n"
        "\n"
        "Set the display FacetedObject\n"
        "SetDisplay FacetedObjectName\n"
        "\n"
        "Create a new, empty FacetedObject\n"
        "CreateNew FacetedObjectName\n"
        "\n"
        "concatenate two FacetedObjects (adds second object to first object)\n"
        "Concatenate FacetedObjectName FacetedObjectName KeepNames\n"
        "Set KeepNames to non-zero if want object names kept\n"
        "\n"
        "report basic stats on object\n"
        "Stats FacetedObjectName\n"
        "\n"
        "simple convex polygon triangulation\n"
        "Triangulate FacetedObjectName\n"
        "\n"
        "delete an object\n"
        "Delete FacetedObjectName\n"
        "\n"
        "Voxelize (produces a voxel based version of triangles and optionally outlines them if outlineVoxels != 0)\n"
        "Voxelize FacetedObjectName x y z\n"
        "Voxelize FacetedObjectName x y z outlineVoxels\n"
        "x y z are the size of the voxels (flaoting point values)\n"
        "\n"
        "Isosurface a voxel version (use 64 or 196 to either include or exclude boundary layer)\n"
        "Isosurface FacetedObjectName isolevel\n"
        "Isosurface FacetedObjectName isolevel cubeBased\n"
        "if cube_based is non zero (default) then use cube based algorithm (quicker, fewer triangles, less smooth)\n"
        "\n"
        "Merge close or identical vertices\n"
        "MergeVertices FacetedObjectName tolerance\n"
        "\n"
        "Set the epsilon for new vertices (this value is used to decide whether a vertex matches an existing one)\n"
        "the default is 1e-8\n"
        "SetEpsilon epsilon\n"
        "\n";
        
        
        return (1);
    }

    if (objFile)
    {
        gDisplayObject = new FacetedObject();
        ParseOBJFile(objFile, gDisplayObject);
    }

    if (threeDSFile)
    {
        gDisplayObject = new FacetedObject();
        Parse3DSFile(threeDSFile, gDisplayObject);
        string buffer(threeDSFile);
        if (buffer.substr(buffer.length() - 4) == ".3ds" ||
            buffer.substr(buffer.length() - 4) == ".3DS" )
        {
            buffer.erase(buffer.length() - 4);
        }
        buffer.append(".obj");
        WriteOBJFile(buffer.c_str(), gDisplayObject);
    }

    if (anysurfFile)
    {
        gDisplayObject = new FacetedObject();
        ParseANYSURFFile(anysurfFile, gDisplayObject);
        string buffer(anysurfFile);
        if (buffer.substr(buffer.length() - 8) == ".anysurf" ||
            buffer.substr(buffer.length() - 8) == ".ANYSURF" )
        {
            buffer.erase(buffer.length() - 8);
        }
        buffer.append(".obj");
        WriteOBJFile(buffer.c_str(), gDisplayObject);
    }

    if (pcFile)
    {
        gDisplayObject = new FacetedObject();
        ParsePointCloudFile(pcFile, size, gDisplayObject);
        string buffer(pcFile);
        if (buffer.substr(buffer.length() - 4) == ".asc" ||
            buffer.substr(buffer.length() - 4) == ".ASC" )
        {
            buffer.erase(buffer.length() - 4);
        }
        buffer.append(".obj");
        WriteOBJFile(buffer.c_str(), gDisplayObject);
    }

    if (twoDPolylineFile)
    {
        gDisplayObject = new FacetedObject();
        Parse2DPolylineFile(twoDPolylineFile, gDisplayObject);
        string buffer(twoDPolylineFile);
        if (buffer.substr(buffer.length() - 4) == ".txt" ||
            buffer.substr(buffer.length() - 4) == ".TXT" )
        {
            buffer.erase(buffer.length() - 4);
        }
        buffer.append(".obj");
        WriteOBJFile(buffer.c_str(), gDisplayObject);
    }
    
    if (instructionFile)
        ExecuteInstructionFile(instructionFile);

#ifndef NO_GUI
    if (gDisplayObject)
    {
        StartGlut();

        glutMainLoop();
    }
#endif

    return 0;
}

// read and execute the instruction file
void ExecuteInstructionFile(const char *filename)
{
    TextFile theFile;
    char buffer[256];
    char line[256];
    char *tokens[256];
    int numTokens;
    int count = 0;
    FacetedObject *object;
    FacetedObject *object2;

    theFile.SetExitOnError(true);
    theFile.ReadFile(filename);
    
    if (gSub1)
    {
        theFile.Replace(gSub1, gSub2);
    }

    while (theFile.ReadNextLine(line, sizeof(buffer), true, '#') == false)
    {
        count++;
        strcpy(buffer, line);
        numTokens = TextFile::ReturnTokens(buffer, tokens, sizeof(tokens));

        if (gDebug == DebugOBJMangler)
            cerr << "ExecuteInstructionFile:\tnumTokens\t" << numTokens << "\n";
                    
        if (gVerboseFlag) cout << line << "\n";
        cout.flush();

        // read in a new FacetedObject from a .obj file
        // ReadOBJFile FacetedObjectName Filename
        if (strcmp(tokens[0], "ReadOBJFile") == 0)
        {
            if (numTokens != 3) ParseError(tokens, numTokens, count, "Wrong number of tokens");
            if (gObjectList.Get(tokens[1])) ParseError(tokens, numTokens, count, "FacetedObject name not unique");
            object = new FacetedObject();
            object->SetName(tokens[1]);
            ParseOBJFile(tokens[2], object);
            gObjectList.Add(object);
        }

        // read in a new FacetedObject from a .anysurf file
        // ReadANYSURFFile FacetedObjectName Filename
        else
            if (strcmp(tokens[0], "ReadANYSURFFile") == 0)
            {
                if (numTokens != 3) ParseError(tokens, numTokens, count, "Wrong number of tokens");
                if (gObjectList.Get(tokens[1])) ParseError(tokens, numTokens, count, "FacetedObject name not unique");
                object = new FacetedObject();
                object->SetName(tokens[1]);
                ParseANYSURFFile(tokens[2], object);
                gObjectList.Add(object);
            }

        // read in a new FacetedObject from a .3ds file
        // Read3DSFile FacetedObjectName Filename
        else
            if (strcmp(tokens[0], "Read3DSFile") == 0)
            {
                if (numTokens != 3) ParseError(tokens, numTokens, count, "Wrong number of tokens");
                if (gObjectList.Get(tokens[1])) ParseError(tokens, numTokens, count, "FacetedObject name not unique");
                object = new FacetedObject();
                object->SetName(tokens[1]);
                Parse3DSFile(tokens[2], object);
                gObjectList.Add(object);
            }

        // read in a new FacetedObject from a .asc point cloud file
        // ReadPointCloudFile FacetedObjectName Filename size
        else
            if (strcmp(tokens[0], "ReadPointCloudFile") == 0)
            {
                if (numTokens != 4) ParseError(tokens, numTokens, count, "Wrong number of tokens");
                if (gObjectList.Get(tokens[1])) ParseError(tokens, numTokens, count, "FacetedObject name not unique");
                object = new FacetedObject();
                object->SetName(tokens[1]);
                ParsePointCloudFile(tokens[2], strtod(tokens[3], 0), object);
                gObjectList.Add(object);
            }

        // write a FacetedObject to a .obj file. Precision is number of decimal points to use (optional)
        // WriteOBJFile FacetedObjectName Filename precision
        else
            if (strcmp(tokens[0], "WriteOBJFile") == 0)
            {
                if (numTokens != 3 && numTokens != 4) ParseError(tokens, numTokens, count, "Wrong number of tokens");
                object = gObjectList.Get(tokens[1]);
                if (object == 0) ParseError(tokens, numTokens, count, "FacetedObject name not found");
                if (numTokens == 4)
                    WriteOBJFile(tokens[2], object, strtol(tokens[3], 0, 10));
                else
                    WriteOBJFile(tokens[2], object);
            }

        // write a Voxelated FacetedObject to a raw .dat file plus descriptive .txt file
        // WriteVoxelFiles FacetedObjectName RawFilename TextFilename
        // WriteVoxelFiles FacetedObjectName RawFilename TextFilename bits
        else
            if (strcmp(tokens[0], "WriteVoxelFiles") == 0)
            {
                if (numTokens != 4 && numTokens != 5) ParseError(tokens, numTokens, count, "Wrong number of tokens");
                object = gObjectList.Get(tokens[1]);
                if (object == 0) ParseError(tokens, numTokens, count, "FacetedObject name not found");
                if (numTokens == 4)
                    WriteVoxelFiles(tokens[2], tokens[3], object, 8);
                else
                    WriteVoxelFiles(tokens[2], tokens[3], object, strtol(tokens[4], 0, 10));
            }
        
        // write a Voxelated FacetedObject to a .vtk file
        // set BinaryFlag to zero if ascii file wanted
        // WriteVoxelVTKFile FacetedObjectName VTKFilename BinaryFlag
        else
            if (strcmp(tokens[0], "WriteVoxelVTKFile") == 0)
            {
                if (numTokens != 3 && numTokens != 4) ParseError(tokens, numTokens, count, "Wrong number of tokens");
                object = gObjectList.Get(tokens[1]);
                if (object == 0) ParseError(tokens, numTokens, count, "FacetedObject name not found");
                if (numTokens == 4)
                    WriteVoxelVTKFile(tokens[2], object, strtol(tokens[3], 0, 10));
                else
                    WriteVoxelVTKFile(tokens[2], object, 1);
            }
        
        // write a Voxelated FacetedObject to a DICOM file
        // WriteVoxelDICOMFile FacetedObjectName DICOMFilename
        else
            if (strcmp(tokens[0], "WriteVoxelDICOMFile") == 0)
            {
                if (numTokens != 3) ParseError(tokens, numTokens, count, "Wrong number of tokens");
                object = gObjectList.Get(tokens[1]);
                if (object == 0) ParseError(tokens, numTokens, count, "FacetedObject name not found");
                WriteVoxelDICOMFile(tokens[2], object);
            }
        
        // convert groups to objects
        // ConvertGroupsToObjects FacetedObjectName
        else
            if (strcmp(tokens[0], "ConvertGroupsToObjects") == 0)
            {
                if (numTokens != 2) ParseError(tokens, numTokens, count, "Wrong number of tokens");
                object = gObjectList.Get(tokens[1]);
                if (object == 0) ParseError(tokens, numTokens, count, "FacetedObject name not found");
                object->ConvertGroupsToObjects();
            }

        // Sort by groups
        // SortByGroup FacetedObjectName
        else
            if (strcmp(tokens[0], "SortByGroup") == 0)
            {
                if (numTokens != 2) ParseError(tokens, numTokens, count, "Wrong number of tokens");
                object = gObjectList.Get(tokens[1]);
                if (object == 0) ParseError(tokens, numTokens, count, "FacetedObject name not found");
                object->SortFacesByGroup();
            }

        // Reverse Face Winding
        // ReverseWinding FacetedObjectName
        else
            if (strcmp(tokens[0], "ReverseWinding") == 0)
            {
                if (numTokens != 2) ParseError(tokens, numTokens, count, "Wrong number of tokens");
                object = gObjectList.Get(tokens[1]);
                if (object == 0) ParseError(tokens, numTokens, count, "FacetedObject name not found");
                object->ReverseWinding();
            }

        // Mirror about x=0, y=0 and z=0 planes
        // set axis to non-zero to flip
        // Mirror FacetedObjectName x y z
        else
            if (strcmp(tokens[0], "Mirror") == 0)
            {
                if (numTokens != 5) ParseError(tokens, numTokens, count, "Wrong number of tokens");
                object = gObjectList.Get(tokens[1]);
                if (object == 0) ParseError(tokens, numTokens, count, "FacetedObject name not found");
                object->Mirror((bool)strtol(tokens[2], 0, 10), (bool)strtol(tokens[3], 0, 10), (bool)strtol(tokens[4], 0, 10));
            }

        // Swap pair of axes
        // x = 0, y = 1, z = 2
        // SwapAxes FacetedObjectName axis1 axis2
        else
            if (strcmp(tokens[0], "SwapAxes") == 0)
            {
                if (numTokens != 4) ParseError(tokens, numTokens, count, "Wrong number of tokens");
                object = gObjectList.Get(tokens[1]);
                if (object == 0) ParseError(tokens, numTokens, count, "FacetedObject name not found");
                object->SwapAxes(strtol(tokens[2], 0, 10), strtol(tokens[3], 0, 10));
            }

        // Rotate (as in free swap) axes
        // x = 0, y = 1, z = 2
        // RotateAxes FacetedObjectName axis0 axis1 axis2
        else
            if (strcmp(tokens[0], "RotateAxes") == 0)
            {
                if (numTokens != 5) ParseError(tokens, numTokens, count, "Wrong number of tokens");
                object = gObjectList.Get(tokens[1]);
                if (object == 0) ParseError(tokens, numTokens, count, "FacetedObject name not found");
                object->RotateAxes(strtol(tokens[2], 0, 10), strtol(tokens[3], 0, 10), strtol(tokens[4], 0, 10));
            }

        // Apply an axis-angle rotation
        // note angle is in degrees
        // RotateAxisAngle FacetedObjectName x y z angle
        else
            if (strcmp(tokens[0], "RotateAxisAngle") == 0)
            {
                if (numTokens != 6) ParseError(tokens, numTokens, count, "Wrong number of tokens");
                object = gObjectList.Get(tokens[1]);
                if (object == 0) ParseError(tokens, numTokens, count, "FacetedObject name not found");
                object->RotateAxisAngle(strtod(tokens[2], 0), strtod(tokens[3], 0), strtod(tokens[4], 0), strtod(tokens[5], 0));
            }
        
        // Apply a quaternion rotation
        // note order is the same as ODE
        // RotateQuaternion FacetedObjectName w x y z
        else
            if (strcmp(tokens[0], "RotateQuaternion") == 0)
            {
                if (numTokens != 6) ParseError(tokens, numTokens, count, "Wrong number of tokens");
                object = gObjectList.Get(tokens[1]);
                if (object == 0) ParseError(tokens, numTokens, count, "FacetedObject name not found");
                object->RotateQuaternion(strtod(tokens[2], 0), strtod(tokens[3], 0), strtod(tokens[4], 0), strtod(tokens[5], 0));
            }
        
        // Move
        // Move FacetedObjectName x y z
        else
            if (strcmp(tokens[0], "Move") == 0)
            {
                if (numTokens != 5) ParseError(tokens, numTokens, count, "Wrong number of tokens");
                object = gObjectList.Get(tokens[1]);
                if (object == 0) ParseError(tokens, numTokens, count, "FacetedObject name not found");
                object->Move(strtod(tokens[2], 0), strtod(tokens[3], 0), strtod(tokens[4], 0));
            }

        // Scale
        // Scale FacetedObjectName x y z
        else
            if (strcmp(tokens[0], "Scale") == 0)
            {
                if (numTokens != 5) ParseError(tokens, numTokens, count, "Wrong number of tokens");
                object = gObjectList.Get(tokens[1]);
                if (object == 0) ParseError(tokens, numTokens, count, "FacetedObject name not found");
                object->Scale(strtod(tokens[2], 0), strtod(tokens[3], 0), strtod(tokens[4], 0));
            }

        // Set the display FacetedObject
        // SetDisplay FacetedObjectName
        else
            if (strcmp(tokens[0], "SetDisplay") == 0)
            {
                if (numTokens != 2) ParseError(tokens, numTokens, count, "Wrong number of tokens");
                object = gObjectList.Get(tokens[1]);
                if (object == 0) ParseError(tokens, numTokens, count, "FacetedObject name not found");
                gDisplayObject = object;
            }

        // Create a new, empty FacetedObject
        // CreateNew FacetedObjectName
        else
            if (strcmp(tokens[0], "CreateNew") == 0)
            {
                if (numTokens != 2) ParseError(tokens, numTokens, count, "Wrong number of tokens");
                object = gObjectList.Get(tokens[1]);
                if (gObjectList.Get(tokens[1])) ParseError(tokens, numTokens, count, "FacetedObject name not unique");
                object = new FacetedObject();
                object->SetName(tokens[1]);
                gObjectList.Add(object);
            }

        // concatenate two FacetedObjects (adds second object to first object)
        // Concatenate FacetedObjectName FacetedObjectName KeepNames
        // Set KeepNames to non-zero if want object names kept
        else
            if (strcmp(tokens[0], "Concatenate") == 0)
            {
                if (numTokens != 3 && numTokens != 4) ParseError(tokens, numTokens, count, "Wrong number of tokens");
                object = gObjectList.Get(tokens[1]);
                object2 = gObjectList.Get(tokens[2]);
                if (object == 0) ParseError(tokens, numTokens, count, "FacetedObject name not found");
                if (object2 == 0) ParseError(tokens, numTokens, count, "FacetedObject name not found");
                if (numTokens == 3)
                    object->Concatenate(object2, 0);
                else
                    object->Concatenate(object2, strtol(tokens[3], 0, 10));
            }

        // report basic stats on object
        // Stats FacetedObjectName
        else
            if (strcmp(tokens[0], "Stats") == 0)
            {
                if (numTokens != 2) ParseError(tokens, numTokens, count, "Wrong number of tokens");
                object = gObjectList.Get(tokens[1]);
                if (object == 0) ParseError(tokens, numTokens, count, "FacetedObject name not found");
                object->Stats();
            }

        // simple convex polygon triangulation
        // Triangulate FacetedObjectName
        else
            if (strcmp(tokens[0], "Triangulate") == 0)
            {
                if (numTokens != 2) ParseError(tokens, numTokens, count, "Wrong number of tokens");
                object = gObjectList.Get(tokens[1]);
                if (object == 0) ParseError(tokens, numTokens, count, "FacetedObject name not found");
                object->Triangulate();
            }

        // delete an object
        // Delete FacetedObjectName
        else
            if (strcmp(tokens[0], "Delete") == 0)
            {
                if (numTokens != 2) ParseError(tokens, numTokens, count, "Wrong number of tokens");
                object = gObjectList.Get(tokens[1]);
                if (object == 0) ParseError(tokens, numTokens, count, "FacetedObject name not found");
                gObjectList.Delete(tokens[1]);
            }
        
        // Voxelize (produces a voxel based version of triangles and optionally outlines them if outlineVoxels != 0)
        // Voxelize FacetedObjectName x y z
        // Voxelize FacetedObjectName x y z outlineVoxels
        // x y z are the size of the voxels (floating point values)
        else
            if (strcmp(tokens[0], "Voxelize") == 0)
            {
                if (numTokens != 5 && numTokens != 6) ParseError(tokens, numTokens, count, "Wrong number of tokens");
                object = gObjectList.Get(tokens[1]);
                if (object == 0) ParseError(tokens, numTokens, count, "FacetedObject name not found");
                if (numTokens == 5)
                    object->Voxelize(strtod(tokens[2], 0), strtod(tokens[3], 0), strtod(tokens[4], 0), 0);
                else
                    object->Voxelize(strtod(tokens[2], 0), strtod(tokens[3], 0), strtod(tokens[4], 0), strtol(tokens[5], 0, 10));
            }

        // Isosurface a voxel version (use 64 or 196 to either include or exclude boundary layer)
        // Isosurface FacetedObjectName isolevel
        // Isosurface FacetedObjectName isolevel cubeBased
        // if cube_based is non zero (default) then use cube based algorithm (quicker, fewer triangles, less smooth)
        else
            if (strcmp(tokens[0], "Isosurface") == 0)
            {
                if (numTokens != 3 && numTokens != 4) ParseError(tokens, numTokens, count, "Wrong number of tokens");
                object = gObjectList.Get(tokens[1]);
                if (object == 0) ParseError(tokens, numTokens, count, "FacetedObject name not found");
                if (numTokens == 3)
                    object->Isosurface(strtol(tokens[2], 0, 10), 1);
                else
                    object->Isosurface(strtol(tokens[2], 0, 10), strtol(tokens[3], 0, 10));
            }
        
        // Merge close or identical vertices
        // MergeVertices FacetedObjectName tolerance
        else
            if (strcmp(tokens[0], "MergeVertices") == 0)
            {
                if (numTokens != 3) ParseError(tokens, numTokens, count, "Wrong number of tokens");
                object = gObjectList.Get(tokens[1]);
                if (object == 0) ParseError(tokens, numTokens, count, "FacetedObject name not found");
                object->MergeVertices(strtod(tokens[2], 0));
            }
        
        // Set the epsilon for new vertices (this value is used to decide whether a vertex matches an existing one)
        // the default is 1e-8
        // SetEpsilon epsilon
        else
            if (strcmp(tokens[0], "SetEpsilon") == 0)
            {
                if (numTokens != 2) ParseError(tokens, numTokens, count, "Wrong number of tokens");
                MyVertex::SetEpsilon(strtod(tokens[1], 0));
            }
    
        else
        {
            ParseError(tokens, numTokens, count, "Unrecognised line");
        }
    }
}

// Produce a parse error message and terminate program
void ParseError(char **tokens, int numTokens, int count, const char *message)
{
    cerr << "Parse error on line " << count << "\n";
    for (int i = 0; i < numTokens; i++)
        cerr << tokens[i] << " ";
    cerr << "\n";
    if (message) cerr << message << "\n";
    exit(1);
}

// parse an OBJ file to a FacetedObject
void ParseOBJFile(const char *filename, FacetedObject *obj)
{
    const int kBufferSize = 64000;
    TextFile theFile;
    char *line = new char[kBufferSize];
    char *buffer = new char[kBufferSize];
    char **tokens = new char *[kBufferSize];
    int numTokens;
    int count = 0;
    char currentGroup[256] = "";
    char currentObject[256] = "";
    std::vector<MyVertex *> vertexList;
    std::vector<MyFace *> faceList;
    MyVertex *vertex;
    MyFace *face;
    int i, j;
    MyVertex min(DBL_MAX, DBL_MAX, DBL_MAX);
    MyVertex max(-DBL_MAX, -DBL_MAX, -DBL_MAX);

    theFile.SetExitOnError(true);
    theFile.ReadFile(filename);
    
    JustTheFileName(filename, currentGroup, kBufferSize);
    JustTheFileName(filename, currentObject, kBufferSize);

    // read the file
    while (theFile.ReadNextLine(line, kBufferSize, true, '#', '\\') == false)
    {
        strcpy(buffer, line);
        count++;
        numTokens = TextFile::ReturnTokens(buffer, tokens, kBufferSize);

        // groups
        if (strcmp(tokens[0], "g") == 0)
        {
            if (numTokens > 1) // no blank groups
            {
                strcpy(currentGroup, "");
                for (i = 1; i < numTokens; i++)
                {
                    strcat(currentGroup, tokens[i]);
                    strcat(currentGroup, "_");
                }

                if (gDebug == DebugOBJMangler)
                    cerr << "ParseOBJFile:\tcurrentGroup\t" << currentGroup << "\n";
            }
        }

        // objects
        if (strcmp(tokens[0], "o") == 0)
        {
            if (numTokens > 1) // no blank objects
            {
                strcpy(currentObject, "");
                for (i = 1; i < numTokens; i++)
                {
                    strcat(currentObject, tokens[i]);
                    strcat(currentObject, "_");
                }
                

                if (gDebug == DebugOBJMangler)
                    cerr << "ParseOBJFile:\tcurrentObject\t" << currentObject << "\n";
            }
        }

        // vertices
        if (strcmp(tokens[0], "v") == 0)
        {
            if (numTokens > 3)
            {
                vertex = new MyVertex();
                vertex->x = atof(tokens[1]);
                vertex->y = atof(tokens[2]);
                vertex->z = atof(tokens[3]);
                vertexList.push_back(vertex);

                if (gDebug == DebugOBJMangler)
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
                face = new MyFace();
                face->SetNumVertices(numTokens - 1);
                // note obj files start at 1 not zero
                for (i = 1; i < numTokens; i++)
                    face->SetVertex(i - 1, atoi(tokens[i]) - 1);
                face->SetObject(currentObject);
                face->SetGroup(currentGroup);
                faceList.push_back(face);
            }
        }
    }

    if (gDebug == DebugOBJMangler)
        cerr << "ParseOBJFile:\tmin.x\t" << min.x << "\tmax.x\t" << max.x <<
            "\tmin.y\t" << min.y << "\tmax.y\t" << max.y <<
            "\tmin.z\t" << min.z << "\tmax.z\t" << max.z << "\n";

    // fill out the display object
    obj->ClearLists();

    MyVertex vertexes[maxVerticesPerFace];
    int numVertices;
    for (i = 0; i < (int)faceList.size(); i++)
    {
        numVertices = faceList[i]->GetNumVertices();
        if (numVertices > maxVerticesPerFace)
        {
            std::cerr << "Error in ParseOBJFile\n";
            exit(1);
        }
        for (j = 0; j < numVertices; j++) vertexes[j] = *vertexList[faceList[i]->GetVertex(j)];
        obj->AddFace(vertexes, numVertices);
    }

    // calculate normals
    obj->CalculateNormals();

    // clear memory
    for (i = 0; i < (int)vertexList.size(); i++)
        delete vertexList[i];
    for (i = 0; i < (int)faceList.size(); i++)
        delete faceList[i];

    delete [] line;
    delete [] buffer;
    delete [] tokens;
}

// parse an ANYSURF file to a FacetedObject
void ParseANYSURFFile(const char *filename, FacetedObject *obj)
{
    const int kBufferSize = 64000;
    TextFile theFile;
    char *line = new char[kBufferSize];
    char *buffer = new char[kBufferSize];
    char **tokens = new char *[kBufferSize];
    int numTokens;
    char currentGroup[256] = "";
    char currentObject[256] = "";
    std::vector<MyVertex *> vertexList;
    std::vector<MyFace *> faceList;
    MyVertex *vertex;
    MyFace *face;
    int i, j;
    MyVertex min(DBL_MAX, DBL_MAX, DBL_MAX);
    MyVertex max(-DBL_MAX, -DBL_MAX, -DBL_MAX);

    theFile.SetExitOnError(true);
    theFile.ReadFile(filename);

    theFile.ReadNextLine(line, kBufferSize);
    strcpy(buffer, line);
    numTokens = TextFile::ReturnTokens(buffer, tokens, kBufferSize);
    if (numTokens != 1)
    {
        cerr << "Wrong number of tokens 1\n";
        exit(1);
    }
    if (strcmp(tokens[0], "ANYSURF") != 0)
    {
        cerr << "Not an ANYSURF file\n";
        exit(1);
    }
    
    theFile.ReadNextLine(line, kBufferSize);
    strcpy(buffer, line);
    numTokens = TextFile::ReturnTokens(buffer, tokens, kBufferSize);
    if (numTokens != 2)
    {
        cerr << "Wrong number of tokens 2\n";
        exit(1);
    }
    if (strcmp(tokens[0], "FORMAT") != 0)
    {
        cerr << "FORMAT line not found\n";
        exit(1);
    }
    if (strcmp(tokens[1], "1") != 0)
    {
        cerr << "Only FORMAT 1 supported\n";
        exit(1);
    }
    theFile.ReadNextLine(line, kBufferSize);
    strcpy(buffer, line);
    numTokens = TextFile::ReturnTokens(buffer, tokens, kBufferSize);
    if (numTokens != 2)
    {
        cerr << "Wrong number of tokens 3\n";
        exit(1);
    }
    int numVertices = strtol(tokens[0], 0, 10);
    int numFaces = strtol(tokens[1], 0, 10);
    if (gVerboseFlag) 
        cerr << "numVertices = " << numVertices
            << "numFaces = " << numFaces << "\n";

    for (i = 0; i < numVertices; i++)
    {
        theFile.ReadNextLine(line, kBufferSize);
        strcpy(buffer, line);
        numTokens = TextFile::ReturnTokens(buffer, tokens, kBufferSize);
        if (numTokens > 3)
        {
            vertex = new MyVertex();
            vertex->x = atof(tokens[0]);
            vertex->y = atof(tokens[1]);
            vertex->z = atof(tokens[2]);
            vertexList.push_back(vertex);

            if (gDebug == DebugOBJMangler)
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
        numTokens = TextFile::ReturnTokens(buffer, tokens, kBufferSize);
        if (numTokens > 3)
        {
            face = new MyFace();
            face->SetNumVertices(3); // assume always 3 vertices per face
            for (j = 0; j < 3; j++)
                face->SetVertex(j, atoi(tokens[j]));
            face->SetObject(currentObject);
            face->SetGroup(currentGroup);
            faceList.push_back(face);
        }
    }
        

    if (gDebug == DebugOBJMangler)
        cerr << "ParseANYFACEFile:\tmin.x\t" << min.x << "\tmax.x\t" << max.x <<
            "\tmin.y\t" << min.y << "\tmax.y\t" << max.y <<
            "\tmin.z\t" << min.z << "\tmax.z\t" << max.z << "\n";

    // fill out the display object
    obj->ClearLists();
    
    MyVertex vertexes[maxVerticesPerFace];
    for (i = 0; i < (int)faceList.size(); i++)
    {
        numVertices = faceList[i]->GetNumVertices();
        if (numVertices > maxVerticesPerFace)
        {
            std::cerr << "Error in ParseOBJFile\n";
            exit(1);
        }
        for (j = 0; j < numVertices; j++) vertexes[j] = *vertexList[faceList[i]->GetVertex(j)];
        obj->AddFace(vertexes, numVertices);
    }
    
    // calculate normals
    obj->CalculateNormals();

    // clear memory
    for (i = 0; i < (int)vertexList.size(); i++)
        delete vertexList[i];

    delete [] line;
    delete [] buffer;
    delete [] tokens;
}

// parse an 3DS file to a FacetedObject
// uses l3ds library
void Parse3DSFile(const char *filename, FacetedObject *obj)
{
    L3DS loader;
    unsigned int meshIndex, triangleIndex;
    MyVertex vertex;
    MyFace face;
    unsigned int offset = 0;
    MyVertex min(DBL_MAX, DBL_MAX, DBL_MAX);
    MyVertex max(-DBL_MAX, -DBL_MAX, -DBL_MAX);
    char currentGroup[256] = "";
    char currentObject[256] = "";
    MyVertex vertexes[maxVerticesPerFace];
    
    if (loader.LoadFile(filename) == false)
    {
        cerr << "Error in Parse3DSFile loading: " << filename << "\n";
        exit(1);
    }
    
    for (meshIndex = 0; meshIndex < loader.GetMeshCount(); meshIndex++)
    {
        sprintf(currentGroup, "Mesh_%03d", meshIndex);
        strcpy(currentObject, currentGroup);
        LMesh &mesh = loader.GetMesh(meshIndex);
        for (triangleIndex = 0; triangleIndex < mesh.GetTriangleCount(); triangleIndex++)
        {
            const LTriangle &triangle = mesh.GetTriangle(triangleIndex);
            const LVector4 &vectora = mesh.GetVertex(triangle.a);
            const LVector4 &vectorb = mesh.GetVertex(triangle.b);
            const LVector4 &vectorc = mesh.GetVertex(triangle.c);
            vertexes[0] = MyVertex(vectora.x * vectora.w, vectora.y * vectora.w, vectora.z * vectora.w);
            vertexes[1] = MyVertex(vectorb.x * vectora.w, vectorb.y * vectora.w, vectorb.z * vectora.w);
            vertexes[2] = MyVertex(vectorc.x * vectorc.w, vectorc.y * vectorc.w, vectorb.z * vectorc.w);
            obj->AddFace(vertexes, 3);
        }
        
        if (gDebug == DebugOBJMangler)
            cerr << "Parse3DSFile:\tmeshIndex\t" << meshIndex 
                 << "\ttriangleIndex\t" << triangleIndex
                 << "\toffset\t" << offset << "\n";
    }
    if (gDebug == DebugOBJMangler)
        cerr << "Parse3DSFile:\tmin.x\t" << min.x << "\tmax.x\t" << max.x <<
            "\tmin.y\t" << min.y << "\tmax.y\t" << max.y <<
            "\tmin.z\t" << min.z << "\tmax.z\t" << max.z << "\n";
    
}

// read in an ascii point cloud replacing each point with
// a shape of size = size
void ParsePointCloudFile(const char *filename, double size, FacetedObject *obj)
{
    TextFile theFile;
    MyVertex *vertex;
    std::vector<MyVertex *> list;
    double x, y, z;
    unsigned int i;
    double size2 = size / 2;
    int j;
    
    theFile.SetExitOnError(true);
    theFile.ReadFile(filename);

    while (1)
    {
        theFile.SetExitOnError(false);
        if (theFile.ReadNext(&x)) break;
        if (theFile.ReadNext(&y)) break;
        if (theFile.ReadNext(&z)) break;
        
        vertex = new MyVertex;
        vertex->x = x;
        vertex->y = y;
        vertex->z = z;
        list.push_back(vertex);
    }
    
    MyVertex v[4];
    Vector origin;
    Vector side0(size, 0, 0);
    Vector side1(0, size, 0);
    Vector side2(0, 0, size);
    for (i = 0; i < list.size(); i++)
    {   
        origin.x = list[i]->x - size2;
        origin.y = list[i]->y - size2;
        origin.z = list[i]->z - size2;
        Cuboid cuboid(origin, side0, side1, side2);
   
        for (j = 0; j < 6; j++)
        {
            Rectangle rectangle = cuboid.GetFace(j);
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
            obj->AddFace(v, 4);
        }
    }

    for (i = 0; i < list.size(); i++) delete list[i];
}

// read in an ascii polyline file
// format:
// line x1 y1 x2 y2 x3 y3 x4 y4
void Parse2DPolylineFile(const char *filename, FacetedObject *obj)
{
    TextFile theFile;
    MyVertex *vertex = new MyVertex[1000000];
    char buffer[256];
    int vertexCount;
    double x, y;
    
    theFile.SetExitOnError(true);
    theFile.ReadFile(filename);

    theFile.SetExitOnError(false);
    buffer[0] = 0;
    while (1)
    {
        if (strcmp(buffer, "line") == 0)
        {
            vertexCount = 0;
            while (1)
            {
                if (theFile.ReadNext(buffer, 256)) break;
                if (strchr("+-0123456789.", buffer[0])) x = strtod(buffer, 0);
                else break;
                if (theFile.ReadNext(buffer, 256)) break;
                if (strchr("+-0123456789.", buffer[0])) y = strtod(buffer, 0);
                else break;
                vertex[vertexCount].x = x;
                vertex[vertexCount].y = y;
                vertex[vertexCount].z = 0;
                vertexCount++;
                if (vertexCount >= 1000000) break;
            }
            obj->AddFace(vertex, vertexCount);
        }
        else if (theFile.ReadNext(buffer, 256)) break;
    }
        
}

// Write a FacetedObject out as a OBJ
void WriteOBJFile(const char *filename, FacetedObject *obj, int precision)
{
    int i, j;
    ofstream out(filename);
    time_t theTime = time(0);
    struct tm *theLocalTime = localtime(&theTime);
    char currentGroup[256] = "";
    char currentObject[256] = "";
    char timestr[256];
    sprintf(timestr, "Run_%04d-%02d-%02d_%02d.%02d.%02d", 
            theLocalTime->tm_year + 1900, theLocalTime->tm_mon + 1, 
            theLocalTime->tm_mday,
            theLocalTime->tm_hour, theLocalTime->tm_min,
            theLocalTime->tm_sec);
    
    // write out the vertices
    out << "# File produced by OBJMangler " << timestr << "\n";
    
    out.precision(precision); 
    for (i = 0; i < obj->GetNumVertices(); i++)
    {
        out << "v " << obj->GetVertex(i)->x << " " <<
        obj->GetVertex(i)->y << " " <<
        obj->GetVertex(i)->z << "\n";
    }

    // write out the faces, groups and objects
    for (i = 0; i < obj->GetNumFaces(); i++)
    {
        if (obj->GetFace(i)->GetGroup())
        {
            if (strcmp(obj->GetFace(i)->GetGroup(), currentGroup) != 0)
            {
                strcpy(currentGroup, obj->GetFace(i)->GetGroup());
                out << "g " << currentGroup << "\n";
            }
        }

        if (obj->GetFace(i)->GetObject())
        {
            if (strcmp(obj->GetFace(i)->GetObject(), currentObject) != 0)
            {
                strcpy(currentObject, obj->GetFace(i)->GetObject());
                out << "o " << currentObject << "\n";
            }
        }

        out << "f ";
        for (j = 0; j < obj->GetFace(i)->GetNumVertices(); j++)
        {
            // note obj files vertex list start at 1 not zero
            if (j == obj->GetFace(i)->GetNumVertices() - 1)
                out << obj->GetFace(i)->GetVertex(j) + 1 << "\n";
            else
                out << obj->GetFace(i)->GetVertex(j) + 1 << " ";
        }
    }
}

// write a Voxelated FacetedObject to a raw .dat file plus descriptive .txt file
void WriteVoxelFiles(const char *rawFilename, const char *textFilename, FacetedObject *obj, int bits)
{
    if (obj->mVoxelData == 0)
    {
        std::cerr << "Error: No data to write to " << rawFilename << "\n";
        exit(1);
    }
    
    int i;
    ofstream out(textFilename);
    time_t theTime = time(0);
    struct tm *theLocalTime = localtime(&theTime);
    
    out << "# File produced by OBJMangler " << asctime(theLocalTime);
    out.precision(16); // should be plenty
    out << "# Number of voxels (X, Y, Z)\n";
    out << obj->mVoxelNumbers.x << " " << obj->mVoxelNumbers.y << " " << obj->mVoxelNumbers.z << "\n";
    out << "# Size of a voxels (X, Y, Z)\n";
    out << obj->mVoxelSize.x << " " << obj->mVoxelSize.y << " " << obj->mVoxelSize.z << "\n";
    out << "# Centre of first voxel (X, Y, Z)\n";
    out << obj->mVoxelOrigin.x << " " << obj->mVoxelOrigin.y << " " << obj->mVoxelOrigin.z << "\n";
    
    out.close();
    
    ofstream out2(rawFilename);
    if (bits <= 8)
    {
        out2.write((char *)obj->mVoxelData, obj->mVoxelNumbers.x * obj->mVoxelNumbers.y * obj->mVoxelNumbers.z );
    }
    else
    {
        int nVoxels = obj->mVoxelNumbers.x * obj->mVoxelNumbers.y * obj->mVoxelNumbers.z;
        // uint16_t *data = new uint16_t[nVoxels];
        int16_t *data = new int16_t[nVoxels];
        for (i = 0; i < nVoxels; i++) data[i] = static_cast<uint16_t>(obj->mVoxelData[i]);
        out2.write((char *)data, nVoxels * 2 );
        delete [] data;
    }
    out2.close();
}

// write a Voxelated FacetedObject as a calibrated vtk volume data file
void WriteVoxelVTKFile(const char *vtkFilename, FacetedObject *obj, int binary_flag)
{
    if (obj->mVoxelData == 0)
    {
        std::cerr << "Error: No data to write to " << vtkFilename << "\n";
        exit(1);
    }
    
    FILE *fout = fopen(vtkFilename, "w");
    time_t theTime = time(0);
    struct tm *theLocalTime = localtime(&theTime);
    
    fprintf(fout, "# vtk DataFile Version 2.0\n");
    fprintf(fout, "Data produced by OBJMangler %s \n", asctime(theLocalTime));
    if (binary_flag)
        fprintf(fout, "BINARY\n"); // binary version
    else
        fprintf(fout, "ASCII\n"); // ascii version
    fprintf(fout, "DATASET STRUCTURED_POINTS\n");
    fprintf(fout, "DIMENSIONS %d %d %d\n", obj->mVoxelNumbers.x, obj->mVoxelNumbers.y, obj->mVoxelNumbers.z);
    fprintf(fout, "ORIGIN %g %g %g\n", obj->mVoxelOrigin.x, obj->mVoxelOrigin.y, obj->mVoxelOrigin.z);
    fprintf(fout, "SPACING %g %g %g\n", obj->mVoxelSize.x, obj->mVoxelSize.y, obj->mVoxelSize.z);
    fprintf(fout, "POINT_DATA %d\n", obj->mVoxelNumbers.x * obj->mVoxelNumbers.y * obj->mVoxelNumbers.z);
    fprintf(fout, "SCALARS scalars unsigned_char\n");
    fprintf(fout, "LOOKUP_TABLE default\n");
    
    std::string line;
    std::stringstream val;
    if (binary_flag)
    {
        fwrite(obj->mVoxelData, obj->mVoxelNumbers.x * obj->mVoxelNumbers.y * obj->mVoxelNumbers.z, 1, fout);
    }
    else
    {
        for (int iz = 0; iz < obj->mVoxelNumbers.z; iz++)
        {
            for (int iy = 0; iy < obj->mVoxelNumbers.y; iy++)
            {
                line.clear();
                for (int ix = 0; ix < obj->mVoxelNumbers.x; ix++)
                {
                    val << (int)obj->mVoxelData[ix + iy * obj->mVoxelNumbers.x + iz * obj->mVoxelNumbers.x * obj->mVoxelNumbers.y];
                    line.append(val.str());
                    val.clear();
                    if (line.length() > 72)
                    {
                        fprintf(fout, "%s\n", line.c_str());
                        line.clear();
                    }
                    else
                    {
                        line.append(" ");
                    }
                }
            }
        }
    }
    
    fclose(fout);
}

// write a Voxelated FacetedObject as a calibrated DICOM data file
void WriteVoxelDICOMFile(const char *dicomFilename, FacetedObject *obj)
{
    if (obj->mVoxelData == 0)
    {
        std::cerr << "Error: No data to write to " << dicomFilename << "\n";
        exit(1);
    }
    
    ofstream s(dicomFilename);
    write_basic_dicom(s, obj->mVoxelData, obj->mVoxelNumbers.x, obj->mVoxelNumbers.y, obj->mVoxelNumbers.z, obj->mVoxelSize.x, obj->mVoxelSize.y, obj->mVoxelSize.z);

}

// extract just the file name (no path, no extension)
int JustTheFileName(const char *input, char *output, int len)
{
    std::string buf;
    int i, j, l;
    int dot_pos;
    int slash_pos = 0;
    int err = 0;
    
    l = strlen(input);
    
    // look for the last '/'
    for (i = l; i >= 0; i--)
    {
        if (input[i] == '/')
        {
            slash_pos = i;
            break;
        }
    }
    
    dot_pos = l;
    // look for '.'
    for (i = slash_pos + 1; i < l; i++)
    {
        if (input[i] == '.')
        {
            dot_pos = i;
            break;
        }
    }

    j = 0;
    for (i = slash_pos + 1; i < dot_pos; i++)
    {
        output[j] = input[i];
        j++;
        if (j > len - 1) 
        {
            err = __LINE__;
            break;
        }
    }
    output[j] = 0;
    
    return err;
}
