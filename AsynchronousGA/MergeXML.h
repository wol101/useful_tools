/*
 *  MergeXML.h
 *  MergeXML
 *
 *  Created by Bill Sellers on Wed Dec 17 2003.
 *  Copyright (c) 2003 Bill Sellers. All rights reserved.
 *
 */

#ifndef MergeXML_h
#define MergeXML_h

#include "XMLContainerList.h"

#include <vector>
#include <string>

namespace AsynchronousGA
{

class MergeXML
{
public:
    MergeXML();
    ~MergeXML();

    void ExecuteInstructionFile(const char *bigString);
    void ParseError(char **tokens, int numTokens, int count, const char *message);

private:
    bool m_ignoreError;
    XMLContainerList m_objectList;
    std::vector<std::string> m_oldStringList;
    std::vector<std::string> m_newStringList;

};

}

#endif

