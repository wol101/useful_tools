/*
 *  AnyMuscle.cpp
 *  AnyBodyReader
 *
 *  Created by Bill Sellers on 18/10/2007.
 *  Copyright 2007 Bill Sellers. All rights reserved.
 *
 */

#include "AnyMuscle.h"
#include "AnyString.h"
#include "AnyRef.h"
#include "AnyRefNode.h"
#include "AnySeg.h"
#include "AnySurfCylinder.h"
#include "AnyMuscleModel.h"

extern int gDebug;

AnyMuscle::AnyMuscle(std::string type, std::string name, AnyFolder *parent): AnyFolder(type, name, parent)
{
}

std::string AnyMuscle::GetOriginID()
{
    std::vector<AnyRefNode *> list;
    GetAnyRefNodeList(&list);
    if (list.size() < 2) return "";
    AnySeg *anySeg = list[0]->GetParentSeg();
    if (anySeg) return anySeg->GetPath();
    return "";
}

std::string AnyMuscle::GetInsertionID()
{
    std::vector<AnyRefNode *> list;
    GetAnyRefNodeList(&list);
    if (list.size() < 2) return "";
    AnySeg *anySeg = list[list.size() - 1]->GetParentSeg();
    if (anySeg) return anySeg->GetPath();
    return "";
}

pgd::Vector AnyMuscle::GetOriginGlobalPosition()
{
    std::vector<AnyRefNode *> list;
    GetAnyRefNodeList(&list);
    if (list.size() < 2) return pgd::Vector(0, 0, 0);
    return list[0]->GetGlobalPosition();
}

pgd::Vector AnyMuscle::GetInsertionGlobalPosition()
{
    std::vector<AnyRefNode *> list;
    GetAnyRefNodeList(&list);
    if (list.size() < 2) return pgd::Vector(0, 0, 0);
    return list[list.size() - 1]->GetGlobalPosition();
}

unsigned int AnyMuscle::GetNumNodes()
{
    std::vector<AnyRefNode *> list;
    GetAnyRefNodeList(&list);
    return list.size();
}

std::string AnyMuscle::GetNodeID(unsigned int n)
{
    std::vector<AnyRefNode *> list;
    GetAnyRefNodeList(&list);
    if (list.size() <= n) return "";
    AnySeg *anySeg = list[n]->GetParentSeg();
    if (anySeg) return anySeg->GetPath();
    return "";
}

pgd::Vector AnyMuscle::GetOriginGlobalPosition(unsigned int n)
{
    std::vector<AnyRefNode *> list;
    GetAnyRefNodeList(&list);
    if (list.size() <= n) return pgd::Vector(0, 0, 0);
    return list[n]->GetGlobalPosition();
}

void AnyMuscle::GetAnyRefNodeList(std::vector<AnyRefNode *> *list)
{
    for (unsigned int i = 0; i < m_ChildrenList.size(); i++)
    {
        AnyRef *anyRef = dynamic_cast<AnyRef *>(m_ChildrenList[i]);
        if (anyRef) 
        {
            //std::cerr << anyRef->GetPath() << "\n";
            AnyRefNode *anyRefNode = dynamic_cast<AnyRefNode *>(anyRef->GetLinked(false));
            if (anyRefNode) list->push_back(anyRefNode);
        }
        else
        {
            AnyRefNode *anyRefNode = dynamic_cast<AnyRefNode *>(m_ChildrenList[i]);
            if (anyRefNode) list->push_back(anyRefNode);
        }
    }
}

void AnyMuscle::GetAnySurfCylinderList(std::vector<AnySurfCylinder *> *list)
{
    for (unsigned int i = 0; i < m_ChildrenList.size(); i++)
    {
        AnyRef *anyRef = dynamic_cast<AnyRef *>(m_ChildrenList[i]);
        if (anyRef) 
        {
            //std::cerr << anyRef->GetPath() << "\n";
            AnySurfCylinder *anySurfCylinder = dynamic_cast<AnySurfCylinder *>(anyRef->GetLinked(false));
            if (anySurfCylinder) list->push_back(anySurfCylinder);
        }
        else
        {
            AnySurfCylinder *anySurfCylinder = dynamic_cast<AnySurfCylinder *>(m_ChildrenList[i]);
            if (anySurfCylinder) list->push_back(anySurfCylinder);
        }
    }
}

AnyMuscleModel *AnyMuscle::GetAnyMuscleModel()
{
    for (unsigned int i = 0; i < m_ChildrenList.size(); i++)
    {
        AnyRef *anyRef = dynamic_cast<AnyRef *>(m_ChildrenList[i]);
        if (anyRef) 
        {
            //std::cerr << anyRef->GetPath() << "\n";
            AnyMuscleModel *anyMuscleModel = dynamic_cast<AnyMuscleModel *>(anyRef->GetLinked(false));
            if (anyMuscleModel) return anyMuscleModel;
        }
        else
        {
            AnyMuscleModel *anyMuscleModel = dynamic_cast<AnyMuscleModel *>(m_ChildrenList[i]);
            if (anyMuscleModel) return anyMuscleModel;
        }
    }
    return 0;
}

/*
std::ostream& operator<<(std::ostream &out, AnyMuscle &g)
{
    out << g.GetType() << " " << g.GetPath() << "\n";
    for (unsigned int i = 0; i < g.m_ChildrenList.size(); i++)
    {
        AnyRef *anyRef = dynamic_cast<AnyRef *>(g.m_ChildrenList[i]);
        if (anyRef) 
        {
            AnyRefNode *anyRefNode = dynamic_cast<AnyRefNode *>(anyRef->GetLinked(false));
            if (anyRefNode) 
                out << i << " " << *anyRefNode << "\n"; 
        }
        else
        {
            AnyRefNode *anyRefNode = dynamic_cast<AnyRefNode *>(g.m_ChildrenList[i]);
            if (anyRefNode) 
                out << i << " " << *anyRefNode << "\n"; 
        }
    }
    
    return out;
}
*/
