#include <iostream>

#include <QString>
#include <QStringList>
#include <QList>

#include <vtkAlgorithm.h>

#include "DataFile.h"
#include "InputFileParser.h"
#include "PipelineObject.h"

InputFileParser::InputFileParser()
{
    m_DebugFlag = false;
}

InputFileParser::~InputFileParser()
{
    for (int i = 0; i < m_PipelineObjects.size(); i++) delete m_PipelineObjects[i];
}

// parse the instructions in the input file
// return zero on success
int InputFileParser::ParseFile(QString &inputFileName)
{
    DataFile inputFile;
    inputFile.SetExitOnError(false);
    if (inputFile.ReadFile(inputFileName.toUtf8()))
    {
        std::cerr << "InputFileParser::ParseFile error reading " << (const char *)inputFileName.toUtf8() << "\n";
        return 1;
    }

    const int kBufferSize = 10000;
    char buffer[kBufferSize];
    while (true)
    {
        if (inputFile.ReadNextLine(buffer, kBufferSize, true, '#', '\\')) break;
        m_InputLines.push_back(buffer);
    }

    if (m_InputLines.size() == 0)
    {
        std::cerr << "InputFileParser::ParseFile no commands found\n";
        return 1;
    }

    PipelineObject *newPipelineObject;
    vtkAlgorithm *inputAlgorithm = 0;
    for (int i = 0; i < m_InputLines.size(); i++)
    {
        newPipelineObject = new PipelineObject();
        newPipelineObject->SetDebugFlag(m_DebugFlag);
        inputAlgorithm = newPipelineObject->ParseCommandLine(m_InputLines[i], inputAlgorithm);
        if (inputAlgorithm == 0) return 1;
        m_PipelineObjects.push_back(newPipelineObject);
    }

    return 0;
}

