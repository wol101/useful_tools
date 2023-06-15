#ifndef PIPELINEOBJECT_H
#define PIPELINEOBJECT_H

class vtkAlgorithm;
class QString;

class PipelineObject
{
public:
    PipelineObject();
    ~PipelineObject();

    vtkAlgorithm *ParseCommandLine(const QString &commandLine, vtkAlgorithm *inputAlgorithm);

    void SetDebugFlag(bool flag) { m_DebugFlag = flag; }

    static void QuotedWhitespaceSplit(const QString &inStr, const QString &whitespace, const QString &quotes, QStringList *output);

protected:

    vtkAlgorithm *ReadNRRD();
    vtkAlgorithm *ReadDICOM();
    vtkAlgorithm *ReadTIFFImageStack();
    vtkAlgorithm *ReadSTL();
    vtkAlgorithm *ReadPLY();
    vtkAlgorithm *ReadASCIIPointCloud();
    vtkAlgorithm *MarchingCubes(vtkAlgorithm *inputAlgorithm);
    vtkAlgorithm *ImageMarchingCubes(vtkAlgorithm *inputAlgorithm);
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
    vtkAlgorithm *Render(vtkAlgorithm *inputAlgorithm); // note, this one returns the input algorithm


    int GetParameter(const QString &parameter, Qt::CaseSensitivity cs = Qt::CaseInsensitive);
    int FindMatch(const QString &parameter, Qt::CaseSensitivity cs = Qt::CaseInsensitive);
    int GetListOfParameter(const QString &parameter, int *startIndex, int *endIndex, Qt::CaseSensitivity cs = Qt::CaseInsensitive);


    QStringList m_ListOfCommands;

    vtkAlgorithm *m_Algorithm;

    bool m_DebugFlag;

};

#endif // PIPELINEOBJECT_H
