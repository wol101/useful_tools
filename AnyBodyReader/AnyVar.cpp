/*
 *  AnyVar.cpp
 *  AnyBodyReader
 *
 *  Created by Bill Sellers on 09/10/2007.
 *  Copyright 2007 Bill Sellers. All rights reserved.
 *
 */

#include <iostream>
#include "AnyVar.h"
#include "ExpressionParser.h"

extern int gDebug;

AnyVar::AnyVar(std::string type, std::string name, AnyFolder *parent): AnyFolder(type, name, parent)
{
    m_Value = 0;
}

AnyFolder *AnyVar::ParseTokens(std::vector<std::string> &tokens, unsigned int &index)
{
    if (m_Name == "TotalMass")
        std::cerr << GetPath() << "\n";
    
    m_Status = 0;
    
    index++;
    if (tokens[index] != "=") 
    {
        std::cerr << __FILE__ << " " << __LINE__ << " Warning " << "Expected = in " << m_Name << "\n";
        return 0;
    }
    index++;
    
    std::string expression;
    while (tokens[index] != ";")
    {
        expression += tokens[index];
        index++;
    }
    index++;
    ExpressionParser expressionParser;
    expressionParser.SetFolder(m_Parent);
    if (expressionParser.CreateFromString(expression))
    {
        //expressionParser.SubstituteNames(&expression);
        std::cerr << __FILE__ << " " << __LINE__ << " Warning " << "Error parsing " << expression << " in " << m_Name << "\n";
        m_Content = expression;
        m_Status = 1;
        return this;
    }
    ExpressionParserValue v = expressionParser.Evaluate();
    switch (v.type)
    {
        case EPNumber:
            m_Value = v.value;
            break;
            
/*        case EPVectorVal:
            m_Data = v.vector.x;
            break;
            
        case EPMatrix3x3Val:
            m_Data = v.matrix.e11;
            break;*/
            
            
        default:
            //expressionParser.SubstituteNames(&expression);
            std::cerr << __FILE__ << " " << __LINE__ << " Warning " << "Incorrect return type for " << expression << " in " << m_Name << "\n";
            m_Content = expression;
            m_Status = 1;
            return this;
    }
    
    if (gDebug > 0)
    {
        std::cerr << "Created AnyVar " << GetPath() << "\n";
        std::cerr << m_Value << "\n";
    }
    return this;
}

// output to a stream
std::ostream& operator<<(std::ostream &out, AnyVar &g)
{
    out << "AnyVar " << g.GetPath() << " " << g.GetValue();
    return out;
}
