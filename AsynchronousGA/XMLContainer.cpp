/*
 *  XMLContainer.cc
 *  MergeXML
 *
 *  Created by Bill Sellers on Wed Dec 17 2003.
 *  Copyright (c) 2003 Bill Sellers. All rights reserved.
 *
 */
 
#include <map>
#include <string>
#include <sstream>
#include <float.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string.h>

#include "XMLContainer.h"
#include "MergeUtil.h"
#include "DataFile.h"
#include "MergeExpressionParser.h"
#include "rapidxml.hpp"
#include "rapidxml_print.hpp"

namespace AsynchronousGA
{

extern int gMergeXMLVerboseLevel;

XMLContainer::XMLContainer()
{
    m_Doc = 0;
    m_DocFile = 0;
    m_CaseSensitiveXMLAttributes = true;
}

XMLContainer::~XMLContainer()
{
    if (m_Doc) delete m_Doc;
    if (m_DocFile) delete m_DocFile;
}


int XMLContainer::LoadXML(const char *filename)
{
    rapidxml::xml_node<char> *cur;

    if (m_Doc) delete m_Doc;
    if (m_DocFile) delete m_DocFile;

    m_Doc = new rapidxml::xml_document<char>();
    m_DocFile = new DataFile();
    
    if (m_DocFile->ReadFile(filename)) return 1;

    char *data = m_DocFile->GetRawData();
    CleanExpressions((char *)data);
    
    try
    {
        // do the basic XML parsing
        m_Doc->parse<rapidxml::parse_default>(data);

        cur = m_Doc->first_node();
        if (cur == NULL)
        {
            fprintf(stderr,"Empty document\n");
            throw 1;
        }
        
        m_RootNode = cur->name();
            
        // now parse the elements in the file

        cur = cur->first_node();
        while (cur)
        {
            m_TagContentsList.push_back(cur);
            cur = cur->next_sibling();
        }
    }

    catch(...)
    {
        return 1;
    }

    return 0;
}


int XMLContainer::WriteXML(const char *filename)
{
    std::stringstream outputStream;
    outputStream.setf(std::ios_base::scientific, std::ios_base::floatfield);
    outputStream.precision(17);

    outputStream << "<GAITSYMODE>\n\n";

    for (unsigned int i = 0; i < m_TagContentsList.size(); i++)
    {
        outputStream << (*m_TagContentsList[i]) << "\n";
    }

    outputStream << "</GAITSYMODE>\n\n";
    
    int len = outputStream.tellp(); // note tellp is the number of characters added after initialisation
    char *buf = new char[len + 1];
    memcpy(buf, outputStream.str().c_str(), len);
    buf[len] = 0;
    RecoverExpressions(buf);
    
    std::ofstream outputFile(filename, std::ios::out | std::ios::binary);
    outputFile.write(buf, len);
    outputFile.close();
    
    return 0;
}

int XMLContainer::Merge(XMLContainer *sim, const char *element, const char *idAttribute, const char *mergeAttribute, double proportion, int startIndex, int endIndex)
{
    std::string tagToMerge;
    rapidxml::xml_node<char> *localNodePtr, *remoteNodePtr;
    unsigned int localTagIndex, remoteTagIndex;
    bool matchOK;
    char *localBuf, *remoteBuf;
    int returnCode = 0;
    
    tagToMerge = element;

    // now look for this tag locally
    for (localTagIndex = 0; localTagIndex < m_TagContentsList.size(); localTagIndex++)
    {
        localNodePtr = m_TagContentsList[localTagIndex];
        if (strcmp(tagToMerge.c_str(), (const char *)localNodePtr->name()) == 0) // name match
        {
            // now look for this tag remotely
            for (remoteTagIndex = 0; remoteTagIndex < sim->m_TagContentsList.size(); remoteTagIndex++)
            {
                remoteNodePtr = sim->m_TagContentsList[remoteTagIndex];
                if (strcmp(tagToMerge.c_str(), (const char *)remoteNodePtr->name()) == 0) // name match
                {
                    matchOK = true;
                    // now check the idAttribute
                    if (strlen(idAttribute) > 0)
                    {
                        localBuf = DoXmlGetProp(localNodePtr, (const char *)idAttribute);
                        if (localBuf)
                        {
                            remoteBuf = DoXmlGetProp(remoteNodePtr, (const char *)idAttribute);
                            if (remoteBuf)
                            {
                                if (strcmp((char *)localBuf, (char *)remoteBuf) != 0) 
                                {
                                    matchOK = false;
                                }
                            }
                        }
                    }

                    if (matchOK) // we've got a match
                    {
                        if (Merge(localNodePtr, remoteNodePtr, proportion, mergeAttribute, startIndex, endIndex))
                        {
                            returnCode = 1;
                        }
                    }
                }
            }
        }
    }
        

    return returnCode;
}

int XMLContainer::MergeID(XMLContainer *sim, const char *element, const char *idAttribute, const char *id, const char *mergeAttribute, double proportion, int startIndex, int endIndex)
{
    std::string tagToMerge;
    rapidxml::xml_node<char> *localNodePtr, *remoteNodePtr;
    unsigned int localTagIndex, remoteTagIndex;
    bool matchOK;
    char *localBuf, *remoteBuf;
    int returnCode = 0;
    
    tagToMerge = element;

    // now look for this tag locally
    for (localTagIndex = 0; localTagIndex < m_TagContentsList.size(); localTagIndex++)
    {
        localNodePtr = m_TagContentsList[localTagIndex];
        if (strcmp(tagToMerge.c_str(), (const char *)localNodePtr->name()) == 0) // name match
        {
            // now look for this tag remotely
            for (remoteTagIndex = 0; remoteTagIndex < sim->m_TagContentsList.size(); remoteTagIndex++)
            {
                remoteNodePtr = sim->m_TagContentsList[remoteTagIndex];
                if (strcmp(tagToMerge.c_str(), (const char *)remoteNodePtr->name()) == 0) // name match
                {
                    matchOK = true;
                    // now check the idAttribute
                    if (strlen(idAttribute) > 0)
                    {
                        localBuf = DoXmlGetProp(localNodePtr, (const char *)idAttribute);
                        if (localBuf)
                        {
                            if (strcmp((char *)localBuf, id) == 0)
                            {
                                remoteBuf = DoXmlGetProp(remoteNodePtr, (const char *)idAttribute);
                                if (remoteBuf)
                                {
                                    if (strcmp((char *)localBuf, (char *)remoteBuf) != 0) 
                                    {
                                        matchOK = false;
                                    }
                                }
                            }
                            else matchOK = false;
                        }
                    }

                    if (matchOK) // we've got a match
                    {
                        if (Merge(localNodePtr, remoteNodePtr, proportion, mergeAttribute, startIndex, endIndex))
                        {
                            returnCode = 1;
                        }
                    }
                }
            }
        }
    }
        

    return returnCode;
}

int XMLContainer::Merge(rapidxml::xml_node<char> *node1, rapidxml::xml_node<char> *node2, double proportion, const char *name, int startIndex, int endIndex)
{
    char *prop1 = 0;
    char *prop2 = 0;
    int i, n1, n2;
    char **list1 = 0;
    char **list2 = 0;
    rapidxml::xml_attribute<char> *attrPtr = 0;
    double v, v1, v2;
        
    std::ostringstream out;
    out.precision(17);

    try
    {
        prop1 = DoXmlGetProp(node1, (const char *)name);
        if (prop1 == 0) throw 1;
        // if (strstr((char *)prop1, "[[")) throw 1; // don't merge if [[expressions]]
        
        prop2 = DoXmlGetProp(node2, (const char *)name);
        if (prop2 == 0) throw 1;
        //if (strstr((char *)prop1, "[[")) throw 1; // don't merge if [[expressions]]
        
        n1 = DataFile::CountTokens((char *)prop1);
        n2 = DataFile::CountTokens((char *)prop2);

        if (n1 != n2) throw 1;
        if (startIndex >= n1) throw 1;
        if (endIndex < 0 || endIndex >= n1) endIndex = n1 - 1;

        list1 = new char *[n1];
        list2 = new char *[n2];
        DataFile::ReturnTokens((char *)prop1, list1, n1);
        DataFile::ReturnTokens((char *)prop2, list2, n2);
                
        for (i = 0; i < startIndex; i++) 
        {
            out << list1[i];
            if (i < (n1 - 1)) out << " ";
        }
            
        for (i = startIndex; i <= endIndex; i++)
        {
            if (gMergeXMLVerboseLevel > 2)
                std::cerr << "XMLContainer::Merge Merge " << proportion << " " << list1[i] << " " << list2[i] << "\n";
            if (strchr("+-0123456789", list1[i][0]) && strchr("+-0123456789", list2[i][0])) // quick and dirty check for a number
            {
                v1 = MergeUtil::Double(list1[i]);
                v2 = MergeUtil::Double(list2[i]);
                v = v1 + proportion * (v2 - v1);
                out << v;
                if (i < (n1 - 1)) out << " ";
            }
            else
            {
                if (proportion <= 0.5)
                    out << list1[i];
                else
                    out << list2[i];
                if (i < (n1 - 1)) out << " ";
            }
        }

        for (i = endIndex + 1; i < n1; i++) 
        {
            out << list1[i];
            if (i < (n1 - 1)) out << " ";
        }
        
        attrPtr = DoXmlReplaceProp(node1, (char *)name, (char *)out.str().c_str());
        if (attrPtr == 0) throw 1;
        if (gMergeXMLVerboseLevel > 1)
            std::cerr << "XMLContainer::Merge DoXmlReplaceProp \"" << name << "\" \"" << out.str() << "\"\n";
    }

    catch (...)
    {
        if (list1) delete [] list1;
        if (list2) delete [] list2;
        return 1;
    }

    delete [] list1;
    delete [] list2;
    return 0;
}

void XMLContainer::CleanExpressions(char *dataPtr)
{
    char *ptr1 = dataPtr;

    char *ptr2 = strstr(ptr1, "[[");
    while (ptr2)
    {
        ptr2 += 2;
        ptr1 = strstr(ptr2, "]]");
        if (ptr2 == 0)
        {
            std::cerr << "Error: could not find matching ]]\n";
            exit(1);
        }

        ReplaceChar(ptr2, ptr1 - ptr2, '<', '{');
        ReplaceChar(ptr2, ptr1 - ptr2, '>', '}');
        ReplaceChar(ptr2, ptr1 - ptr2, '&', '#');

        ptr1 += 2;
        ptr2 = strstr(ptr1, "[[");
    }
}

void XMLContainer::RecoverExpressions(char *dataPtr)
{
    char *ptr1 = dataPtr;

    char *ptr2 = strstr(ptr1, "[[");
    while (ptr2)
    {
        ptr2 += 2;
        ptr1 = strstr(ptr2, "]]");
        if (ptr2 == 0)
        {
            std::cerr << "Error: could not find matching ]]\n";
            exit(1);
        }

        ReplaceChar(ptr2, ptr1 - ptr2, '{', '<');
        ReplaceChar(ptr2, ptr1 - ptr2, '}', '>');
        ReplaceChar(ptr2, ptr1 - ptr2, '#', '&');

        ptr1 += 2;
        ptr2 = strstr(ptr1, "[[");
    }
}

void XMLContainer::ReplaceChar(char *p1, int len, char c1, char c2)
{
    for (int i = 0; i < len; i++)
        if (p1[i] == c1) p1[i] = c2;
}

// returns zero on success
int XMLContainer::Operate(const char *operation, const char *element, const char *idAttribute, const char *idValue, const char *changeAttribute, int offset)
{
    MergeExpressionInput input;
    MergeExpressionParser expressionParser;
    int len = strlen(operation);
    char *op = new char[len + 1];
    strcpy(op, operation);
        
    if (expressionParser.CreateFromString(op, len)) 
    {
        std::cerr << "MergeExpressionParser.CreateFromString error:\n" << op << "\n";
        delete [] op;
        return 1;
    }
    
    std::string tagToChange;
    rapidxml::xml_node<char> *localNodePtr;
    unsigned int localTagIndex;
    char *localBuf;
    int n1, i;
        
    tagToChange = element;

    // now look for the element
    for (localTagIndex = 0; localTagIndex < m_TagContentsList.size(); localTagIndex++)
    {
        localNodePtr = m_TagContentsList[localTagIndex];
        if (strcmp(tagToChange.c_str(), (const char *)localNodePtr->name()) == 0) // name match
        {
            localBuf = DoXmlGetProp(localNodePtr, (const char *)idAttribute);
            if (localBuf)
            {
                if (strcmp(idValue, (const char *)localBuf) == 0 || strcmp(idValue, "*") == 0) // allow any match with '*'
                {
                    localBuf = DoXmlGetProp(localNodePtr, (const char *)changeAttribute);
                    if (localBuf)
                    {
                        n1 = DataFile::CountTokens((char *)localBuf);
                        if (offset < n1)
                        {
                            char *buf = (char *)malloc(strlen((char *)localBuf) + 1);
                            strcpy(buf, (char *)localBuf);
                            char **ptrs = (char **)malloc(n1 * sizeof(char *));
                            DataFile::ReturnTokens(buf, ptrs, n1);
                            std::ostringstream out;
                            out.precision(17);
                            out.setf( std::ios::scientific );
                            input.v = MergeUtil::Double(ptrs[offset]);
                            for (i = 0; i < n1; i++)
                            {
                                if (i != offset) out << ptrs[i];
                                else out << expressionParser.Evaluate(&input);
                                if (i < (n1 - 1)) out << " ";
                            }
                            if (DoXmlReplaceProp(localNodePtr, (char *)changeAttribute, (char *)out.str().c_str()) == 0)
                            {
                                delete [] op;
                                delete [] ptrs;
                                delete buf;
                                return 1;
                            }
                            if (gMergeXMLVerboseLevel > 1)
                                std::cerr << "XMLContainer::Operate DoXmlReplaceProp \"" << changeAttribute << "\" \"" << out.str() << "\"\n";
                            delete [] ptrs;
                            delete buf;
                        }
                        else
                        {
                            if (gMergeXMLVerboseLevel > 1)
                                std::cerr << "XMLContainer::Operate offset >= n1\n";
                        }
                    }
                }
            }
        }
    }
    
    delete [] op;
    return 0;
}

// returns zero on success
int XMLContainer::Set(const char *operation, const char *element, const char *idAttribute, const char *idValue, const char *changeAttribute, int offset)
{
    std::string tagToChange;
    rapidxml::xml_node<char> *localNodePtr;
    unsigned int localTagIndex;
    char *localBuf;
    int n1, i;
    
    tagToChange = element;
    
    // now look for the element
    for (localTagIndex = 0; localTagIndex < m_TagContentsList.size(); localTagIndex++)
    {
        localNodePtr = m_TagContentsList[localTagIndex];
        if (strcmp(tagToChange.c_str(), (const char *)localNodePtr->name()) == 0) // name match
        {
            localBuf = DoXmlGetProp(localNodePtr, (const char *)idAttribute);
            if (localBuf)
            {
                if (strcmp(idValue, (const char *)localBuf) == 0 || strcmp(idValue, "*") == 0) // allow any match with '*'
                {
                    localBuf = DoXmlGetProp(localNodePtr, (const char *)changeAttribute);
                    if (localBuf)
                    {
                        n1 = DataFile::CountTokens((char *)localBuf);
                        if (offset <= n1)
                        {
                            char *buf = (char *)malloc(strlen((char *)localBuf) + 1);
                            strcpy(buf, (char *)localBuf);
                            char **ptrs = (char **)malloc(n1 * sizeof(char *));
                            DataFile::ReturnTokens(buf, ptrs, n1);
                            std::ostringstream out;
                            out.precision(17);
                            out.setf( std::ios::scientific );
                            for (i = 0; i < n1; i++)
                            {
                                if (i != offset) out << ptrs[i];
                                else out << operation;
                                if (i < (n1 - 1)) out << " ";
                            }
                            if (i == offset) out << " " << operation; // special case when appending a new value
                            if (DoXmlReplaceProp(localNodePtr, (char *)changeAttribute, (char *)out.str().c_str()) == 0)
                            {
                                delete [] ptrs;
                                delete buf;
                                return 1;
                            }
                            if (gMergeXMLVerboseLevel > 1)
                                std::cerr << "XMLContainer::Set DoXmlReplaceProp \"" << changeAttribute << "\" \"" << out.str() << "\"\n";
                            delete [] ptrs;
                            delete buf;
                        }
                        else
                        {
                            if (gMergeXMLVerboseLevel > 1)
                                std::cerr << "XMLContainer::Set offset > n1\n";
                        }
                    }
                }
            }
        }
    }
    
    return 0;
}

// returns zero on success
int XMLContainer::Scale(const char *operation, const char *element, const char *idAttribute, const char *idValue, const char *changeAttribute, const char *referenceID)
{
    std::string tagToChange;
    rapidxml::xml_node<char> *localNodePtr;
    unsigned int localTagIndex;
    char *localBuf;
    int n1, i;
    
    tagToChange = element;
    
    // now look for the element
    for (localTagIndex = 0; localTagIndex < m_TagContentsList.size(); localTagIndex++)
    {
        localNodePtr = m_TagContentsList[localTagIndex];
        if (strcmp(tagToChange.c_str(), (const char *)localNodePtr->name()) == 0) // name match
        {
            localBuf = DoXmlGetProp(localNodePtr, (const char *)idAttribute);
            if (localBuf)
            {
                std::string idValueStore((const char *)localBuf);
                if (strcmp(idValue, (const char *)localBuf) == 0 || strcmp(idValue, "*") == 0) // allow any match with '*'
                {
                    localBuf = DoXmlGetProp(localNodePtr, (const char *)changeAttribute);
                    if (localBuf)
                    {
                        n1 = DataFile::CountTokens((char *)localBuf);
                        if (n1 == 4)
                        {
                            char *buf = (char *)malloc(strlen((char *)localBuf) + 1);
                            strcpy(buf, (char *)localBuf);
                            char **ptrs = (char **)malloc(n1 * sizeof(char *));
                            DataFile::ReturnTokens(buf, ptrs, n1);
                            
                            if (strcmp(ptrs[0], referenceID) != 0) // doesn't match - not an error
                            {
                                delete [] ptrs;
                                delete buf;
                                continue;
                            }
                            
                            double scale = MergeUtil::Double(operation); // get scale factor
                            
                            std::ostringstream out;
                            out.precision(17);
                            out.setf( std::ios::scientific );
                            out << ptrs[0];
                            
                            for (i = 1; i < n1; i++)
                            {
                                out << " " << MergeUtil::Double(ptrs[i]) * scale;
                            }
                            if (DoXmlReplaceProp(localNodePtr, (char *)changeAttribute, (char *)out.str().c_str()) == 0) // this is an error condition so abort
                            {
                                delete [] ptrs; 
                                delete buf;
                                return 1;
                            }
                            if (gMergeXMLVerboseLevel > 1)
                                std::cerr << "XMLContainer::Scale DoXmlReplaceProp \"" << idValueStore << "\" \"" << changeAttribute << "\" \"" << out.str() << "\"\n";
                            delete [] ptrs;
                            delete buf;
                        }
                        else
                        {
                            if (gMergeXMLVerboseLevel > 1)
                                std::cerr << "XMLContainer::Scale must have 4 elements in property\n";
                        }
                    }
                }
            }
        }
    }
    
    return 0;
}

// returns zero on success
int XMLContainer::Swap(const char *element, const char *idAttribute, const char *idValue, const char *changeAttribute, int offset1, int offset2)
{
    std::string tagToChange;
    rapidxml::xml_node<char> *localNodePtr;
    unsigned int localTagIndex;
    char *localBuf;
    int n1, i;
    
    tagToChange = element;
    
    // now look for the element
    for (localTagIndex = 0; localTagIndex < m_TagContentsList.size(); localTagIndex++)
    {
        localNodePtr = m_TagContentsList[localTagIndex];
        if (strcmp(tagToChange.c_str(), (const char *)localNodePtr->name()) == 0) // name match
        {
            localBuf = DoXmlGetProp(localNodePtr, (const char *)idAttribute);
            if (localBuf)
            {
                if (strcmp(idValue, (const char *)localBuf) == 0 || strcmp(idValue, "*") == 0) // allow any match with '*'
                {
                    localBuf = DoXmlGetProp(localNodePtr, (const char *)changeAttribute);
                    if (localBuf)
                    {
                        n1 = DataFile::CountTokens((char *)localBuf);
                        if (offset1 < n1 && offset2 < n1)
                        {
                            char *buf = (char *)malloc(strlen((char *)localBuf) + 1);
                            strcpy(buf, (char *)localBuf);
                            char **ptrs = (char **)malloc(n1 * sizeof(char *));
                            DataFile::ReturnTokens(buf, ptrs, n1);
                            std::ostringstream out;
                            out.precision(17);
                            out.setf( std::ios::scientific );
                            for (i = 0; i < n1; i++)
                            {
                                if (i == offset1) out << ptrs[offset2];
                                else if (i == offset2) out << ptrs[offset1];
                                else out << ptrs[i];
                                if (i < (n1 - 1)) out << " ";
                            }
                            if (DoXmlReplaceProp(localNodePtr, (char *)changeAttribute, (char *)out.str().c_str()) == 0)
                            {
                                delete [] ptrs;
                                delete buf;
                                return 1;
                            }
                            if (gMergeXMLVerboseLevel > 1)
                                std::cerr << "XMLContainer::Swap DoXmlReplaceProp \"" << changeAttribute << "\" \"" << out.str() << "\"\n";
                            delete [] ptrs;
                            delete buf;
                        }
                        else
                        {
                            if (gMergeXMLVerboseLevel > 1)
                                std::cerr << "XMLContainer::Operate offset >= n1\n";
                        }
                    }
                }
            }
        }
    }
    
    return 0;
}

// returns zero on success
int XMLContainer::Generate(const char *operation, const char *element, const char *idAttribute, const char *idValue, const char *changeAttribute)
{
    std::string tagToChange;
    rapidxml::xml_node<char> *localNodePtr;
    unsigned int localTagIndex;
    char *localBuf;
    
    tagToChange = element;
    
    // now look for the element
    for (localTagIndex = 0; localTagIndex < m_TagContentsList.size(); localTagIndex++)
    {
        localNodePtr = m_TagContentsList[localTagIndex];
        if (strcmp(tagToChange.c_str(), (const char *)localNodePtr->name()) == 0) // name match
        {
            localBuf = DoXmlGetProp(localNodePtr, (const char *)idAttribute);
            if (localBuf)
            {
                if (strcmp(idValue, (const char *)localBuf) == 0 || strcmp(idValue, "*") == 0) // allow any match with '*'
                {
                    rapidxml::xml_attribute<char> *ptr = DoXmlReplaceProp(localNodePtr, (const char *)changeAttribute, (const char *)operation);
                    if (ptr)
                    {
                        if (gMergeXMLVerboseLevel > 1)
                            std::cerr << "XMLContainer::Generate \"" << changeAttribute << "\" \"" << element << "\"\n";
                    }
                }
            }
        }
    }
    
    return 0;
}

// returns zero on success
int XMLContainer::Delete(const char *element, const char *idAttribute, const char *idValue, const char *changeAttribute)
{
    std::string tagToChange;
    rapidxml::xml_node<char> *localNodePtr;
    unsigned int localTagIndex;
    char *localBuf;
    
    tagToChange = element;
    
    
    // now look for the element
    for (localTagIndex = 0; localTagIndex < m_TagContentsList.size(); localTagIndex++)
    {
        localNodePtr = m_TagContentsList[localTagIndex];
        if (strcmp(tagToChange.c_str(), (const char *)localNodePtr->name()) == 0) // name match
        {
            localBuf = DoXmlGetProp(localNodePtr, (const char *)idAttribute);
            if (localBuf)
            {
                if (strcmp(idValue, (const char *)localBuf) == 0 || strcmp(idValue, "*") == 0) // allow any match with '*'
                {
                    DoXmlRemoveProp(localNodePtr, (const char *)changeAttribute);
                }
            }
        }
    }
    
    return 0;
}

char *XMLContainer::DoXmlGetProp(rapidxml::xml_node<char> *cur, const char *name)
{
    char *buf = 0;
    if (m_CaseSensitiveXMLAttributes)
    {
        for (rapidxml::xml_attribute<char> *attr = cur->first_attribute(); attr; attr = attr->next_attribute())
        {
            if (strcasecmp(name, attr->name()) == 0)
            {
                buf = attr->value();
                break;
            }
        }
    }
    else
    {
        for (rapidxml::xml_attribute<char> *attr = cur->first_attribute(); attr; attr = attr->next_attribute())
        {
            if (strcmp(name, attr->name()) == 0)
            {
                buf = attr->value();
                break;
            }
        }
    }

    return buf;
}

rapidxml::xml_attribute<char> *XMLContainer::DoXmlReplaceProp(rapidxml::xml_node<char> *cur, const char *name, const char *newValue)
{
    rapidxml::xml_attribute<char> *ptr = 0;
    if (m_CaseSensitiveXMLAttributes)
    {
        for (rapidxml::xml_attribute<char> *attr = cur->first_attribute(); attr; attr = attr->next_attribute())
        {
            if (strcasecmp(name, attr->name()) == 0)
            {
                ptr = attr;
                break;
            }
        }
    }
    else
    {
        for (rapidxml::xml_attribute<char> *attr = cur->first_attribute(); attr; attr = attr->next_attribute())
        {
            if (strcmp(name, attr->name()) == 0)
            {
                ptr = attr;
                break;
            }
        }
    }

    if (ptr) cur->remove_attribute(ptr);
    char *allocatedName = cur->document()->allocate_string(name);
    char *allocatedValue = cur->document()->allocate_string(newValue);
    ptr = cur->document()->allocate_attribute(allocatedName, allocatedValue);
    cur->append_attribute(ptr);
    return ptr;
}

void XMLContainer::DoXmlRemoveProp(rapidxml::xml_node<char> *cur, const char *name)
{
    rapidxml::xml_attribute<char> *ptr = 0;
    if (m_CaseSensitiveXMLAttributes)
    {
        for (rapidxml::xml_attribute<char> *attr = cur->first_attribute(); attr; attr = attr->next_attribute())
        {
            if (strcasecmp(name, attr->name()) == 0)
            {
                ptr = attr;
                break;
            }
        }
    }
    else
    {
        for (rapidxml::xml_attribute<char> *attr = cur->first_attribute(); attr; attr = attr->next_attribute())
        {
            if (strcmp(name, attr->name()) == 0)
            {
                ptr = attr;
                break;
            }
        }
    }

    if (ptr) cur->remove_attribute(ptr);
}

rapidxml::xml_attribute<char> *XMLContainer::DoXmlHasProp(rapidxml::xml_node<char> *cur, const char *name)
{
    rapidxml::xml_attribute<char> *ptr = 0;
    if (m_CaseSensitiveXMLAttributes)
    {
        for (rapidxml::xml_attribute<char> *attr = cur->first_attribute(); attr; attr = attr->next_attribute())
        {
            if (strcasecmp(name, attr->name()) == 0)
            {
                ptr = attr;
                break;
            }
        }
    }
    else
    {
        for (rapidxml::xml_attribute<char> *attr = cur->first_attribute(); attr; attr = attr->next_attribute())
        {
            if (strcmp(name, attr->name()) == 0)
            {
                ptr = attr;
                break;
            }
        }
    }

    return ptr;
}

}
