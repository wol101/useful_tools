/*
 *  AnyJoint.h
 *  AnyBodyReader
 *
 *  Created by Bill Sellers on 18/10/2007.
 *  Copyright 2007 Bill Sellers. All rights reserved.
 *
 */

#include "AnyFolder.h"

class AnyJoint: public AnyFolder
{
public:
    
    AnyJoint(std::string type, std::string name, AnyFolder *parent);

    virtual std::string GetBody1ID();
    virtual std::string GetBody2ID();
    virtual pgd::Vector GetHingeAnchor();
    virtual pgd::Vector GetHingeAxis();

};

