/*
 *  AnyMuscle.h
 *  AnyBodyReader
 *
 *  Created by Bill Sellers on 18/10/2007.
 *  Copyright 2007 Bill Sellers. All rights reserved.
 *
 */

#include "AnyFolder.h"
#include <vector>

class AnyRefNode;
class AnySurfCylinder;
class AnyMuscleModel;

class AnyMuscle: public AnyFolder
{
public:
    
    AnyMuscle(std::string type, std::string name, AnyFolder *parent);

    virtual std::string GetOriginID();
    virtual std::string GetInsertionID();
    virtual pgd::Vector GetOriginGlobalPosition();
    virtual pgd::Vector GetInsertionGlobalPosition();

    virtual unsigned int GetNumNodes();
    virtual std::string GetNodeID(unsigned int n);
    virtual pgd::Vector GetOriginGlobalPosition(unsigned int n);
    
    virtual void GetAnyRefNodeList(std::vector<AnyRefNode *> *list);
    virtual void GetAnySurfCylinderList(std::vector<AnySurfCylinder *> *list);
    virtual AnyMuscleModel *GetAnyMuscleModel();


};

