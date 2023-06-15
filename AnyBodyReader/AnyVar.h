/*
 *  AnyVar.h
 *  AnyBodyReader
 *
 *  Created by Bill Sellers on 09/10/2007.
 *  Copyright 2007 Bill Sellers. All rights reserved.
 *
 */


#include "PGDMath.h"
#include "AnyFolder.h"

class AnyVar: public AnyFolder
{
public:
    
    AnyVar(std::string type, std::string name, AnyFolder *parent);
    
    virtual AnyFolder *ParseTokens(std::vector<std::string> &tokens, unsigned int &index);
    
    virtual double GetValue() { return m_Value; };

    friend std::ostream& operator<<(std::ostream &out, AnyVar &g);

protected:
        
        double m_Value;
    
};

