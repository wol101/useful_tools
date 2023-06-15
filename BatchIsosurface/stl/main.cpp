#include <vector>
#include <string>
#include <iostream>

#include "InputFileParser.h"
#include "PipelineObject.h"

int main(int argc, char *argv[])
{
    std::vector<std::string> arguments;
    for (int i = 0; i < argc; i++) arguments.push_back(std::string(argv[i]));
    std::string inputFile;
    bool debugFlag = false;

    std::string usage =
            "Usage: BatchIsosurface -i inputFile\n"
            "Options:\n"
            "-i --input inputFile containing list of instructions\n"
            "-d --debug turn on debugging\n";

    bool errorState = false;
    for (int i = 1; i < arguments.size(); i++)
    {
        if (arguments.at(i) == "-i" || arguments.at(i) == "--input")
        {
            i++;
            if (i < arguments.size()) inputFile = arguments.at(i);
            else errorState = true;
            continue;
        }

        if (arguments.at(i) == "-d" || arguments.at(i) == "--debug")
        {
            debugFlag = true;
            continue;
        }

        std::cerr << usage;
        return 1;

    }

    if (inputFile.size() == 0 || errorState)
    {
        std::cerr << usage;
        return 1;
    }
    
    InputFileParser inputParser;
    inputParser.SetDebugFlag(debugFlag);
    if (inputParser.ParseFile(inputFile)) return 1;

        return 0;
}



