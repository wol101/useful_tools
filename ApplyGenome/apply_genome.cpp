#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>

#include "DataFile.h"
#include "XMLConverter.h"


int main(int argc, char **argv)
{
    char *inputXML = 0;
    char *inputGenome = 0;
    char *outputXML = 0;
    int i;

    for (i = 1; i < argc; i++)
    {
        if (strcmp(argv[i], "--inputXML") == 0)
        {
            i++;
            if (i >= argc) goto err;
            inputXML = argv[i];
            continue;
        }

        if (strcmp(argv[i], "--inputGenome") == 0)
        {
            i++;
            if (i >= argc) goto err;
            inputGenome = argv[i];
            continue;
        }

        if (strcmp(argv[i], "--outputXML") == 0)
        {
            i++;
            if (i >= argc) goto err;
            outputXML = argv[i];
            continue;
        }


err:
        printf("Usage: apply_genome args\n");
        printf("--inputXML input_file.xml\n");
        printf("--inputGenome genome_file.txt\n");
        printf("--outputXML output_file.xml\n");
        return 1;
    }
    
    if (inputXML == 0 || inputGenome == 0 || outputXML == 0) goto err;

    DataFile genomeData;
    double val;
    int ival, genomeSize;
    genomeData.ReadFile(inputGenome);
    genomeData.ReadNext(&ival);
    genomeData.ReadNext(&genomeSize);
    double *data = new double[genomeSize];
    for (int i = 0; i < genomeSize; i++)
    {
        genomeData.ReadNext(&val);
        data[i] = val;
        genomeData.ReadNext(&val); genomeData.ReadNext(&val); genomeData.ReadNext(&val);
        if (ival == -2) genomeData.ReadNext(&val); // skip the extra parameter
    }
    
    XMLConverter myXMLConverter;
    myXMLConverter.LoadBaseXMLFile(inputXML);
    myXMLConverter.ApplyGenome(genomeSize, data);
    int len;
    char *buf = (char *)myXMLConverter.GetFormattedXML(&len);
    
    DataFile outputXMLData;
    outputXMLData.SetRawData(buf);
    outputXMLData.WriteFile(outputXML);
}
