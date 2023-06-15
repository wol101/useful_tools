/*
 *  XMLContainer.cc
 *  MergeXML
 *
 *  Created by Bill Sellers on Wed Dec 17 2003.
 *  Copyright (c) 2003 Bill Sellers. All rights reserved.
 *
 */

#include "XMLContainer.h"
#include "MergeUtil.h"
#include "DataFile.h"
#include "MergeExpressionParser.h"

#include "rapidxml.hpp"
#include "rapidxml_print.hpp"
#include "pystring.h"

#include <map>
#include <string>
#include <sstream>
#include <float.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string.h>
#include <memory>


XMLContainer::XMLContainer()
{
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
    CleanExpressions(data);

    // do the basic XML parsing
    m_Doc->parse<rapidxml::parse_default>(data);

    cur = m_Doc->first_node();
    if (cur == nullptr)
    {
        fprintf(stderr,"Empty document\n");
        return 1;
    }

    m_RootNode = cur->name();

    // now parse the elements in the file

    cur = cur->first_node();
    while (cur)
    {
        m_TagContentsList.push_back(cur);
        cur = cur->next_sibling();
    }

    return 0;
}


int XMLContainer::WriteXML(const char *filename)
{
    std::stringstream outputStream;
    outputStream.precision(17);

    outputStream << "<" << m_RootNode << ">\n\n";

    for (unsigned int i = 0; i < m_TagContentsList.size(); i++)
    {
        outputStream << (*m_TagContentsList[i]) << "\n";
    }

    outputStream << "</" << m_RootNode << ">\n\n";

    std::string buf = outputStream.str();
    RecoverExpressions(&buf[0]);

    std::ofstream outputFile(filename, std::ios::out | std::ios::binary);
    outputFile.write(buf.c_str(), std::streamsize(buf.size()));
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
        if (strcmp(tagToMerge.c_str(), localNodePtr->name()) == 0) // name match
        {
            // now look for this tag remotely
            for (remoteTagIndex = 0; remoteTagIndex < sim->m_TagContentsList.size(); remoteTagIndex++)
            {
                remoteNodePtr = sim->m_TagContentsList[remoteTagIndex];
                if (strcmp(tagToMerge.c_str(), remoteNodePtr->name()) == 0) // name match
                {
                    matchOK = true;
                    // now check the idAttribute
                    if (strlen(idAttribute) > 0)
                    {
                        localBuf = DoXmlGetProp(localNodePtr, idAttribute);
                        if (localBuf)
                        {
                            remoteBuf = DoXmlGetProp(remoteNodePtr, idAttribute);
                            if (remoteBuf)
                            {
                                if (strcmp(localBuf, remoteBuf) != 0)
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
        if (strcmp(tagToMerge.c_str(), localNodePtr->name()) == 0) // name match
        {
            // now look for this tag remotely
            for (remoteTagIndex = 0; remoteTagIndex < sim->m_TagContentsList.size(); remoteTagIndex++)
            {
                remoteNodePtr = sim->m_TagContentsList[remoteTagIndex];
                if (strcmp(tagToMerge.c_str(), remoteNodePtr->name()) == 0) // name match
                {
                    matchOK = true;
                    // now check the idAttribute
                    if (strlen(idAttribute) > 0)
                    {
                        localBuf = DoXmlGetProp(localNodePtr, idAttribute);
                        if (localBuf)
                        {
                            if (strcmp(localBuf, id) == 0)
                            {
                                remoteBuf = DoXmlGetProp(remoteNodePtr, idAttribute);
                                if (remoteBuf)
                                {
                                    if (strcmp(localBuf, remoteBuf) != 0)
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
    rapidxml::xml_attribute<char> *attrPtr = nullptr;
    double v, v1, v2;

    std::ostringstream out;
    out.precision(17);

    char *prop1 = DoXmlGetProp(node1, name);
    if (prop1 == nullptr) return 1;
    // if (strstr(prop1, "[[")) return 1; // don't merge if [[expressions]]

    char *prop2 = DoXmlGetProp(node2, name);
    if (prop2 == nullptr) return 1;
    //if (strstr(prop1, "[[")) return 1; // don't merge if [[expressions]]

    std::vector<std::string> list1, list2;
    pystring::split(std::string(prop1), list1);
    std::vector<std::string> t2;
    pystring::split(std::string(prop2), list2);

    if (list1.size() != list2.size()) return 1;
    if (startIndex >= int(list1.size())) return 1;
    if (endIndex < 0 || endIndex >= int(list1.size())) endIndex = int(list1.size()) - 1;

    for (int i = 0; i < startIndex; i++)
    {
        out << list1[i];
        if (i < (int(list1.size()) - 1)) out << " ";
    }

    for (int i = startIndex; i <= endIndex; i++)
    {
        if (m_XMLContainerVerbosityLevel > 2) std::cerr << "XMLContainer::Merge Merge " << proportion << " " << list1[i] << " " << list2[i] << "\n";
        if (strchr("+-0123456789", list1[i][0]) && strchr("+-0123456789", list2[i][0])) // quick and dirty check for a number
        {
            v1 = MergeUtil::Double(list1[i].c_str());
            v2 = MergeUtil::Double(list2[i].c_str());
            v = v1 + proportion * (v2 - v1);
            out << v;
            if (i < (int(list1.size()) - 1)) out << " ";
        }
        else
        {
            if (proportion <= 0.5)
                out << list1[i];
            else
                out << list2[i];
            if (i < (int(list1.size()) - 1)) out << " ";
        }
    }

    for (int i = endIndex + 1; i < int(list1.size()); i++)
    {
        out << list1[i];
        if (i < (int(list1.size()) - 1)) out << " ";
    }

    attrPtr = DoXmlReplaceProp(node1, name, out.str().c_str());
    if (attrPtr == nullptr) return 1;
    if (m_XMLContainerVerbosityLevel > 1) std::cerr << "XMLContainer::Merge DoXmlReplaceProp \"" << name << "\" \"" << out.str() << "\"\n";
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
        if (ptr2 == nullptr)
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
        if (ptr2 == nullptr)
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

void XMLContainer::ReplaceChar(char *p1, size_t len, char c1, char c2)
{
    for (size_t i = 0; i < len; i++)
        if (p1[i] == c1) p1[i] = c2;
}

// returns zero on success
int XMLContainer::Operate(const char *operation, const char *element, const char *idAttribute, const char *idValue, const char *changeAttribute, int offset, std::map<std::string, double> *extraVariables)
{
    MergeExpressionInput input;
    MergeExpressionParser expressionParser;
    expressionParser.setExtraVariables(extraVariables);
    std::string op(operation);

    if (expressionParser.CreateFromString(&op[0], int(op.size())))
    {
        std::cerr << "MergeExpressionParser.CreateFromString error:\n" << op << "\n";
        return 1;
    }

    std::string tagToChange;
    rapidxml::xml_node<char> *localNodePtr;
    unsigned int localTagIndex;
    char *localBuf;

    tagToChange = element;

    // now look for the element
    for (localTagIndex = 0; localTagIndex < m_TagContentsList.size(); localTagIndex++)
    {
        localNodePtr = m_TagContentsList[localTagIndex];
        if (strcmp(tagToChange.c_str(), localNodePtr->name()) == 0) // name match
        {
            localBuf = DoXmlGetProp(localNodePtr, idAttribute);
            if (localBuf)
            {
                if (strcmp(idValue, localBuf) == 0 || strcmp(idValue, "*") == 0) // allow any match with '*'
                {
                    localBuf = DoXmlGetProp(localNodePtr, changeAttribute);
                    if (localBuf)
                    {
                        std::vector<std::string> tokens;
                        pystring::split(std::string(localBuf), tokens);
                        if (offset < (int)tokens.size())
                        {
                            std::ostringstream out;
                            out.precision(17);
                            input.v = MergeUtil::Double(tokens[offset].c_str());
                            for (int i = 0; i < (int)tokens.size(); i++)
                            {
                                if (i != offset) out << tokens[i];
                                else out << expressionParser.Evaluate(&input);
                                if (i < ((int)tokens.size() - 1)) out << " ";
                            }
                            if (DoXmlReplaceProp(localNodePtr, changeAttribute, out.str().c_str()) == nullptr)
                            {
                                return 1;
                            }
                            if (m_XMLContainerVerbosityLevel > 1) std::cerr << "XMLContainer::Operate DoXmlReplaceProp \"" << changeAttribute << "\" \"" << out.str() << "\"\n";
                        }
                        else
                        {
                            if (m_XMLContainerVerbosityLevel > 1) std::cerr << "XMLContainer::Operate offset >= n1\n";
                        }
                    }
                }
            }
        }
    }

    return 0;
}

// returns zero on success
int XMLContainer::Global(const char *globalName, const char *element, const char *idAttribute, const char *idValue, const char *changeAttribute, int offset, std::map<std::string, double> *extraVariables)
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
        if (strcmp(tagToChange.c_str(), localNodePtr->name()) == 0) // name match
        {
            localBuf = DoXmlGetProp(localNodePtr, idAttribute);
            if (localBuf)
            {
                if (strcmp(idValue, localBuf) == 0 || strcmp(idValue, "*") == 0) // allow any match with '*'
                {
                    localBuf = DoXmlGetProp(localNodePtr, changeAttribute);
                    if (localBuf)
                    {
                        std::vector<std::string> tokens;
                        pystring::split(std::string(localBuf), tokens);
                        if (offset < int(tokens.size()))
                        {
                            std::ostringstream out;
                            out.precision(17);
                            (*extraVariables)[globalName] = MergeUtil::Double(tokens[offset].c_str());
                            if (m_XMLContainerVerbosityLevel > 1) std::cerr << "XMLContainer::Global DoXmlReplaceProp \"" << changeAttribute << "\" " << globalName << " = " << (*extraVariables)[globalName] << "\n";
                        }
                        else
                        {
                            if (m_XMLContainerVerbosityLevel > 1) std::cerr << "XMLContainer::Global offset >= n1\n";
                        }
                    }
                }
            }
        }
    }

    return 0;
}
// returns zero on success
int XMLContainer::Set(const char *operation, const char *element, const char *idAttribute, const char *idValue, const char *changeAttribute, int offset)
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
        if (strcmp(tagToChange.c_str(), localNodePtr->name()) == 0) // name match
        {
            localBuf = DoXmlGetProp(localNodePtr, idAttribute);
            if (localBuf)
            {
                if (strcmp(idValue, localBuf) == 0 || strcmp(idValue, "*") == 0) // allow any match with '*'
                {
                    localBuf = DoXmlGetProp(localNodePtr, changeAttribute);
                    if (localBuf)
                    {
                        if (offset < 0)
                        {
                            DoXmlReplaceProp(localNodePtr, changeAttribute, operation);
                            if (m_XMLContainerVerbosityLevel > 1)
                                std::cerr << "XMLContainer::Set DoXmlReplaceProp \"" << changeAttribute << "\" \"" << operation << "\"\n";
                        }
                        else
                        {
                            std::vector<std::string> tokens;
                            pystring::split(std::string(localBuf), tokens);
                            if (offset <= int(tokens.size()))
                            {
                                std::ostringstream out;
                                out.precision(17);
                                for (int i = 0; i < int(tokens.size()); i++)
                                {
                                    if (i != offset) out << tokens[i];
                                    else out << operation;
                                    if (i < (int(tokens.size()) - 1)) out << " ";
                                }
                                if (int(tokens.size()) == offset) out << " " << operation; // special case when appending a new value
                                if (DoXmlReplaceProp(localNodePtr, changeAttribute, out.str().c_str()) == nullptr)
                                {
                                    return 1;
                                }
                                if (m_XMLContainerVerbosityLevel > 1)
                                    std::cerr << "XMLContainer::Set DoXmlReplaceProp \"" << changeAttribute << "\" \"" << out.str() << "\"\n";
                            }
                            else
                            {
                                if (m_XMLContainerVerbosityLevel > 1)
                                    std::cerr << "XMLContainer::Set offset > n1\n";
                            }
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

    tagToChange = element;

    // now look for the element
    for (localTagIndex = 0; localTagIndex < m_TagContentsList.size(); localTagIndex++)
    {
        localNodePtr = m_TagContentsList[localTagIndex];
        if (strcmp(tagToChange.c_str(), localNodePtr->name()) == 0) // name match
        {
            localBuf = DoXmlGetProp(localNodePtr, idAttribute);
            if (localBuf)
            {
                std::string idValueStore(localBuf);
                if (strcmp(idValue, localBuf) == 0 || strcmp(idValue, "*") == 0) // allow any match with '*'
                {
                    localBuf = DoXmlGetProp(localNodePtr, changeAttribute);
                    if (localBuf)
                    {
                        std::vector<std::string> tokens;
                        pystring::split(std::string(localBuf), tokens);
                        if (tokens.size() == 4)
                        {
                            if (strcmp(tokens[0].c_str(), referenceID) != 0) // doesn't match - not an error
                            {
                                continue;
                            }

                            double scale = MergeUtil::Double(operation); // get scale factor

                            std::ostringstream out;
                            out.precision(17);
                            out << tokens[0];

                            for (int i = 1; i < int(tokens.size()); i++)
                            {
                                out << " " << MergeUtil::Double(tokens[i].c_str()) * scale;
                            }
                            if (DoXmlReplaceProp(localNodePtr, changeAttribute, out.str().c_str()) == nullptr) // this is an error condition so abort
                            {
                                return 1;
                            }
                            if (m_XMLContainerVerbosityLevel > 1)
                                std::cerr << "XMLContainer::Scale DoXmlReplaceProp \"" << idValueStore << "\" \"" << changeAttribute << "\" \"" << out.str() << "\"\n";
                        }
                        else
                        {
                            if (m_XMLContainerVerbosityLevel > 1)
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

    tagToChange = element;

    // now look for the element
    for (localTagIndex = 0; localTagIndex < m_TagContentsList.size(); localTagIndex++)
    {
        localNodePtr = m_TagContentsList[localTagIndex];
        if (strcmp(tagToChange.c_str(), localNodePtr->name()) == 0) // name match
        {
            localBuf = DoXmlGetProp(localNodePtr, idAttribute);
            if (localBuf)
            {
                if (strcmp(idValue, localBuf) == 0 || strcmp(idValue, "*") == 0) // allow any match with '*'
                {
                    localBuf = DoXmlGetProp(localNodePtr, changeAttribute);
                    if (localBuf)
                    {
                        std::vector<std::string> tokens;
                        pystring::split(std::string(localBuf), tokens);
                        if (offset1 < int(tokens.size()) && offset2 < int(tokens.size()))
                        {
                            std::ostringstream out;
                            out.precision(17);
                            for (int i = 0; i < int(tokens.size()); i++)
                            {
                                if (i == offset1) out << tokens[offset2];
                                else if (i == offset2) out << tokens[offset1];
                                else out << tokens[i];
                                if (i < (int(tokens.size()) - 1)) out << " ";
                            }
                            if (DoXmlReplaceProp(localNodePtr, changeAttribute, out.str().c_str()) == nullptr)
                            {
                                return 1;
                            }
                            if (m_XMLContainerVerbosityLevel > 1)
                                std::cerr << "XMLContainer::Swap DoXmlReplaceProp \"" << changeAttribute << "\" \"" << out.str() << "\"\n";
                        }
                        else
                        {
                            if (m_XMLContainerVerbosityLevel > 1)
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
        if (strcmp(tagToChange.c_str(), localNodePtr->name()) == 0) // name match
        {
            localBuf = DoXmlGetProp(localNodePtr, idAttribute);
            if (localBuf)
            {
                if (strcmp(idValue, localBuf) == 0 || strcmp(idValue, "*") == 0) // allow any match with '*'
                {
                    rapidxml::xml_attribute<char> *ptr = DoXmlReplaceProp(localNodePtr, changeAttribute, operation);
                    if (ptr)
                    {
                        if (m_XMLContainerVerbosityLevel > 1)
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
        if (strcmp(tagToChange.c_str(), localNodePtr->name()) == 0) // name match
        {
            localBuf = DoXmlGetProp(localNodePtr, idAttribute);
            if (localBuf)
            {
                if (strcmp(idValue, localBuf) == 0 || strcmp(idValue, "*") == 0) // allow any match with '*'
                {
                    DoXmlRemoveProp(localNodePtr, changeAttribute);
                }
            }
        }
    }

    return 0;
}

// returns zero on success
int XMLContainer::Create(const char *element, const char *idAttribute, const char *idValue)
{
    std::string tagToCreate;
    rapidxml::xml_node<char> *localNodePtr;
    unsigned int localTagIndex;
    char *localBuf;

    tagToCreate = element;

    // now look for the element
    for (localTagIndex = 0; localTagIndex < m_TagContentsList.size(); localTagIndex++)
    {
        localNodePtr = m_TagContentsList[localTagIndex];
        if (strcmp(tagToCreate.c_str(), localNodePtr->name()) == 0) // name match
        {
            localBuf = DoXmlGetProp(localNodePtr, idAttribute);
            if (localBuf)
            {
                if (strcmp(idValue, localBuf) == 0) // allow any match with '*'
                {
                    return __LINE__; // currently do not create a duplicate ID
                }
            }
        }
    }
    rapidxml::xml_node<char> *newNode = CreateNewNode(m_Doc->first_node(), element);
    if (!newNode) return __LINE__;
    DoXmlReplaceProp(newNode, idAttribute, idValue);
    m_TagContentsList.push_back(newNode);

    return 0;
}

char *XMLContainer::DoXmlGetProp(rapidxml::xml_node<char> *cur, const char *name)
{
    char *buf = nullptr;
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
    rapidxml::xml_attribute<char> *ptr = nullptr;
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
    rapidxml::xml_attribute<char> *ptr = nullptr;
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

// create a new xml node

rapidxml::xml_node<char> *XMLContainer::CreateNewNode(rapidxml::xml_node<char> *parent, const char *tag)
{
    char *allocatedTag = parent->document()->allocate_string(tag);
    rapidxml::xml_node<char> *child = parent->document()->allocate_node(rapidxml::node_element, allocatedTag);
    if (child) parent->append_node(child);
    return child;
}


#if defined(RAPIDXML_NO_EXCEPTIONS)
// this is the required rapidxml error handler when RAPIDXML_NO_EXCEPTIONS is used to disable exceptions
void rapidxml::parse_error_handler(const char *what, void *where)
{
    std::cout << "rapidxml::parse_error_handler Parse error (what) " << what << "\n";
    std::cout << "(where) " << where << "\n";
}
#endif
