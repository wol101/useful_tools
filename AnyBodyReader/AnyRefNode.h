/*
 *  AnyRefNode.h
 *  AnyBodyReader
 *
 *  Created by Bill Sellers on 10/10/2007.
 *  Copyright 2007 Bill Sellers. All rights reserved.
 *
 */

#include "AnyFolder.h"

class AnySeg;

class AnyRefNode: public AnyFolder
{
public:
    
    AnyRefNode(std::string type, std::string name, AnyFolder *parent);

    virtual pgd::Vector GetGlobalPosition();
    virtual pgd::Matrix3x3 GetGlobalRotationMatrix();
    virtual AnySeg *GetParentSeg();

};
