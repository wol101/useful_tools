/*
 *  XMLConverter.h
 *  GA
 *
 *  Created by Bill Sellers on Fri Dec 12 2003.
 *  Copyright (c) 2003 Bill Sellers. All rights reserved.
 *
 */

#ifndef XMLConverter_h
#define XMLConverter_h

#include <vector>
#include <string>

class Genome;
class DataFile;
class ExpressionParser;

class XMLConverter
{
public:
    XMLConverter();

    int LoadBaseXMLFile(const char *filename);
    int LoadBaseXMLString(const char *dataPtr, size_t length);
    int ApplyGenome(int genomeSize, double *genomeData);
    char* GetFormattedXML(size_t *docTxtLen);

    const std::string &BaseXMLString() const;

    void Clear();

private:

    void ConvertVectorBrackets();

    std::string m_BaseXMLString;
    std::vector<std::string> m_SmartSubstitutionTextComponents;
    std::vector<std::string> m_SmartSubstitutionParserText;
    std::vector<double> m_SmartSubstitutionValues;
    std::string m_SmartSubstitutionTextBuffer;
};


#endif
