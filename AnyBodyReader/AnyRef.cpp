/*
 *  AnyRef.cpp
 *  AnyBodyReader
 *
 *  Created by Bill Sellers on 09/10/2007.
 *  Copyright 2007 Bill Sellers. All rights reserved.
 *
 */

#include <iostream>
#include "AnyRef.h"
#include "AnyVar.h"
#include "AnyVec.h"
#include "AnyMat.h"
#include "AnyFunTransform3D.h"

extern int gDebug;

AnyRef::AnyRef(std::string type, std::string name, AnyFolder *parent): AnyFolder(type, name, parent)
{
}

AnyFolder *AnyRef::ParseTokens(std::vector<std::string> &tokens, unsigned int &index)
{
    if (tokens[index] == "&") // this is the definition
    {
        index++;
        index++;
        if (tokens[index] != "=") 
        {
            std::cerr << __FILE__ << " " << __LINE__ << " Error " << "Expected = in " << m_Name << "\n";
            return 0;
        }
        index++;
        
        m_Link = "";
        while (tokens[index] != ";")
        {
            m_Link += tokens[index];
            index++;
        }
        index++;
        
        if (gDebug > 0)
        {
            std::cerr << "Created AnyRef " << m_Name << "\n";
            std::cerr << m_Type << " " << m_Link << "\n";
        }
        return this;
    }
    
    // but here it is being used
    std::cerr << GetPath() << "\n";
    AnyFolder *anyFolder = GetLinked(false);
    if (anyFolder == 0) 
    {
        std::cerr << __FILE__ << " " << __LINE__ << " Warning " << tokens[index] << " not found in " << m_Name << "\n";
        index++;
        int braceCount = 0;
        std::string content;
        while (tokens[index] != ";" || braceCount != 0)
        {
            if (tokens[index] == "{") braceCount++;
            else if (tokens[index] == "}") braceCount--;
            content += tokens[index];
            index++;
        }
        std::cerr << __FILE__ << " " << __LINE__ << " Warning ignoring" << "\n";
        std::cerr << content << "\n";
        index++;
        return 0;
    }
    if (gDebug > 0)
    {
        std::cerr << "Reference redirected from " << GetPath() << " to " << anyFolder->GetPath() << "\n";
    }
    // now need to temporarily change parent
    AnyFolder *parentStore = anyFolder->GetParent();
    std::string nameStore = anyFolder->GetName();
    anyFolder->SetParent(m_Parent);
    anyFolder->SetName(m_Name);
    AnyFolder *retValue = anyFolder->ParseTokens(tokens, index);
    anyFolder->SetParent(parentStore);
    anyFolder->SetName(nameStore);
    return retValue;
}

AnyFolder *AnyRef::Search(std::string path, bool useAlternateTree)
{
    // if searching up, just search up
    if (path[0] == '.')
    {
        if (m_Parent == 0)
        {
            std::cerr << __FILE__ << " " << __LINE__ << " Warning " << "Reached root folder without finding " << path << "\n";
            return 0;
        }
        std::string newPath = path.substr(1);
        return m_Parent->Search(newPath, useAlternateTree);
    }
    
    // but if searching down add in the link
    std::string newPath = m_Link + "." + path;
    if (gDebug > 0)
    {
        std::cerr << "Reference converted " << path << " to " << newPath << "\n";
    }
    return m_Parent->Search(newPath, useAlternateTree);
}

AnyFolder *AnyRef::GetLinked(bool useAlternateTree)
{
    AnyFolder *anyFolder = m_Parent->Search(m_Link, useAlternateTree);
    if (anyFolder == 0) return 0;
    
    AnyRef *anyRef = dynamic_cast<AnyRef *>(anyFolder);
    if (anyRef) return anyRef->GetLinked(useAlternateTree);
    
    return anyFolder;
}

// output to a stream
std::ostream& operator<<(std::ostream &out, AnyRef &g)
{
    out << "AnyRef " << g.GetPath() << " " << g.GetLink();
    return out;
}
