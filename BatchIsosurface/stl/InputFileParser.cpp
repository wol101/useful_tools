#include "DataFile.h"
#include "InputFileParser.h"
#include "PipelineObject.h"

#include <vtkAlgorithm.h>

#include <iostream>

InputFileParser::InputFileParser()
{
    m_DebugFlag = false;
}

// parse the instructions in the input file
// return zero on success
int InputFileParser::ParseFile(std::string &inputFileName)
{
    DataFile inputFile;
    inputFile.SetExitOnError(false);
    if (inputFile.ReadFile(inputFileName.c_str()))
    {
        std::cerr << "InputFileParser::ParseFile error reading " << inputFileName << "\n";
        return 1;
    }

    const int kBufferSize = 10000;
    char buffer[kBufferSize];
    while (true)
    {
        if (inputFile.ReadNextLine(buffer, kBufferSize, true, '#')) break;
        m_InputLines.push_back(buffer);
    }

    if (m_InputLines.size() == 0)
    {
        std::cerr << "InputFileParser::ParseFile no commands found\n";
        return 1;
    }

    vtkAlgorithm *inputAlgorithm = 0;
    for (size_t i = 0; i < m_InputLines.size(); i++)
    {
        auto newPipelineObject = std::make_unique<PipelineObject>();
        newPipelineObject->SetDebugFlag(m_DebugFlag);
        inputAlgorithm = newPipelineObject->ParseCommandLine(m_InputLines[i], inputAlgorithm);
        if (inputAlgorithm == 0) return 1;
        m_PipelineObjects.push_back(std::move(newPipelineObject));
    }

    return 0;
}

