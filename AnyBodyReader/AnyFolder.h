/*
 *  AnyFolder.h
 *  AnyBodyReader
 *
 *  Created by Bill Sellers on 06/10/2007.
 *  Copyright 2007 Bill Sellers. All rights reserved.
 *
 */

#ifndef AnyFolder_h
#define AnyFolder_h

#include <string>
#include <map>
#include <vector>
#include <iostream>
#include "PGDMath.h"

class AnyFolder
{
public:
    AnyFolder(std::string type, std::string name, AnyFolder *parent);
    virtual ~AnyFolder();

    virtual AnyFolder *ParseTokens(std::vector<std::string> &tokens, unsigned int &index);

    virtual AnyFolder *AddChild(AnyFolder *child);
    virtual int RemoveChild(AnyFolder *child);
    virtual AnyFolder *Search(std::string path, bool useAlternateTree);

    virtual void SetParent(AnyFolder *parent) { m_Parent = parent; };
    virtual AnyFolder *GetParent() { return m_Parent; };
    
    virtual std::string GetName() { return m_Name; };
    virtual void SetName(std::string name) { m_Name = name; };
    virtual std::string GetContent() { return m_Content; };
    virtual std::string GetType() { return m_Type; };
    virtual std::string GetPath();
    virtual int GetStatus() { return m_Status; };
    virtual int GetDefinitionPoint() { return m_DefinitionPoint; };
    virtual void SetDefinitionPoint(int definitionPoint) { m_DefinitionPoint = definitionPoint; };
    virtual std::string GetPathWhenCreated() { return m_PathWhenCreated; };
    virtual void SetPathWhenCreated(std::string name) { m_PathWhenCreated = name; };
    
    virtual int GetProperty(std::string name, double *value);
    virtual int GetProperty(std::string name, pgd::Vector *vector);
    virtual int GetProperty(std::string name, pgd::Matrix3x3 *matrix);
    virtual int GetProperty(std::string name, std::string *theString);
    
    virtual void GetAllChildren(std::vector<AnyFolder *> *list);
    virtual void GetAllParents(std::vector<AnyFolder *> *list);
    
    virtual void SetAlternateTree(AnyFolder *anyFolder) { m_AlternateTree = anyFolder; };
    virtual AnyFolder *GetAlternateTree() { return m_AlternateTree; };
    
    virtual pgd::Vector GetGlobalPosition() { return pgd::Vector(0, 0, 0); };
    virtual pgd::Matrix3x3 GetGlobalRotationMatrix() { return pgd::Matrix3x3(1, 0, 0, 0, 1, 0, 0, 0, 1); };
    
    static void Tokenizer(const char *buf, std::vector<std::string> &tokens);
    
    friend std::ostream& operator<<(std::ostream &out, AnyFolder &g);
    
protected:
    
    std::map<std::string, AnyFolder *> m_Children;
    std::vector<AnyFolder *> m_ChildrenList;
    AnyFolder *m_Parent;
    std::string m_Name;
    std::string m_Content;
    std::string m_Type;
    std::string m_PathWhenCreated;
    AnyFolder *m_AlternateTree;
    int m_Status;
    int m_DefinitionPoint;
};


#endif
