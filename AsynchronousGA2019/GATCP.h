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

#include "DataFile.h"

#include <vector>

class AsynchronousGAQtWidget;
class Population;
class XMLConverter;
class Genome;

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
    static std::string ConvertAddressToString(uint32_t address);
    static std::string ToString(const char * const printfFormatString, ...);

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

    struct RequestMessage
    {
        char text[16];
        uint32_t senderIP;
        uint32_t senderPort;
        uint32_t runID;
        double score;
    };

    struct RunSpecifier
    {
        Genome *genome;
        time_t startTime;
        uint32_t senderIP;
        uint32_t senderPort;
    };


private:
    int Evolve(Population *startPopulation, Population *thePopulation, std::ostream *outFile, const std::string &dirname);

    DataFile m_baseXMLFile;
    std::vector<uint32_t> m_md5 = {0, 0, 0, 0};

    int m_logLevel = 0;
    bool m_enetInitialised = false;

    AsynchronousGAQtWidget *m_callingWidget = nullptr;
    void ReportProgress(const std::string &message, int logLevel);
};

#endif

#endif

