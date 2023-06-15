/*
 *  AnyString.h
 *  AnyBodyReader
 *
 *  Created by Bill Sellers on 10/10/2007.
 *  Copyright 2007 Bill Sellers. All rights reserved.
 *
 */

#include "AnyFolder.h"
#include <string>

class AnyString: public AnyFolder
{
public:
    
    AnyString(std::string type, std::string name, AnyFolder *parent);
    
    virtual AnyFolder *ParseTokens(std::vector<std::string> &tokens, unsigned int &index);
    
    virtual std::string GetString() { return m_String; };
    
    friend std::ostream& operator<<(std::ostream &out, AnyString &g);
    
protected:
        
        std::string m_String;
    
};
