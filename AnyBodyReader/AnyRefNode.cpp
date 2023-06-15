/*
 *  AnyRefNode.cpp
 *  AnyBodyReader
 *
 *  Created by Bill Sellers on 10/10/2007.
 *  Copyright 2007 Bill Sellers. All rights reserved.
 *
 */

#include "AnyRefNode.h"

#include "AnyVar.h"
#include "AnyVec.h"
#include "AnyMat.h"
#include "AnySeg.h"

extern int gDebug;

AnyRefNode::AnyRefNode(std::string type, std::string name, AnyFolder *parent): AnyFolder(type, name, parent)
{
    unsigned int index = 0;
    AnyFolder *anyFolder;
    std::vector<std::string> tokens;
    
    index = 0;
    anyFolder = new AnyVec("AnyVec", "sRel", this);
    Tokenizer("sRel={0,0,0};", tokens);
    anyFolder->ParseTokens(tokens, index);
    AddChild(anyFolder);
    
    tokens.clear();
    index = 0;
    anyFolder = new AnyMat("AnyMat", "ARel", this);
    Tokenizer("ARel={{1,0,0},{0,1,0},{0,0,1}};", tokens);
    anyFolder->ParseTokens(tokens, index);
    AddChild(anyFolder);
    
}

AnySeg *AnyRefNode::GetParentSeg()
{
    std::vector<AnyFolder *> list;
    GetAllParents(&list);
    AnySeg *parent;
    for (unsigned int i = 0; i < list.size(); i++)
    {
        parent = dynamic_cast<AnySeg *>(list[i]);
        if (parent) return parent;
    }
    return 0;
}

pgd::Vector AnyRefNode::GetGlobalPosition()
{
    if (dynamic_cast<AnySeg *>(m_Parent) == 0 && dynamic_cast<AnyRefNode *>(m_Parent) == 0)
    {
        std::cerr << __FILE__ << " " << __LINE__ << " Error invalid parent in " << m_Name << "\n";
        return pgd::Vector(0, 0, 0);
    }

    pgd::Vector pos = m_Parent->GetGlobalPosition();
    pgd::Matrix3x3 rot = m_Parent->GetGlobalRotationMatrix();
    
    pgd::Vector offset;
    GetProperty("sRel", &offset);
    
    pgd::Vector rotatedOffset = rot * offset;
    pgd::Vector globalPosition = pos + rotatedOffset;
    return globalPosition;
}

pgd::Matrix3x3 AnyRefNode::GetGlobalRotationMatrix()
{
    if (dynamic_cast<AnySeg *>(m_Parent) == 0 && dynamic_cast<AnyRefNode *>(m_Parent) == 0)
    {
        std::cerr << __FILE__ << " " << __LINE__ << " Error invalid parent in " << m_Name << "\n";
        return pgd::Matrix3x3(1, 0, 0, 0, 1, 0, 0, 0, 1);
    }
    
    pgd::Matrix3x3 parentGlobalRotation = m_Parent->GetGlobalRotationMatrix();
    
    pgd::Matrix3x3 myRotationMatrix;
    GetProperty("ARel", &myRotationMatrix);
    
    pgd::Matrix3x3 globalRotation = myRotationMatrix * parentGlobalRotation;
    return globalRotation;
}

