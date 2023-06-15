#include <QString>
#include <QStringList>
#include <QRegExp>

#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkOBJReader.h"
#include "vtkSmoothPolyDataFilter.h"
#include "vtkDecimatePro.h"
#include "vtkPolyDataNormals.h"
#include "vtkPolyDataMapper.h"
#include "vtkActor.h"
#include "vtkProperty.h"
#include "vtkCamera.h"
#include "vtkOBJExporter.h"
#include "vtkStructuredPointsReader.h"
#include "vtkImageGaussianSmooth.h"
#include "vtkImageToStructuredPoints.h"
#include "vtkContourFilter.h"
#include "vtkMarchingCubes.h"
#include "vtkDICOMImageReader.h"
#include "vtkPLYWriter.h"
#include "vtkSmartPointer.h"
#include "vtkSliceCubes.h"
#include "vtkVolume16Reader.h"
#include "vtkSTLWriter.h"
#include "vtkAlgorithm.h"
#include "vtkWindowedSincPolyDataFilter.h"
#include "vtkImageMarchingCubes.h"
#include "vtkTIFFReader.h"
#include "vtkSTLReader.h"
#include "vtkPLYReader.h"
#include "vtkVertexGlyphFilter.h"
#include "vtkLookupTable.h"
#include "vtkGlyph3D.h"
#include "vtkCubeSource.h"
#include "vtkTransformPolyDataFilter.h"
#include "vtkGeneralTransform.h"

#include "vtkNrrdReader.h"
#include "vtkASCIIPointCloudReader.h"

#include "PipelineObject.h"

PipelineObject::PipelineObject()
{
    m_Algorithm = 0;
    m_DebugFlag = true;
}

PipelineObject::~PipelineObject()
{
    if (m_Algorithm) m_Algorithm->Delete();
}

// parse the string which contains the instruction plus any parameters
// returns the output port if successful
// returns zero otherwise
vtkAlgorithm *PipelineObject::ParseCommandLine(const QString &commandLine, vtkAlgorithm *inputAlgorithm)
{
    // m_ListOfCommands = commandLine.split(QRegExp("\\s+")); // this one splits of whitespace

    QuotedWhitespaceSplit(commandLine, QString(" \t\r\n"), QString("'\""), &m_ListOfCommands);

    if (m_ListOfCommands.size() == 0)
    {
        std::cerr << "ParseCommandLine command missing\n";
        return 0;
    }

    if (m_ListOfCommands[0].compare("ReadNRRD", Qt::CaseInsensitive) == 0) return ReadNRRD();
    if (m_ListOfCommands[0].compare("ReadDICOM", Qt::CaseInsensitive) == 0) return ReadDICOM();
    if (m_ListOfCommands[0].compare("ReadTIFFImageStack", Qt::CaseInsensitive) == 0) return ReadTIFFImageStack();
    if (m_ListOfCommands[0].compare("ReadSTL", Qt::CaseInsensitive) == 0) return ReadSTL();
    if (m_ListOfCommands[0].compare("ReadPLY", Qt::CaseInsensitive) == 0) return ReadPLY();
    if (m_ListOfCommands[0].compare("ReadASCIIPointCloud", Qt::CaseInsensitive) == 0) return ReadASCIIPointCloud();
    if (m_ListOfCommands[0].compare("MarchingCubes", Qt::CaseInsensitive) == 0) return MarchingCubes(inputAlgorithm);
    if (m_ListOfCommands[0].compare("ImageMarchingCubes", Qt::CaseInsensitive) == 0) return ImageMarchingCubes(inputAlgorithm);
    if (m_ListOfCommands[0].compare("Contour", Qt::CaseInsensitive) == 0) return Contour(inputAlgorithm);
    if (m_ListOfCommands[0].compare("VertexGlyph", Qt::CaseInsensitive) == 0) return VertexGlyph(inputAlgorithm);
    if (m_ListOfCommands[0].compare("Glyph3D", Qt::CaseInsensitive) == 0) return Glyph3D(inputAlgorithm);
    if (m_ListOfCommands[0].compare("WritePLY", Qt::CaseInsensitive) == 0) return WritePLY(inputAlgorithm);
    if (m_ListOfCommands[0].compare("WriteSTL", Qt::CaseInsensitive) == 0) return WriteSTL(inputAlgorithm);
    if (m_ListOfCommands[0].compare("Decimate", Qt::CaseInsensitive) == 0) return Decimate(inputAlgorithm);
    if (m_ListOfCommands[0].compare("SincSmooth", Qt::CaseInsensitive) == 0) return SincSmooth(inputAlgorithm);
    if (m_ListOfCommands[0].compare("Smooth", Qt::CaseInsensitive) == 0) return Smooth(inputAlgorithm);
    if (m_ListOfCommands[0].compare("Normals", Qt::CaseInsensitive) == 0) return Normals(inputAlgorithm);
    if (m_ListOfCommands[0].compare("TransformPolyData", Qt::CaseInsensitive) == 0) return TransformPolyData(inputAlgorithm);
    if (m_ListOfCommands[0].compare("Render", Qt::CaseInsensitive) == 0) return Render(inputAlgorithm);

    std::cerr << "ParseCommandLine unrecognised command\n";
    return 0;
}

// Read a NRRD file
// returns the algorithm pointer if successful
// returns zero otherwise
vtkAlgorithm *PipelineObject::ReadNRRD()
{
    vtkNrrdReader *reader = vtkNrrdReader::New();
    if (m_DebugFlag) reader->DebugOn();

    int index;
    index = GetParameter("FileName");
    if (index >= 0) reader->SetFileName(m_ListOfCommands[index].toUtf8());
    else
    {
        std::cerr << "ReadNRRD missing FileName\n";
        return 0;
    }

    m_Algorithm = reader;
    return m_Algorithm;
}


// Read a DICOM file
// returns the algorithm pointer if successful
// returns zero otherwise
vtkAlgorithm *PipelineObject::ReadDICOM()
{
    vtkDICOMImageReader *reader = vtkDICOMImageReader::New();
    if (m_DebugFlag) reader->DebugOn();

    int index;
    index = GetParameter("DirectoryName");
    if (index >= 0) reader->SetDirectoryName(m_ListOfCommands[index].toUtf8());
    else
    {
        std::cerr << "ReadDICOM missing DirectoryName\n";
        return 0;
    }

    m_Algorithm = reader;
    return m_Algorithm;
}


// Read a TIFF Image Stack
// returns the algorithm pointer if successful
// returns zero otherwise
vtkAlgorithm *PipelineObject::ReadTIFFImageStack()
{

    // Start by loading some data.
    vtkTIFFReader *reader = vtkTIFFReader::New();
    if (m_DebugFlag) reader->DebugOn();

    int index, startIndex, endIndex;
    index = GetParameter("FilePrefix");
    if (index >= 0) reader->SetFilePrefix(m_ListOfCommands[index].toUtf8());
    else
    {
        std::cerr << "ReadTIFFImageStack missing FilePrefix\n";
        return 0;
    }

    index = GetParameter("FilePattern");
    if (index >= 0) reader->SetFilePattern(m_ListOfCommands[index].toUtf8());
    else
    {
        std::cerr << "ReadTIFFImageStack missing FilePattern\n";
        return 0;
    }

    if (GetListOfParameter("DataExtent", &startIndex, &endIndex) != -1)
    {
        if ((endIndex - startIndex) == 6)
        {
            reader->SetDataExtent(m_ListOfCommands[startIndex].toInt(),
                                  m_ListOfCommands[startIndex+1].toInt(),
                                  m_ListOfCommands[startIndex+2].toInt(),
                                  m_ListOfCommands[startIndex+3].toInt(),
                                  m_ListOfCommands[startIndex+4].toInt(),
                                  m_ListOfCommands[startIndex+5].toInt());
        }
        else
        {
            std::cerr << "ReadTIFFImageStack DataExtent has wrong arguments\n";
            return 0;
        }

    }
    else
    {
        std::cerr << "ReadTIFFImageStack missing DataExtent\n";
        return 0;
    }


    if (GetListOfParameter("DataSpacing", &startIndex, &endIndex) != -1)
    {
        if ((endIndex - startIndex) == 3)
        {
            reader->SetDataSpacing(m_ListOfCommands[startIndex].toDouble(),
                                   m_ListOfCommands[startIndex+1].toDouble(),
                                   m_ListOfCommands[startIndex+2].toDouble());
        }
        else
        {
            std::cerr << "ReadTIFFImageStack DataSpacing has wrong arguments\n";
            return 0;
        }
    }
    else reader->SetDataSpacing(1, 1, 1);

    reader->SetDataOrigin(0.0, 0.0, 0.0);
    reader->SetDataScalarTypeToUnsignedShort();

    // These flags allow us to override the origin and resolution specified in the TIFF file
    // bool originSpecifiedFlag = reader->GetOriginSpecifiedFlag();
    // bool spacingSpecifiedFlag = reader->GetSpacingSpecifiedFlag();
    reader->SetOriginSpecifiedFlag(true);
    reader->SetSpacingSpecifiedFlag(true);

    m_Algorithm = reader;
    return m_Algorithm;
}


// Read an STL file
// returns the algorithm pointer if successful
// returns zero otherwise
vtkAlgorithm *PipelineObject::ReadSTL()
{
    vtkSTLReader *reader = vtkSTLReader::New();
    if (m_DebugFlag) reader->DebugOn();

    int index;
    index = GetParameter("FileName");
    if (index >= 0) reader->SetFileName(m_ListOfCommands[index].toUtf8());
    else
    {
        std::cerr << "ReadSTL missing FileName\n";
        return 0;
    }

    m_Algorithm = reader;
    return m_Algorithm;
}

// Read a PLY file
// returns the algorithm pointer if successful
// returns zero otherwise
vtkAlgorithm *PipelineObject::ReadPLY()
{
    vtkPLYReader *reader = vtkPLYReader::New();
    if (m_DebugFlag) reader->DebugOn();

    int index;
    index = GetParameter("FileName");
    if (index >= 0) reader->SetFileName(m_ListOfCommands[index].toUtf8());
    else
    {
        std::cerr << "ReadPLY missing FileName\n";
        return 0;
    }

    m_Algorithm = reader;
    return m_Algorithm;
}


// Read an ASCI Point Cloud file
// returns the algorithm pointer if successful
// returns zero otherwise
vtkAlgorithm *PipelineObject::ReadASCIIPointCloud()
{
    vtkASCIIPointCloudReader *reader = vtkASCIIPointCloudReader::New();
    if (m_DebugFlag) reader->DebugOn();

    int index;
    index = GetParameter("FileName");
    if (index >= 0) reader->SetFileName(m_ListOfCommands[index].toUtf8());
    else
    {
        std::cerr << "vtkASCIIPointCloudReader missing FileName\n";
        return 0;
    }

    index = GetParameter("SkipLines");
    if (index >= 0) reader->vtkSetSkipLines(m_ListOfCommands[index].toInt());

    index = GetParameter("XYZColumn");
    if (index >= 0) reader->vtkSetXYZColumn(m_ListOfCommands[index].toInt());
    index = GetParameter("RGBColumn");
    if (index >= 0) reader->vtkSetRGBColumn(m_ListOfCommands[index].toInt());
    index = GetParameter("GreyColumn");
    if (index >= 0) reader->vtkSetGreyColumn(m_ListOfCommands[index].toInt());

    m_Algorithm = reader;
    return m_Algorithm;
}


// Perform Marching cube algorithm
// returns the algorithm pointer if successful
// returns zero otherwise
vtkAlgorithm *PipelineObject::MarchingCubes(vtkAlgorithm *inputAlgorithm)
{

    vtkMarchingCubes *iso = vtkMarchingCubes::New();
    if (m_DebugFlag) iso->DebugOn();
    iso->SetInputConnection(inputAlgorithm->GetOutputPort());

    int index;
    index = GetParameter("ComputeNormals");
    if (index >= 0) iso->SetComputeNormals(m_ListOfCommands[index].toInt());
    index = GetParameter("ComputeGradients");
    if (index >= 0) iso->SetComputeGradients(m_ListOfCommands[index].toInt());
    index = GetParameter("ComputeScalars");
    if (index >= 0) iso->SetComputeScalars(m_ListOfCommands[index].toInt());

    int startIndex, endIndex;
    int count = 0;
    if (GetListOfParameter("IsoValues", &startIndex, &endIndex) != -1)
    {
        for (index = startIndex; index < endIndex; index++)
        {
            iso->SetValue(count, m_ListOfCommands[index].toDouble());
            count++;
        }
    }

    if (count == 0)
    {
        std::cerr << "MarchingCubes missing IsoValues\n";
        return 0;
    }

    m_Algorithm = iso;
    return m_Algorithm;
}

// Perform Marching cube algorithm
// this version does not require the full image set to be loaded
// returns the algorithm pointer if successful
// returns zero otherwise
vtkAlgorithm *PipelineObject::ImageMarchingCubes(vtkAlgorithm *inputAlgorithm)
{

    // this version does not require the full image set to be loaded (but seems to crash)
    vtkImageMarchingCubes *iso = vtkImageMarchingCubes::New();
    if (m_DebugFlag) iso->DebugOn();
    iso->SetInputConnection(inputAlgorithm->GetOutputPort());

    int index;
    index = GetParameter("ComputeNormals");
    if (index >= 0) iso->SetComputeNormals(m_ListOfCommands[index].toInt());
    index = GetParameter("ComputeGradients");
    if (index >= 0) iso->SetComputeGradients(m_ListOfCommands[index].toInt());
    index = GetParameter("ComputeScalars");
    if (index >= 0) iso->SetComputeScalars(m_ListOfCommands[index].toInt());

    index = GetParameter("InputMemoryLimit");
    if (index >= 0) iso->SetInputMemoryLimit(m_ListOfCommands[index].toInt());
    else iso->SetInputMemoryLimit(10000); // set memory limit to 10,000 kb

    int startIndex, endIndex;
    int count = 0;
    if (GetListOfParameter("IsoValues", &startIndex, &endIndex) != -1)
    {
        for (index = startIndex; index < endIndex; index++)
        {
            iso->SetValue(count, m_ListOfCommands[index].toDouble());
            count++;
        }
    }

    if (count == 0)
    {
        std::cerr << "ImageMarchingCubes missing IsoValues\n";
        return 0;
    }

    m_Algorithm = iso;
    return m_Algorithm;
}


// Perform a general purpose contour algorithm
// returns the algorithm pointer if successful
// returns zero otherwise
vtkAlgorithm *PipelineObject::Contour(vtkAlgorithm *inputAlgorithm)
{

    vtkContourFilter *iso = vtkContourFilter::New();
    if (m_DebugFlag) iso->DebugOn();
    iso->SetInputConnection(inputAlgorithm->GetOutputPort());

    int index;
    index = GetParameter("ComputeNormals");
    if (index >= 0) iso->SetComputeNormals(m_ListOfCommands[index].toInt());
    index = GetParameter("ComputeGradients");
    if (index >= 0) iso->SetComputeGradients(m_ListOfCommands[index].toInt());
    index = GetParameter("ComputeScalars");
    if (index >= 0) iso->SetComputeScalars(m_ListOfCommands[index].toInt());

    int startIndex, endIndex;
    int count = 0;
    if (GetListOfParameter("IsoValues", &startIndex, &endIndex) != -1)
    {
        for (index = startIndex; index < endIndex; index++)
        {
            iso->SetValue(count, m_ListOfCommands[index].toDouble());
            count++;
        }
    }

    if (count == 0)
    {
        std::cerr << "Contour missing IsoValues\n";
        return 0;
    }

    m_Algorithm = iso;
    return m_Algorithm;
}

// Performs a vtkVertexGlyphFilter algorithm to visualise point data
// returns the algorithm pointer if successful
// returns zero otherwise
vtkAlgorithm *PipelineObject::VertexGlyph(vtkAlgorithm *inputAlgorithm)
{

    vtkVertexGlyphFilter *glyph = vtkVertexGlyphFilter::New();
    if (m_DebugFlag) glyph->DebugOn();
    glyph->SetInputConnection(inputAlgorithm->GetOutputPort());

    m_Algorithm = glyph;
    return m_Algorithm;
}


// Performs a vtkGlyph3DFilter algorithm to visualise point data
// returns the algorithm pointer if successful
// returns zero otherwise
vtkAlgorithm *PipelineObject::Glyph3D(vtkAlgorithm *inputAlgorithm)
{

    vtkGlyph3D *glyph3D = vtkGlyph3D::New();
    if (m_DebugFlag) glyph3D->DebugOn();
    glyph3D->SetInputConnection(inputAlgorithm->GetOutputPort());

    // Use a cube for the glyph
    vtkCubeSource *cubeSource = vtkCubeSource::New();
    glyph3D->SetSourceConnection(cubeSource->GetOutputPort());

    int startIndex, endIndex;
    if (GetListOfParameter("Size", &startIndex, &endIndex) != -1)
    {
        if ((endIndex - startIndex) == 3)
        {
            cubeSource->SetXLength(m_ListOfCommands[startIndex].toDouble());
            cubeSource->SetYLength(m_ListOfCommands[startIndex+1].toDouble());
            cubeSource->SetZLength(m_ListOfCommands[startIndex+2].toDouble());
        }
        glyph3D->SetScaleModeToDataScalingOff();
    }

    int index;
    index = GetParameter("ColorMode");
    if (index >= 0) glyph3D->SetColorMode(m_ListOfCommands[index].toInt());

    m_Algorithm = glyph3D;
    return m_Algorithm;
}


// Write a PLY file
// returns the algorithm pointer if successful
// returns zero otherwise
vtkAlgorithm *PipelineObject::WritePLY(vtkAlgorithm *inputAlgorithm)
{
    vtkPolyDataAlgorithm *polyDataAlgorithm = dynamic_cast<vtkPolyDataAlgorithm *>(inputAlgorithm);
    if (polyDataAlgorithm == 0)
    {
        std::cerr << "WritePLY needs vtkPolyDataAlgorithm input\n";
        return 0;
    }
    polyDataAlgorithm->GetOutput()->GlobalReleaseDataFlagOn();


    vtkPLYWriter *writer = vtkPLYWriter::New();
    if (m_DebugFlag) writer->DebugOn();
    writer->SetInput(polyDataAlgorithm->GetOutput());

    int index;
    index = GetParameter("FileName");
    if (index >= 0) writer->SetFileName(m_ListOfCommands[index].toUtf8());
    else
    {
        std::cerr << "WritePLY missing FileName\n";
        return 0;
    }

    if (FindMatch("Binary") != -1) writer->SetFileTypeToBinary();
    if (FindMatch("Ascii") != -1) writer->SetFileTypeToASCII();

    writer->Write();

    m_Algorithm = writer;
    return m_Algorithm;
}

// Write a STL file
// returns the algorithm pointer if successful
// returns zero otherwise
vtkAlgorithm *PipelineObject::WriteSTL(vtkAlgorithm *inputAlgorithm)
{
    vtkPolyDataAlgorithm *polyDataAlgorithm = dynamic_cast<vtkPolyDataAlgorithm *>(inputAlgorithm);
    if (polyDataAlgorithm == 0)
    {
        std::cerr << "WriteSTL needs vtkPolyDataAlgorithm input\n";
        return 0;
    }
    polyDataAlgorithm->GetOutput()->GlobalReleaseDataFlagOn();


    vtkSTLWriter *writer = vtkSTLWriter::New();
    if (m_DebugFlag) writer->DebugOn();
    writer->SetInput(polyDataAlgorithm->GetOutput());

    int index;
    index = GetParameter("FileName");
    if (index >= 0) writer->SetFileName(m_ListOfCommands[index].toUtf8());
    else
    {
        std::cerr << "WriteSTL missing FileName\n";
        return 0;
    }

    if (FindMatch("Binary") != -1) writer->SetFileTypeToBinary();
    if (FindMatch("Ascii") != -1) writer->SetFileTypeToASCII();

    writer->Write();

    m_Algorithm = writer;
    return m_Algorithm;
}

// Perform Decimate algorithm
// returns the algorithm pointer if successful
// returns zero otherwise
vtkAlgorithm *PipelineObject::Decimate(vtkAlgorithm *inputAlgorithm)
{
    vtkDecimatePro *decimator = vtkDecimatePro::New();
    if (m_DebugFlag) decimator->DebugOn();
    decimator->SetInputConnection(inputAlgorithm->GetOutputPort());

    int index;
    index = GetParameter("TargetReduction");
    if (index >= 0) decimator->SetTargetReduction(m_ListOfCommands[index].toDouble());
    index = GetParameter("FeatureAngle");
    if (index >= 0) decimator->SetFeatureAngle(m_ListOfCommands[index].toDouble());
    index = GetParameter("MaximumError");
    if (index >= 0) decimator->SetMaximumError(m_ListOfCommands[index].toDouble());
    index = GetParameter("AbsoluteError");
    if (index >= 0) decimator->SetAbsoluteError(m_ListOfCommands[index].toDouble());
    index = GetParameter("ErrorIsAbsolute");
    if (index >= 0) decimator->SetErrorIsAbsolute(m_ListOfCommands[index].toInt());
    index = GetParameter("AccumulateError");
    if (index >= 0) decimator->SetAccumulateError(m_ListOfCommands[index].toInt());
    index = GetParameter("SplitAngle");
    if (index >= 0) decimator->SetSplitAngle(m_ListOfCommands[index].toDouble());
    index = GetParameter("Splitting");
    if (index >= 0) decimator->SetSplitting(m_ListOfCommands[index].toInt());
    index = GetParameter("PreSplitMesh");
    if (index >= 0) decimator->SetPreSplitMesh(m_ListOfCommands[index].toInt());
    index = GetParameter("BoundaryVertexDeletion");
    if (index >= 0) decimator->SetBoundaryVertexDeletion(m_ListOfCommands[index].toInt());
    index = GetParameter("PreserveTopology");
    if (index >= 0) decimator->SetPreserveTopology(m_ListOfCommands[index].toInt());
    index = GetParameter("Degree");
    if (index >= 0) decimator->SetDegree(m_ListOfCommands[index].toInt());
    index = GetParameter("InflectionPointRatio");
    if (index >= 0) decimator->SetInflectionPointRatio(m_ListOfCommands[index].toDouble());

    m_Algorithm = decimator;
    return m_Algorithm;
}

// Perform Windowed Sinc polygon smoothing algorithm
// returns the algorithm pointer if successful
// returns zero otherwise
vtkAlgorithm *PipelineObject::SincSmooth(vtkAlgorithm *inputAlgorithm)
{
    vtkWindowedSincPolyDataFilter *smoother = vtkWindowedSincPolyDataFilter::New();
    if (m_DebugFlag) smoother->DebugOn();
    smoother->SetInputConnection(inputAlgorithm->GetOutputPort());

    int index;
    index = GetParameter("NumberOfIterations");
    if (index >= 0) smoother->SetNumberOfIterations(m_ListOfCommands[index].toInt());
    index = GetParameter("PassBand");
    if (index >= 0) smoother->SetPassBand(m_ListOfCommands[index].toDouble());
    index = GetParameter("FeatureEdgeSmoothing");
    if (index >= 0) smoother->SetFeatureEdgeSmoothing(m_ListOfCommands[index].toInt());
    index = GetParameter("FeatureAngle");
    if (index >= 0) smoother->SetFeatureAngle(m_ListOfCommands[index].toDouble());
    index = GetParameter("EdgeAngle");
    if (index >= 0) smoother->SetEdgeAngle(m_ListOfCommands[index].toDouble());
    index = GetParameter("BoundarySmoothing");
    if (index >= 0) smoother->SetBoundarySmoothing(m_ListOfCommands[index].toInt());
    index = GetParameter("NonManifoldSmoothing");
    if (index >= 0) smoother->SetNonManifoldSmoothing(m_ListOfCommands[index].toInt());

    m_Algorithm = smoother;
    return m_Algorithm;
}

// Perform Laplacian polygon smoothing algorithm
// returns the algorithm pointer if successful
// returns zero otherwise
vtkAlgorithm *PipelineObject::Smooth(vtkAlgorithm *inputAlgorithm)
{
    vtkSmoothPolyDataFilter *smoother = vtkSmoothPolyDataFilter::New();
    if (m_DebugFlag) smoother->DebugOn();
    smoother->SetInputConnection(inputAlgorithm->GetOutputPort());

    int index;
    index = GetParameter("NumberOfIterations");
    if (index >= 0) smoother->SetNumberOfIterations(m_ListOfCommands[index].toInt());
    index = GetParameter("Convergence");
    if (index >= 0) smoother->SetConvergence(m_ListOfCommands[index].toDouble());
    index = GetParameter("FeatureEdgeSmoothing");
    if (index >= 0) smoother->SetFeatureEdgeSmoothing(m_ListOfCommands[index].toInt());
    index = GetParameter("FeatureAngle");
    if (index >= 0) smoother->SetFeatureAngle(m_ListOfCommands[index].toDouble());
    index = GetParameter("EdgeAngle");
    if (index >= 0) smoother->SetEdgeAngle(m_ListOfCommands[index].toDouble());
    index = GetParameter("BoundarySmoothing");
    if (index >= 0) smoother->SetBoundarySmoothing(m_ListOfCommands[index].toInt());
    index = GetParameter("RelaxationFactor");
    if (index >= 0) smoother->SetRelaxationFactor(m_ListOfCommands[index].toDouble());

    m_Algorithm = smoother;
    return m_Algorithm;
}

// Perform normal generation algorithm
// returns the algorithm pointer if successful
// returns zero otherwise
vtkAlgorithm *PipelineObject::Normals(vtkAlgorithm *inputAlgorithm)
{
    vtkPolyDataNormals *normals = vtkPolyDataNormals::New();
    if (m_DebugFlag) normals->DebugOn();
    normals->SetInputConnection(inputAlgorithm->GetOutputPort());

    int index;
    index = GetParameter("FeatureAngle");
    if (index >= 0) normals->SetFeatureAngle(m_ListOfCommands[index].toDouble());
    index = GetParameter("Splitting");
    if (index >= 0) normals->SetSplitting(m_ListOfCommands[index].toInt());
    index = GetParameter("Consistency");
    if (index >= 0) normals->SetConsistency(m_ListOfCommands[index].toInt());
    index = GetParameter("FlipNormals");
    if (index >= 0) normals->SetFlipNormals(m_ListOfCommands[index].toInt());
    index = GetParameter("AutoOrientNormals");
    if (index >= 0) normals->SetAutoOrientNormals(m_ListOfCommands[index].toInt());
    index = GetParameter("NonManifoldTraversal");
    if (index >= 0) normals->SetNonManifoldTraversal(m_ListOfCommands[index].toInt());
    index = GetParameter("ComputePointNormals");
    if (index >= 0) normals->SetComputePointNormals(m_ListOfCommands[index].toInt());
    index = GetParameter("ComputeCellNormals");
    if (index >= 0) normals->SetComputeCellNormals(m_ListOfCommands[index].toInt());

    m_Algorithm = normals;
    return m_Algorithm;
}

// Perform a transformation algorithm
// returns the algorithm pointer if successful
// returns zero otherwise
vtkAlgorithm *PipelineObject::TransformPolyData(vtkAlgorithm *inputAlgorithm)
{
    vtkTransformPolyDataFilter *transform = vtkTransformPolyDataFilter::New();
    if (m_DebugFlag) transform->DebugOn();
    transform->SetInputConnection(inputAlgorithm->GetOutputPort());

    vtkGeneralTransform *generalTransform = vtkGeneralTransform::New();
    generalTransform->Identity();

    int startIndex, endIndex;
    if (GetListOfParameter("Translate", &startIndex, &endIndex) != -1)
    {
        if ((endIndex - startIndex) == 3)
        {
            generalTransform->Translate(m_ListOfCommands[startIndex].toDouble(), m_ListOfCommands[startIndex+1].toDouble(), m_ListOfCommands[startIndex+2].toDouble());
        }
    }
    if (GetListOfParameter("RotateWXYZ", &startIndex, &endIndex) != -1) // note (angle, axisx, axisy, axisz) with angle in degrees
    {
        if ((endIndex - startIndex) == 4)
        {
            generalTransform->RotateWXYZ(m_ListOfCommands[startIndex].toDouble(), m_ListOfCommands[startIndex+1].toDouble(), m_ListOfCommands[startIndex+2].toDouble(), m_ListOfCommands[startIndex+3].toDouble());
        }
    }
    if (GetListOfParameter("Scale", &startIndex, &endIndex) != -1)
    {
        if ((endIndex - startIndex) == 3)
        {
            generalTransform->Scale(m_ListOfCommands[startIndex].toDouble(), m_ListOfCommands[startIndex+1].toDouble(), m_ListOfCommands[startIndex+2].toDouble());
        }
    }

    transform->SetTransform(generalTransform);

    m_Algorithm = transform;
    return m_Algorithm;
}

// Render a model
// Returns the **INPUT** algorithm on success
// returns zero otherwise
vtkAlgorithm * PipelineObject::Render(vtkAlgorithm *inputAlgorithm)
{
    vtkPolyDataMapper *isoMapper = vtkPolyDataMapper::New();
    isoMapper->SetInputConnection(inputAlgorithm->GetOutputPort());

    vtkActor *isoActor = vtkActor::New();
    isoActor->SetMapper(isoMapper);

    int index, startIndex, endIndex;
    if (GetListOfParameter("Colour", &startIndex, &endIndex) != -1)
    {
        if ((endIndex - startIndex) == 3)
            isoActor->GetProperty()->SetColor(m_ListOfCommands[startIndex].toDouble(),
                                              m_ListOfCommands[startIndex+1].toDouble(),
                                              m_ListOfCommands[startIndex+2].toDouble());
    }
    else isoActor->GetProperty()->SetColor(0.7,0.8,0.5); // default colour

    vtkRenderer *aRenderer = vtkRenderer::New();
    aRenderer->SetBackground(1,1,1);

    vtkRenderWindow *renWin = vtkRenderWindow::New();
    renWin->AddRenderer(aRenderer);
    if (GetListOfParameter("WindowSize", &startIndex, &endIndex) != -1)
    {
        if ((endIndex - startIndex) == 2)
            renWin->SetSize(m_ListOfCommands[startIndex].toInt(), m_ListOfCommands[startIndex+1].toInt());
    }
    else renWin->SetSize(800, 600); // default size

    index = GetParameter("ScalarVisibility"); // set to 1 to enable scalar shading
    if (index >= 0) isoMapper->SetScalarVisibility(m_ListOfCommands[index].toInt());

    if (GetListOfParameter("ScalarRange", &startIndex, &endIndex) != -1)
    {
        if ((endIndex - startIndex) == 2)
            isoMapper->SetScalarRange(m_ListOfCommands[startIndex].toDouble(),
                                      m_ListOfCommands[startIndex+1].toDouble());
    }

    vtkRenderWindowInteractor *iren = vtkRenderWindowInteractor::New();
    iren->SetRenderWindow(renWin);

    vtkCamera *aCamera = vtkCamera::New();
    aCamera->SetViewUp (0, 0, -1);
    aCamera->SetPosition (0, 1, 0);
    aCamera->SetFocalPoint (0, 0, 0);
    aCamera->ComputeViewPlaneNormal();

    aRenderer->AddActor(isoActor);
    aRenderer->SetActiveCamera(aCamera);
    aRenderer->ResetCamera ();

    aCamera->Dolly(1.5); // enlarge image a bit
    aRenderer->ResetCameraClippingRange(); // needs to be done after dolly

    iren->Initialize();
    iren->Start();

    index = GetParameter("OBJFileName");
    if (index != -1)
    {
        QString prefix;
        if (m_ListOfCommands[index].endsWith(".obj", Qt::CaseInsensitive)) prefix = m_ListOfCommands[index].left(m_ListOfCommands[index].size() - 4);
        else prefix = m_ListOfCommands[index];
        vtkOBJExporter *exporter = vtkOBJExporter::New();
        exporter->SetFilePrefix(prefix.toUtf8());
        exporter->SetInput(renWin);
        exporter->Write();
        exporter->Delete();
    }

    // delete all the allocations
    aCamera->Delete();
    iren->Delete();
    renWin->Delete();
    aRenderer->Delete();
    isoActor->Delete();
    isoMapper->Delete();

    return inputAlgorithm;
}

// function to split a QString into a list of tokens as a QStringList
// split(QRegExp("\\s+")) would work if we weren't worried about quoted strings
// this one can use multiple whitespace and quote character
void PipelineObject::QuotedWhitespaceSplit(const QString &inStr, const QString &whitespace, const QString &quotes, QStringList *output)
{
    bool firstTime = true;
    QString currentString;
    int i = 0;
    int quoteIndex;
    while (i < inStr.size())
    {
        if (whitespace.contains(inStr[i]) == false)
        {
            if (firstTime) firstTime = false;
            else output->push_back(currentString);
            currentString.clear();

            quoteIndex = quotes.indexOf(inStr[i]);
            if (quoteIndex != -1)
            {
                while (true)
                {
                    i++;
                    if (i >= inStr.size()) break;
                    if (inStr[i] == quotes[quoteIndex]) break;
                    currentString.append(inStr[i]);
                }
            }
            else
            {
                currentString.append(inStr[i]);
                while (true)
                {
                    i++;
                    if (i >= inStr.size()) break;
                    if (whitespace.contains(inStr[i])) break;
                    currentString.append(inStr[i]);
                }
            }
        }
        i++;
    }
    if (firstTime == false) output->push_back(currentString);
}


// function to retrieve the value of a named parameter from a token list
// it returns the index after a name match
// or -1 if the parameter is not found
// cs is Qt::CaseInsensitive (default) or Qt::CaseSensitive
int PipelineObject::GetParameter(const QString &parameter, Qt::CaseSensitivity cs)
{
    for (int i = 0; i < m_ListOfCommands.size() - 1; i++)
    {
        if (m_ListOfCommands[i].compare(parameter, cs) == 0) return i + 1;
    }

    return -1;
}

// function to retrieve the index of a named parameter from a token list
// it returns the index of a name match
// or -1 if the parameter is not found
// cs is Qt::CaseInsensitive (default) or Qt::CaseSensitive
int PipelineObject::FindMatch(const QString &parameter, Qt::CaseSensitivity cs)
{
    for (int i = 0; i < m_ListOfCommands.size(); i++)
    {
        if (m_ListOfCommands[i].compare(parameter, cs) == 0) return i;
    }

    return -1;
}

// function to retrieve the value of a named parameter from a token list
// it returns the index after a name match
// or -1 if the parameter is not found
// cs is Qt::CaseInsensitive (default) or Qt::CaseSensitive
int PipelineObject::GetListOfParameter(const QString &parameter, int *startIndex, int *endIndex, Qt::CaseSensitivity cs)
{
    int i, j;
    for (i = 0; i < m_ListOfCommands.size() - 1; i++)
    {
        if (m_ListOfCommands[i].compare(parameter, cs) == 0)
        {
            *startIndex = i + 1;
            // end index is the next non-number
            QRegExp numberMatch("([+-]?)(?=\\d|\\.\\d)\\d*(\\.\\d*)?([Ee]([+-]?\\d+))?");
            for (j = *startIndex; j < m_ListOfCommands.size(); j++)
            {
                if (numberMatch.exactMatch(m_ListOfCommands[j]) == false) break;
            }
            *endIndex = j;
            return *startIndex;
        }
    }

    return -1;
}
