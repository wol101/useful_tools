/*
 *  GAMain.cpp
 *  AsynchronousGA
 *
 *  Created by Bill Sellers on 19/10/2010.
 *  Copyright 2010 Bill Sellers. All rights reserved.
 *
 */

#ifdef USE_ASIO

#include "Preferences.h"
#include "Genome.h"
#include "Population.h"
#include "Mating.h"
#include "Random.h"
#include "Statistics.h"
#include "GAASIO.h"
#include "XMLConverter.h"
#include "MD5.h"
#include "ServerASIO.h"

#include "pystring.h"

#ifdef USE_QT_AGA
#include "AsynchronousGAQtWidget.h"
#include <QMetaObject>
#include <QString>
#endif

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <fstream>
#include <sys/stat.h>
#include <sys/timeb.h>
#include <queue>
#include <map>
#include <cfloat>
#include <unistd.h>
#include <algorithm>
#include <cstdarg>
#include <thread>
#include <cinttypes>
#include <filesystem>
#include <regex>

using namespace std::string_literals;

class CloseFileGuard
{
public:
    CloseFileGuard(std::ofstream *file) { m_file = file; }
    ~CloseFileGuard() { if (m_file) m_file->close(); }
private:
    std::ofstream *m_file = nullptr;
};

#ifdef COMMAND_LINE_VERSION
int main(int argc, char *argv[])
{
    int i;
    char *parameterFile = 0;
    char *outputDirectory = 0;
    char *startingPopulation = 0;
    char *baseXMLFile = 0;
    int logLevel = 1;

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
        std::cerr << "(-s) --startingPopulation population_file\n";
        std::cerr << "(-b) --baseXMLFile model XML_config_file (required)\n";
        std::cerr << "(-l) --logLevel n\n";
        std::cerr << "(-d) --debug n\n";
        std::cerr << "\n";
        return (e);
    }

    GAMain ga(nullptr);
    ga.SetLogLevel(logLevel);
    ga.LoadBaseXMLFile(baseXMLFile);
    return ga.Process(parameterFile, outputDirectory, startingPopulation);
}
#endif

GAMain::GAMain(AsynchronousGAQtWidget *callingWidget)
{
    m_callingWidget = callingWidget;
}

int GAMain::Process(const std::string &parameterFile, const std::string &outputDirectory, const std::string &startingPopulation)

{
    time_t theTime = time(nullptr);
    struct tm *theLocalTime = localtime(&theTime);
    std::string logFileName;

    if (m_baseXMLFile.GetSize() == 0)
    {
        ReportProgress("Error: XML base file missing"s, 0);
        return __LINE__;
    }

    m_parameterFile = pystring::os::path::abspath(parameterFile, std::filesystem::current_path().u8string());
    if (g_prefs->ReadPreferences(m_parameterFile))
    {
        ReportProgress("Error reading "s + m_parameterFile, 0);
        return (__LINE__);
    }
    ReportProgress(m_parameterFile + " read"s, 0);

    // sanity check some of the parameters
    if (g_prefs->parentsToKeep >= g_prefs->populationSize)
    {
        ReportProgress("Error: parentsToKeep must be lower than populationSize"s, 0);
        return __LINE__;
    }

    // overwrite starting population if defined
    if (startingPopulation.size()) g_prefs->startingPopulation = startingPopulation;
    g_prefs->startingPopulation = pystring::os::path::abspath(g_prefs->startingPopulation, std::filesystem::current_path().u8string());

    // create a directory for all the output
    if (outputDirectory.size())
    {
        m_outputFolderName = pystring::os::path::abspath(outputDirectory, std::filesystem::current_path().u8string());
    }
    else
    {
        std::string timedFolder = ToString("Run_%04d-%02d-%02d_%02d.%02d.%02d",
                                           theLocalTime->tm_year + 1900,
                                           theLocalTime->tm_mon + 1,
                                           theLocalTime->tm_mday,
                                           theLocalTime->tm_hour,
                                           theLocalTime->tm_min,
                                           theLocalTime->tm_sec);
        m_outputFolderName = pystring::os::path::abspath(timedFolder, std::filesystem::current_path().u8string());
    }
    if (std::filesystem::exists(m_outputFolderName))
    {
        if (!std::filesystem::is_directory(std::filesystem::status(m_outputFolderName)))
        {
            ReportProgress(ToString("Error \"%s\" exists and is not a folder", m_outputFolderName.c_str()), 0);
            return __LINE__;
        }
    }
    else
    {
        try
        {
            std::filesystem::create_directories(m_outputFolderName);
        }
        catch (...)
        {
            ReportProgress("Error creating directory "s + m_outputFolderName, 0);
            return __LINE__;
        }
        ReportProgress(m_outputFolderName + " created"s, 0);
    }

    // write log
    logFileName = pystring::os::path::join(m_outputFolderName, "log.txt"s);
    m_outputLogFile.open(logFileName.c_str());
    if (m_outputLogFile.fail())
    {
        ReportProgress(ToString("Error opening \"%s\": %s", logFileName.c_str(), std::strerror(errno)), 0);
        return __LINE__;
    }
    CloseFileGuard closeFileGuard(&m_outputLogFile);
    m_outputLogFile << "GA build " << __DATE__ << " " << __TIME__ "\n";
    m_outputLogFile << "Log produced " << asctime(theLocalTime);
    m_outputLogFile << "parameterFile \"" << m_parameterFile << "\"\n";
    m_outputLogFile << *g_prefs << "\n";
    m_outputLogFile.flush();
    ReportProgress(logFileName + " opened"s, 0);

    // initialise the population
    m_startPopulation.SetGlobalCircularMutation(g_prefs->circularMutation);
    m_startPopulation.SetResizeControl(g_prefs->resizeControl);
    m_startPopulation.SetSelectionType(g_prefs->parentSelection);
    m_startPopulation.SetParentsToKeep(g_prefs->parentsToKeep);
    if (m_startPopulation.ReadPopulation(g_prefs->startingPopulation.c_str()))
    {
        ReportProgress("Error reading starting population: "s + g_prefs->startingPopulation, 0);
        return __LINE__;
    }
    ReportProgress(g_prefs->startingPopulation + " read"s, 0);

    if (m_startPopulation.GetPopulationSize() != g_prefs->populationSize)
    {
        ReportProgress("Info: Starting population size "s + std::to_string(m_startPopulation.GetPopulationSize()) + " does not match specified population size "s + std::to_string(g_prefs->populationSize) + " - using standard fixup"s, 0);
        if (m_startPopulation.ResizePopulation(g_prefs->populationSize))
        {
            ReportProgress("Error: Starting population size does not match specified population size - unable to fix"s, 0);
            return __LINE__;
        }
    }
    if (m_startPopulation.GetGenome(0)->GetGenomeLength() != g_prefs->genomeLength)
    {
        ReportProgress("Error: Starting population genome does not match specified genome length"s, 0);
        return __LINE__;
    }
    if (g_prefs->randomiseModel)
    {
        m_startPopulation.Randomise();
    }

    m_evolvePopulation.SetGlobalCircularMutation(g_prefs->circularMutation);
    m_evolvePopulation.SetResizeControl(g_prefs->resizeControl);
    m_evolvePopulation.SetSelectionType(g_prefs->parentSelection);
    m_evolvePopulation.SetParentsToKeep(g_prefs->parentsToKeep);

    if (Evolve())
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

int GAMain::Evolve()
{
    // This is the asynchronous evolution loop
    time_t evolveStartTime = time(nullptr);
    m_evolveIdentifier = evolveStartTime;
    uint32_t submitCount = 0;
    uint32_t returnCount = 0;
    int startPopulationIndex = 0;
    int parent1Rank, parent2Rank;
    int mutationCount;
    Genome *offspring;
    Genome *parent1;
    Genome *parent2;
    TenPercentiles tenPercentiles;
    double maxFitness = -DBL_MAX;
    double lastMaxFitness = -DBL_MAX;
    std::string filename;
    bool stopSendingFlag = false;
    std::map<uint32_t, std::unique_ptr<RunSpecifier>> runningList;
    std::map<std::string, time_t> hostTime;
 #ifdef USE_QT_AGA
    bool shouldStop = false;
#endif

    ReportProgress(ToString("evolveIdentifier = %" PRIu64, m_evolveIdentifier), 1);

    // start the TCP server
    ServerASIO *server = new ServerASIO();
    if (server->setPort(uint16_t(g_prefs->tcpPort)))
    {
        ReportProgress(ToString("Unable to set listening port to %d", g_prefs->tcpPort), 0);
        return __LINE__;
    }
    server->attach("req_gen_"s, std::bind(&GAMain::handleRequestGenome, this, std::placeholders::_1));
    server->attach("req_xml_"s, std::bind(&GAMain::handleRequestXML, this, std::placeholders::_1));
    server->attach("score___"s, std::bind(&GAMain::handleScore, this, std::placeholders::_1));
    std::thread *serverThread = new std::thread(&ServerASIO::start, server);
    StopServerASIOGuard serverGuard(server, serverThread);
    m_requestGenomeQueueEnabled = true;
    server->getLocalAddress(&m_ipAddress, &m_port);

    int progressValue = 0;
    int lastProgressValue = -1;
    time_t lastTime = evolveStartTime;
    time_t lastSlowTime = evolveStartTime;
    int fastPeriodicTaskInterval = 1;
    int slowPeriodicTaskInterval = 100;
    int hostDeleteInterval = 600;
    int jobDeleteInterval = 600;
    while (returnCount < uint32_t(g_prefs->maxReproductions) && stopSendingFlag == false)
    {
        time_t currentTime = time(nullptr);
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
                for (auto &&it = hostTime.begin(); it != hostTime.end();)
                {
                    if (currentTime - it->second > hostDeleteInterval)
                    {
                        ReportProgress(ToString("Client %s has disconnected", it->first.c_str()), 1);
                        it = hostTime.erase(it); // erase invalidates the iterator but returns the next valid iterator
                    }
                    else { it++; }
                }
            }
            for (auto &&it = runningList.begin(); it != runningList.end();)
            {
                if (currentTime - it->second->startTime > jobDeleteInterval)
                {
                    ReportProgress(ToString("RunID %" PRIu32 " has been deleted", it->first), 2);
                    it = runningList.erase(it); // erase invalidates the iterator but returns the next valid iterator
                }
                else { it++; }
            }
        }


        size_t genomeQueueSize = GenomeRequestQueueSize();
        if (genomeQueueSize)
        {
            MessageASIO message;
            GetNextGenomeRequest(&message);
            const RequestMessage *messageContent = reinterpret_cast<const RequestMessage *>(message.content.data());
            offspring = new Genome();
            // if we are still working from the start population, just get the next one
            if (startPopulationIndex < m_startPopulation.GetPopulationSize())
            {
                *offspring = *m_startPopulation.GetGenome(startPopulationIndex);
                startPopulationIndex++;
            }
            else
            {
                // create a new offspring
                mutationCount = 0;
                while (mutationCount == 0) // this means we always get some mutation (no point in getting unmutated offspring)
                {
                    if (m_evolvePopulation.GetPopulationSize()) parent1 = m_evolvePopulation.ChooseParent(&parent1Rank);
                    else parent1 = m_startPopulation.ChooseParent(&parent1Rank);
                    if (g_random->CoinFlip(g_prefs->crossoverChance))
                    {
                        if (m_evolvePopulation.GetPopulationSize()) parent2 = m_evolvePopulation.ChooseParent(&parent2Rank);
                        else parent2 = m_startPopulation.ChooseParent(&parent2Rank);
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
            std::vector<char> dataMessage(sizeof(DataMessage) + m_startPopulation.GetFirstGenome()->GetGenomeLength() * sizeof(double));
            DataMessage *dataMessagePtr = reinterpret_cast<DataMessage *>(dataMessage.data());
            strncpy(dataMessagePtr->text, "genome", 16);
//            server.GetMyAddress(&dataMessagePtr->senderIP, &dataMessagePtr->senderPort);
            dataMessagePtr->evolveIdentifier = m_evolveIdentifier;
            dataMessagePtr->runID = submitCount;
            dataMessagePtr->genomeLength = m_startPopulation.GetFirstGenome()->GetGenomeLength();
            dataMessagePtr->xmlLength = uint32_t(m_baseXMLFile.GetSize());
            std::copy(std::begin(m_md5), std::end(m_md5), std::begin(dataMessagePtr->md5));
            std::copy_n(offspring->GetGenes(), offspring->GetGenomeLength(), dataMessagePtr->payload.genome);
            auto sharedPtr = message.session.lock();
            if (sharedPtr)
            {
                sharedPtr->write(dataMessage.data(), dataMessage.size());
                std::unique_ptr<RunSpecifier> runSpecifier = std::make_unique<RunSpecifier>();
                runSpecifier->genome = offspring;
                runSpecifier->startTime = currentTime;
                runSpecifier->senderPort = messageContent->senderPort;
                runSpecifier->senderIP = messageContent->senderIP;
                runningList[submitCount] = std::move(runSpecifier);
                std::string address = ConvertAddressPortToString(messageContent->senderIP, uint16_t(messageContent->senderPort));
                ReportProgress(ToString("Sample %" PRIu32 " [%zu bytes] sent to %s evolveIdentifier %" PRIu64, submitCount, dataMessage.size(), address.c_str(), m_evolveIdentifier), 2);
                submitCount++;
            }
            else
            {
                ReportProgress(ToString("Sample %" PRIu32 " evolveIdentifier %" PRIu64 " unable to lock pointer", submitCount, m_evolveIdentifier), 1);
            }
            continue;
        }

        size_t scoreQueueSize = ScoreQueueSize();
        if (scoreQueueSize)
        {
            MessageASIO message;
            GetNextScore(&message);
            const RequestMessage *messageContent = reinterpret_cast<const RequestMessage *>(message.content.data());
#ifdef USE_QT_AGA
            if (returnCount % 100 == 0) QMetaObject::invokeMethod(m_callingWidget, "setResultNumber", Qt::QueuedConnection, Q_ARG(int, returnCount));
#endif
            uint32_t index = messageContent->runID;
            double result = messageContent->score;
            std::string address = ConvertAddressPortToString(messageContent->senderIP, messageContent->senderPort);
            ReportProgress(ToString("Sample %" PRIu32 " score %g from %s evolveIdentifier %" PRIu64, index, result, address.c_str(), messageContent->evolveIdentifier), 2);
            auto iter = runningList.find(index);
            if (messageContent->evolveIdentifier != m_evolveIdentifier || iter == runningList.end())
            {
                ReportProgress(ToString("Sample %" PRIu32 " not found score %g from %s evolveIdentifier %" PRIu64, index, result, address.c_str(), messageContent->evolveIdentifier), 1);
                continue;
            }
            iter->second->genome->SetFitness(result);
            m_evolvePopulation.InsertGenome(iter->second->genome, g_prefs->populationSize);
            runningList.erase(iter);

            if (returnCount % uint32_t(g_prefs->outputStatsEvery) == uint32_t(g_prefs->outputStatsEvery) - 1)
            {
                CalculateTenPercentiles(&m_evolvePopulation, &tenPercentiles);
                m_outputLogFile << std::setw(10) << returnCount << " ";
                m_outputLogFile << tenPercentiles << "\n";
                m_outputLogFile.flush();
            }

            if (returnCount % uint32_t(g_prefs->saveBestEvery) == uint32_t(g_prefs->saveBestEvery) - 1 || returnCount == 1)
            {
                if (m_evolvePopulation.GetLastGenome()->GetFitness() > maxFitness)
                {
                    maxFitness = m_evolvePopulation.GetLastGenome()->GetFitness();
                    filename = pystring::os::path::join(m_outputFolderName, ToString(m_bestGenomeModel.c_str(), returnCount));
                    try
                    {
                        ReportProgress("Writing "s + filename, 1);
                        std::ofstream bestFile(filename);
                        bestFile << *m_evolvePopulation.GetLastGenome();
                    }
                    catch (...) { ReportProgress("Error writing "s + filename, 0); }
#ifdef USE_QT_AGA
                    QMetaObject::invokeMethod(m_callingWidget, "setBestScore", Qt::QueuedConnection, Q_ARG(double, maxFitness));
#endif
                }
            }

            if (returnCount % uint32_t(g_prefs->savePopEvery) == uint32_t(g_prefs->savePopEvery) - 1 || returnCount == 0)
            {
                filename = pystring::os::path::join(m_outputFolderName, ToString(m_bestPopulationModel.c_str(), returnCount));
                ReportProgress("Writing "s + filename, 1);
                int err = m_evolvePopulation.WritePopulation(filename.c_str(), g_prefs->outputPopulationSize);
                if (err) { ReportProgress("Error writing "s + filename, 0); }
            }

            if (returnCount % uint32_t(g_prefs->improvementReproductions) == uint32_t(g_prefs->improvementReproductions) - 1)
            {
                ReportProgress(ToString("Fitness change for %d reproductions is %g", g_prefs->improvementReproductions, maxFitness - lastMaxFitness), 2);
                if ( maxFitness - lastMaxFitness < g_prefs->improvementThreshold ) stopSendingFlag = true; // it will now quit
                lastMaxFitness = maxFitness;
            }

            returnCount++;
            continue;
        }

        if (!scoreQueueSize && !genomeQueueSize) { std::this_thread::sleep_for(std::chrono::microseconds(m_loopSleepTimeMicroSeconds)); }
    }

    returnCount--; // reduce return count back to the value for the last actual return
    ReportProgress(ToString("GA evolveIdentifier = %" PRIu64 " ended returnCount = %" PRIu32 "", m_evolveIdentifier, returnCount), 1);

    if (m_evolvePopulation.GetLastGenome()->GetFitness() > maxFitness)
    {
        filename = pystring::os::path::join(m_outputFolderName, ToString(m_bestGenomeModel.c_str(), returnCount));
        if (!std::filesystem::exists(filename))
        {
            try
            {
                ReportProgress("Writing final "s + filename, 1);
                std::ofstream bestFile(filename);
                bestFile << *m_evolvePopulation.GetLastGenome();
            }
            catch(...) { ReportProgress("Error writing "s + filename, 0); }
        }
    }

    filename = pystring::os::path::join(m_outputFolderName, ToString(m_bestPopulationModel.c_str(), returnCount));
    if (!std::filesystem::exists(filename))
    {
        int err = m_evolvePopulation.WritePopulation(filename.c_str(), g_prefs->outputPopulationSize);
        if (err) { ReportProgress("Error writing "s + filename, 0); }
    }

    if (g_prefs->onlyKeepBestGenome) OnlyKeepLastMatching(m_bestGenomeRegex);
    if (g_prefs->onlyKeepBestPopulation) OnlyKeepLastMatching(m_bestPopulationRegex);

    m_requestGenomeQueueEnabled = false;
    ClearGenomeRequestQueue();
    ClearScoreQueue();

    return 0;
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
    std::string hostURL;
    enum
    {
        O32_LITTLE_ENDIAN = 0x03020100ul,
        O32_BIG_ENDIAN = 0x00010203ul,
        O32_PDP_ENDIAN = 0x01000302ul,      /* DEC PDP-11 (aka ENDIAN_LITTLE_WORD) */
        O32_HONEYWELL_ENDIAN = 0x02030001ul /* Honeywell 316 (aka ENDIAN_BIG_WORD) */
    };
    const union { unsigned char bytes[4]; uint32_t value; } o32_host_order = { { 0, 1, 2, 3 } };

    if (o32_host_order.value == O32_BIG_ENDIAN)
    {
        hostURL = (std::to_string(address & 0xff) + "."s +
                   std::to_string((address >> 8) & 0xff) + "."s +
                   std::to_string((address >> 16) & 0xff) + "."s +
                   std::to_string(address >> 24) +":"s +
                   std::to_string(port));
    }
    if (o32_host_order.value == O32_LITTLE_ENDIAN)
    {
        hostURL = (std::to_string(address >> 24) +":"s +
                   std::to_string((address >> 16) & 0xff) + "."s +
                   std::to_string((address >> 8) & 0xff) + "."s +
                   std::to_string(address & 0xff) + "."s +
                   std::to_string(port));
    }
    return hostURL;
}

std::string GAMain::ConvertAddressToString(uint32_t address)
{
    std::string hostURL;
    enum
    {
        O32_LITTLE_ENDIAN = 0x03020100ul,
        O32_BIG_ENDIAN = 0x00010203ul,
        O32_PDP_ENDIAN = 0x01000302ul,      /* DEC PDP-11 (aka ENDIAN_LITTLE_WORD) */
        O32_HONEYWELL_ENDIAN = 0x02030001ul /* Honeywell 316 (aka ENDIAN_BIG_WORD) */
    };
    const union { unsigned char bytes[4]; uint32_t value; } o32_host_order = { { 0, 1, 2, 3 } };
    if (o32_host_order.value == O32_BIG_ENDIAN)
    {
        hostURL = (std::to_string(address & 0xff) + "."s +
                   std::to_string((address >> 8) & 0xff) + "."s +
                   std::to_string((address >> 16) & 0xff) + "."s +
                   std::to_string(address >> 24));
    }
    if (o32_host_order.value == O32_LITTLE_ENDIAN)
    {
        hostURL = (std::to_string(address >> 24) +":"s +
                   std::to_string((address >> 16) & 0xff) + "."s +
                   std::to_string((address >> 8) & 0xff) + "."s +
                   std::to_string(address & 0xff) + "."s);
    }
    return hostURL;
}

void GAMain::ReportProgress(const std::string &message, int logLevel)
{
    if (m_logLevel >= logLevel)
    {
        std::cerr << message << "\n";
#ifdef USE_QT_AGA
        if (m_callingWidget)
        {
            QString qMessage = QString::fromStdString(message);
            QMetaObject::invokeMethod(m_callingWidget, "appendProgress", Qt::QueuedConnection, Q_ARG(QString, qMessage));
        }
#endif
    }
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
    const int iLen = std::vsnprintf(nullptr, 0, printfFormatString, vaArgsCopy);
    va_end(vaArgsCopy);

    // return a formatted string without risking memory mismanagement
    // and without assuming any compiler or platform specific behavior
    std::unique_ptr<char[]> zc = std::make_unique<char[]>(iLen + 1);
    std::vsnprintf(zc.get(), size_t(iLen + 1), printfFormatString, vaArgs);
    va_end(vaArgs);
    return std::string(zc.get(), size_t(iLen));
}

void GAMain::handleRequestGenome(MessageASIO message)
{
    if (message.content.size() < sizeof(RequestMessage)) return;
    if (!m_requestGenomeQueueEnabled) return;
    if (auto sharedPtr1 = message.session.lock())
    {
        std::unique_lock<std::mutex> lock(m_requestGenomeMutex);
        for (auto &&it : m_requestGenomeQueue)
        {
            // check to see whether we already have a genome request from this session
            if (auto sharedPtr2 = it.session.lock())
            {
                if (sharedPtr1.get() == sharedPtr2.get()) return;
            }
        }
        m_requestGenomeQueue.push_back(message);
    }
}

void GAMain::handleRequestXML(MessageASIO message)
{
    if (message.content.size() < sizeof(RequestMessage)) return;
    const RequestMessage *messageContent = reinterpret_cast<const RequestMessage *>(message.content.data());
    std::vector<char> dataMessage(sizeof(DataMessage) + m_baseXMLFile.GetSize() * sizeof(char));
    DataMessage *dataMessagePtr = reinterpret_cast<DataMessage *>(dataMessage.data());
    strncpy(dataMessagePtr->text, "xml", 16);
    dataMessagePtr->senderIP = m_ipAddress[0] * 0xffffff + m_ipAddress[1] * 0xffff + m_ipAddress[2] * 0xff + m_ipAddress[3];
    dataMessagePtr->senderPort = m_port;
    dataMessagePtr->runID = std::numeric_limits<uint32_t>::max();
    dataMessagePtr->evolveIdentifier = m_evolveIdentifier;
    dataMessagePtr->genomeLength = m_startPopulation.GetFirstGenome()->GetGenomeLength();
    dataMessagePtr->xmlLength = uint32_t(m_baseXMLFile.GetSize());
    std::copy(std::begin(m_md5), std::end(m_md5), std::begin(dataMessagePtr->md5));
    std::copy_n(m_baseXMLFile.GetRawData(), m_baseXMLFile.GetSize(), dataMessagePtr->payload.xml);
    if (auto sharedPtr = message.session.lock())
        sharedPtr->write(dataMessage.data(), dataMessage.size());
    std::string address = ConvertAddressPortToString(messageContent->senderIP, messageContent->senderPort);
    ReportProgress(ToString("XML %zu bytes sent to %s", int(dataMessage.size()), address.c_str()), 2);
}

int GAMain::OnlyKeepLastMatching(const std::string &regexPattern)
{
    const std::regex regex(regexPattern);
    std::vector<std::string> directoryContents;
    try
    {
        for (auto &&entry : std::filesystem::directory_iterator(m_outputFolderName))
        {
            std::string basename = pystring::os::path::basename(entry.path().u8string());
            if (std::regex_match(basename, regex)) { directoryContents.push_back(entry.path().u8string()); }
        }
    }
    catch (...)
    {
        ReportProgress("Error reading output directory "s + m_outputFolderName, 0);
        return __LINE__;
    }
    if (directoryContents.size() == 0)
    {
        ReportProgress(ToString("Error: could not find \"%s\" in \"%s\"", regexPattern.c_str(), m_outputFolderName.c_str()), 0);
        return __LINE__;
    }
    std::sort(directoryContents.begin(), directoryContents.end());
    for (size_t i = 0; i < directoryContents.size() - 1; i++) // loop to all but last entry
    {
        try
        {
            ReportProgress("Removing "s + directoryContents[i], 1);
            std::filesystem::remove(directoryContents[i]);
        }
        catch (...)
        {
            ReportProgress("Error deleting "s + directoryContents[i], 0);
            return __LINE__;
        }
    }
    return 0;
}

void GAMain::handleScore(MessageASIO message)
{
    if (message.content.size() < sizeof(RequestMessage)) return;
    std::unique_lock<std::mutex> lock(m_scoreMutex);
    m_scoreQueue.push_back(message);
}

size_t GAMain::GenomeRequestQueueSize()
{
    std::unique_lock<std::mutex> lock(m_requestGenomeMutex);
    return (m_requestGenomeQueue.size());
}

size_t GAMain::ScoreQueueSize()
{
    std::unique_lock<std::mutex> lock(m_scoreMutex);
    return (m_scoreQueue.size());
}

void GAMain::GetNextGenomeRequest(MessageASIO *message)
{
    std::unique_lock<std::mutex> lock(m_requestGenomeMutex);
    *message = m_requestGenomeQueue.front();
    m_requestGenomeQueue.pop_front();
}

void GAMain::GetNextScore(MessageASIO *message)
{
    std::unique_lock<std::mutex> lock(m_scoreMutex);
    *message = m_scoreQueue.front();
    m_scoreQueue.pop_front();
}

void GAMain::ClearGenomeRequestQueue()
{
    std::unique_lock<std::mutex> lock(m_requestGenomeMutex);
    m_requestGenomeQueue.clear();
}

void GAMain::ClearScoreQueue()
{
    std::unique_lock<std::mutex> lock(m_scoreMutex);
    m_scoreQueue.clear();
}



#endif

