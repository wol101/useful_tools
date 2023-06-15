/*
 *  AnySeg.cpp
 *  AnyBodyReader
 *
 *  Created by Bill Sellers on 09/10/2007.
 *  Copyright 2007 Bill Sellers. All rights reserved.
 *
 */

#include "AnySeg.h"
#include "AnyVar.h"
#include "AnyVec.h"
#include "AnyMat.h"
#include "AnyRefNode.h"
#include "AnyDrawSurf.h"

extern int gDebug;

AnySeg::AnySeg(std::string type, std::string name, AnyFolder *parent): AnyFolder(type, name, parent)
{
    unsigned int index = 0;
    AnyFolder *anyFolder;
    std::vector<std::string> tokens;
    
    anyFolder = new AnyVar("AnyVar", "Mass", this);
    Tokenizer("Mass=0;", tokens);
    anyFolder->ParseTokens(tokens, index);
    AddChild(anyFolder);
    
    tokens.clear();
    index = 0;
    anyFolder = new AnyVec("AnyVec", "Jii", this);
    Tokenizer("Jii={0,0,0};", tokens);
    anyFolder->ParseTokens(tokens, index);
    AddChild(anyFolder);
    
    tokens.clear();
    index = 0;
    anyFolder = new AnyVec("AnyVec", "r0", this);
    Tokenizer("r0={0,0,0};", tokens);
    anyFolder->ParseTokens(tokens, index);
    AddChild(anyFolder);
    
    tokens.clear();
    index = 0;
    anyFolder = new AnyVec("AnyVec", "rDot0", this);
    Tokenizer("rDot0={0,0,0};", tokens);
    anyFolder->ParseTokens(tokens, index);
    AddChild(anyFolder);
    
    tokens.clear();
    index = 0;
    anyFolder = new AnyVec("AnyVec", "omega0", this);
    Tokenizer("omega0={0,0,0};", tokens);
    anyFolder->ParseTokens(tokens, index);
    AddChild(anyFolder);
    
    // note Jij is Ixy,Ixz,Iyz
    tokens.clear();
    index = 0;
    anyFolder = new AnyVec("AnyVec", "Jij", this);
    Tokenizer("Jij={0,0,0};", tokens);
    anyFolder->ParseTokens(tokens, index);
    AddChild(anyFolder);
    
    tokens.clear();
    index = 0;
    anyFolder = new AnyVec("AnyVec", "sCoM", this);
    Tokenizer("sCoM={0,0,0};", tokens);
    anyFolder->ParseTokens(tokens, index);
    AddChild(anyFolder);
    
    tokens.clear();
    index = 0;
    anyFolder = new AnyMat("AnyMat", "Axes0", this);
    Tokenizer("Axes0={{1,0,0},{0,1,0},{0,0,1}};", tokens);
    anyFolder->ParseTokens(tokens, index);
    AddChild(anyFolder);
    
}

// calculate the MoI around the centre of mass
// note matrix uses Mathematica convention
// {{Ixx, Ixy, Izx}, 
//  {Ixy, Iyy, Iyz}, 
//  {Izx, Iyz, Izz}} 
pgd::Matrix3x3 AnySeg::GetMoIAroundCM()
{
    pgd::Vector jii;
    pgd::Vector jij;
    pgd::Vector com;
    double mass;
    
    GetProperty("Jii", &jii);
    GetProperty("Jij", &jij);
    GetProperty("sCoM", &com);
    GetProperty("Mass", &mass);

#ifdef MOI_NOT_AROUND_CM
    double ixxp = jii.x;
    double iyyp = jii.y;
    double izzp = jii.z;
    double ixyp = jij.x;
    double izxp = jij.y;
    double iyzp = jij.z;
    double x = com.x;
    double y = com.y;
    double z = com.z;
    
    double ixx = ixxp - mass*y*y - mass*z*z;
    double iyy = iyyp - mass*x*x - mass*z*z;
    double izz = izzp - mass*x*x - mass*y*y;
    double ixy = ixyp + mass*x*y;
    double iyz = iyzp + mass*y*z;
    double izx = izxp + mass*x*z;
#else
    double ixx = jii.x;
    double iyy = jii.y;
    double izz = jii.z;
    double ixy = jij.x;
    double izx = jij.y;
    double iyz = jij.z;
#endif
    
    pgd::Matrix3x3 moi;
    moi.e11 = ixx;
    moi.e12 = ixy;
    moi.e13 = izx;
    moi.e21 = ixy;
    moi.e22 = iyy;
    moi.e23 = iyz;
    moi.e31 = izx;
    moi.e32 = iyz;
    moi.e33 = izz;
    return moi;
}

// returns CM position in world coordinates
pgd::Vector AnySeg::GetCMPosition()
{
    pgd::Vector pos;
    pgd::Vector com;
    pgd::Matrix3x3 rot;
    
    GetProperty("r0", &pos);
    GetProperty("sCoM", &com);
    GetProperty("Axes0", &rot);
    
    pgd::Vector offset = rot * com;
    pgd::Vector position = pos + offset;
    
    return position;
}

// returns mass
double AnySeg::GetMass()
{
    double mass;
    GetProperty("Mass", &mass);
    return mass;
}

// returns orientation
pgd::Matrix3x3 AnySeg::GetGlobalRotationMatrix()
{
    pgd::Matrix3x3 rot;
    GetProperty("Axes0", &rot);
    return rot;
}

// returns position
pgd::Vector AnySeg::GetGlobalPosition()
{
    pgd::Vector vec;
    GetProperty("r0", &vec);
    return vec;
}

AnyDrawSurf *AnySeg::GetAnyDrawSurf()
{
    std::vector<AnyFolder *> list;
    GetAllChildren(&list);
    AnyDrawSurf *anyDrawSurf;
    for (unsigned int i = 0; i < list.size(); i++) 
    {
        anyDrawSurf = dynamic_cast<AnyDrawSurf *>(list[i]);
        if (anyDrawSurf) return anyDrawSurf;
    }
    return 0;
}


