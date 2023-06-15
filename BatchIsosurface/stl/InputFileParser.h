#ifndef INPUTFILEPARSER_H
#define INPUTFILEPARSER_H

#include <string>
#include <vector>
#include <memory>

class PipelineObject;

class InputFileParser
{
public:
    InputFileParser();

    int ParseFile(std::string &inputFileName);
    void SetDebugFlag(bool flag) { m_DebugFlag = flag; }

protected:

    std::vector<std::string> m_InputLines;
    std::vector<std::unique_ptr<PipelineObject>> m_PipelineObjects;

    bool m_DebugFlag;

};

#endif // INPUTFILEPARSER_H
