#include "vtkASCIIPointCloudReader.h"

#include "vtkCellArray.h"
#include "vtkCellData.h"
#include "vtkPointData.h"
#include "vtkDoubleArray.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkObjectFactory.h"
#include "vtkPLY.h"
#include "vtkPolyData.h"
#include "vtkUnsignedCharArray.h"

#include "DataFile.h"

#include <ctype.h>
#include <stddef.h>

vtkCxxRevisionMacro(vtkASCIIPointCloudReader, "$Revision: 1.5 $");
vtkStandardNewMacro(vtkASCIIPointCloudReader);


// Construct object with merging set to true.
vtkASCIIPointCloudReader::vtkASCIIPointCloudReader()
{
    this->FileName = NULL;
    this->NumLinesToSkip = 0;
    XYZColumn = 0;
    RGBColumn = 3;
    GreyColumn = 3;
    DataType = 0;


    this->SetNumberOfInputPorts(0);
}

vtkASCIIPointCloudReader::~vtkASCIIPointCloudReader()
{
    if (this->FileName)
    {
        delete [] this->FileName;
    }
}

int vtkASCIIPointCloudReader::RequestData(
        vtkInformation *vtkNotUsed(request),
        vtkInformationVector **vtkNotUsed(inputVector),
        vtkInformationVector *outputVector)
{
    if (!this->FileName)
    {
        vtkErrorMacro(<<"FileName must be specified.");
        return 0;
    }

    DataFile dataFile;
    if (dataFile.ReadFile(this->FileName))
    {
        vtkErrorMacro(<<"Could not open Point Cloud file");
        return 0;
    }

    char line[1024];
    int i;
    for (i = 0; i < this->NumLinesToSkip; i++)
    {
        if (dataFile.ReadNextLine(line, 1024))
        {
            vtkErrorMacro(<<"Could not skip header lines");
            return 0;
        }
    }

    vtkPoints *points = vtkPoints::New();
    points->SetDataTypeToDouble();
    points->Reset();

    vtkDoubleArray *greys = vtkDoubleArray::New();
    greys->Reset();
    greys->SetName("Grey");

    vtkUnsignedCharArray *rgbs = vtkUnsignedCharArray::New();
    rgbs->Reset();
    rgbs->SetName("RGB");
    rgbs->SetNumberOfComponents(3);


    int tokenCount;
    char *ptrs[16];
    double x, y, z;
    double grey;
    unsigned char rgb[3];

    if (dataFile.ReadNextLine(line, 1024))
    {
        vtkErrorMacro(<<"Error reading data");
        return 0;
    }

    while (dataFile.ReadNextLine(line, 1024) == false)
    {
        tokenCount = DataFile::ReturnTokens(line, ptrs, 16);

        if (DataType == 0)
        {
            if (tokenCount < XYZColumn + 3) continue;
            x = strtod(ptrs[XYZColumn], 0);
            y = strtod(ptrs[XYZColumn + 1], 0);
            z = strtod(ptrs[XYZColumn + 2], 0);
            points->InsertNextPoint(x, y, z);
            continue;
        }

        if (DataType == 1)
        {
            if (tokenCount < XYZColumn + 3 || tokenCount < GreyColumn + 1) continue;
            x = strtod(ptrs[XYZColumn], 0);
            y = strtod(ptrs[XYZColumn + 1], 0);
            z = strtod(ptrs[XYZColumn + 2], 0);
            points->InsertNextPoint(x, y, z);
            grey = strtod(ptrs[GreyColumn], 0);
            greys->InsertNextValue(grey);
            continue;
        }

        if (DataType == 2)
        {
            if (tokenCount < XYZColumn + 3 || tokenCount < RGBColumn + 3) continue;
            x = strtod(ptrs[XYZColumn], 0);
            y = strtod(ptrs[XYZColumn + 1], 0);
            z = strtod(ptrs[XYZColumn + 2], 0);
            points->InsertNextPoint(x, y, z);
            rgb[0] = strtol(ptrs[RGBColumn], 0, 10);
            rgb[1] = strtol(ptrs[RGBColumn + 1], 0, 10);
            rgb[2] = strtol(ptrs[RGBColumn + 2], 0, 10);
            rgbs->InsertNextTupleValue(rgb);
            continue;
        }
    }

    vtkCellArray *vertices = vtkCellArray::New();
    vertices->Reset();

    int numPoints = points->GetNumberOfPoints();
    for( vtkIdType j = 0; j < (vtkIdType)numPoints; ++j )
    {
        vertices->InsertNextCell( 1 );
        vertices->InsertCellPoint( j );
    }

    // get the info object
    vtkInformation *outInfo = outputVector->GetInformationObject(0);

    // get the ouptut
    vtkPolyData *output = vtkPolyData::SafeDownCast(outInfo->Get(vtkDataObject::DATA_OBJECT()));

    output->SetPoints(points);
    output->SetVerts(vertices);

    if (DataType == 1)
    {
        output->GetPointData()->SetScalars(greys);
        vtkDebugMacro( << "Using grey values");
    }

    if (DataType == 2)
    {
        output->GetPointData()->SetScalars(rgbs);
        vtkDebugMacro( << "Using RGB values");
    }

    points->Delete();
    vertices->Delete();
    greys->Delete();
    rgbs->Delete();

    output->Modified();

    vtkDebugMacro( << "Read: " << numPoints << " points");

    return 1;

}

void vtkASCIIPointCloudReader::PrintSelf(ostream& os, vtkIndent indent)
{
    this->Superclass::PrintSelf(os,indent);

    os << indent << "File Name: "
       << (this->FileName ? this->FileName : "(none)") << "\n";

}
