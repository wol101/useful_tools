/*
 *  AnyMuscleModel.cpp
 *  AnyBodyReader
 *
 *  Created by Bill Sellers on 03/11/2007.
 *  Copyright 2007 Bill Sellers. All rights reserved.
 *
 */

#include "AnyMuscleModel.h"

#include "AnyVar.h"
#include "AnyVec.h"
#include "AnyMat.h"
#include "AnySeg.h"

extern int gDebug;

AnyMuscleModel::AnyMuscleModel(std::string type, std::string name, AnyFolder *parent): AnyFolder(type, name, parent)
{
    unsigned int index = 0;
    AnyFolder *anyFolder;
    std::vector<std::string> tokens;
    
    index = 0;
    anyFolder = new AnyVar("AnyVar", "F0", this);
    Tokenizer("F0=0;", tokens);
    anyFolder->ParseTokens(tokens, index);
    AddChild(anyFolder);
    
    tokens.clear();
    index = 0;
    anyFolder = new AnyVar("AnyVar", "Lfbar", this);
    Tokenizer("Lfbar=0;", tokens);
    anyFolder->ParseTokens(tokens, index);
    AddChild(anyFolder);
    
    tokens.clear();
    index = 0;
    anyFolder = new AnyVar("AnyVar", "Gammabar", this);
    Tokenizer("Gammabar=0;", tokens);
    anyFolder->ParseTokens(tokens, index);
    AddChild(anyFolder);
    
    tokens.clear();
    index = 0;
    anyFolder = new AnyVar("AnyVar", "Epsilonbar", this);
    Tokenizer("Epsilonbar=0;", tokens);
    anyFolder->ParseTokens(tokens, index);
    AddChild(anyFolder);
    
    tokens.clear();
    index = 0;
    anyFolder = new AnyVar("AnyVar", "Lt0", this);
    Tokenizer("Lt0=0;", tokens);
    anyFolder->ParseTokens(tokens, index);
    AddChild(anyFolder);
    
    tokens.clear();
    index = 0;
    anyFolder = new AnyVar("AnyVar", "K1", this);
    Tokenizer("K1=0;", tokens);
    anyFolder->ParseTokens(tokens, index);
    AddChild(anyFolder);
    
    tokens.clear();
    index = 0;
    anyFolder = new AnyVar("AnyVar", "Fcfast", this);
    Tokenizer("Fcfast=0;", tokens);
    anyFolder->ParseTokens(tokens, index);
    AddChild(anyFolder);
    
    tokens.clear();
    index = 0;
    anyFolder = new AnyVar("AnyVar", "Jt", this);
    Tokenizer("Jt=0;", tokens);
    anyFolder->ParseTokens(tokens, index);
    AddChild(anyFolder);
    
    tokens.clear();
    index = 0;
    anyFolder = new AnyVar("AnyVar", "Jpe", this);
    Tokenizer("Jpe=0;", tokens);
    anyFolder->ParseTokens(tokens, index);
    AddChild(anyFolder);
    
    tokens.clear();
    index = 0;
    anyFolder = new AnyVar("AnyVar", "K2", this);
    Tokenizer("K2=0;", tokens);
    anyFolder->ParseTokens(tokens, index);
    AddChild(anyFolder);
    
    tokens.clear();
    index = 0;
    anyFolder = new AnyVar("AnyVar", "PEFactor", this);
    Tokenizer("PEFactor=0;", tokens);
    anyFolder->ParseTokens(tokens, index);
    AddChild(anyFolder);
    
}
