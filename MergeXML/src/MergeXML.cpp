/*
 *  MergeXML.cc
 *  MergeXML
 *
 *  Created by Bill Sellers on Wed Dec 17 2003.
 *  Copyright (c) 2003 Bill Sellers. All rights reserved.
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <iostream>
#include <fstream>
#include <map>
#include <vector>
#include <string>

#include "XMLContainer.h"
#include "XMLContainerList.h"
#include "DataFile.h"
#include "MergeUtil.h"

int gMergeXMLVerboseLevel;

static bool gIgnoreError;
static XMLContainerList gObjectList;
static std::vector<std::string> gOldStringList;
static std::vector<std::string> gNewStringList;

void ExecuteInstructionFile(const char *filename);
void ParseError(char **tokens, int numTokens, int count, const char *message);

#ifndef USE_MPI
int main(int argc, char *argv[])
#else
int MergeXMLMain(int argc, char *argv[])
#endif
{
    char *instructionFile = 0;
    int i;
    gMergeXMLVerboseLevel = 0;
    gIgnoreError = false;

    // parse the command line
    try
    {
        for (i = 1; i < argc; i++)
        {
            // do something with arguments

            if (strcmp(argv[i], "--instructionFile") == 0 ||
                strcmp(argv[i], "-i") == 0)
            {
                i++;
                if (i >= argc) throw(__LINE__);
                instructionFile = argv[i];
            }

            else
            if (strcmp(argv[i], "--replace") == 0 ||
                strcmp(argv[i], "-r") == 0)
            {
                i++;
                if (i >= argc) throw(__LINE__);
                gOldStringList.push_back(std::string(argv[i]));
                i++;
                if (i >= argc) throw(__LINE__);
                gNewStringList.push_back(std::string(argv[i]));
            }

            else
            if (strcmp(argv[i], "--verbose") == 0 ||
                strcmp(argv[i], "-v") == 0)
            {
                gMergeXMLVerboseLevel++;
            }

            else
            if (strcmp(argv[i], "--ignoreError") == 0 ||
                strcmp(argv[i], "-e") == 0)
            {
                gIgnoreError = true;
            }

            else
            {
                throw(__LINE__);
            }
        }
        if (instructionFile == 0) throw(__LINE__);
    }
    
    catch (int e)
    {
        std::cerr << argv[0] << " program\n";
        std::cerr << "Build " << __DATE__ << " " << __TIME__ << "\n";
        std::cerr << "(-i) --instructionFile instruction_file_name\n";
        std::cerr << "(-r) --replace oldString newString\n";
        std::cerr << "(-v) --verbose\n";
        std::cerr << "(-e) --ignoreError\n";
        std::cerr << "\nNote: multiple replace strings can be specified\n";
        std::cerr << "Command line error thrown on line " << e << "\n";
        return 1;
    }

    if (instructionFile)
        ExecuteInstructionFile(instructionFile);

    gOldStringList.clear();
    gNewStringList.clear();
    gObjectList.Clear();
    return 0;        
}
    
// read and execute the instruction file
void ExecuteInstructionFile(const char *filename)
{
    DataFile theFile;
    char buffer[256];
    char line[256];
    char *tokens[256];
    int numTokens;
    int count = 0;
    XMLContainer *object;
    XMLContainer *object2;
    int rCount;

    theFile.SetExitOnError(true);
    theFile.ReadFile(filename);

    // do the global substitution
    if (gOldStringList.size() > 0)
    {
        for (unsigned int i = 0; i < gOldStringList.size(); i++)
        {
            rCount = theFile.Replace(gOldStringList[i].c_str(), gNewStringList[i].c_str());
            if (gMergeXMLVerboseLevel) std::cerr << "Replace \"" << gOldStringList[i] << 
                "\" with \"" << gNewStringList[i] << "\" Count = " << rCount << "\n";
        }
    }
    
    while (theFile.ReadNextLine(line, sizeof(buffer), true, '#', '\\') == false)
    {
        count++;
        strcpy(buffer, line);
        numTokens = DataFile::ReturnTokens(buffer, tokens, sizeof(tokens));

        if (gMergeXMLVerboseLevel > 1)
            std::cerr << "ExecuteInstructionFile:\tnumTokens\t" << numTokens << "\n";
                    
        if (gMergeXMLVerboseLevel) std::cerr << line << "\n";

        // read in a new XML file
        // ReadXMLFile XMLObjectName Filename
        if (strcmp(tokens[0], "ReadXMLFile") == 0)
        {
            if (numTokens != 3) ParseError(tokens, numTokens, count, "Wrong number of tokens");
            if (gObjectList.Get(tokens[1])) ParseError(tokens, numTokens, count, "XMLObject name not unique");
            object = new XMLContainer();
            object->SetName(tokens[1]);
            if (object->LoadXML(tokens[2]))
                ParseError(tokens, numTokens, count, "Error reading XML file");
            gObjectList.Add(object);
        }

        // write a FacetedObject to a .obj file
        // WriteXMLFile XMLObjectName Filename
        else
            if (strcmp(tokens[0], "WriteXMLFile") == 0)
            {
                if (numTokens != 3) ParseError(tokens, numTokens, count, "Wrong number of tokens");
                object = gObjectList.Get(tokens[1]);
                if (object == 0) ParseError(tokens, numTokens, count, "XMLObject name not found");
                if (object->WriteXML(tokens[2]))
                    ParseError(tokens, numTokens, count, "Error writing XML file");
            }

        // merge two XML files
        // Merge XMLObjectName1 XMLObjectName2 Element IDAttribute MergeAttribute Fraction
        // Merge XMLObjectName1 XMLObjectName2 Element IDAttribute MergeAttribute Fraction StartIndex EndIndex
        else
            if (strcmp(tokens[0], "Merge") == 0)
            {
                if (numTokens != 7 && numTokens != 9) ParseError(tokens, numTokens, count, "Wrong number of tokens");
                object = gObjectList.Get(tokens[1]);
                if (object == 0) ParseError(tokens, numTokens, count, "XMLObjectName name not found");
                object2 = gObjectList.Get(tokens[2]);
                if (object2 == 0) ParseError(tokens, numTokens, count, "XMLObjectName name not found");
                if (numTokens == 7)
                {
                    if (object->Merge(object2, tokens[3], tokens[4], tokens[5], MergeUtil::Double(tokens[6])))
                        ParseError(tokens, numTokens, count, "Error merging XML files");
                }
                else if (numTokens == 9)
                {
                    if (object->Merge(object2, tokens[3], tokens[4], tokens[5], MergeUtil::Double(tokens[6]), MergeUtil::Int(tokens[7]), MergeUtil::Int(tokens[8])))
                        ParseError(tokens, numTokens, count, "Error merging XML files");
                }
            }

        // merge two XML files only specified IDAttribute
        // MergeID XMLObjectName1 XMLObjectName2 Element IDAttribute ID MergeAttribute Fraction
        // MergeID XMLObjectName1 XMLObjectName2 Element IDAttribute ID MergeAttribute Fraction StartIndex EndIndex
        else
            if (strcmp(tokens[0], "MergeID") == 0)
            {
                if (numTokens != 8 && numTokens != 10) ParseError(tokens, numTokens, count, "Wrong number of tokens");
                object = gObjectList.Get(tokens[1]);
                if (object == 0) ParseError(tokens, numTokens, count, "XMLObjectName name not found");
                object2 = gObjectList.Get(tokens[2]);
                if (object2 == 0) ParseError(tokens, numTokens, count, "XMLObjectName name not found");
                if (numTokens == 8)
                {
                    if (object->MergeID(object2, tokens[3], tokens[4], tokens[5], tokens[6], MergeUtil::Double(tokens[7])))
                        ParseError(tokens, numTokens, count, "Error merging XML files");
                }
                else if (numTokens == 10)
                {
                    if (object->MergeID(object2, tokens[3], tokens[4], tokens[5], tokens[6], MergeUtil::Double(tokens[7]), MergeUtil::Int(tokens[8]), MergeUtil::Int(tokens[9])))
                        ParseError(tokens, numTokens, count, "Error merging XML files");
                }
            }

        // Operate on an XML value
        // Operate XMLObjectName1 Operation Element IDAttribute IDValue ChangeAttribute Offset
        else
            if (strcmp(tokens[0], "Operate") == 0)
            {
                if (numTokens != 8) ParseError(tokens, numTokens, count, "Wrong number of tokens");
                object = gObjectList.Get(tokens[1]);
                if (object == 0) ParseError(tokens, numTokens, count, "XMLObjectName name not found");
                if (object->Operate(tokens[2], tokens[3], tokens[4], tokens[5], tokens[6], MergeUtil::Int(tokens[7])))
                    ParseError(tokens, numTokens, count, "Error performing Operate");
            }

        // Set an XML value (will work with string values and can also increase the length of the argument list by 1)
        // Set XMLObjectName1 Operation Element IDAttribute IDValue ChangeAttribute Offset
        else
            if (strcmp(tokens[0], "Set") == 0)
            {
                if (numTokens != 8) ParseError(tokens, numTokens, count, "Wrong number of tokens");
                object = gObjectList.Get(tokens[1]);
                if (object == 0) ParseError(tokens, numTokens, count, "XMLObjectName name not found");
                if (object->Set(tokens[2], tokens[3], tokens[4], tokens[5], tokens[6], MergeUtil::Int(tokens[7])))
                    ParseError(tokens, numTokens, count, "Error performing Set");
            }

        // Scale an XML position key
        // Scale XMLObjectName1 Operation Element IDAttribute IDValue ChangeAttribute ReferenceID
        else
            if (strcmp(tokens[0], "Scale") == 0)
            {
                if (numTokens != 8) ParseError(tokens, numTokens, count, "Wrong number of tokens");
                object = gObjectList.Get(tokens[1]);
                if (object == 0) ParseError(tokens, numTokens, count, "XMLObjectName name not found");
                if (object->Scale(tokens[2], tokens[3], tokens[4], tokens[5], tokens[6], tokens[7]))
                    ParseError(tokens, numTokens, count, "Error performing Scale");
            }

        // Swap two XML values
        // Swap XMLObjectName1 Element IDAttribute IDValue ChangeAttribute Offset1 Offset2
        else
            if (strcmp(tokens[0], "Swap") == 0)
            {
                if (numTokens != 8) ParseError(tokens, numTokens, count, "Wrong number of tokens");
                object = gObjectList.Get(tokens[1]);
                if (object == 0) ParseError(tokens, numTokens, count, "XMLObjectName name not found");
                if (object->Swap(tokens[2], tokens[3], tokens[4], tokens[5], MergeUtil::Int(tokens[6]), MergeUtil::Int(tokens[7])))
                    ParseError(tokens, numTokens, count, "Error performing Swap");
            }
        
        // Generate a new XML values
        // Generate XMLObjectName1 Operation Element IDAttribute IDValue GenerateAttribute
        else
            if (strcmp(tokens[0], "Generate") == 0)
            {
                if (numTokens != 7) ParseError(tokens, numTokens, count, "Wrong number of tokens");
                object = gObjectList.Get(tokens[1]);
                if (object == 0) ParseError(tokens, numTokens, count, "XMLObjectName name not found");
                if (object->Generate(tokens[2], tokens[3], tokens[4], tokens[5], tokens[6]))
                    ParseError(tokens, numTokens, count, "Error performing Generate");
            }
    
        // Delete an XML values
        // Delete XMLObjectName1 Element IDAttribute IDValue DeleteAttribute
        else
            if (strcmp(tokens[0], "Delete") == 0)
            {
                if (numTokens != 6) ParseError(tokens, numTokens, count, "Wrong number of tokens");
                object = gObjectList.Get(tokens[1]);
                if (object == 0) ParseError(tokens, numTokens, count, "XMLObjectName name not found");
                if (object->Delete(tokens[2], tokens[3], tokens[4], tokens[5]))
                    ParseError(tokens, numTokens, count, "Error performing Delete");
            }
    
        else
        {
            ParseError(tokens, numTokens, count, "Unrecognised line");
        }
    }
}

// Produce a parse error message and terminate program
void ParseError(char **tokens, int numTokens, int count, const char *message)
{
    std::cerr << "Parse error on line " << count << "\n";
    for (int i = 0; i < numTokens; i++)
        std::cerr << tokens[i] << " ";
    std::cerr << "\n";
    if (message) std::cerr << message << "\n";
    if (gIgnoreError == false) exit(1);
}

