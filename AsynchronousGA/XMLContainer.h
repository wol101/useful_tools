/*
 *  XMLContainer.h
 *  MergeXML
 *
 *  Created by Bill Sellers on Wed Dec 17 2003.
 *  Copyright (c) 2003 Bill Sellers. All rights reserved.
 *
 */

#ifndef XMLContainer_h
#define XMLContainer_h

#include <map>
#include <string>
#include <vector>

namespace rapidxml { template<class Ch> class xml_document; }
namespace rapidxml { template<class Ch> class xml_node; }
namespace rapidxml { template<class Ch> class xml_attribute; }

namespace AsynchronousGA
{

class DataFile;

class XMLContainer
{
public:
    
    XMLContainer();
    ~XMLContainer();

    void SetName(const char *name) { m_Name = name; }
    const char *GetName() { return m_Name.c_str(); }
    
    char *DoXmlGetProp(rapidxml::xml_node<char> *cur, const char *name);
    rapidxml::xml_attribute<char> *DoXmlReplaceProp(rapidxml::xml_node<char> *cur, const char *name, const char *newValue);
    void DoXmlRemoveProp(rapidxml::xml_node<char> *cur, const char *name);
    rapidxml::xml_attribute<char> *DoXmlHasProp(rapidxml::xml_node<char> *cur, const char *name);

    int LoadXML(const char *file);
    int WriteXML(const char *file);
    int Merge(XMLContainer *sim, const char *element, const char *idAttribute, const char *mergeAttribute, double proportion, int startIndex = 0, int endIndex = -1);
    int MergeID(XMLContainer *sim, const char *element, const char *idAttribute, const char *id, const char *mergeAttribute, double proportion, int startIndex = 0, int endIndex = -1);
    int Operate(const char *operation, const char *element, const char *idAttribute, const char *idValue, const char *changeAttribute, int offset);
    int Set(const char *operation, const char *element, const char *idAttribute, const char *idValue, const char *changeAttribute, int offset);
    int Scale(const char *operation, const char *element, const char *idAttribute, const char *idValue, const char *changeAttribute, const char *referenceID);
    int Swap(const char *element, const char *idAttribute, const char *idValue, const char *changeAttribute, int offset1, int offset2);
    int Generate(const char *operation, const char *element, const char *idAttribute, const char *idValue, const char *changeAttribute);
    int Delete(const char *element, const char *idAttribute, const char *idValue, const char *changeAttribute);

    int Merge(rapidxml::xml_node<char> *node1, rapidxml::xml_node<char> *node2, double proportion, const char *name, int startIndex, int endIndex);

protected:
    void CleanExpressions(char *dataPtr);
    void RecoverExpressions(char *dataPtr);
    static void ReplaceChar(char *p1, int len, char c1, char c2);
    
    rapidxml::xml_document<char> *m_Doc;
    DataFile *m_DocFile;
    
    std::string m_RootNode;
    std::vector<rapidxml::xml_node<char> *> m_TagContentsList;
    
    std::string m_Name;
    
    bool m_CaseSensitiveXMLAttributes;
};

}

#endif

