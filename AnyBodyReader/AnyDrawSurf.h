/*
 *  AnyDrawSurf.h
 *  AnyBodyReader
 *
 *  Created by Bill Sellers on 10/10/2007.
 *  Copyright 2007 Bill Sellers. All rights reserved.
 *
 */

#include "AnyFolder.h"

class AnyFunTransform3D;

class AnyDrawSurf: public AnyFolder
{
public:
    
    AnyDrawSurf(std::string type, std::string name, AnyFolder *parent);
    
    virtual AnyFunTransform3D *GetAnyFunTransform3D();

};
