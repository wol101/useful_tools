#ifdef USE_ENET

/*
 *  GAENET.cpp
 *  AsynchronousGA
 *
 *  Created by Bill Sellers on 08/01/2018.
 *  Copyright 2018 Bill Sellers. All rights reserved.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <fstream>
#include <sys/stat.h>
#include <sys/timeb.h>
#include <queue>
#include <map>
#include <float.h>
#include <unistd.h>

#include "enet/enet.h"
#include "pystring.h"

#ifdef USE_QT_AGA
#include "AsynchronousGAQtWidget.h"
#include <QMetaObject>
#include <QString>
#endif

#include "Preferences.h"
#include "Genome.h"
#include "Population.h"
#include "Mating.h"
#include "Random.h"
#include "Statistics.h"
#include "GATCP.h"
#include "WindowsGlue.h"
#include "XMLConverter.h"
#include "MD5.h"
#include "TCPIPMessage.h"

#include "GAENET.h"

using namespace std::string_literals;

#ifdef COMMAND_LINE_VERSION
int main(int argc, char *argv[])
{
    int i;
    char *parameterFile = 0;
    char *outputDirectory = 0;
    char *startingPopulation = 0;
    char *baseXMLFile = 0;
    int logLevel = 1;
    int debugControl = 0;

    // parse the command line
    try
    {
        for (i = 1; i < argc; i++)
        {
            // do something with arguments
            if (strcmp(argv[i], "--parameterFile") == 0 ||
                strcmp(argv[i], "-p") == 0)
            {
                i++;
                if (i >= argc) throw(1);
                parameterFile = argv[i];
            }
            else
                if (strcmp(argv[i], "--outputDirectory") == 0 ||
                    strcmp(argv[i], "-o") == 0)
                {
                    i++;
                    if (i >= argc) throw(1);
                    outputDirectory = argv[i];
                }
            else
                if (strcmp(argv[i], "--startingPopulation") == 0 ||
                    strcmp(argv[i], "-s") == 0)
                {
                    i++;
                    if (i >= argc) throw(1);
                    startingPopulation = argv[i];
                }
            else
                if (strcmp(argv[i], "--baseXMLFile") == 0 ||
                    strcmp(argv[i], "-b") == 0)
                {
                    i++;
                    if (i >= argc) throw(1);
                    baseXMLFile = argv[i];
                }
            else
                if (strcmp(argv[i], "--debug") == 0 ||
                    strcmp(argv[i], "-d") == 0)
                {
                    i++;
                    if (i >= argc) throw(1);
                    debugControl = strtol(argv[i], 0, 10);
                }
            else
                if (strcmp(argv[i], "--logLevel") == 0 ||
                    strcmp(argv[i], "-l") == 0)
                {
                    i++;
                    if (i >= argc) throw(1);
                    logLevel = strtol(argv[i], 0, 10);
                }
            else
            {
                throw(__LINE__);
            }
        }
        if (parameterFile == 0 || baseXMLFile == 0) throw(__LINE__);
    }
    // catch argument errors
    catch (int e)
    {
        std::cerr << "\nGenetic Algorithm program\n";
        std::cerr << "Build " << __DATE__ << " " << __TIME__ << "\n";
        std::cerr << "Supported options:\n\n";
        std::cerr << "(-p) --parameterFile file_name (required)\n";
        std::cerr << "(-o) --outputDirectory directory_name\n";
        std::cerr << "(-s) --startingPopulation population_file";
        std::cerr << "(-b) --baseXMLFile model XML_config_file (required)";
        std::cerr << "(-l) --logLevel n\n";
        std::cerr << "(-d) --debug n\n";
        std::cerr << "\n";
        return (e);
    }
    if (enet_initialize () != 0)
    {
        std::cerr << "An error occurred while initializing ENet.\n";
        return __LINE__;
    }
    GAMain ga;
    ga.Initialise();
    ga.SetLogLevel(logLevel);
    ga.LoadBaseXMLFile(baseXMLFile);
    int status = ga.Process(parameterFile, outputDirectory, startingPopulation);
    return status;
}
#endif


GAMain::GAMain()
{
}

GAMain::~GAMain()
{
    if (m_enetInitialised) enet_deinitialize();
}

int GAMain::Initialise()
{
    if (m_enetInitialised == false)
    {
        int status = enet_initialize();
        if (status != 0)
        {
            // std::cerr << "An error occurred while initializing ENet.\n";
            ReportProgress("An error occurred while initializing ENet", 0);
            return __LINE__;
        }
        m_enetInitialised = true;
    }
    return 0;
}

int GAMain::Process(const std::string &parameterFile, const std::string &outputDirectory, const std::string &startingPopulation)
{
    time_t theTime = time(nullptr);
    struct tm *theLocalTime = localtime(&theTime);
    Population startPopulation;
    Population evolvePopulation;
    std::ofstream outFile;
    std::string logFileName;
    std::string dirname;

    if (g_prefs->ReadPreferences(parameterFile))
    {
        // std::cerr << "Error reading " << parameterFile << "\n";
        ReportProgress("Error reading "s + parameterFile, 0);
        return (__LINE__);
    }
    ReportProgress(parameterFile + " read"s, 0);
    // overwrite starting population if defined
    if (startingPopulation.size()) g_prefs->startingPopulation = startingPopulation;

    // create a directory for all the output
    if (outputDirectory.size())
    {
        dirname = outputDirectory;
#ifdef _WIN32
        std::wstring wdirname = WindowsGlue::ToWide(outputDirectory);
        struct _stat sb;
        if (_wstat(wdirname.c_str(), &sb) == -1)
        {
            int error = _wmkdir(wdirname.c_str());
            if (error)
            {
                ReportProgress("Error creating directory "s + outputDirectory, 0);
                return __LINE__;
            }
            ReportProgress(outputDirectory + " created"s, 0);
        }
        else
        {
            if ((sb.st_mode & S_IFDIR) == 0)
            {
                ReportProgress("Error creating directory (exists but not a directory) "s + outputDirectory, 0);
                return __LINE__;
            }
        }
#else
        dirname = outputDirectory;
        struct stat sb;
        if (stat(dirname.c_str(), &sb) == -1)
        {
            int error = mkdir(dirname.c_str(), 0777);
            if (error)
            {
                ReportProgress("Error creating directory "s + outputDirectory, 0);
                return __LINE__;
            }
            ReportProgress(outputDirectory + " created"s, 0);
        }
        else
        {
            if ((sb.st_mode & S_IFDIR) == 0)
            {
                ReportProgress("Error creating directory (exists but not a directory) "s + outputDirectory, 0);
                return __LINE__;
            }
        }
#endif
    }
    else
    {
        dirname = ToString("Run_%04d-%02d-%02d_%02d.%02d.%02d",
                           theLocalTime->tm_year + 1900,
                           theLocalTime->tm_mon + 1,
                           theLocalTime->tm_mday,
                           theLocalTime->tm_hour,
                           theLocalTime->tm_min,
                           theLocalTime->tm_sec);
#ifdef _WIN32
        int error = _mkdir(dirname.c_str());
#else
        int error = mkdir(dirname.c_str(), 0777);
#endif
        if (error)
        {
            // std::cerr << "Error creating directory " << dirname << "\n";
            ReportProgress("Error creating directory "s + dirname, 0);
            return __LINE__;
        }
        ReportProgress(dirname + " created"s, 0);
    }

    // write log
    logFileName = pystring::os::path::join(dirname, "log.txt"s);
    outFile.open(logFileName.c_str());
    outFile << "GA build " << __DATE__ << " " << __TIME__ "\n";
    outFile << "Log produced " << asctime(theLocalTime);
    outFile << "parameterFile \"" << parameterFile << "\"\n";
    outFile << *g_prefs << "\n";
    outFile.flush();
    ReportProgress(logFileName + " opened"s, 0);

    // initialise the population
    if (startPopulation.ReadPopulation(g_prefs->startingPopulation.c_str()))
    {
        // std::cerr << "Error reading starting population: " << g_prefs->startingPopulation << "\n";
        ReportProgress("Error reading starting population: "s + g_prefs->startingPopulation, 0);
        return __LINE__;
    }
    ReportProgress(g_prefs->startingPopulation + " read"s, 0);

    if (startPopulation.GetPopulationSize() != g_prefs->populationSize)
    {
        // std::cerr << "Info: Starting population size does not match specified population size - using standard fixup\n";
        ReportProgress("Info: Starting population size "s + std::to_string(startPopulation.GetPopulationSize()) + " does not match specified population size "s + std::to_string(g_prefs->populationSize) + " - using standard fixup"s, 0);
        if (startPopulation.ResizePopulation(g_prefs->populationSize))
        {
            // std::cerr << "Error: Starting population size does not match specified population size - unable to fix\n";
            ReportProgress("Error: Starting population size does not match specified population size - unable to fix"s, 0);
            return __LINE__;
        }
    }
    if (startPopulation.GetGenome(0)->GetGenomeLength() != g_prefs->genomeLength)
    {
        // std::cerr << "Error: Starting population genome does not match specified genome length\n";
        ReportProgress("Error: Starting population genome does not match specified genome length"s, 0);
        return __LINE__;
    }
    if (g_prefs->randomiseModel)
    {
        startPopulation.Randomise();
    }

    startPopulation.SetGlobalCircularMutation(g_prefs->circularMutation);
    startPopulation.SetResizeControl(g_prefs->resizeControl);

    evolvePopulation.SetSelectionType(g_prefs->parentSelection);
    evolvePopulation.SetParentsToKeep(g_prefs->parentsToKeep);

    if (Evolve(&startPopulation, &evolvePopulation, &outFile, dirname))
    {
        // std::cerr << "Terminated due to Evolve failure\n";
        ReportProgress("Error: Terminated due to Evolve failure"s, 0);
        return __LINE__;
    }

    return 0;
}

int GAMain::LoadBaseXMLFile(const std::string &filename)
{
    if (m_baseXMLFile.ReadFile(filename))
    {
        m_baseXMLFile.ClearData();
        std::fill(std::begin(m_md5), std::end(m_md5), 0);
        return 1;
    }
    const uint32_t *md5Array = md5(m_baseXMLFile.GetRawData(), int(m_baseXMLFile.GetSize()));
    std::copy_n(md5Array, sizeof(m_md5) / sizeof(m_md5[0]), m_md5);
    return 0;
}

int GAMain::Evolve(Population *startPopulation, Population *thePopulation, std::ostream *outFile, const std::string &dirname)
{
    struct RunSpecifier
    {
        std::unique_ptr<Genome> genome;
        time_t startTime;
    };

    // start the ENET server
    ENetAddress address = {};
    ENetHost *server;
    address.host = ENET_HOST_ANY;
    address.port = enet_uint16(g_prefs->tcpPort);
    size_t peerCount = ENET_PROTOCOL_MAXIMUM_PEER_ID - 1;
    size_t channelLimit = 2;
    enet_uint32 incomingBandwidth = 0;
    enet_uint32 outgoingBandwidth = 0;
    server = enet_host_create (&address, peerCount, channelLimit, incomingBandwidth, outgoingBandwidth);
    if (server == nullptr)
    {
        ReportProgress("enet_host_create error"s, 0);
        return __LINE__;
    }

    std::string serverURL = ConvertAddressPortToString(server->address.host, server->address.port);
    ReportProgress("ENET host "s + serverURL + " open"s, 0);

    int returnCount = 0;
    bool stopSendingFlag = false;
    time_t lastTime = time(nullptr);
    time_t periodicTaskInterval = 1;
    bool shouldStop = false;
    int progressValue = 0;
    int lastProgressValue = -1;
    int startPopulationIndex = 0;
    uint32_t submitCount = 0;
    std::map<uint32_t, std::unique_ptr<RunSpecifier>> runningList;
    double maxFitness = -DBL_MAX;
    double lastMaxFitness = -DBL_MAX;
    std::string lastBestGenomeFile;
    std::string lastBestPopulationFile;
    ENetEvent event = {};

    while (returnCount < g_prefs->maxReproductions && stopSendingFlag == false)
    {
        time_t currentTime = time(nullptr);
        if (currentTime >= lastTime + periodicTaskInterval) // this part of the loop is for things that don't need to be done all that often
        {
            lastTime = currentTime;
#ifdef USE_QT_AGA
            QMetaObject::invokeMethod(m_callingWidget, "logLevel", Qt::DirectConnection, Q_RETURN_ARG(int, m_logLevel));
            QMetaObject::invokeMethod(m_callingWidget, "tryToStopGA", Qt::DirectConnection, Q_RETURN_ARG(bool, shouldStop));
            if (shouldStop)
            {
                ReportProgress("Stopped by user"s, 0);
                break;
            }
#endif
            progressValue = int(100 * returnCount / g_prefs->maxReproductions);
            if (progressValue != lastProgressValue)
            {
                lastProgressValue = progressValue;
#ifdef USE_QT_AGA
                QMetaObject::invokeMethod(m_callingWidget, "setProgressValue", Qt::QueuedConnection, Q_ARG(int, progressValue));
#endif
            }
        }
        // check for ENET events
        enet_uint32 timeout = 0; // zero means return immediately
        int status = enet_host_service(server, &event, timeout); // note: this call creates an event.packet that needs to be destroyed
        // std::cerr << "status " << status << "\n";
        if (status > 0)
        {
            std::string hostURL = ConvertAddressPortToString(event.peer->address.host, event.peer->address.port);
            if (event.type == ENET_EVENT_TYPE_CONNECT)
            {
                ReportProgress("Client "s + hostURL + " has connected"s, 1);
            }

            if (event.type == ENET_EVENT_TYPE_DISCONNECT)
            {
                ReportProgress("Client "s + hostURL + " has disconnected"s, 1);
            }

            if (event.type == ENET_EVENT_TYPE_RECEIVE)
            {
                if (event.packet->dataLength >= sizeof(TCPIPMessage))
                {
                    TCPIPMessage *tcpMessage = reinterpret_cast<TCPIPMessage *>(event.packet->data);
                    if (strcmp(tcpMessage->text, "reqjob") == 0) // the client wants a new genome to work with
                    {
                        ReportProgress("A reqjob packet of length "s + std::to_string(event.packet->dataLength) + " was received from "s + hostURL + " on channel "s + std::to_string(event.channelID), 2);
                        // need to create the new genome
                        std::unique_ptr<Genome> offspring = std::make_unique<Genome>();
                        // if we are still working from the start population, just get the next one
                        if (startPopulationIndex < startPopulation->GetPopulationSize())
                        {
                            // OUT_VAR(startPopulationIndex);
                            *offspring = *startPopulation->GetGenome(startPopulationIndex);
                            startPopulationIndex++;
                        }
                        else
                        {
                            // create a new offspring
                            int mutationCount = 0;
                            while (mutationCount == 0) // this means we always get some mutation (no point in getting unmutated offspring)
                            {
                                int parent1Rank;
                                Genome *parent1;
                                if (thePopulation->GetPopulationSize()) parent1 = thePopulation->ChooseParent(&parent1Rank);
                                else parent1 = startPopulation->ChooseParent(&parent1Rank);
                                if (g_random->CoinFlip(g_prefs->crossoverChance))
                                {
                                    int parent2Rank;
                                    Genome *parent2;
                                    if (thePopulation->GetPopulationSize()) parent2 = thePopulation->ChooseParent(&parent2Rank);
                                    else parent2 = startPopulation->ChooseParent(&parent2Rank);
                                    mutationCount += Mate(parent1, parent2, offspring.get(), g_prefs->crossoverType);
                                }
                                else
                                {
                                    *offspring = *parent1;
                                }
                                if (g_prefs->multipleGaussian)  mutationCount += MultipleGaussianMutate(offspring.get(), g_prefs->gaussianMutationChance);
                                else mutationCount += GaussianMutate(offspring.get(), g_prefs->gaussianMutationChance);
                                mutationCount += FrameShiftMutate(offspring.get(), g_prefs->frameShiftMutationChance);
                                mutationCount += DuplicationMutate(offspring.get(), g_prefs->duplicationMutationChance);
                            }
                        }

                        /* Create a reliable packet of required size */
                        size_t packetSize = sizeof(TCPIPMessage) + size_t(offspring->GetGenomeLength()) * sizeof(offspring->GetGenes()[0]);
                        ENetPacket *packet = enet_packet_create(nullptr, packetSize, ENET_PACKET_FLAG_RELIABLE);
                        TCPIPMessage *outTcpMessage = reinterpret_cast<TCPIPMessage *>(packet->data);
                        strcpy(outTcpMessage->text, "reqjob");
                        outTcpMessage->genomeLength = uint32_t(size_t(offspring->GetGenomeLength()) * sizeof(offspring->GetGenes()[0]));
                        outTcpMessage->runID = submitCount;
                        std::copy(std::begin(m_md5), std::end(m_md5), std::begin(outTcpMessage->md5));
                        outTcpMessage->senderIP = server->address.host;
                        outTcpMessage->senderPort = server->address.port;
                        outTcpMessage->score = 0;
                        memcpy(reinterpret_cast<char *>(packet->data) + sizeof(TCPIPMessage), offspring->GetGenes(), size_t(offspring->GetGenomeLength()) * sizeof(offspring->GetGenes()[0]));
                        enet_uint8 channelID = 0;
                        enet_peer_send(event.peer, channelID, packet);
                        enet_host_flush(server);
                        ReportProgress("A packet of length "s +std::to_string(packet->dataLength) + " was sent to "s + hostURL + " on channel "s + std::to_string(channelID), 2);
                        std::unique_ptr<RunSpecifier> runSpecifier = std::make_unique<RunSpecifier>();
                        runSpecifier->genome = std::move(offspring);
                        runSpecifier->startTime =  time(nullptr);
                        runningList[submitCount] = std::move(runSpecifier);
                        ReportProgress("Sample "s + std::to_string(submitCount) + " sent to "s + hostURL, 2);
                        submitCount++;
                    }

                    if (strcmp(tcpMessage->text, "reqxml") == 0) // the client wants a new master xml file to work with
                    {
                        ReportProgress("A reqxml packet of length "s + std::to_string(event.packet->dataLength) + " was received from "s + hostURL + " on channel "s + std::to_string(event.channelID), 2);
                        /* Create a reliable packet of required size */
                        size_t packetSize = sizeof(TCPIPMessage) + m_baseXMLFile.GetSize();
                        ENetPacket *packet = enet_packet_create(nullptr, packetSize, ENET_PACKET_FLAG_RELIABLE);
                        TCPIPMessage *outTcpMessage = reinterpret_cast<TCPIPMessage *>(packet->data);
                        strcpy(outTcpMessage->text, "reqxml");
                        outTcpMessage->xmlLength = uint32_t(m_baseXMLFile.GetSize());
                        outTcpMessage->runID = submitCount;
                        std::copy(std::begin(m_md5), std::end(m_md5), std::begin(outTcpMessage->md5));
                        outTcpMessage->senderIP = server->address.host;
                        outTcpMessage->senderPort = server->address.port;
                        outTcpMessage->score = 0;
                        memcpy(reinterpret_cast<char *>(packet->data) + sizeof(TCPIPMessage), m_baseXMLFile.GetRawData(), m_baseXMLFile.GetSize());
                        enet_uint8 channelID = 0;
                        enet_peer_send(event.peer, channelID, packet);
                        enet_host_flush(server);
                        ReportProgress("A packet of length "s +std::to_string(packet->dataLength) + " was sent to "s + hostURL + " on channel "s + std::to_string(channelID), 2);
                    }

                    if (strcmp(tcpMessage->text, "result") == 0) // the client has a result for us
                    {
                        ReportProgress("A result packet of length "s +std::to_string(event.packet->dataLength) + " was sent to "s + hostURL + " on channel "s + std::to_string(event.channelID), 2);
#ifdef USE_QT_AGA
                        if (returnCount % 100 == 0) QMetaObject::invokeMethod(m_callingWidget, "setResultNumber", Qt::QueuedConnection, Q_ARG(int, returnCount));
#endif
                        uint32_t index = tcpMessage->runID;
                        double result = tcpMessage->score;
                        auto iter = runningList.find(index);
                        if (iter != runningList.end())
                        {
                            ReportProgress("Sample "s + std::to_string(index) + " score "s + std::to_string(result), 2);
                            iter->second->genome->SetFitness(result);
                            thePopulation->InsertGenome(iter->second->genome.get(), g_prefs->populationSize);
                            iter->second->genome.release(); // ownership of the genome has passed to thePopulation
                            runningList.erase(iter);
                            // OUT_VAR(thePopulation->GetPopulationSize());

                            if (returnCount % g_prefs->outputStatsEvery == g_prefs->outputStatsEvery - 1)
                            {
                                // std::cerr << "main returnCount = " << returnCount << " outputStatsEvery = " << g_prefs->outputStatsEvery << "\n";
                                TenPercentiles tenPercentiles;
                                CalculateTenPercentiles(thePopulation, &tenPercentiles);
                                *outFile << std::setw(10) << returnCount << " ";
                                *outFile << tenPercentiles << "\n";
                                outFile->flush();
                            }

                            if (returnCount % g_prefs->saveBestEvery == g_prefs->saveBestEvery - 1 || returnCount == 1)
                            {
                                // std::cerr << "main returnCount = " << returnCount << " saveBestEvery = " << g_prefs->saveBestEvery << "\n";
                                if (thePopulation->GetLastGenome()->GetFitness() > maxFitness)
                                {
                                    maxFitness = thePopulation->GetLastGenome()->GetFitness();
                                    char filename[0xffff];
                                    snprintf(filename, sizeof(filename), "%s/BestGenome_%012d.txt", dirname.c_str(), returnCount);
                                    std::ofstream bestFile(filename);
                                    bestFile << *thePopulation->GetLastGenome();
                                    bestFile.close();
                                    if (g_prefs->onlyKeepBestGenome)
                                    {
                                        if (lastBestGenomeFile.size()) unlink(lastBestGenomeFile.c_str());
                                    }
                                    lastBestGenomeFile = filename;
#ifdef USE_QT_AGA
                                    QMetaObject::invokeMethod(m_callingWidget, "setBestScore", Qt::QueuedConnection, Q_ARG(double, maxFitness));
#endif
                                }
                            }

                            if (returnCount % g_prefs->savePopEvery == g_prefs->savePopEvery - 1 || returnCount == 1)
                            {
                                // std::cerr << "main returnCount = " << returnCount << " savePopEvery = " << g_prefs->savePopEvery << "\n";
                                char filename[0xffff];
                                snprintf(filename, sizeof(filename), "%s/Population_%012d.txt", dirname.c_str(), returnCount);
                                thePopulation->WritePopulation(filename, g_prefs->outputPopulationSize);
                                if (g_prefs->onlyKeepBestPopulation)
                                {
                                    if (lastBestPopulationFile.size()) unlink(lastBestPopulationFile.c_str());
                                }
                                lastBestPopulationFile = filename;
                            }

                            if (returnCount % g_prefs->improvementReproductions == g_prefs->improvementReproductions - 1)
                            {
                                ReportProgress("Fitness change for "s +std::to_string(g_prefs->improvementReproductions) + " reproductions is "s + std::to_string(maxFitness - lastMaxFitness), 2);
                                // std::cerr << "main returnCount = " << returnCount << " improvementReproductions = " << g_prefs->improvementReproductions << "\n";
                                if (maxFitness - lastMaxFitness < g_prefs->improvementThreshold) stopSendingFlag = true; // it will now quit
                                lastMaxFitness = maxFitness;
                            }

                            returnCount++;
                        }
                    }
                }
                /* Clean up the packet now that we're done using it. */
                enet_packet_destroy(event.packet);
            }
        }
    }

    if (returnCount > 0)
    {
        returnCount--; // reduce return count back to the value for the last actual return

        if (thePopulation->GetLastGenome()->GetFitness() > maxFitness)
        {
            char filename[0xffff];
            snprintf(filename, sizeof(filename), "%s/BestGenome_%012d.txt", dirname.c_str(), returnCount);
            if (lastBestGenomeFile != filename)
            {
                std::ofstream bestFile(filename);
                bestFile << *thePopulation->GetLastGenome();
                bestFile.close();
                if (g_prefs->onlyKeepBestGenome)
                {
                    if (lastBestGenomeFile.size()) unlink(lastBestGenomeFile.c_str());
                }
            }
        }

        char filename[0xffff];
        snprintf(filename, sizeof(filename), "%s/Population_%012d.txt", dirname.c_str(), returnCount);
        if (lastBestPopulationFile != filename)
        {
            thePopulation->WritePopulation(filename, g_prefs->outputPopulationSize);
            if (g_prefs->onlyKeepBestPopulation)
            {
                if (lastBestPopulationFile.size()) unlink(lastBestPopulationFile.c_str());
            }
        }
    }

    enet_host_destroy(server);
    return 0;
}

AsynchronousGAQtWidget *GAMain::callingWidget() const
{
    return m_callingWidget;
}

void GAMain::setCallingWidget(AsynchronousGAQtWidget *callingWidget)
{
    m_callingWidget = callingWidget;
}

void GAMain::ApplyGenome(const std::string &inputGenome, const std::string &inputXML, const std::string &outputXML)
{
    DataFile genomeData;
    double val;
    int ival, genomeSize;
    genomeData.ReadFile(inputGenome);
    genomeData.ReadNext(&ival);
    genomeData.ReadNext(&genomeSize);
    std::vector<double> data;
    data.reserve(size_t(genomeSize));
    for (int i = 0; i < genomeSize; i++)
    {
        genomeData.ReadNext(&val);
        data.push_back(val);
        genomeData.ReadNext(&val); genomeData.ReadNext(&val); genomeData.ReadNext(&val);
        if (ival == -2) genomeData.ReadNext(&val); // skip the extra parameter
    }

    XMLConverter myXMLConverter;
    myXMLConverter.LoadBaseXMLFile(inputXML.c_str());
    myXMLConverter.ApplyGenome(genomeSize, data.data());
    size_t len;
    char *buf = reinterpret_cast<char *>(myXMLConverter.GetFormattedXML(&len));

    DataFile outputXMLData;
    outputXMLData.SetRawData(buf, len);
    outputXMLData.WriteFile(outputXML);
}

void GAMain::SetServerPort(int port)
{
    g_prefs->tcpPort = port;
}

std::string GAMain::ConvertAddressPortToString(uint32_t address, uint16_t port)
{
    std::string hostURL = (std::to_string(address & 0xff) + "."s +
                           std::to_string((address >> 8) & 0xff) + "."s +
                           std::to_string((address >> 16) & 0xff) + "."s +
                           std::to_string(address >> 24) +":"s +
                           std::to_string(port));
    return hostURL;
}

void GAMain::ReportProgress(const std::string &message, int logLevel)
{
    std::cerr << message << "\n";
#ifdef USE_QT_AGA
    if (m_logLevel >= logLevel && m_callingWidget)
    {
        QString qMessage = QString::fromStdString(message);
        QMetaObject::invokeMethod(m_callingWidget, "appendProgress", Qt::QueuedConnection, Q_ARG(QString, qMessage));
    }
#endif
}

// convert to string using printf style formatting and variable numbers of arguments
std::string GAMain::ToString(const char * const printfFormatString, ...)
{
    // initialize use of the variable argument array
    va_list vaArgs;
    va_start(vaArgs, printfFormatString);

    // reliably acquire the size
    // from a copy of the variable argument array
    // and a functionally reliable call to mock the formatting
    va_list vaArgsCopy;
    va_copy(vaArgsCopy, vaArgs);
    const int iLen = std::vsnprintf(NULL, 0, printfFormatString, vaArgsCopy);
    va_end(vaArgsCopy);

    // return a formatted string without risking memory mismanagement
    // and without assuming any compiler or platform specific behavior
    std::unique_ptr<char[]> zc = std::make_unique<char[]>(iLen + 1);
    std::vsnprintf(zc.get(), iLen + 1, printfFormatString, vaArgs);
    va_end(vaArgs);
    return std::string(zc.get(), iLen);
}


#endif

