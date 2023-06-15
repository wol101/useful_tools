#ifdef USE_TCP

#ifndef GAMAIN_H
#define GAMAIN_H

/*
 *  GAMain.h
 *  AsynchronousGA
 *
 *  Created by Bill Sellers on 19/10/2010.
 *  Copyright 2010 Bill Sellers. All rights reserved.
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


#ifdef USE_QT_AGA
    void setCallingWidget(AsynchronousGAQtWidget *callingWidget) { m_callingWidget = callingWidget; }
#endif

    void SetLogLevel(int logLevel) { m_logLevel = logLevel; }

private:
    int Evolve(Population *startPopulation, Population *thePopulation, std::ostream *outFile, const char *dirname);

    DataFile *m_baseXMLFile;

    int m_logLevel;

#ifdef USE_QT_AGA
    AsynchronousGAQtWidget *m_callingWidget;
#endif
};

}

#endif

#endif

