/*
 *  AnySeg.h
 *  AnyBodyReader
 *
 *  Created by Bill Sellers on 09/10/2007.
 *  Copyright 2007 Bill Sellers. All rights reserved.
 *
 */

#include "AnyFolder.h"

class AnyDrawSurf;

class AnySeg: public AnyFolder
{
public:
    
    AnySeg(std::string type, std::string name, AnyFolder *parent);
    
    virtual pgd::Matrix3x3 GetMoIAroundCM();
    virtual pgd::Vector GetCMPosition();
    virtual pgd::Vector GetGlobalPosition();
    virtual pgd::Matrix3x3 GetGlobalRotationMatrix();
    virtual double GetMass();
    virtual AnyDrawSurf *GetAnyDrawSurf();

};

