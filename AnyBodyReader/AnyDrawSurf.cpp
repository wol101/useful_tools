/*
 *  AnyDrawSurf.cpp
 *  AnyBodyReader
 *
 *  Created by Bill Sellers on 10/10/2007.
 *  Copyright 2007 Bill Sellers. All rights reserved.
 *
 */

#include "AnyDrawSurf.h"

#include "AnyVar.h"
#include "AnyVec.h"
#include "AnyRef.h"
#include "AnyString.h"
#include "AnyFunTransform3D.h"

extern int gDebug;

AnyDrawSurf::AnyDrawSurf(std::string type, std::string name, AnyFolder *parent): AnyFolder(type, name, parent)
{
    unsigned int index = 0;
    AnyFolder *anyFolder;
    std::vector<std::string> tokens;
    
    index = 0;
    anyFolder = new AnyString("AnyString", "FileName", this);
    Tokenizer("FileName=\"\";", tokens);
    anyFolder->ParseTokens(tokens, index);
    AddChild(anyFolder);
    
    tokens.clear();
    index = 0;
    anyFolder = new AnyVec("AnyVec", "RGB", this);
    Tokenizer("RGB={1,1,1};", tokens);
    anyFolder->ParseTokens(tokens, index);
    AddChild(anyFolder);
    
    tokens.clear();
    index = 0;
    anyFolder = new AnyVec("AnyVec", "ScaleXYZ", this);
    Tokenizer("ScaleXYZ={1,1,1};", tokens);
    anyFolder->ParseTokens(tokens, index);
    AddChild(anyFolder);
    
    tokens.clear();
    index = 0;
    anyFolder = new AnyVar("AnyVar", "Transparency", this);
    Tokenizer("Transparency=0;", tokens);
    anyFolder->ParseTokens(tokens, index);
    AddChild(anyFolder);
    
}

AnyFunTransform3D *AnyDrawSurf::GetAnyFunTransform3D()
{
    AnyFunTransform3D *anyFunTransform3D;
    AnyRef *anyRef;
    for (unsigned int i = 0; i < m_ChildrenList.size(); i++) 
    {
        anyRef = dynamic_cast<AnyRef *>(m_ChildrenList[i]);
        if (anyRef) anyFunTransform3D = dynamic_cast<AnyFunTransform3D *>(anyRef->GetLinked(false));
        else anyFunTransform3D = dynamic_cast<AnyFunTransform3D *>(m_ChildrenList[i]);
        if (anyFunTransform3D) return anyFunTransform3D;
    }
    return 0;
}

