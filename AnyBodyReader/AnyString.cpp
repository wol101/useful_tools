/*
 *  AnyString.cpp
 *  AnyBodyReader
 *
 *  Created by Bill Sellers on 10/10/2007.
 *  Copyright 2007 Bill Sellers. All rights reserved.
 *
 */

#include "AnyString.h"

#include <iostream>
#include "AnyVar.h"
#include "ExpressionParser.h"

extern int gDebug;

AnyString::AnyString(std::string type, std::string name, AnyFolder *parent): AnyFolder(type, name, parent)
{
}

AnyFolder *AnyString::ParseTokens(std::vector<std::string> &tokens, unsigned int &index)
{
    index++;
    if (tokens[index] != "=") 
    {
        std::cerr << __FILE__ << " " << __LINE__ << " Warning " << "Expected = in " << m_Name << "\n";
        return 0;
    }
    index++;
    
    m_String.clear();
    while (tokens[index] != ";")
    {
        m_String += tokens[index];
        index++;
    }
    index++;
    
    if (gDebug > 0)
    {
        std::cerr << "Created AnyVar " << m_Name << "\n";
        std::cerr << m_String << "\n";
    }
    return this;
}

// output to a stream
std::ostream& operator<<(std::ostream &out, AnyString &g)
{
    out << "AnyString " << g.GetPath() << " " << g.GetString();
    return out;
}
