#ifdef USE_TCP_PTYPES

#ifndef GATCPPTYPES_H
#define GATCPPTYPES_H

/*
 *  GATCPptypes.h
 *  AsynchronousGA
 *
 *  Created by Bill Sellers on 08/01/2018.
 *  Copyright 2018 Bill Sellers. All rights reserved.
 *
 */

#include <iostream>

#ifdef USE_QT_AGA
class AsynchronousGAQtWidget;
#endif

namespace AsynchronousGA
{

class Population;
class XMLConverter;
class DataFile;

class GAMain
{
public:
    GAMain();
    ~GAMain();

#ifdef _WIN32
    int LoadBaseXMLFile(const wchar_t *filename);
    int Process(const wchar_t * const parameterFile, const wchar_t * const outputDirectory, const wchar_t * const startingPopulation);
#else
    int LoadBaseXMLFile(const char *filename);
    int Process(const char * const parameterFile, const char * const outputDirectory, const char * const startingPopulation);
#endif

    static void ApplyGenome( const char *inputGenome, const char *inputXML, const char *outputXML);


#ifdef USE_QT_AGA
    void setCallingWidget(AsynchronousGAQtWidget *callingWidget) { m_callingWidget = callingWidget; }
#endif

    void SetLogLevel(int logLevel) { m_logLevel = logLevel; }
    void SetServerPort(int port);

private:
    int Evolve(Population *startPopulation, Population *thePopulation, std::ostream *outFile, const char *dirname);

    DataFile *m_baseXMLFile;
    uint32_t m_md5[4];

    int m_logLevel;

#ifdef USE_QT_AGA
    AsynchronousGAQtWidget *m_callingWidget;
#endif
};

}


#endif // GATCPPTYPES_H

#endif

