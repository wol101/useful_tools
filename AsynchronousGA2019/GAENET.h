#ifdef USE_ENET

#ifndef GAENET_H
#define GAENET_H

/*
 *  GAENET.h
 *  AsynchronousGA
 *
 *  Created by Bill Sellers on 08/01/2018.
 *  Copyright 2018 Bill Sellers. All rights reserved.
 *
 */

#include "DataFile.h"

class AsynchronousGAQtWidget;
class QString;

class Population;
class XMLConverter;

class GAMain
{
public:
    GAMain();
    ~GAMain();

    int Initialise();

    int LoadBaseXMLFile(const std::string &filename);
    int Process(const std::string &parameterFile, const std::string &outputDirectory, const std::string &startingPopulation);

    static void ApplyGenome(const std::string &inputGenome, const std::string &inputXML, const std::string &outputXML);

    void SetLogLevel(int logLevel) { m_logLevel = logLevel; }
    void SetServerPort(int port);

    static std::string ConvertAddressPortToString(uint32_t address, uint16_t port);
    static std::string ToString(const char * const printfFormatString, ...);

    AsynchronousGAQtWidget *callingWidget() const;
    void setCallingWidget(AsynchronousGAQtWidget *callingWidget);

private:
    int Evolve(Population *startPopulation, Population *thePopulation, std::ostream *outFile, const std::string &dirname);

    DataFile m_baseXMLFile;
    uint32_t m_md5[4] = {0, 0, 0, 0};

    int m_logLevel = 0;
    bool m_enetInitialised = false;

    AsynchronousGAQtWidget *m_callingWidget = nullptr;
    void ReportProgress(const std::string &message, int logLevel);
};


#endif // GAENET_H

#endif

