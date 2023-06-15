/*
 *  AnyRef.h
 *  AnyBodyReader
 *
 *  Created by Bill Sellers on 09/10/2007.
 *  Copyright 2007 Bill Sellers. All rights reserved.
 *
 */

#include "AnyFolder.h"
#include "PGDMath.h"

class AnyRef: public AnyFolder
{
public:
    
    AnyRef(std::string type, std::string name, AnyFolder *parent);
    
    virtual AnyFolder *ParseTokens(std::vector<std::string> &tokens, unsigned int &index);
    virtual AnyFolder *Search(std::string path, bool useAlternateTree);
    
    virtual std::string GetLink() { return m_Link; };
    virtual AnyFolder *GetLinked(bool useAlternateTree);
    
protected:
        
    std::string m_Link;
};


