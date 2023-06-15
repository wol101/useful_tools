/*
 *  AnyFolder.cpp
 *  AnyBodyReader
 *
 *  Created by Bill Sellers on 06/10/2007.
 *  Copyright 2007 Bill Sellers. All rights reserved.
 *
 */

#include <iostream>

#include "AnyFolder.h"
#include "AnyVar.h"
#include "AnyVec.h"
#include "AnyMat.h"
#include "AnyRef.h"
#include "AnyFunTransform3D.h"
#include "AnySeg.h"
#include "AnyRefNode.h"
#include "AnyString.h"
#include "AnyDrawSurf.h"
#include "AnyJoint.h"
#include "AnyMuscle.h"
#include "AnySurfCylinder.h"
#include "AnyMuscleModel.h"

extern int gDebug;

AnyFolder::AnyFolder(std::string type, std::string name, AnyFolder *parent)
{
    m_Type = type;
    m_Name = name;
    m_Parent = parent;
    if (parent) m_AlternateTree = parent->GetAlternateTree();
    else m_AlternateTree = 0;
    m_Status = 0;
    m_DefinitionPoint = -1;
    
    if (gDebug > 0)
    {
        if (m_Parent)
            std::cerr << "Creating AnyFolder " << m_Name << " Parent " << m_Parent->GetPath() << "\n";
        else
            std::cerr << "Creating AnyFolder " << m_Name << " Parent " << 0 << "\n";
    }          
}

AnyFolder::~AnyFolder()
{
    std::map<std::string, AnyFolder *>::iterator iter;
    for (iter = m_Children.begin(); iter != m_Children.end(); iter++) delete iter->second;
}

AnyFolder *AnyFolder::AddChild(AnyFolder *child)
{
    if (m_Children.find(child->GetName()) != m_Children.end())
    {
        std::cerr << __FILE__ << " " << __LINE__ << " Warning " <<  child->GetName() << " already exists in " << m_Name << "\n";
        return 0;
    }
    
    m_Children[child->GetName()] = child;
    m_ChildrenList.push_back(child);
    child->SetPathWhenCreated(child->GetPath());
    return child;
}

AnyFolder *AnyFolder::Search(std::string path, bool useAlternateTree)
{
//#define TRYALTERNATIVEFIRST
#ifdef TRYALTERNATIVEFIRST
    if (m_AlternateTree && useAlternateTree) 
    {
        AnyFolder *alternativeResult = 0;
        AnyFolder *mainResult = 0;
        std::string newPath = GetPathWhenCreated() + "." + path;
        alternativeResult = m_AlternateTree->Search(newPath, false);
        mainResult = Search(path, false);
        if (alternativeResult == 0) return mainResult;
        if (mainResult == 0) return alternativeResult;
        if (alternativeResult->GetDefinitionPoint() > mainResult->GetDefinitionPoint()) return alternativeResult;
        return mainResult;
    }
#endif
    
    std::map<std::string, AnyFolder *>::iterator iter;
    
    if (path[0] == '.')
    {
        if (m_Parent == 0)
        {
            std::cerr << __FILE__ << " " << __LINE__ << " Warning " << "Reached root folder without finding " << path << "\n";
            return 0;
        }
        std::string newPath = path.substr(1);
        return m_Parent->Search(newPath, useAlternateTree);
    }
    
    unsigned int pos = path.find(".");
    if (pos == std::string::npos)
    {
        if (path == "Main")
        {
            if (m_Parent == 0) return this;
            else return m_Parent->Search(path, useAlternateTree);
        }
        iter = m_Children.find(path);
        if (iter == m_Children.end())
        {
#ifndef TRYALTERNATIVEFIRST
            if (m_AlternateTree && useAlternateTree) 
            {
                std::string newPath = GetPath() + "." + path;
                AnyFolder *r = m_AlternateTree->Search(newPath, useAlternateTree);
                // if (r) std::cerr << newPath << " found in alternate tree\n";
                return r;
            }
#endif
            std::cerr << __FILE__ << " " << __LINE__ << " Warning " << "Could not find " << path << " in " << m_Name << "\n";
            if (gDebug > 3)
                std::cerr << __FILE__ << " " << __LINE__ << " " << *this << "\n";
            return 0;
        }
        //if (dynamic_cast<AnyRef *>(iter->second))
        //{
        //    std::string ref = dynamic_cast<AnyRef *>(iter->second)->GetPath();
        //    return Search(ref);
        //}
        return iter->second;
    }
    
    std::string parent = path.substr(0, pos);
    std::string child = path.substr(pos + 1);
    if (parent == "Main")
    {
        if (m_Parent == 0) return Search(child, useAlternateTree);
        else return m_Parent->Search(path, useAlternateTree);
    }
    iter = m_Children.find(parent);
    if (iter == m_Children.end())
    {
#ifndef TRYALTERNATIVEFIRST
        if (m_AlternateTree && useAlternateTree)            
        {
            std::string newPath = GetPath() + "." + path;
            AnyFolder *r = m_AlternateTree->Search(newPath, useAlternateTree);
            // if (r) std::cerr << newPath << " found in alternate tree\n";
            return r;
        } 
#endif        
        std::cerr << __FILE__ << " " << __LINE__ << " Warning " << "Could not find " << path << " in " << m_Name << "\n";
        if (gDebug > 3)
            std::cerr << __FILE__ << " " << __LINE__ << " " << *this << "\n";
        return 0;
    }
    return iter->second->Search(child, useAlternateTree);
}

std::string AnyFolder::GetPath()
{
    if (m_Parent == 0) return m_Name;
    else return m_Parent->GetPath() + "." + m_Name;
}

AnyFolder *AnyFolder::ParseTokens(std::vector<std::string> &tokens, unsigned int &index)
{
    if (m_Name == "ModelEnvironmentConnection")
        std::cerr << GetPath() << "\n";
    
    AnyFolder *anyFolderPtr;
    index++;
    if (tokens[index] != "=") 
    {
        std::cerr << __FILE__ << " " << __LINE__ << " " << "Error Expected = in " << m_Name << "\n";
        return 0;
    }
    index++;
    if (tokens[index] == "{") 
    {
        index++;
        
        while (tokens[index] != "}")
        {
            if (tokens[index + 1] == "&")
            {
                index++;
                AnyRef *anyRef = new AnyRef("AnyRef", tokens[index + 1], this);
                if (anyRef)
                {
                    AddChild(anyRef);
                    anyRef->SetDefinitionPoint(index);
                    if ((anyRef->ParseTokens(tokens, index)) == 0)
                        RemoveChild(anyRef);
                }
            }
            else if (tokens[index] == "AnyVar" || tokens[index] == "AnyInt" || tokens[index] == "AnyFloat")
            {
                index++;
                AnyVar *anyVar = new AnyVar(tokens[index - 1], tokens[index], this);
                if (anyVar)
                {
                    AddChild(anyVar);
                    anyVar->SetDefinitionPoint(index);
                    if ((anyVar->ParseTokens(tokens, index)) == 0)
                        RemoveChild(anyVar);
                }
            }
            else if (tokens[index] == "AnyVec3")
            {
                index++;
                AnyVec *anyVec = new AnyVec(tokens[index - 1], tokens[index], this);
                if (anyVec)
                {
                    AddChild(anyVec);
                    anyVec->SetDefinitionPoint(index);
                    if ((anyVec->ParseTokens(tokens, index)) == 0)
                        RemoveChild(anyVec);
                }
            }
            else if (tokens[index] == "AnyMat33" || tokens[index] == "AnyMatrix")
            {
                index++;
                AnyMat *anyMat = new AnyMat(tokens[index - 1], tokens[index], this);
                if (anyMat)
                {
                    AddChild(anyMat);
                    anyMat->SetDefinitionPoint(index);
                    if ((anyMat->ParseTokens(tokens, index)) == 0)
                        RemoveChild(anyMat);
                }
            }
            else if (tokens[index] == "AnyFunTransform3DIdentity" || tokens[index] == "AnyFunTransform3DLin" || tokens[index] == "AnyFunTransform3D")
            {
                index++;
                AnyFunTransform3D *anyFunTransform3D = new AnyFunTransform3D(tokens[index - 1], tokens[index], this);
                if (anyFunTransform3D)
                {
                    AddChild(anyFunTransform3D);
                    anyFunTransform3D->SetDefinitionPoint(index);
                    if ((anyFunTransform3D->ParseTokens(tokens, index)) == 0)
                        RemoveChild(anyFunTransform3D);
                }
            }
            else if (tokens[index] == "AnySeg")
            {
                index++;
                AnySeg *anySeg = new AnySeg(tokens[index - 1], tokens[index], this);
                if (anySeg)
                {
                    AddChild(anySeg);
                    anySeg->SetDefinitionPoint(index);
                    if ((anySeg->ParseTokens(tokens, index)) == 0)
                        RemoveChild(anySeg);
                }
            }
            else if (tokens[index] == "AnyFolder" || tokens[index] == "AnyKinMeasureOrg")
            {
                index++;
                AnyFolder *anyFolder = new AnyFolder(tokens[index - 1], tokens[index], this);
                if (anyFolder)
                {
                    AddChild(anyFolder);
                    anyFolder->SetDefinitionPoint(index);
                    if ((anyFolder->ParseTokens(tokens, index)) == 0)
                        RemoveChild(anyFolder);
                }
            }
            else if (tokens[index] == "AnyRefNode" || tokens[index] == "AnyRefFrame")
            {
                index++;
                AnyRefNode *anyRefNode = new AnyRefNode(tokens[index - 1], tokens[index], this);
                if (anyRefNode)
                {
                    AddChild(anyRefNode);
                    anyRefNode->SetDefinitionPoint(index);
                    if ((anyRefNode->ParseTokens(tokens, index)) == 0)
                        RemoveChild(anyRefNode);
                }
            }
            else if (tokens[index] == "AnyString" || tokens[index] == "AnySurfFile")
            {
                index++;
                AnyString *anyString = new AnyString(tokens[index - 1], tokens[index], this);
                if (anyString)
                {
                    AddChild(anyString);
                    anyString->SetDefinitionPoint(index);
                    if ((anyString->ParseTokens(tokens, index)) == 0)
                        RemoveChild(anyString);
                }
            }
            else if (tokens[index] == "AnyDrawSurf")
            {
                index++;
                AnyDrawSurf *anyDrawSurf = new AnyDrawSurf(tokens[index - 1], tokens[index], this);
                if (anyDrawSurf)
                {
                    AddChild(anyDrawSurf);
                    anyDrawSurf->SetDefinitionPoint(index);
                    if ((anyDrawSurf->ParseTokens(tokens, index)) == 0)
                        RemoveChild(anyDrawSurf);
                }
            }
            else if (tokens[index] == "AnyJoint" || tokens[index] == "AnySphericalJoint" || tokens[index] == "AnyRevoluteJoint" || tokens[index] == "AnyUniversalJoint")
            {
                index++;
                AnyJoint *anyJoint = new AnyJoint(tokens[index - 1], tokens[index], this);
                if (anyJoint)
                {
                    AddChild(anyJoint);
                    anyJoint->SetDefinitionPoint(index);
                    if ((anyJoint->ParseTokens(tokens, index)) == 0)
                        RemoveChild(anyJoint);
                }
            }
            else if (tokens[index] == "AnyMuscle" || tokens[index] == "AnyViaPointMuscle" || tokens[index] == "AnyShortestPathMuscle")
            {
                index++;
                AnyMuscle *anyMuscle = new AnyMuscle(tokens[index - 1], tokens[index], this);
                if (anyMuscle)
                {
                    AddChild(anyMuscle);
                    anyMuscle->SetDefinitionPoint(index);
                    if ((anyMuscle->ParseTokens(tokens, index)) == 0)
                        RemoveChild(anyMuscle);
                }
            }
            else if (tokens[index] == "AnySurfCylinder")
            {
                index++;
                AnySurfCylinder *anySurfCylinder = new AnySurfCylinder(tokens[index - 1], tokens[index], this);
                if (anySurfCylinder)
                {
                    AddChild(anySurfCylinder);
                    anySurfCylinder->SetDefinitionPoint(index);
                    if ((anySurfCylinder->ParseTokens(tokens, index)) == 0)
                        RemoveChild(anySurfCylinder);
                }
            }
            else if (tokens[index] == "AnyMuscleModel" || tokens[index] == "AnyMuscleModel3E")
            {
                index++;
                AnyMuscleModel *anyMuscleModel = new AnyMuscleModel(tokens[index - 1], tokens[index], this);
                if (anyMuscleModel)
                {
                    AddChild(anyMuscleModel);
                    anyMuscleModel->SetDefinitionPoint(index);
                    if ((anyMuscleModel->ParseTokens(tokens, index)) == 0)
                        RemoveChild(anyMuscleModel);
                }
            }
            
            else if (tokens[index].substr(0, 3) == "Any") 
            {
                std::cerr << __FILE__ << " " << __LINE__ << " Warning " << tokens[index] << " unrecognised" << "\n";
                index++;
                int braceCount = 0;
                std::string content;
                while (tokens[index] != ";" || braceCount != 0)
                {
                    if (tokens[index] == "{") braceCount++;
                    else if (tokens[index] == "}") braceCount--;
                    m_Content += tokens[index];
                    index++;
                }
                std::cerr << __FILE__ << " " << __LINE__ << " Warning ignoring" << "\n";
                std::cerr << content << "\n";
                index++;
            }
            
            else if ((anyFolderPtr = Search(tokens[index], false)))
            {
                anyFolderPtr->ParseTokens(tokens, index);
            }
            else
            {
                std::cerr << __FILE__ << " " << __LINE__ << " Warning " << tokens[index] << " not found in " << m_Name << "\n";
                index++;
                int braceCount = 0;
                std::string content;
                while (tokens[index] != ";" || braceCount != 0)
                {
                    if (tokens[index] == "{") braceCount++;
                    else if (tokens[index] == "}") braceCount--;
                    content += tokens[index];
                    index++;
                }
                std::cerr << __FILE__ << " " << __LINE__ << " Warning ignoring" << "\n";
                std::cerr << content << "\n";
                index++;
            }
            
            if (index >= tokens.size())
            {
                std::cerr << __FILE__ << " " << __LINE__ << " Error " << "Expected } in " << m_Name << "\n";
                return 0;
            }
        }
        index++;
    }
    else
    {
        while (tokens[index] != ";")
        {
            m_Content += tokens[index];
            index++;
            if (index >= tokens.size())
            {
                std::cerr << __FILE__ << " " << __LINE__ << " Error " << "Expected } in " << m_Name << "\n";
                return 0;
            }
        }
    }
    
    if (tokens[index] != ";") 
    {
        std::cerr << __FILE__ << " " << __LINE__ << " Error " << "Expected ; in " << m_Name << "\n";
        return 0;
    }
    index++;
    
    return this;
}


void AnyFolder::Tokenizer(const char *constbuf, std::vector<std::string> &tokens)
{
    char *ptr = (char *)malloc(strlen(constbuf) + 1);
    strcpy(ptr, constbuf);
    char *qp;
    char byte;
    char *stopList = "{};,=:&()";
    char oneChar[2] = {0, 0};
    
    while (*ptr)
    {
        // find non-whitespace
        if (*ptr < 33)
        {
            ptr++;
            continue;
        }
        
        // is it in stoplist
        if (strchr(stopList, *ptr))
        {
            oneChar[0] = *ptr;
            tokens.push_back(oneChar);
            ptr++;
            continue;
        }
        
        // is it a double quote?
        if (*ptr == '"')
        {
            ptr++;
            qp = strchr(ptr, '"');
            if (qp)
            {
                *qp = 0;
                tokens.push_back(ptr);
                *qp = '"';
                ptr = qp + 1;
                continue;
            }
        }
        
        qp = ptr;
        while (*qp >= 33 && strchr(stopList, *qp) == 0 && *qp != '"') 
        {
            qp++;
        }
        byte = *qp;
        *qp = 0;
        tokens.push_back(ptr);
        if (byte == 0) break;
        *qp = byte;
        ptr = qp;
    }
}

int AnyFolder::RemoveChild(AnyFolder *child)
{
    std::map<std::string, AnyFolder *>::iterator iter;
    for (iter = m_Children.begin(); iter != m_Children.end(); iter++) 
    {
        if (iter->second == child)
        {
            m_Children.erase(iter);
            delete child;
            return 0;
        }
    }
    return __LINE__;
}

void AnyFolder::GetAllChildren(std::vector<AnyFolder *> *list)
{
    std::map<std::string, AnyFolder *>::iterator iter;
    for (iter = m_Children.begin(); iter != m_Children.end(); iter++) 
    {
        list->push_back(iter->second);
        iter->second->GetAllChildren(list);
    }
}

void AnyFolder::GetAllParents(std::vector<AnyFolder *> *list)
{
    if (m_Parent == 0) return;
    list->push_back(m_Parent);
    m_Parent->GetAllParents(list);
}

// output to a stream
std::ostream& operator<<(std::ostream &out, AnyFolder &g)
{
    out << g.GetPath() << "\n";
    std::map<std::string, AnyFolder *>::iterator iter;
    for (iter = g.m_Children.begin(); iter != g.m_Children.end(); iter++) 
    {
        if (dynamic_cast<AnyMat *>(iter->second)) out << *dynamic_cast<AnyMat *>(iter->second) << "\n"; 
        else if (dynamic_cast<AnyVec *>(iter->second)) out << *dynamic_cast<AnyVec *>(iter->second) << "\n"; 
        else if (dynamic_cast<AnyVar *>(iter->second)) out << *dynamic_cast<AnyVar *>(iter->second) << "\n"; 
        else if (dynamic_cast<AnyString *>(iter->second)) out << *dynamic_cast<AnyString *>(iter->second) << "\n"; 
        else if (dynamic_cast<AnyRef *>(iter->second)) out << *dynamic_cast<AnyRef *>(iter->second) << "\n"; 
        else out << *iter->second << "\n";
    }
    return out;
}

// get the value of a contained item. Returns non zero on error
int AnyFolder::GetProperty(std::string name, double *value)
{
    AnyVar *anyVar = dynamic_cast<AnyVar *>(Search(name, false));
    if (anyVar == 0) return __LINE__;
    *value = anyVar->GetValue();
    return 0;
}

int AnyFolder::GetProperty(std::string name, pgd::Vector *vector)
{
    AnyVec *anyVec = dynamic_cast<AnyVec *>(Search(name, false));
    if (anyVec == 0) return __LINE__;
    *vector = anyVec->GetVector();
    return 0;
}

int AnyFolder::GetProperty(std::string name, pgd::Matrix3x3 *matrix)
{
    AnyMat *anyMat = dynamic_cast<AnyMat *>(Search(name, false));
    if (anyMat == 0) return __LINE__;
    *matrix = anyMat->GetMatrix3x3();
    return 0;
}

int AnyFolder::GetProperty(std::string name, std::string *theString )
{
    AnyString *anyString = dynamic_cast<AnyString *>(Search(name, false));
    if (anyString == 0) return __LINE__;
    *theString = anyString->GetString();
    return 0;
}

