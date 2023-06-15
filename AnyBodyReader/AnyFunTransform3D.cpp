/*
 *  AnyFunTransform3D.cpp
 *  AnyBodyReader
 *
 *  Created by Bill Sellers on 09/10/2007.
 *  Copyright 2007 Bill Sellers. All rights reserved.
 *
 */

#include <iostream>
#include "AnyFunTransform3D.h"
#include "AnyMat.h"
#include "AnyVec.h"

extern int gDebug;

AnyFunTransform3D::AnyFunTransform3D(std::string type, std::string name, AnyFolder *parent): AnyFolder(type, name, parent)
{
    unsigned int index = 0;
    AnyFolder *anyFolder;
    std::vector<std::string> tokens;
    
    anyFolder = new AnyMat("AnyMat", "ScaleMat", this);
    Tokenizer("ScaleMat={{1,0,0},{0,1,0},{0,0,1}};", tokens);
    anyFolder->ParseTokens(tokens, index);
    AddChild(anyFolder);
    
    tokens.clear();
    index = 0;
    anyFolder = new AnyVec("AnyVec", "Offset", this);
    Tokenizer("Offset={0,0,0};", tokens);
    anyFolder->ParseTokens(tokens, index);
    AddChild(anyFolder);
    
    
}

int AnyFunTransform3D::Calculate(pgd::Vector *vector)
{
    AnyVec *offset = dynamic_cast<AnyVec *>(m_Children["Offset"]);
    if (offset == 0) return __LINE__;
    if (offset->GetStatus() != 0) return __LINE__;
    AnyMat *scalemat = dynamic_cast<AnyMat *>(m_Children["ScaleMat"]);
    if (scalemat == 0) return __LINE__;
    if (scalemat->GetStatus() != 0) return __LINE__;

    pgd::Vector offsetVector = *vector - offset->GetVector();
    pgd::Vector scaledVector = scalemat->GetMatrix3x3() * offsetVector;
    *vector = scaledVector + offset->GetVector();
    return 0;
}

