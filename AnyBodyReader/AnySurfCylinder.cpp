/*
 *  AnySurfCylinder.cpp
 *  AnyBodyReader
 *
 *  Created by Bill Sellers on 31/10/2007.
 *  Copyright 2007 Bill Sellers. All rights reserved.
 *
 */

#include "AnySurfCylinder.h"

#include "AnyVar.h"
#include "AnyVec.h"
#include "AnyMat.h"
#include "AnySeg.h"

extern int gDebug;

AnySurfCylinder::AnySurfCylinder(std::string type, std::string name, AnyFolder *parent): AnyFolder(type, name, parent)
{
    unsigned int index = 0;
    AnyFolder *anyFolder;
    std::vector<std::string> tokens;
    
    index = 0;
    anyFolder = new AnyVar("AnyVar", "Radius", this);
    Tokenizer("Radius=0;", tokens);
    anyFolder->ParseTokens(tokens, index);
    AddChild(anyFolder);
    
    tokens.clear();
    index = 0;
    anyFolder = new AnyVar("AnyVar", "Length", this);
    Tokenizer("Length=0;", tokens);
    anyFolder->ParseTokens(tokens, index);
    AddChild(anyFolder);
    
}
