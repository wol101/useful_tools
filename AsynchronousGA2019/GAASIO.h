#ifdef USE_ASIO

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
#include "ServerASIO.h"
#include "Population.h"

#include <string>
#include <vector>
#include <queue>
#include <mutex>
#include <iostream>
#include <fstream>
#include <inttypes.h>

class AsynchronousGAQtWidget;
class XMLConverter;
class Genome;
class SessionASIO;

class GAMain
{
public:
    GAMain(AsynchronousGAQtWidget *callingWidget);

    int LoadBaseXMLFile(const std::string &filename);
    int Process(const std::string &parameterFile, const std::string &outputDirectory, const std::string &startingPopulation);

    static void ApplyGenome(const std::string &inputGenome, const std::string &inputXML, const std::string &outputXML);

    void SetLogLevel(int logLevel) { m_logLevel = logLevel; }
    void SetServerPort(int port);

    static std::string ConvertAddressPortToString(uint32_t address, uint16_t port);
    static std::string ConvertAddressToString(uint32_t address);
    static std::string ToString(const char * const printfFormatString, ...);

    void handleRequestGenome(MessageASIO message);
    void handleRequestXML(MessageASIO message);
    void handleScore(MessageASIO message);

    struct DataMessage
    {
        char text[16];
        uint64_t evolveIdentifier;
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
        uint64_t evolveIdentifier;
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
    int Evolve();

    size_t GenomeRequestQueueSize();
    size_t ScoreQueueSize();
    void GetNextGenomeRequest(MessageASIO *message);
    void GetNextScore(MessageASIO *message);
    void ClearGenomeRequestQueue();
    void ClearScoreQueue();

    DataFile m_baseXMLFile;
    std::vector<uint32_t> m_md5 = {0, 0, 0, 0};
    uint64_t m_evolveIdentifier = 0;

    int m_logLevel = 0;

    AsynchronousGAQtWidget *m_callingWidget = nullptr;
    void ReportProgress(const std::string &message, int logLevel);

    std::deque<MessageASIO> m_requestGenomeQueue;
    std::deque<MessageASIO> m_scoreQueue;
    std::mutex m_requestGenomeMutex;
    std::mutex m_scoreMutex;
    std::atomic<bool> m_requestGenomeQueueEnabled = {false};
    uint64_t m_loopSleepTimeMicroSeconds = 1;

    Population m_startPopulation;
    Population m_evolvePopulation;
    std::ofstream m_outputLogFile;
    std::string m_outputFolderName;
    const std::string m_bestGenomeModel{"BestGenome_%012" PRIu32 ".txt"};
    const std::string m_bestPopulationModel{"Population_%012" PRIu32 ".txt"};
    const std::string m_bestGenomeRegex{"BestGenome_[0-9]+.txt"};
    const std::string m_bestPopulationRegex{"Population_[0-9]+.txt"};
    int OnlyKeepLastMatching(const std::string &regexPattern);
    std::string m_parameterFile;

    std::array<uint8_t, 4> m_ipAddress = {0, 0, 0, 0};
    std::uint16_t m_port = 0;

};

#endif

#endif

