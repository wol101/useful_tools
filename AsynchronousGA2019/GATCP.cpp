/*
 *  GAMain.cpp
 *  AsynchronousGA
 *
 *  Created by Bill Sellers on 19/10/2010.
 *  Copyright 2010 Bill Sellers. All rights reserved.
 *
 */

#ifdef USE_TCP

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
#include <algorithm>
#include <cstdarg>

#ifdef USE_QT_AGA
#include "AsynchronousGAQtWidget.h"
#include <QMetaObject>
#include <QString>
#endif

#include "pystring.h"

#include "TCP.h"
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

    TCPUpDown tcpUpDown;
    if (tcpUpDown.status())
    {
        std::cerr << "Error initialising TCP library\n";
        return __LINE__;
    }
    GAMain ga;
    ga.SetLogLevel(logLevel);
    ga.LoadBaseXMLFile(baseXMLFile);
    return ga.Process(parameterFile, outputDirectory, startingPopulation);
}
#endif

GAMain::GAMain()
{
}

GAMain::~GAMain()
{
}

int GAMain::Initialise()
{
    return 0;
}

int GAMain::Process(const std::string &parameterFile, const std::string &outputDirectory, const std::string &startingPopulation)

{
    time_t theTime = time(0);
    struct tm *theLocalTime = localtime(&theTime);
    Population startPopulation;
    Population evolvePopulation;
    std::ofstream outFile;
    std::string logFileName;
    std::string dirname;

    if (m_baseXMLFile.GetSize() == 0)
    {
        ReportProgress("Error: XML base file missing"s, 0);
        return __LINE__;
    }

    if (g_prefs->ReadPreferences(parameterFile))
    {
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
        ReportProgress("Error reading starting population: "s + g_prefs->startingPopulation, 0);
        return __LINE__;
    }
    ReportProgress(g_prefs->startingPopulation + " read"s, 0);

    if (startPopulation.GetPopulationSize() != g_prefs->populationSize)
    {
        ReportProgress("Info: Starting population size "s + std::to_string(startPopulation.GetPopulationSize()) + " does not match specified population size "s + std::to_string(g_prefs->populationSize) + " - using standard fixup"s, 0);
        if (startPopulation.ResizePopulation(g_prefs->populationSize))
        {
            ReportProgress("Error: Starting population size does not match specified population size - unable to fix"s, 0);
            return __LINE__;
        }
    }
    if (startPopulation.GetGenome(0)->GetGenomeLength() != g_prefs->genomeLength)
    {
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
    for (size_t i = 0; i < m_md5.size(); i++) m_md5[i] = md5Array[i];
    return 0;
}

int GAMain::Evolve(Population *startPopulation, Population *thePopulation, std::ostream *outFile, const std::string &dirname)
{
    // This is the asynchronous evolution loop
    int startPopulationIndex = 0;
    int submitCount = 0;
    int returnCount = 0;
    int parent1Rank, parent2Rank;
    int mutationCount;
    Genome *offspring;
    Genome *parent1;
    Genome *parent2;
    TenPercentiles tenPercentiles;
    std::string lastBestGenomeFile;
    std::string lastBestPopulationFile;
    double maxFitness = -DBL_MAX;
    double lastMaxFitness = -DBL_MAX;
    std::string filename;
    bool stopSendingFlag = false;
    std::map<int, std::unique_ptr<RunSpecifier>> runningList;
    std::map<std::string, time_t> hostTime;
    int numBytes;
    bool shouldStop = false;
    RequestMessage requestMessage;
    std::string ip;

    // start the TCP server
    TCP server;
    if (server.StartServer(g_prefs->tcpPort) != 0)
    {
        ReportProgress(ToString("Evolve: Error setting up server - port %d probably in use", g_prefs->tcpPort), 0);
        return __LINE__;
    }
    ReportProgress(ToString("TCP/IP port %d open", g_prefs->tcpPort), 1);
    TCPStopServerGuard stopServer(&server);

    int progressValue = 0;
    int lastProgressValue = -1;
    time_t lastTime = time(0);
    time_t lastSlowTime = time(0);
    int fastPeriodicTaskInterval = 1;
    int slowPeriodicTaskInterval = 100;
    int hostDeleteInterval = 600;
    int jobDeleteInterval = 600;
    while (returnCount < g_prefs->maxReproductions && stopSendingFlag == false)
    {
        time_t currentTime = time(0);
        if (currentTime >= lastTime + fastPeriodicTaskInterval) // this part of the loop is for things that don't need to be done all that often
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
        if (currentTime >= lastSlowTime + slowPeriodicTaskInterval) // this part of the loop is for things that don't need to be done all that often
        {
            lastSlowTime = currentTime;
            if (m_logLevel)
            {
                for (std::map<std::string, time_t>::iterator it = hostTime.begin(); it != hostTime.end();)
                {
                    if (currentTime - it->second > hostDeleteInterval)
                    {
                        ReportProgress(ToString("Client %s has disconnected", it->first.c_str()), 1);
                        it = hostTime.erase(it); // erase invalidates the iterator but returns the next valid iterator
                    }
                    else { it++; }
                }
            }
            for (std::map<int, std::unique_ptr<RunSpecifier>>::iterator it = runningList.begin(); it != runningList.end();)
            {
                if (currentTime - it->second->startTime > jobDeleteInterval)
                {
                    ReportProgress(ToString("RunID %d has been deleted", it->first), 2);
                    it = runningList.erase(it); // erase invalidates the iterator but returns the next valid iterator
                }
                else { it++; }
            }
        }
        // first check to see if there are connection requests
        TCP connection;
        if (server.CheckReceiver(0, 1000))
        {
#if defined(_WIN32) || defined(WIN32)
            if (connection.StartAcceptor(server.GetSocket()) == INVALID_SOCKET)
#else
            if (connection.StartAcceptor(server.GetSocket()) == -1)
#endif
            {
                ReportProgress("Error starting acceptor"s, 0);
                continue;
            }
            TCPStopAcceptorGuard stopAcceptor(&connection);
            uint32_t senderAddress, senderPort;
            connection.GetSenderAddress(&senderAddress, &senderPort);
            if (m_logLevel)
            {
                ip = ConvertAddressPortToString(senderAddress, senderPort);
                if (hostTime.find(ip) == hostTime.end())
                {
                    ReportProgress(ToString("Client %s has connected", ip.c_str()), 1);
                }
                hostTime[ip] = currentTime;
            }
            if (connection.CheckReceiver(5, 0)) // anything to receive?
            {
                // read the command from the sender
                numBytes = connection.ReceiveData(requestMessage.text, sizeof(RequestMessage), 10, 0);
                ReportProgress(ToString("Read %d bytes from %s: %s", numBytes, ip.c_str(), requestMessage.text), 2);
                if (numBytes != sizeof(RequestMessage))
                {
                    ReportProgress(ToString("Error receiving command from %s", ip.c_str()), 2);
                    continue;
                }

                if (strcmp(requestMessage.text, "req_xml") == 0)
                {
                    std::vector<char> dataMessage(sizeof(DataMessage) + m_baseXMLFile.GetSize() * sizeof(char));
                    DataMessage *dataMessagePtr = reinterpret_cast<DataMessage *>(dataMessage.data());
                    server.GetMyAddress(&dataMessagePtr->senderIP, &dataMessagePtr->senderPort);
                    dataMessagePtr->runID = submitCount;
                    dataMessagePtr->genomeLength = startPopulation->GetFirstGenome()->GetGenomeLength();
                    dataMessagePtr->xmlLength = uint32_t(m_baseXMLFile.GetSize());
                    std::copy(std::begin(m_md5), std::end(m_md5), std::begin(dataMessagePtr->md5));
                    std::copy_n(m_baseXMLFile.GetRawData(), m_baseXMLFile.GetSize(), dataMessagePtr->payload.xml);

                    numBytes = connection.SendData(dataMessage.data(), int(dataMessage.size()));
                    if (numBytes != int(dataMessage.size()))
                    {
                        ReportProgress(ToString("Error sending XML %d bytes to %s", int(dataMessage.size()), ip.c_str()), 2);
                        continue;
                    }
                    ReportProgress(ToString("XML %d bytes sent to %s", int(dataMessage.size()), ip.c_str()), 1);
                    continue;
                }

                if (strcmp(requestMessage.text, "req_genome") == 0)
                {
                    offspring = new Genome();
                    // if we are still working from the start population, just get the next one
                    if (startPopulationIndex < startPopulation->GetPopulationSize())
                    {
                        *offspring = *startPopulation->GetGenome(startPopulationIndex);
                        startPopulationIndex++;
                    }
                    else
                    {
                        // create a new offspring
                        mutationCount = 0;
                        while (mutationCount == 0) // this means we always get some mutation (no point in getting unmutated offspring)
                        {
                            if (thePopulation->GetPopulationSize()) parent1 = thePopulation->ChooseParent(&parent1Rank);
                            else parent1 = startPopulation->ChooseParent(&parent1Rank);
                            if (g_random->CoinFlip(g_prefs->crossoverChance))
                            {
                                if (thePopulation->GetPopulationSize()) parent2 = thePopulation->ChooseParent(&parent2Rank);
                                else parent2 = startPopulation->ChooseParent(&parent2Rank);
                                *offspring = *parent1;
                                mutationCount += Mate(parent1, parent2, offspring, g_prefs->crossoverType);
                            }
                            else
                            {
                                *offspring = *parent1;
                            }
                            if (g_prefs->multipleGaussian)  mutationCount += MultipleGaussianMutate(offspring, g_prefs->gaussianMutationChance);
                            else mutationCount += GaussianMutate(offspring, g_prefs->gaussianMutationChance);

                            mutationCount += FrameShiftMutate(offspring, g_prefs->frameShiftMutationChance);
                            mutationCount += DuplicationMutate(offspring, g_prefs->duplicationMutationChance);
                        }
                    }
                    // got a genome to send
                    std::vector<char> dataMessage(sizeof(DataMessage) + startPopulation->GetFirstGenome()->GetGenomeLength() * sizeof(double));
                    DataMessage *dataMessagePtr = reinterpret_cast<DataMessage *>(dataMessage.data());
                    server.GetMyAddress(&dataMessagePtr->senderIP, &dataMessagePtr->senderPort);
                    dataMessagePtr->runID = submitCount;
                    dataMessagePtr->genomeLength = startPopulation->GetFirstGenome()->GetGenomeLength();
                    dataMessagePtr->xmlLength = uint32_t(m_baseXMLFile.GetSize());
                    std::copy(std::begin(m_md5), std::end(m_md5), std::begin(dataMessagePtr->md5));
                    std::copy_n(offspring->GetGenes(), offspring->GetGenomeLength(), dataMessagePtr->payload.genome);
                    numBytes = connection.SendData(dataMessage.data(), int(dataMessage.size()));
                    if (numBytes != int(dataMessage.size()))
                    {
                        ReportProgress(ToString("Error sending genome %d bytes to %s", dataMessage.size(), ip.c_str()), 2);
                        continue;
                    }
                    ReportProgress(ToString("Sent genome %d bytes from %s", dataMessage.size(), ip.c_str()), 2);
                    std::unique_ptr<RunSpecifier> runSpecifier = std::make_unique<RunSpecifier>();
                    runSpecifier->genome = offspring;
                    runSpecifier->startTime = currentTime;
                    runSpecifier->senderPort = requestMessage.senderPort;
                    runSpecifier->senderIP = requestMessage.senderIP;
                    runningList[submitCount] = std::move(runSpecifier);
                    ReportProgress(ToString("Sample %d sent to %s", submitCount, ip.c_str()), 1);
                    submitCount++;
                    continue;
                }


                // (3) a client has an answer for us
                if (strcmp(requestMessage.text, "result") == 0)
                {
#ifdef USE_QT_AGA
                    if (returnCount % 100 == 0) QMetaObject::invokeMethod(m_callingWidget, "setResultNumber", Qt::QueuedConnection, Q_ARG(int, returnCount));
#endif
                    int index = requestMessage.runID;
                    double result = requestMessage.score;
                    ReportProgress(ToString("Sample %d score %g from %s", index, result, ip.c_str()), 2);
                    auto iter = runningList.find(index);
                    if (iter == runningList.end())
                    {
                        ReportProgress(ToString("Sample %d not found", index, result, ip.c_str()), 1);
                        continue;
                    }
                    iter->second->genome->SetFitness(result);
                    thePopulation->InsertGenome(iter->second->genome, g_prefs->populationSize);
                    runningList.erase(iter);

                    if (returnCount % g_prefs->outputStatsEvery == g_prefs->outputStatsEvery - 1)
                    {
                        CalculateTenPercentiles(thePopulation, &tenPercentiles);
                        *outFile << std::setw(10) << returnCount << " ";
                        *outFile << tenPercentiles << "\n";
                        outFile->flush();
                    }

                    if (returnCount % g_prefs->saveBestEvery == g_prefs->saveBestEvery - 1 || returnCount == 1)
                    {
                        if (thePopulation->GetLastGenome()->GetFitness() > maxFitness)
                        {
                            maxFitness = thePopulation->GetLastGenome()->GetFitness();
                            filename = ToString("%s/BestGenome_%012d.txt", dirname.c_str(), returnCount);
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

                    if (returnCount % g_prefs->savePopEvery == g_prefs->savePopEvery - 1 || returnCount == 0)
                    {
                        filename = ToString("%s/Population_%012d.txt", dirname.c_str(), returnCount);
                        thePopulation->WritePopulation(filename.c_str(), g_prefs->outputPopulationSize);
                        if (g_prefs->onlyKeepBestPopulation)
                        {
                            if (lastBestPopulationFile.size()) unlink(lastBestPopulationFile.c_str());
                        }
                        lastBestPopulationFile = filename;
                    }

                    if (returnCount % g_prefs->improvementReproductions == g_prefs->improvementReproductions - 1)
                    {
                        ReportProgress(ToString("Fitness change for %d reproductions is %g", g_prefs->improvementReproductions, maxFitness - lastMaxFitness), 2);
                        if ( maxFitness - lastMaxFitness < g_prefs->improvementThreshold ) stopSendingFlag = true; // it will now quit
                        lastMaxFitness = maxFitness;
                    }

                    returnCount++;
                    continue;
                }

                // (3) Unrecognised message
                ReportProgress(ToString("Unrecognised message: %s", requestMessage.text), 0);

            } // anything to receive block end
            ReportProgress(ToString("Client %s disconnected", ip.c_str()), 1);
        } // TCP connection goes out of scope
    }

    returnCount--; // reduce return count back to the value for the last actual return

    if (thePopulation->GetLastGenome()->GetFitness() > maxFitness)
    {
        filename = ToString("%s/BestGenome_%012d.txt", dirname.c_str(), returnCount);
        if (filename != lastBestGenomeFile)
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

    filename = ToString("%s/Population_%012d.txt", dirname.c_str(), returnCount);
    if (filename != lastBestPopulationFile)
    {
        thePopulation->WritePopulation(filename.c_str(), g_prefs->outputPopulationSize);
        if (g_prefs->onlyKeepBestPopulation)
        {
            if (lastBestPopulationFile.size()) unlink(lastBestPopulationFile.c_str());
        }
    }

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

std::string GAMain::ConvertAddressToString(uint32_t address)
{
    std::string hostURL = (std::to_string(address & 0xff) + "."s +
                           std::to_string((address >> 8) & 0xff) + "."s +
                           std::to_string((address >> 16) & 0xff) + "."s +
                           std::to_string(address >> 24));
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

