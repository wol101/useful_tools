/*
 *  AnyFunTransform3D.h
 *  AnyBodyReader
 *
 *  Created by Bill Sellers on 09/10/2007.
 *  Copyright 2007 Bill Sellers. All rights reserved.
 *
 */

#ifndef AnyFunTransform3D_h
#define AnyFunTransform3D_h

#include "AnyFolder.h"
#include "PGDMath.h"

class AnyFunTransform3D: public AnyFolder
{
public:
    
    AnyFunTransform3D(std::string type, std::string name, AnyFolder *parent);
    
    virtual int Calculate(pgd::Vector *vector);

};

#endif
