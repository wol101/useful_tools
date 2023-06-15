/*
 *  AnyJoint.cpp
 *  AnyBodyReader
 *
 *  Created by Bill Sellers on 18/10/2007.
 *  Copyright 2007 Bill Sellers. All rights reserved.
 *
 */

#include "AnyJoint.h"
#include "AnyString.h"
#include "AnyRefNode.h"
#include "AnyRef.h"

extern int gDebug;

AnyJoint::AnyJoint(std::string type, std::string name, AnyFolder *parent): AnyFolder(type, name, parent)
{
    unsigned int index = 0;
    AnyFolder *anyFolder;
    std::vector<std::string> tokens;
    
    anyFolder = new AnyString("AnyString", "Axis", this);
    Tokenizer("Axis=w;", tokens);
    anyFolder->ParseTokens(tokens, index);
    AddChild(anyFolder);
    
    tokens.clear();
    index = 0;
    anyFolder = new AnyString("AnyString", "Axis1", this);
    Tokenizer("Axis1=w;", tokens);
    anyFolder->ParseTokens(tokens, index);
    AddChild(anyFolder);
    
    tokens.clear();
    index = 0;
    anyFolder = new AnyString("AnyString", "Axis2", this);
    Tokenizer("Axis2=w;", tokens);
    anyFolder->ParseTokens(tokens, index);
    AddChild(anyFolder);
}

std::string AnyJoint::GetBody1ID()
{
    AnyRefNode *anyRefNode;
    AnyRef *anyRef;
    
    for (unsigned int i = 0; i < m_ChildrenList.size(); i++) 
    {
        anyRefNode = dynamic_cast<AnyRefNode *>(m_ChildrenList[i]);
        if (anyRefNode) break;
        anyRef = dynamic_cast<AnyRef *>(m_ChildrenList[i]);
        if (anyRef)
        {
            anyRefNode = dynamic_cast<AnyRefNode *>(anyRef->GetLinked(false));
            if (anyRefNode) break;
        }
    }
    if (anyRefNode == 0) return "";
    return anyRefNode->GetParent()->GetPath();
}

std::string AnyJoint::GetBody2ID()
{
    AnyRefNode *anyRefNode;
    AnyRef *anyRef;
    int refCount = 0;
    
    for (unsigned int i = 0; i < m_ChildrenList.size(); i++) 
    {
        anyRefNode = dynamic_cast<AnyRefNode *>(m_ChildrenList[i]);
        if (anyRefNode) 
        {
            refCount++;
            if (refCount == 2) break;
        }
        anyRef = dynamic_cast<AnyRef *>(m_ChildrenList[i]);
        if (anyRef)
        {
            anyRefNode = dynamic_cast<AnyRefNode *>(anyRef->GetLinked(false));
            if (anyRefNode) 
            {
                refCount++;
                if (refCount == 2) break;
            }
        }
    }
    if (anyRefNode == 0) return "";
    return anyRefNode->GetParent()->GetPath();
}

pgd::Vector AnyJoint::GetHingeAnchor()
{
    AnyRefNode *anyRefNode;
    AnyRef *anyRef;
    pgd::Vector vec(0, 0, 0);
    
    for (unsigned int i = 0; i < m_ChildrenList.size(); i++) 
    {
        anyRefNode = dynamic_cast<AnyRefNode *>(m_ChildrenList[i]);
        if (anyRefNode) break;
        anyRef = dynamic_cast<AnyRef *>(m_ChildrenList[i]);
        if (anyRef)
        {
            anyRefNode = dynamic_cast<AnyRefNode *>(anyRef->GetLinked(false));
            if (anyRefNode) break;
        }
    }
    if (anyRefNode == 0) return vec;
    vec = anyRefNode->GetGlobalPosition();
    return vec;
}

pgd::Vector AnyJoint::GetHingeAxis()
{
    AnyRefNode *anyRefNode;
    AnyRef *anyRef;
    pgd::Vector vec(0, 0, 0);
    
    for (unsigned int i = 0; i < m_ChildrenList.size(); i++) 
    {
        anyRefNode = dynamic_cast<AnyRefNode *>(m_ChildrenList[i]);
        if (anyRefNode) break;
        anyRef = dynamic_cast<AnyRef *>(m_ChildrenList[i]);
        if (anyRef)
        {
            anyRefNode = dynamic_cast<AnyRefNode *>(anyRef->GetLinked(false));
            if (anyRefNode) break;
        }
    }
    if (anyRefNode == 0) return vec;
    pgd::Quaternion q = MakeQfromM(anyRefNode->GetGlobalRotationMatrix());
    
    std::string axis;
    GetProperty("Axis", &axis);
    if (axis == "x") vec.x = 1;
    else if (axis == "y") vec.y = 1;
    else vec.z = 1;
    
    vec = QVRotate(q, vec);
    return vec;
}


// output to a stream
/*
 std::ostream& operator<<(std::ostream &out, AnyJoint &g)
{
    out << g.GetType() << " " << g.GetPath() << "\n";
    std::map<std::string, AnyFolder *>::iterator iter;
    for (iter = g.m_Children.begin(); iter != g.m_Children.end(); iter++) 
    {
        AnyString *anyString = dynamic_cast<AnyString *>(iter->second);
        if (anyString) 
        {
            if (anyString->GetString() != "w") out << *anyString << "\n"; 
        }
    }
    for (unsigned int i = 0; i < g.m_ChildrenList.size(); i++) 
    {
        if (dynamic_cast<AnyRefNode *>(g.m_ChildrenList[i])) out << *dynamic_cast<AnyRefNode *>(g.m_ChildrenList[i]) << "\n";
        else
        {
            AnyRef *anyRef = dynamic_cast<AnyRef *>(g.m_ChildrenList[i]);
            if (anyRef) 
            {
                AnyRefNode *anyRefNode = dynamic_cast<AnyRefNode *>(anyRef->GetLinked(false));
                if (anyRefNode) 
                    out << *anyRefNode << "\n"; 
            }
        }
    }
    return out;
}
*/

