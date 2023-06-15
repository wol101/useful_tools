#ifndef PIPELINEOBJECT_H
#define PIPELINEOBJECT_H

#include <string>
#include <vector>

class vtkAlgorithm;

class PipelineObject
{
public:
    PipelineObject();
    ~PipelineObject();

    vtkAlgorithm *ParseCommandLine(const std::string &commandLine, vtkAlgorithm *inputAlgorithm);

    void SetDebugFlag(bool flag) { m_DebugFlag = flag; }

    static void QuotedWhitespaceSplit(const std::string &inStr, const std::string &whitespace, const std::string &quotes, std::vector<std::string> *output);

protected:

    vtkAlgorithm *ReadNRRD();
    vtkAlgorithm *ReadDICOM();
    vtkAlgorithm *ReadTIFFImageStack();
    vtkAlgorithm *ReadSTL();
    vtkAlgorithm *ReadPLY();
    vtkAlgorithm *ReadASCIIPointCloud();
    vtkAlgorithm *MarchingCubes(vtkAlgorithm *inputAlgorithm);
    vtkAlgorithm *ImageMarchingCubes(vtkAlgorithm *inputAlgorithm);
    vtkAlgorithm *FlyingEdges3D(vtkAlgorithm *inputAlgorithm);
    vtkAlgorithm *Contour(vtkAlgorithm *inputAlgorithm);
    vtkAlgorithm *VertexGlyph(vtkAlgorithm *inputAlgorithm);
    vtkAlgorithm *Glyph3D(vtkAlgorithm *inputAlgorithm);
    vtkAlgorithm *WritePLY(vtkAlgorithm *inputAlgorithm);
    vtkAlgorithm *WriteSTL(vtkAlgorithm *inputAlgorithm);
    vtkAlgorithm *Decimate(vtkAlgorithm *inputAlgorithm);
    vtkAlgorithm *SincSmooth(vtkAlgorithm *inputAlgorithm);
    vtkAlgorithm *Smooth(vtkAlgorithm *inputAlgorithm);
    vtkAlgorithm *Normals(vtkAlgorithm *inputAlgorithm);
    vtkAlgorithm *TransformPolyData(vtkAlgorithm *inputAlgorithm);


    int GetParameter(const std::string &parameter);
    int FindMatch(const std::string &parameter);
    int GetListOfParameter(const std::string &parameter, int *startIndex, int *endIndex);


    std::vector<std::string> m_ListOfCommands;

    vtkAlgorithm *m_Algorithm;

    bool m_DebugFlag;

};

#endif // PIPELINEOBJECT_H
