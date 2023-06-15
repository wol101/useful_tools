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

#include "MergeXML.h"

namespace AsynchronousGA
{

int gMergeXMLVerboseLevel = 0;

MergeXML::MergeXML()
{
    m_ignoreError = true;
}

MergeXML::~MergeXML()
{
}
    
// read and execute the instruction file
void MergeXML::ExecuteInstructionFile(const char *bigString)
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
    theFile.SetRawData(bigString);

    // do the global substitution
    if (m_oldStringList.size() > 0)
    {
        for (unsigned int i = 0; i < m_oldStringList.size(); i++)
        {
            rCount = theFile.Replace(m_oldStringList[i].c_str(), m_newStringList[i].c_str());
            if (gMergeXMLVerboseLevel) std::cerr << "Replace \"" << m_oldStringList[i] <<
                "\" with \"" << m_newStringList[i] << "\" Count = " << rCount << "\n";
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
            if (m_objectList.Get(tokens[1])) ParseError(tokens, numTokens, count, "XMLObject name not unique");
            object = new XMLContainer();
            object->SetName(tokens[1]);
            if (object->LoadXML(tokens[2]))
                ParseError(tokens, numTokens, count, "Error reading XML file");
            m_objectList.Add(object);
        }

        // write a FacetedObject to a .obj file
        // WriteXMLFile XMLObjectName Filename
        else
            if (strcmp(tokens[0], "WriteXMLFile") == 0)
            {
                if (numTokens != 3) ParseError(tokens, numTokens, count, "Wrong number of tokens");
                object = m_objectList.Get(tokens[1]);
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
                object = m_objectList.Get(tokens[1]);
                if (object == 0) ParseError(tokens, numTokens, count, "XMLObjectName name not found");
                object2 = m_objectList.Get(tokens[2]);
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
                object = m_objectList.Get(tokens[1]);
                if (object == 0) ParseError(tokens, numTokens, count, "XMLObjectName name not found");
                object2 = m_objectList.Get(tokens[2]);
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
                object = m_objectList.Get(tokens[1]);
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
                object = m_objectList.Get(tokens[1]);
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
                object = m_objectList.Get(tokens[1]);
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
                object = m_objectList.Get(tokens[1]);
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
                object = m_objectList.Get(tokens[1]);
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
                object = m_objectList.Get(tokens[1]);
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
void MergeXML::ParseError(char **tokens, int numTokens, int count, const char *message)
{
    std::cerr << "Parse error on line " << count << "\n";
    for (int i = 0; i < numTokens; i++)
        std::cerr << tokens[i] << " ";
    std::cerr << "\n";
    if (message) std::cerr << message << "\n";
    if (m_ignoreError == false) exit(1);
}

}

