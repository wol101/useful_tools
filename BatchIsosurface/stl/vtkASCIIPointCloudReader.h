#ifndef VTKASCIIPOINTCLOUDREADER_H
#define VTKASCIIPOINTCLOUDREADER_H

#include "vtkPolyDataAlgorithm.h"

class vtkASCIIPointCloudReader : public vtkPolyDataAlgorithm
{
public:
    void PrintSelf(ostream& os, vtkIndent indent);

    // Description:
    // Construct object with merging set to true.
    static vtkASCIIPointCloudReader *New();

    // Description:
    // Specify file name ASCII Point Cloud.
    vtkSetStringMacro(FileName);
    vtkGetStringMacro(FileName);

    // Specify the number of header lines to skip
    void vtkSetSkipLines(int numLines) { NumLinesToSkip = numLines; }

    // Specify the locations of the data (columns numbers start at 0)
    void vtkSetXYZColumn(int c1) { XYZColumn = c1;}
    void vtkSetGreyColumn(int c1) { GreyColumn = c1; DataType = 1; }
    void vtkSetRGBColumn(int c1) { RGBColumn = c1; DataType = 2; }

protected:
    vtkASCIIPointCloudReader();
    ~vtkASCIIPointCloudReader();

    char *FileName;
    int NumLinesToSkip;
    int XYZColumn;
    int RGBColumn;
    int GreyColumn;
    int DataType;

    int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *);
private:
    vtkASCIIPointCloudReader(const vtkASCIIPointCloudReader&);  // Not implemented.
    void operator=(const vtkASCIIPointCloudReader&);  // Not implemented.
};

#endif // VTKASCIIPOINTCLOUDREADER_H
