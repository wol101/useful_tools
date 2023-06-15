/*
 *  AnyVec.h
 *  AnyBodyReader
 *
 *  Created by Bill Sellers on 08/10/2007.
 *  Copyright 2007 Bill Sellers. All rights reserved.
 *
 */

#include <vector>
#include "PGDMath.h"
#include "AnyFolder.h"

class AnyVec: public AnyFolder
{
public:
    
    AnyVec(std::string type, std::string name, AnyFolder *parent);
    
    virtual AnyFolder *ParseTokens(std::vector<std::string> &tokens, unsigned int &index);

    virtual std::vector<double> *GetData() { return &m_Data; };
    virtual pgd::Vector GetVector();
    
    friend std::ostream& operator<<(std::ostream &out, AnyVec &g);
    
protected:
    
    std::vector<double> m_Data;
    
};

