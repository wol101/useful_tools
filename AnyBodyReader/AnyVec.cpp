/*
 *  AnyVec.cpp
 *  AnyBodyReader
 *
 *  Created by Bill Sellers on 08/10/2007.
 *  Copyright 2007 Bill Sellers. All rights reserved.
 *
 */

#include <iostream>
#include "AnyVec.h"
#include "ExpressionParser.h"

extern int gDebug;

AnyVec::AnyVec(std::string type, std::string name, AnyFolder *parent): AnyFolder(type, name, parent)
{
}

pgd::Vector AnyVec::GetVector()
{
    pgd::Vector v(0, 0, 0);
    
    if (m_Data.size() > 0) v.x = m_Data[0];
    if (m_Data.size() > 1) v.y = m_Data[1];
    if (m_Data.size() > 2) v.z = m_Data[2];
    
    return v;
}

AnyFolder *AnyVec::ParseTokens(std::vector<std::string> &tokens, unsigned int &index)
{
    if (GetPath() == "Main.Model.HumanModel.Trunk.SegmentsLumbar.PelvisSeg.r0")
    {
        AnyVec *anyVec = dynamic_cast<AnyVec *>(Search("Main.Model.HumanModel.Trunk.SegmentsLumbar.PelvisSeg.r0", false));
        if (anyVec)
            std::cerr << anyVec << " " << *anyVec << "\n";
    }
    
    
    m_Status = 0;
    
    index++;
    if (tokens[index] != "=") 
    {
        std::cerr << __FILE__ << " " << __LINE__ << " Error " << "Expected = in " << m_Name << "\n";
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
    m_Data.clear();
    switch (v.type)
    {
/*        case EPNumber:
            m_Data.push_back(v.value);
            break;*/
            
        case EPVectorVal:
            m_Data.push_back(v.vector.x);
            m_Data.push_back(v.vector.y);
            m_Data.push_back(v.vector.z);
            break;
            
/*        case EPMatrix3x3Val:
            m_Data.push_back(v.matrix.e11);
            m_Data.push_back(v.matrix.e12);
            m_Data.push_back(v.matrix.e13);
            m_Data.push_back(v.matrix.e21);
            m_Data.push_back(v.matrix.e22);
            m_Data.push_back(v.matrix.e23);
            m_Data.push_back(v.matrix.e31);
            m_Data.push_back(v.matrix.e32);
            m_Data.push_back(v.matrix.e33);
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
        std::cerr << "Created AnyVec " << GetPath() << "\n";
        for (unsigned int i = 0; i < m_Data.size(); i++) std::cerr << m_Data[i] << "\n";
    }
    
    if (GetPath() == "Main.Model.HumanModel.Trunk.SegmentsLumbar.PelvisSeg.r0")
    {
        AnyVec *anyVec = dynamic_cast<AnyVec *>(Search("Main.Model.HumanModel.Trunk.SegmentsLumbar.PelvisSeg.r0", false));
        if (anyVec)
            std::cerr << anyVec << " " << *anyVec << "\n";
    }
    
    return this;
}

// output to a stream
std::ostream& operator<<(std::ostream &out, AnyVec &g)
{
    out << "AnyVec " << g.GetPath() << " ";
    pgd::Vector v = g.GetVector();
    out << "[" << v.x << ", " << v.y << ", " << v.z << "]";
    return out;
}
