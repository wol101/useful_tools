#include "vtkASCIIPointCloudReader.h"
#include "PipelineObject.h"

#include "vtkOBJReader.h"
#include "vtkSmoothPolyDataFilter.h"
#include "vtkDecimatePro.h"
#include "vtkPolyDataNormals.h"
#include "vtkPolyDataMapper.h"
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
#include "vtkFlyingEdges3D.h"

#include <regex>

using namespace std::string_literals;

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
vtkAlgorithm *PipelineObject::ParseCommandLine(const std::string &commandLine, vtkAlgorithm *inputAlgorithm)
{
    QuotedWhitespaceSplit(commandLine, std::string(" \t\r\n"), std::string("'\""), &m_ListOfCommands);

    if (m_ListOfCommands.size() == 0)
    {
        std::cerr << "ParseCommandLine command missing\n";
        return 0;
    }

    if (m_ListOfCommands[0] == "ReadNRRD"s) return ReadNRRD();
    if (m_ListOfCommands[0] == "ReadDICOM"s) return ReadDICOM();
    if (m_ListOfCommands[0] == "ReadTIFFImageStack"s) return ReadTIFFImageStack();
    if (m_ListOfCommands[0] == "ReadSTL"s) return ReadSTL();
    if (m_ListOfCommands[0] == "ReadPLY"s) return ReadPLY();
    if (m_ListOfCommands[0] == "ReadASCIIPointCloud"s) return ReadASCIIPointCloud();
    if (m_ListOfCommands[0] == "MarchingCubes"s) return MarchingCubes(inputAlgorithm);
    if (m_ListOfCommands[0] == "ImageMarchingCubes"s) return ImageMarchingCubes(inputAlgorithm);
    if (m_ListOfCommands[0] == "FlyingEdges3D"s) return FlyingEdges3D(inputAlgorithm);
    if (m_ListOfCommands[0] == "Contour"s) return Contour(inputAlgorithm);
    if (m_ListOfCommands[0] == "VertexGlyph"s) return VertexGlyph(inputAlgorithm);
    if (m_ListOfCommands[0] == "Glyph3D"s) return Glyph3D(inputAlgorithm);
    if (m_ListOfCommands[0] == "WritePLY"s) return WritePLY(inputAlgorithm);
    if (m_ListOfCommands[0] == "WriteSTL"s) return WriteSTL(inputAlgorithm);
    if (m_ListOfCommands[0] == "Decimate"s) return Decimate(inputAlgorithm);
    if (m_ListOfCommands[0] == "SincSmooth"s) return SincSmooth(inputAlgorithm);
    if (m_ListOfCommands[0] == "Smooth"s) return Smooth(inputAlgorithm);
    if (m_ListOfCommands[0] == "Normals"s) return Normals(inputAlgorithm);
    if (m_ListOfCommands[0] == "TransformPolyData"s) return TransformPolyData(inputAlgorithm);

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
    if (index >= 0) reader->SetFileName(m_ListOfCommands[index].c_str());
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
    if (index >= 0) reader->SetDirectoryName(m_ListOfCommands[index].c_str());
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
    if (index >= 0) reader->SetFilePrefix(m_ListOfCommands[index].c_str());
    else
    {
        std::cerr << "ReadTIFFImageStack missing FilePrefix\n";
        return 0;
    }

    index = GetParameter("FilePattern");
    if (index >= 0) reader->SetFilePattern(m_ListOfCommands[index].c_str());
    else
    {
        std::cerr << "ReadTIFFImageStack missing FilePattern\n";
        return 0;
    }

    if (GetListOfParameter("DataExtent", &startIndex, &endIndex) != -1)
    {
        if ((endIndex - startIndex) == 6)
        {
            reader->SetDataExtent(std::stoi(m_ListOfCommands[startIndex]),
                                  std::stoi(m_ListOfCommands[startIndex+1]),
                                  std::stoi(m_ListOfCommands[startIndex+2]),
                                  std::stoi(m_ListOfCommands[startIndex+3]),
                                  std::stoi(m_ListOfCommands[startIndex+4]),
                                  std::stoi(m_ListOfCommands[startIndex+5]));
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
            reader->SetDataSpacing(std::stod(m_ListOfCommands[startIndex]),
                                   std::stod(m_ListOfCommands[startIndex+1]),
                                   std::stod(m_ListOfCommands[startIndex+2]));
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
    if (index >= 0) reader->SetFileName(m_ListOfCommands[index].c_str());
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
    if (index >= 0) reader->SetFileName(m_ListOfCommands[index].c_str());
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
    if (index >= 0) reader->SetFileName(m_ListOfCommands[index].c_str());
    else
    {
        std::cerr << "vtkASCIIPointCloudReader missing FileName\n";
        return 0;
    }

    index = GetParameter("SkipLines");
    if (index >= 0) reader->vtkSetSkipLines(std::stoi(m_ListOfCommands[index]));

    index = GetParameter("XYZColumn");
    if (index >= 0) reader->vtkSetXYZColumn(std::stoi(m_ListOfCommands[index]));
    index = GetParameter("RGBColumn");
    if (index >= 0) reader->vtkSetRGBColumn(std::stoi(m_ListOfCommands[index]));
    index = GetParameter("GreyColumn");
    if (index >= 0) reader->vtkSetGreyColumn(std::stoi(m_ListOfCommands[index]));

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
    if (index >= 0) iso->SetComputeNormals(std::stoi(m_ListOfCommands[index]));
    index = GetParameter("ComputeGradients");
    if (index >= 0) iso->SetComputeGradients(std::stoi(m_ListOfCommands[index]));
    index = GetParameter("ComputeScalars");
    if (index >= 0) iso->SetComputeScalars(std::stoi(m_ListOfCommands[index]));

    int startIndex, endIndex;
    int count = 0;
    if (GetListOfParameter("IsoValues", &startIndex, &endIndex) != -1)
    {
        for (index = startIndex; index < endIndex; index++)
        {
            iso->SetValue(count, std::stod(m_ListOfCommands[index]));
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
    if (index >= 0) iso->SetComputeNormals(std::stoi(m_ListOfCommands[index]));
    index = GetParameter("ComputeGradients");
    if (index >= 0) iso->SetComputeGradients(std::stoi(m_ListOfCommands[index]));
    index = GetParameter("ComputeScalars");
    if (index >= 0) iso->SetComputeScalars(std::stoi(m_ListOfCommands[index]));

    index = GetParameter("InputMemoryLimit");
    if (index >= 0) iso->SetInputMemoryLimit(std::stoi(m_ListOfCommands[index]));
    else iso->SetInputMemoryLimit(10000); // set memory limit to 10,000 kb

    int startIndex, endIndex;
    int count = 0;
    if (GetListOfParameter("IsoValues", &startIndex, &endIndex) != -1)
    {
        for (index = startIndex; index < endIndex; index++)
        {
            iso->SetValue(count, std::stod(m_ListOfCommands[index]));
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

// Perform vtkFlyingEdges3D algorithm
// returns the algorithm pointer if successful
// returns zero otherwise
vtkAlgorithm *PipelineObject::FlyingEdges3D(vtkAlgorithm *inputAlgorithm)
{

    vtkFlyingEdges3D *iso = vtkFlyingEdges3D::New();
    if (m_DebugFlag) iso->DebugOn();
    iso->SetInputConnection(inputAlgorithm->GetOutputPort());

    int index;
    index = GetParameter("ComputeNormals");
    if (index >= 0) iso->SetComputeNormals(std::stoi(m_ListOfCommands[index]));
    index = GetParameter("ComputeGradients");
    if (index >= 0) iso->SetComputeGradients(std::stoi(m_ListOfCommands[index]));
    index = GetParameter("ComputeScalars");
    if (index >= 0) iso->SetComputeScalars(std::stoi(m_ListOfCommands[index]));

    int startIndex, endIndex;
    int count = 0;
    if (GetListOfParameter("IsoValues", &startIndex, &endIndex) != -1)
    {
        for (index = startIndex; index < endIndex; index++)
        {
            iso->SetValue(count, std::stod(m_ListOfCommands[index]));
            count++;
        }
    }

    if (count == 0)
    {
        std::cerr << "FlyingEdges3D missing IsoValues\n";
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
    if (index >= 0) iso->SetComputeNormals(std::stoi(m_ListOfCommands[index]));
    index = GetParameter("ComputeGradients");
    if (index >= 0) iso->SetComputeGradients(std::stoi(m_ListOfCommands[index]));
    index = GetParameter("ComputeScalars");
    if (index >= 0) iso->SetComputeScalars(std::stoi(m_ListOfCommands[index]));

    int startIndex, endIndex;
    int count = 0;
    if (GetListOfParameter("IsoValues", &startIndex, &endIndex) != -1)
    {
        for (index = startIndex; index < endIndex; index++)
        {
            iso->SetValue(count, std::stod(m_ListOfCommands[index]));
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
            cubeSource->SetXLength(std::stod(m_ListOfCommands[startIndex]));
            cubeSource->SetYLength(std::stod(m_ListOfCommands[startIndex+1]));
            cubeSource->SetZLength(std::stod(m_ListOfCommands[startIndex+2]));
        }
        glyph3D->SetScaleModeToDataScalingOff();
    }

    int index;
    index = GetParameter("ColorMode");
    if (index >= 0) glyph3D->SetColorMode(std::stoi(m_ListOfCommands[index]));

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
    writer->SetInputConnection(polyDataAlgorithm->GetOutputPort());

    int index;
    index = GetParameter("FileName");
    if (index >= 0) writer->SetFileName(m_ListOfCommands[index].c_str());
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
    writer->SetInputConnection(polyDataAlgorithm->GetOutputPort());

    int index;
    index = GetParameter("FileName");
    if (index >= 0) writer->SetFileName(m_ListOfCommands[index].c_str());
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
    if (index >= 0) decimator->SetTargetReduction(std::stod(m_ListOfCommands[index]));
    index = GetParameter("FeatureAngle");
    if (index >= 0) decimator->SetFeatureAngle(std::stod(m_ListOfCommands[index]));
    index = GetParameter("MaximumError");
    if (index >= 0) decimator->SetMaximumError(std::stod(m_ListOfCommands[index]));
    index = GetParameter("AbsoluteError");
    if (index >= 0) decimator->SetAbsoluteError(std::stod(m_ListOfCommands[index]));
    index = GetParameter("ErrorIsAbsolute");
    if (index >= 0) decimator->SetErrorIsAbsolute(std::stoi(m_ListOfCommands[index]));
    index = GetParameter("AccumulateError");
    if (index >= 0) decimator->SetAccumulateError(std::stoi(m_ListOfCommands[index]));
    index = GetParameter("SplitAngle");
    if (index >= 0) decimator->SetSplitAngle(std::stod(m_ListOfCommands[index]));
    index = GetParameter("Splitting");
    if (index >= 0) decimator->SetSplitting(std::stoi(m_ListOfCommands[index]));
    index = GetParameter("PreSplitMesh");
    if (index >= 0) decimator->SetPreSplitMesh(std::stoi(m_ListOfCommands[index]));
    index = GetParameter("BoundaryVertexDeletion");
    if (index >= 0) decimator->SetBoundaryVertexDeletion(std::stoi(m_ListOfCommands[index]));
    index = GetParameter("PreserveTopology");
    if (index >= 0) decimator->SetPreserveTopology(std::stoi(m_ListOfCommands[index]));
    index = GetParameter("Degree");
    if (index >= 0) decimator->SetDegree(std::stoi(m_ListOfCommands[index]));
    index = GetParameter("InflectionPointRatio");
    if (index >= 0) decimator->SetInflectionPointRatio(std::stod(m_ListOfCommands[index]));

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
    if (index >= 0) smoother->SetNumberOfIterations(std::stoi(m_ListOfCommands[index]));
    index = GetParameter("PassBand");
    if (index >= 0) smoother->SetPassBand(std::stod(m_ListOfCommands[index]));
    index = GetParameter("FeatureEdgeSmoothing");
    if (index >= 0) smoother->SetFeatureEdgeSmoothing(std::stoi(m_ListOfCommands[index]));
    index = GetParameter("FeatureAngle");
    if (index >= 0) smoother->SetFeatureAngle(std::stod(m_ListOfCommands[index]));
    index = GetParameter("EdgeAngle");
    if (index >= 0) smoother->SetEdgeAngle(std::stod(m_ListOfCommands[index]));
    index = GetParameter("BoundarySmoothing");
    if (index >= 0) smoother->SetBoundarySmoothing(std::stoi(m_ListOfCommands[index]));
    index = GetParameter("NonManifoldSmoothing");
    if (index >= 0) smoother->SetNonManifoldSmoothing(std::stoi(m_ListOfCommands[index]));

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
    if (index >= 0) smoother->SetNumberOfIterations(std::stoi(m_ListOfCommands[index]));
    index = GetParameter("Convergence");
    if (index >= 0) smoother->SetConvergence(std::stod(m_ListOfCommands[index]));
    index = GetParameter("FeatureEdgeSmoothing");
    if (index >= 0) smoother->SetFeatureEdgeSmoothing(std::stoi(m_ListOfCommands[index]));
    index = GetParameter("FeatureAngle");
    if (index >= 0) smoother->SetFeatureAngle(std::stod(m_ListOfCommands[index]));
    index = GetParameter("EdgeAngle");
    if (index >= 0) smoother->SetEdgeAngle(std::stod(m_ListOfCommands[index]));
    index = GetParameter("BoundarySmoothing");
    if (index >= 0) smoother->SetBoundarySmoothing(std::stoi(m_ListOfCommands[index]));
    index = GetParameter("RelaxationFactor");
    if (index >= 0) smoother->SetRelaxationFactor(std::stod(m_ListOfCommands[index]));

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
    if (index >= 0) normals->SetFeatureAngle(std::stod(m_ListOfCommands[index]));
    index = GetParameter("Splitting");
    if (index >= 0) normals->SetSplitting(std::stoi(m_ListOfCommands[index]));
    index = GetParameter("Consistency");
    if (index >= 0) normals->SetConsistency(std::stoi(m_ListOfCommands[index]));
    index = GetParameter("FlipNormals");
    if (index >= 0) normals->SetFlipNormals(std::stoi(m_ListOfCommands[index]));
    index = GetParameter("AutoOrientNormals");
    if (index >= 0) normals->SetAutoOrientNormals(std::stoi(m_ListOfCommands[index]));
    index = GetParameter("NonManifoldTraversal");
    if (index >= 0) normals->SetNonManifoldTraversal(std::stoi(m_ListOfCommands[index]));
    index = GetParameter("ComputePointNormals");
    if (index >= 0) normals->SetComputePointNormals(std::stoi(m_ListOfCommands[index]));
    index = GetParameter("ComputeCellNormals");
    if (index >= 0) normals->SetComputeCellNormals(std::stoi(m_ListOfCommands[index]));

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
            generalTransform->Translate(std::stod(m_ListOfCommands[startIndex]), std::stod(m_ListOfCommands[startIndex+1]), std::stod(m_ListOfCommands[startIndex+2]));
        }
    }
    if (GetListOfParameter("RotateWXYZ", &startIndex, &endIndex) != -1) // note (angle, axisx, axisy, axisz) with angle in degrees
    {
        if ((endIndex - startIndex) == 4)
        {
            generalTransform->RotateWXYZ(std::stod(m_ListOfCommands[startIndex]), std::stod(m_ListOfCommands[startIndex+1]), std::stod(m_ListOfCommands[startIndex+2]), std::stod(m_ListOfCommands[startIndex+3]));
        }
    }
    if (GetListOfParameter("Scale", &startIndex, &endIndex) != -1)
    {
        if ((endIndex - startIndex) == 3)
        {
            generalTransform->Scale(std::stod(m_ListOfCommands[startIndex]), std::stod(m_ListOfCommands[startIndex+1]), std::stod(m_ListOfCommands[startIndex+2]));
        }
    }

    transform->SetTransform(generalTransform);

    m_Algorithm = transform;
    return m_Algorithm;
}


// function to split a std::string into a list of tokens as a std::vector<std::string>
// split(QRegExp("\\s+")) would work if we weren't worried about quoted strings
// this one can use multiple whitespace and quote character
void PipelineObject::QuotedWhitespaceSplit(const std::string &inStr, const std::string &whitespace, const std::string &quotes, std::vector<std::string> *output)
{
    bool firstTime = true;
    std::string currentString;
    int i = 0;
    int quoteIndex;
    while (i < inStr.size())
    {
        if (whitespace.find(inStr[i]) == std::string::npos)
        {
            if (firstTime) firstTime = false;
            else output->push_back(currentString);
            currentString.clear();

            quoteIndex = quotes.find(inStr[i]);
            if (quoteIndex != std::string::npos)
            {
                while (true)
                {
                    i++;
                    if (i >= inStr.size()) break;
                    if (inStr[i] == quotes[quoteIndex]) break;
                    currentString.push_back(inStr[i]);
                }
            }
            else
            {
                currentString.push_back(inStr[i]);
                while (true)
                {
                    i++;
                    if (i >= inStr.size()) break;
                    if (whitespace.find(inStr[i]) != std::string::npos) break;
                    currentString.push_back(inStr[i]);
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
int PipelineObject::GetParameter(const std::string &parameter)
{
    for (int i = 0; i < m_ListOfCommands.size() - 1; i++)
    {
        if (m_ListOfCommands[i] == parameter) return i + 1;
    }

    return -1;
}

// function to retrieve the index of a named parameter from a token list
// it returns the index of a name match
// or -1 if the parameter is not found
int PipelineObject::FindMatch(const std::string &parameter)
{
    for (int i = 0; i < m_ListOfCommands.size(); i++)
    {
        if (m_ListOfCommands[i] == parameter) return i;
    }

    return -1;
}

// function to retrieve the value of a named parameter from a token list
// it returns the index after a name match
// or -1 if the parameter is not found
int PipelineObject::GetListOfParameter(const std::string &parameter, int *startIndex, int *endIndex)
{
    int i, j;
    for (i = 0; i < m_ListOfCommands.size() - 1; i++)
    {
        if (m_ListOfCommands[i] == parameter)
        {
            *startIndex = i + 1;
            // end index is the next non-number
            std::regex numberMatch("([+-]?)(?=\\d|\\.\\d)\\d*(\\.\\d*)?([Ee]([+-]?\\d+))?");
            for (j = *startIndex; j < m_ListOfCommands.size(); j++)
            {
                if (std::regex_match(m_ListOfCommands[j], numberMatch) == false) break;
            }
            *endIndex = j;
            return *startIndex;
        }
    }

    return -1;
}
