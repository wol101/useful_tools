#ifdef USE_UDP

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

#include "DataFile.h"

#include <vector>

#if defined(_WIN32) || defined(__WIN32__) || defined(WIN32)
#include <WS2tcpip.h>
#else
#include <netinet/in.h>
#endif

class AsynchronousGAQtWidget;
class Population;
class XMLConverter;
class Genome;

class GAMain
{
public:
    GAMain();
    ~GAMain();

    int LoadBaseXMLFile(const std::string &filename);
    int Process(const std::string &parameterFile, const std::string &outputDirectory, const std::string &startingPopulation);
    void Initialise() {};

    static void ApplyGenome(const std::string &inputGenome, const std::string &inputXML, const std::string &outputXML);
    static std::string ConvertAddressPortToString(uint32_t address, uint16_t port, bool networkOrder);
    static std::string ConvertAddressToString(uint32_t address, bool networkOrder);
    static std::string ToString(const char * const printfFormatString, ...);

    void SetLogLevel(int logLevel) { m_logLevel = logLevel; }
    void SetServerPort(int port);

    AsynchronousGAQtWidget *callingWidget() const;
    void setCallingWidget(AsynchronousGAQtWidget *callingWidget);

    struct DataMessage
    {
        char text[16];
        uint32_t senderIP;
        uint32_t senderPort;
        uint32_t runID;
        uint32_t genomeLength;
        uint32_t xmlLength;
        uint32_t md5[4];
        union
        {
            double genome[1];
            char xml[1];
        } payload;
    };

    struct RunSpecifier
    {
        Genome *genome;
        time_t startTime;
        struct sockaddr_in sender;
    };

private:
    int Evolve(Population *startPopulation, Population *thePopulation, std::ostream *outFile, const std::string &dirname);

    DataFile m_baseXMLFile;
    std::vector<uint32_t> m_md5 = {0, 0, 0, 0};

    int m_logLevel = 0;
    int m_redundancyPercent = 0;

    AsynchronousGAQtWidget *m_callingWidget = nullptr;
    void ReportProgress(const std::string &message, int logLevel);
};


#endif

#endif

