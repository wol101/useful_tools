#ifdef USE_TCP_PTYPES

/*
 *  GATCPptypes.cpp
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
#include <iomanip>
#include <fstream>
#include <sys/stat.h>
#include <sys/timeb.h>
#include <queue>
#include <map>
#include <float.h>
#include <unistd.h>

#include "ptypes.h"
#include "pinet.h"

#ifdef USE_QT_AGA
#include "AsynchronousGAQtWidget.h"
#include <QMetaObject>
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

#include "GATCPptypes.h"


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

    GAMain ga;
    ga.SetLogLevel(logLevel);
    ga.LoadBaseXMLFile(baseXMLFile);
    return ga.Process(parameterFile, outputDirectory, startingPopulation);
}
#endif

namespace AsynchronousGA
{

Preferences g_prefs;

GAMain::GAMain()
{
    m_baseXMLFile = 0;
    memset(m_md5, 0, sizeof(m_md5));
    m_logLevel = 0;
#ifdef USE_QT_AGA
    m_callingWidget = 0;
#endif
}

GAMain::~GAMain()
{
    if (m_baseXMLFile) delete m_baseXMLFile;
}

#ifdef _WIN32
int GAMain::Process(const wchar_t * const parameterFile, const wchar_t * const outputDirectory, const wchar_t * const startingPopulation)
#else
int GAMain::Process(const char * const parameterFile, const char * const outputDirectory, const char * const startingPopulation)
#endif
{
    char dirname[kBufferSize];
    char filename[kBufferSize];
    int error;
    struct stat sb;
    time_t theTime = time(0);
    struct tm *theLocalTime = localtime(&theTime);
    Population *startPopulation = new Population();
    Population *evolvePopulation = new Population();
    std::ofstream outFile;
    WindowsGlue wg;
#ifdef USE_QT_AGA
    QString message;
#endif

    if (g_prefs->ReadPreferences(parameterFile))
    {
        std::cerr << "Error reading " << wg.ToUTF8Tmp(parameterFile) << "\n";
        return (__LINE__);
    }
#ifdef USE_QT_AGA
    if (m_logLevel > 0)
    {
        message = QString("\"%1\" read").arg(QString::fromWCharArray(wg.ToWideTmp(parameterFile)));
        QMetaObject::invokeMethod(m_callingWidget, "appendProgress", Qt::QueuedConnection, Q_ARG(QString, message));
    }
#endif
    // overwrite starting population if defined
    if (startingPopulation && strlen(wg.ToUTF8Tmp(startingPopulation))) strcpy(g_prefs->startingPopulation, wg.ToUTF8Tmp(startingPopulation));

    // create a directory for all the output
    if (outputDirectory && strlen(wg.ToUTF8Tmp(outputDirectory)))
    {
        strcpy(dirname, wg.ToUTF8Tmp(outputDirectory));

        if (stat(dirname, &sb) == -1)
        {
#ifdef _WIN32
            error = mkdir(dirname);
#else
            error = mkdir(dirname, 0777);
#endif
            if (error)
            {
                std::cerr << "Error creating directory " << dirname << "\n";
                return __LINE__;
            }
        }
        else
        {
            if ((sb.st_mode & S_IFDIR) == 0)
            {
                std::cerr << "Error creating directory (exists but not a directory) " << dirname << "\n";
                return __LINE__;
            }
        }
    }
    else
    {
        sprintf(dirname, "Run_%04d-%02d-%02d_%02d.%02d.%02d",
                theLocalTime->tm_year + 1900, theLocalTime->tm_mon + 1,
                theLocalTime->tm_mday,
                theLocalTime->tm_hour, theLocalTime->tm_min,
                theLocalTime->tm_sec);
#ifdef _WIN32
            error = mkdir(dirname);
#else
            error = mkdir(dirname, 0777);
#endif
        if (error)
        {
            std::cerr << "Error creating directory " << dirname << "\n";
            return __LINE__;
        }
    }
#ifdef USE_QT_AGA
    if (m_logLevel > 0)
    {
        message = QString("\"%1\" created").arg(QString::fromWCharArray(wg.ToWideTmp(dirname)));
        QMetaObject::invokeMethod(m_callingWidget, "appendProgress", Qt::QueuedConnection, Q_ARG(QString, message));
    }
#endif

    // write log
    sprintf(filename, "%s/log.txt", dirname);
    outFile.open(filename);
    outFile << "GA build " << __DATE__ << " " << __TIME__ "\n";
    outFile << "Log produced " << asctime(theLocalTime);
    outFile << "parameterFile \"" << wg.ToUTF8Tmp(parameterFile) << "\"\n";
    outFile << g_prefs;
    outFile.flush();
#ifdef USE_QT_AGA
    if (m_logLevel > 0)
    {
        message = QString("\"%1\" opened").arg(QString::fromWCharArray(wg.ToWideTmp(filename)));
        QMetaObject::invokeMethod(m_callingWidget, "appendProgress", Qt::QueuedConnection, Q_ARG(QString, message));
    }
#endif

    // set the random seed
    RandomSeed(g_prefs->randomSeed);

    // initialise the population
    if (startPopulation->ReadPopulation(g_prefs->startingPopulation))
    {
        std::cerr << "Error reading starting population: " << g_prefs->startingPopulation << "\n";
        return __LINE__;
    }
#ifdef USE_QT_AGA
    if (m_logLevel > 0)
    {
        message = QString("\"%1\" read").arg(QString::fromWCharArray(wg.ToWideTmp(g_prefs->startingPopulation)));
        QMetaObject::invokeMethod(m_callingWidget, "appendProgress", Qt::QueuedConnection, Q_ARG(QString, message));
    }
#endif

    if (startPopulation->GetPopulationSize() != g_prefs->populationSize)
    {
        std::cerr << "Info: Starting population size does not match specified population size - using standard fixup\n";
#ifdef USE_QT_AGA
        if (m_logLevel > 0)
        {
            message = QString("Info: Starting population size %1 does not match specified population size %2 - using standard fixup").arg(startPopulation->GetPopulationSize()).arg(g_prefs->populationSize);
            QMetaObject::invokeMethod(m_callingWidget, "appendProgress", Qt::QueuedConnection, Q_ARG(QString, message));
        }
#endif
        if (startPopulation->ResizePopulation(g_prefs->populationSize))
        {
            std::cerr << "Error: Starting population size does not match specified population size - unable to fix\n";
            return __LINE__;
        }
    }
    if (startPopulation->GetGenome(0)->GetGenomeLength() != g_prefs->genomeLength)
    {
        std::cerr << "Error: Starting population genome does not match specified genome length\n";
        return __LINE__;
    }
    if (g_prefs->randomiseModel)
    {
        startPopulation->Randomise();
    }

    startPopulation->SetGlobalCircularMutation(g_prefs->circularMutation);
    startPopulation->SetResizeControl(g_prefs->resizeControl);

    if (gDebugControl == MainDebug) std::cerr << "main read startingPopulation " << g_prefs->startingPopulation << "\n";

    evolvePopulation->SetSelectionType(g_prefs->parentSelection);
    evolvePopulation->SetParentsToKeep(g_prefs->parentsToKeep);

    if (Evolve(startPopulation, evolvePopulation, &outFile, dirname))
    {
        std::cerr << "Terminated due to Evolve failure\n";
    }

    delete startPopulation;
    delete evolvePopulation;

    return 0;
}

#ifdef _WIN32
int GAMain::LoadBaseXMLFile(const wchar_t *filename)
#else
int GAMain::LoadBaseXMLFile(const char *filename)
#endif
{
    if (m_baseXMLFile) delete m_baseXMLFile;
    m_baseXMLFile = new DataFile();
    if (m_baseXMLFile->ReadFile(filename))
    {
        delete m_baseXMLFile;
        m_baseXMLFile = 0;
        memset(m_md5, 0, sizeof(m_md5));
        return 1;
    }
    memcpy(m_md5, md5(m_baseXMLFile->GetRawData(), m_baseXMLFile->GetSize() + 1), sizeof(m_md5)); // size + 1 to include the terminating zero
    return 0;
}

int GAMain::Evolve(Population *startPopulation, Population *thePopulation, std::ostream *outFile, const char *dirname)
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
    char lastBestGenomeFile[kBufferSize] = "";
    char lastBestPopulationFile[kBufferSize] = "";
    double maxFitness = -DBL_MAX;
    double lastMaxFitness = -DBL_MAX;
    char filename[kBufferSize];
    bool stopSendingFlag = false;
    std::unique_ptr<char[]> commsBuffer(new char[kBigBufferSize]);
    TCPIPMessage *tcpMessage = (TCPIPMessage *)commsBuffer.get();
    struct RunSpecifier
    {
        Genome *genome;
        time_t startTime;
    };
    std::map<int, RunSpecifier *>runningList;
    std::map<std::string, time_t>hostTime;
    int numBytes;
#ifdef USE_QT_AGA
    QString message;
    bool shouldStop = false;
#endif

    // start the TCP server
    int socketNum;
    pt::ipstmserver svr;
    try
    {
        // bind to all local addresses on port 8085
        socketNum = svr.bindall(g_prefs->tcpPort);
    }
    catch(pt::estream* e)
    {
#ifdef USE_QT_AGA
        message = QString("ptypes bindall exception: %1").arg(pt::pconst(e->get_message()));
        QMetaObject::invokeMethod(m_callingWidget, "appendProgress", Qt::QueuedConnection, Q_ARG(QString, message));
#endif
        std::cerr << "ptypes bindall exception: " << pt::pconst(e->get_message()) << "\n";
        delete e;
        return __LINE__;
    }

#ifdef USE_QT_AGA
    if (m_logLevel > 0)
    {
        message = QString("TCP/IP port %1 open").arg(g_prefs->tcpPort);
        QMetaObject::invokeMethod(m_callingWidget, "appendProgress", Qt::QueuedConnection, Q_ARG(QString, message));
    }
#endif

    int progressValue = 0;
    int lastProgressValue = -1;
    time_t lastTime = time(0);
    int periodicTaskInterval = 1;
    int hostDeleteInterval = 360;
    while (returnCount < g_prefs->maxReproductions && stopSendingFlag == false)
    {
        time_t currentTime = time(0);
        if (currentTime >= lastTime + periodicTaskInterval) // this part of the loop is for things that don't need to be done all that often
        {
            lastTime = currentTime;
#ifdef USE_QT_AGA
            QMetaObject::invokeMethod(m_callingWidget, "logLevel", Qt::DirectConnection, Q_RETURN_ARG(int, m_logLevel));
            QMetaObject::invokeMethod(m_callingWidget, "tryToStopGA", Qt::DirectConnection, Q_RETURN_ARG(bool, shouldStop));
            if (shouldStop)
            {
                if (m_logLevel > 0)
                {
                    message = QString("Stopped by user");
                    QMetaObject::invokeMethod(m_callingWidget, "appendProgress", Qt::QueuedConnection, Q_ARG(QString, message));
                }
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
            for (std::map<std::string, time_t>::iterator it = hostTime.begin(); it != hostTime.end();)
            {
                if (lastTime - it->second > hostDeleteInterval)
                {
#ifdef USE_QT_AGA
                    if (m_logLevel > 0)
                    {
                        message = QString("Client %1 has disconnected").arg(it->first.c_str());
                        QMetaObject::invokeMethod(m_callingWidget, "appendProgress", Qt::QueuedConnection, Q_ARG(QString, message));
                    }
#endif
                    it = hostTime.erase(it); // erase invalidates the iterator but returns the next valid iterator
                }
                else
                {
                    it++;
                }
            }
        }
        // first check to see if there are connection requests
        bool st;
        try { st = svr.poll(socketNum, 0); }
        catch(pt::estream* e)
        {
    #ifdef USE_QT_AGA
            message = QString("ptypes poll exception: %1").arg(pt::pconst(e->get_message()));
            QMetaObject::invokeMethod(m_callingWidget, "appendProgress", Qt::QueuedConnection, Q_ARG(QString, message));
    #endif
            std::cerr << "ptypes poll exception: " << pt::pconst(e->get_message()) << "\n";
            delete e;
            continue;
        }
        if (st)
        {
            pt::ipstream client;
            try { st = svr.serve(client, socketNum, 0); }
            catch(pt::estream* e)
            {
        #ifdef USE_QT_AGA
                message = QString("ptypes serve exception: %1").arg(pt::pconst(e->get_message()));
                QMetaObject::invokeMethod(m_callingWidget, "appendProgress", Qt::QueuedConnection, Q_ARG(QString, message));
        #endif
                std::cerr << "ptypes serve exception: " << pt::pconst(e->get_message()) << "\n";
                delete e;
                continue;
            }
            if (st == false)
            {
                std::cerr << "Trouble attaching client to socket\n";
                continue;
            }
            pt::string ip;
            try { ip = client.get_host(); }
            catch(pt::estream* e)
            {
        #ifdef USE_QT_AGA
                message = QString("ptypes get_host exception: %1").arg(pt::pconst(e->get_message()));
                QMetaObject::invokeMethod(m_callingWidget, "appendProgress", Qt::QueuedConnection, Q_ARG(QString, message));
        #endif
                std::cerr << "ptypes get_host exception: " << pt::pconst(e->get_message()) << "\n";
                delete e;
                continue;
            }
            if (hostTime.find((const char *)ip) == hostTime.end())
            {
#ifdef USE_QT_AGA
                if (m_logLevel > 0)
                {
                    message = QString("Client %1 has connected").arg((const char *)ip);
                    QMetaObject::invokeMethod(m_callingWidget, "appendProgress", Qt::QueuedConnection, Q_ARG(QString, message));
                }
#endif
            }
            hostTime[(const char *)ip] = time(0);
            try { st = client.waitfor(100); }
            catch(pt::estream* e)
            {
        #ifdef USE_QT_AGA
                message = QString("ptypes waitfor exception: %1").arg(pt::pconst(e->get_message()));
                QMetaObject::invokeMethod(m_callingWidget, "appendProgress", Qt::QueuedConnection, Q_ARG(QString, message));
        #endif
                std::cerr << "ptypes waitfor exception: " << pt::pconst(e->get_message()) << "\n";
                delete e;
                continue;
            }
            if (st) // anything to receive?
            {
                // read the command from the sender
                try { numBytes = client.read(commsBuffer.get(), TCPIPMessage::StandardMessageSize); }
                catch(pt::estream* e)
                {
            #ifdef USE_QT_AGA
                    message = QString("ptypes read exception: %1").arg(pt::pconst(e->get_message()));
                    QMetaObject::invokeMethod(m_callingWidget, "appendProgress", Qt::QueuedConnection, Q_ARG(QString, message));
            #endif
                    std::cerr << "ptypes read exception: " << pt::pconst(e->get_message()) << "\n";
                    delete e;
                    continue;
                }
#ifdef USE_QT_AGA
                if (m_logLevel > 1)
                {
                    message = QString("Read %1 bytes from %2: %3").arg(numBytes).arg((const char *)ip).arg(commsBuffer.get(), numBytes);
                    QMetaObject::invokeMethod(m_callingWidget, "appendProgress", Qt::QueuedConnection, Q_ARG(QString, message));
                }
#endif
                // a client wants some data to work with
                if (numBytes == TCPIPMessage::StandardMessageSize && (strcmp(commsBuffer.get(), "req_send_length") == 0 || strcmp(commsBuffer.get(), "req_xml_length") == 0))
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
                            if (CoinFlip(g_prefs->crossoverChance))
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
                    // is xml wanted?
                    if (strcmp(commsBuffer.get(), "req_xml_length") == 0 && m_baseXMLFile)
                    {
                        int len = m_baseXMLFile->GetSize() + 1; // because we want the terminating 0 too
                        tcpMessage->length = len;
                        try { numBytes = client.write((char *)tcpMessage, TCPIPMessage::StandardMessageSize); }
                        catch(pt::estream* e)
                        {
                    #ifdef USE_QT_AGA
                            message = QString("ptypes write 1 exception: %1").arg(pt::pconst(e->get_message()));
                            QMetaObject::invokeMethod(m_callingWidget, "appendProgress", Qt::QueuedConnection, Q_ARG(QString, message));
                    #endif
                            std::cerr << "ptypes write 1 exception: " << pt::pconst(e->get_message()) << "\n";
                            delete e;
                            continue;
                        }
                        try { client.flush(); }
                        catch(pt::estream* e)
                        {
                    #ifdef USE_QT_AGA
                            message = QString("ptypes read 1 exception: %1").arg(pt::pconst(e->get_message()));
                            QMetaObject::invokeMethod(m_callingWidget, "appendProgress", Qt::QueuedConnection, Q_ARG(QString, message));
                    #endif
                            std::cerr << "ptypes read 1 exception: " << pt::pconst(e->get_message()) << "\n";
                            delete e;
                            continue;
                        }
                        if (numBytes != TCPIPMessage::StandardMessageSize) continue;
                        try { st = client.waitfor(100); }
                        catch(pt::estream* e)
                        {
                    #ifdef USE_QT_AGA
                            message = QString("ptypes waitfor exception: %1").arg(pt::pconst(e->get_message()));
                            QMetaObject::invokeMethod(m_callingWidget, "appendProgress", Qt::QueuedConnection, Q_ARG(QString, message));
                    #endif
                            std::cerr << "ptypes waitfor exception: " << pt::pconst(e->get_message()) << "\n";
                            delete e;
                            continue;
                        }
                        if (st == false) continue;
                        try { numBytes = client.read(commsBuffer.get(), TCPIPMessage::StandardMessageSize); }
                        catch(pt::estream* e)
                        {
                    #ifdef USE_QT_AGA
                            message = QString("ptypes read 2 exception: %1").arg(pt::pconst(e->get_message()));
                            QMetaObject::invokeMethod(m_callingWidget, "appendProgress", Qt::QueuedConnection, Q_ARG(QString, message));
                    #endif
                            std::cerr << "ptypes read 2 exception: " << pt::pconst(e->get_message()) << "\n";
                            delete e;
                            continue;
                        }
#ifdef USE_QT_AGA
                        if (m_logLevel > 1)
                        {
                            if (numBytes) message = QString("Read %1 bytes from %2: %3").arg(numBytes).arg((const char *)ip).arg(commsBuffer.get());
                            else message = QString("Read %1 bytes from %2").arg(numBytes).arg((const char *)ip);
                            QMetaObject::invokeMethod(m_callingWidget, "appendProgress", Qt::QueuedConnection, Q_ARG(QString, message));
                        }
#endif
                        if (numBytes == TCPIPMessage::StandardMessageSize && strcmp(commsBuffer.get(), "req_xml_data") == 0)
                        {
                            try { numBytes = client.write(m_baseXMLFile->GetRawData(), len); }
                            catch(pt::estream* e)
                            {
                        #ifdef USE_QT_AGA
                                message = QString("ptypes write 2 exception: %1").arg(pt::pconst(e->get_message()));
                                QMetaObject::invokeMethod(m_callingWidget, "appendProgress", Qt::QueuedConnection, Q_ARG(QString, message));
                        #endif
                                std::cerr << "ptypes write 2 exception: " << pt::pconst(e->get_message()) << "\n";
                                delete e;
                                continue;
                            }
                            try { client.flush(); }
                            catch(pt::estream* e)
                            {
                        #ifdef USE_QT_AGA
                                message = QString("ptypes flush exception: %1").arg(pt::pconst(e->get_message()));
                                QMetaObject::invokeMethod(m_callingWidget, "appendProgress", Qt::QueuedConnection, Q_ARG(QString, message));
                        #endif
                                std::cerr << "ptypes flush exception: " << pt::pconst(e->get_message()) << "\n";
                                delete e;
                                continue;
                            }
                            if (numBytes != len) continue;
                        else
                            continue;
                        }
                    }
                    // this should be the genome data request
                    if (strcmp(commsBuffer.get(), "req_send_length") == 0)
                    {
                        int len = offspring->GetGenomeLength() * sizeof(double);
                        tcpMessage->length = len;
                        tcpMessage->runID = submitCount;
                        memcpy(tcpMessage->md5, m_md5, sizeof(m_md5));
                        try { numBytes = client.write((char *)tcpMessage, TCPIPMessage::StandardMessageSize); }
                        catch(pt::estream* e)
                        {
                    #ifdef USE_QT_AGA
                            message = QString("ptypes write 3 exception: %1").arg(pt::pconst(e->get_message()));
                            QMetaObject::invokeMethod(m_callingWidget, "appendProgress", Qt::QueuedConnection, Q_ARG(QString, message));
                    #endif
                            std::cerr << "ptypes write 3 exception: " << pt::pconst(e->get_message()) << "\n";
                            delete e;
                            continue;
                        }
                        try { client.flush(); }
                        catch(pt::estream* e)
                        {
                    #ifdef USE_QT_AGA
                            message = QString("ptypes flush exception: %1").arg(pt::pconst(e->get_message()));
                            QMetaObject::invokeMethod(m_callingWidget, "appendProgress", Qt::QueuedConnection, Q_ARG(QString, message));
                    #endif
                            std::cerr << "ptypes flush exception: " << pt::pconst(e->get_message()) << "\n";
                            delete e;
                            continue;
                        }
                        if (numBytes != TCPIPMessage::StandardMessageSize) continue;
                        try { st = client.waitfor(100); }
                        catch(pt::estream* e)
                        {
                    #ifdef USE_QT_AGA
                            message = QString("ptypes waitfor exception: %1").arg(pt::pconst(e->get_message()));
                            QMetaObject::invokeMethod(m_callingWidget, "appendProgress", Qt::QueuedConnection, Q_ARG(QString, message));
                    #endif
                            std::cerr << "ptypes waitfor exception: " << pt::pconst(e->get_message()) << "\n";
                            delete e;
                            continue;
                        }
                        if (st == false) continue;
                        try { numBytes = client.read(commsBuffer.get(), TCPIPMessage::StandardMessageSize); }
                        catch(pt::estream* e)
                        {
                    #ifdef USE_QT_AGA
                            message = QString("ptypes read 4 exception: %1").arg(pt::pconst(e->get_message()));
                            QMetaObject::invokeMethod(m_callingWidget, "appendProgress", Qt::QueuedConnection, Q_ARG(QString, message));
                    #endif
                            std::cerr << "ptypes read 4 exception: " << pt::pconst(e->get_message()) << "\n";
                            delete e;
                            continue;
                        }
#ifdef USE_QT_AGA
                        if (m_logLevel > 1)
                        {
                            if (numBytes) message = QString("len=%1 submitCount=%2 md5=%3\nRead %4 bytes from %5: %6").arg(len).arg(submitCount).arg(hexDigest(m_md5)).arg(numBytes).arg((const char *)ip).arg(commsBuffer.get());
                            else message = QString("len=%1 submitCount=%2 md5=%3\nRead %4 bytes from %5").arg(len).arg(submitCount).arg(hexDigest(m_md5)).arg(numBytes).arg((const char *)ip);
                            QMetaObject::invokeMethod(m_callingWidget, "appendProgress", Qt::QueuedConnection, Q_ARG(QString, message));
                        }
#endif
                        if (numBytes == TCPIPMessage::StandardMessageSize && strcmp(commsBuffer.get(), "req_send_data") == 0)
                        {
                            try { numBytes = client.write((char *)offspring->GetGenes(), len); }
                            catch(pt::estream* e)
                            {
                        #ifdef USE_QT_AGA
                                message = QString("ptypes write 4 exception: %1").arg(pt::pconst(e->get_message()));
                                QMetaObject::invokeMethod(m_callingWidget, "appendProgress", Qt::QueuedConnection, Q_ARG(QString, message));
                        #endif
                                std::cerr << "ptypes write 4 exception: " << pt::pconst(e->get_message()) << "\n";
                                delete e;
                                continue;
                            }
                            try { client.flush(); }
                            catch(pt::estream* e)
                            {
                        #ifdef USE_QT_AGA
                                message = QString("ptypes flush exception: %1").arg(pt::pconst(e->get_message()));
                                QMetaObject::invokeMethod(m_callingWidget, "appendProgress", Qt::QueuedConnection, Q_ARG(QString, message));
                        #endif
                                std::cerr << "ptypes flush exception: " << pt::pconst(e->get_message()) << "\n";
                                delete e;
                                continue;
                            }
                            if (numBytes != len) continue;
                        }
                        else
                            continue;
                    }
                    RunSpecifier *runSpecifier = new RunSpecifier();
                    runSpecifier->genome = offspring;
                    runSpecifier->startTime =  time(0);
                    runningList[submitCount] = runSpecifier;
#ifdef USE_QT_AGA
                    if (m_logLevel > 1)
                    {
                        message = QString("Sample %1 sent to %2").arg(submitCount).arg((const char *)ip);
                        QMetaObject::invokeMethod(m_callingWidget, "appendProgress", Qt::QueuedConnection, Q_ARG(QString, message));
                    }
#endif
                    submitCount++;
                }

                // a client has an answer for us
                if (numBytes == TCPIPMessage::StandardMessageSize && strcmp(commsBuffer.get(), "result") == 0 && memcmp(tcpMessage->md5, m_md5, sizeof(m_md5)) == 0)
                {
#ifdef USE_QT_AGA
                    if (returnCount % 100 == 0) QMetaObject::invokeMethod(m_callingWidget, "setResultNumber", Qt::QueuedConnection, Q_ARG(int, returnCount));
#endif
                    int index = tcpMessage->runID;
                    double result = tcpMessage->score;
                    std::map<int, RunSpecifier *>::iterator iter = runningList.find(index);
                    if (iter != runningList.end())
                    {
#ifdef USE_QT_AGA
                        if (m_logLevel > 1)
                        {
                            message = QString("Sample %1 score %2 from %3").arg(index).arg(result).arg((const char *)ip);
                            QMetaObject::invokeMethod(m_callingWidget, "appendProgress", Qt::QueuedConnection, Q_ARG(QString, message));
                        }
#endif
                        iter->second->genome->SetFitness(result);
                        thePopulation->InsertGenome(iter->second->genome, g_prefs->populationSize);
                        delete iter->second;
                        runningList.erase(iter);
                        // freeList.push(status.MPI_SOURCE);

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
                                sprintf(filename, "%s/BestGenome_%012d.txt", dirname, returnCount);
                                std::ofstream bestFile(filename);
                                bestFile << *thePopulation->GetLastGenome();
                                bestFile.close();
                                if (g_prefs->onlyKeepBestGenome)
                                {
                                    if (*lastBestGenomeFile) unlink(lastBestGenomeFile);
                                }
                                strcpy(lastBestGenomeFile, filename);
#ifdef USE_QT_AGA
                                QMetaObject::invokeMethod(m_callingWidget, "setBestScore", Qt::QueuedConnection, Q_ARG(double, maxFitness));
#endif

                            }
                        }

                        if (returnCount % g_prefs->savePopEvery == g_prefs->savePopEvery - 1 || returnCount == 1)
                        {
                            sprintf(filename, "%s/Population_%012d.txt", dirname, returnCount);
                            thePopulation->WritePopulation(filename, g_prefs->outputPopulationSize);
                            if (g_prefs->onlyKeepBestPopulation)
                            {
                                if (*lastBestPopulationFile) unlink(lastBestPopulationFile);
                            }
                            strcpy(lastBestPopulationFile, filename);
                        }

                        if (returnCount % g_prefs->improvementReproductions == g_prefs->improvementReproductions - 1)
                        {
#ifdef USE_QT_AGA
                            if (m_logLevel > 1)
                            {
                                message = QString("Fitness change for %1 reproductions is %2").arg(g_prefs->improvementReproductions).arg(maxFitness - lastMaxFitness);
                                QMetaObject::invokeMethod(m_callingWidget, "appendProgress", Qt::QueuedConnection, Q_ARG(QString, message));
                            }
#endif
                            if (maxFitness - lastMaxFitness < g_prefs->improvementThreshold) stopSendingFlag = true; // it will now quit
                            lastMaxFitness = maxFitness;
                        }

                        returnCount++;
                    }
                }
            }
        }
    }

    returnCount--; // reduce return count back to the value for the last actual return

    if (thePopulation->GetLastGenome()->GetFitness() > maxFitness)
    {
        sprintf(filename, "%s/BestGenome_%012d.txt", dirname, returnCount);
        if (strcmp(filename, lastBestGenomeFile) != 0)
        {
            std::ofstream bestFile(filename);
            bestFile << *thePopulation->GetLastGenome();
            bestFile.close();
            if (g_prefs->onlyKeepBestGenome)
            {
                if (*lastBestGenomeFile) unlink(lastBestGenomeFile);
            }
        }
    }

    sprintf(filename, "%s/Population_%012d.txt", dirname, returnCount);
    if (strcmp(filename, lastBestPopulationFile) != 0)
    {
        thePopulation->WritePopulation(filename, g_prefs->outputPopulationSize);
        if (g_prefs->onlyKeepBestPopulation)
        {
            if (*lastBestPopulationFile) unlink(lastBestPopulationFile);
        }
    }

    return 0;
}

void GAMain::ApplyGenome( const char *inputGenome, const char *inputXML, const char *outputXML)
{
    DataFile genomeData;
    double val;
    int ival, genomeSize;
    genomeData.ReadFile(inputGenome);
    genomeData.ReadNext(&ival);
    genomeData.ReadNext(&genomeSize);
    double *data = new double[genomeSize];
    for (int i = 0; i < genomeSize; i++)
    {
        genomeData.ReadNext(&val);
        data[i] = val;
        genomeData.ReadNext(&val); genomeData.ReadNext(&val); genomeData.ReadNext(&val);
        if (ival == -2) genomeData.ReadNext(&val); // skip the extra parameter
    }

    XMLConverter myXMLConverter;
    myXMLConverter.LoadBaseXMLFile(inputXML);
    myXMLConverter.ApplyGenome(genomeSize, data);
    int len;
    char *buf = (char *)myXMLConverter.GetFormattedXML(&len);

    DataFile outputXMLData;
    outputXMLData.SetRawData(buf);
    outputXMLData.WriteFile(outputXML);
}

void GAMain::SetServerPort(int port)
{
    g_prefs->tcpPort = port;
}

}

// Example 2. A multithreaded TCP server that uses the jobqueue class to maintain a thread pool - a fixed list of reusable threads that receive job `assignments' from a queue. The code below can be used as a template for a multithreaded network server.

#include <ptypes.h>

#include <ptime.h>
#include <pasync.h>
#include <pinet.h>

USING_PTYPES


const int testport = 8085;
const int maxthreads = 30;
const int maxtoken = 4096;

const int MSG_MYJOB = MSG_USER + 1;


class myjobthread: public thread
{
protected:
    int id;
    jobqueue* jq;
    virtual void execute();
public:
    myjobthread(int iid, jobqueue* ijq)
        : thread(false), id(iid), jq(ijq)  {}
    ~myjobthread()  { waitfor(); }
};


class myjob: public message
{
public:
    ipstream* client;
    myjob(ipstream* iclient)
        : message(MSG_MYJOB), client(iclient)  {}
    ~myjob()  { delete client; }
};


void myjobthread::execute()
{
    bool quit = false;
    while (!quit)
    {
        // get the next message from the queue
        message* msg = jq->getmessage();

        try
        {
            switch (msg->id)
            {
            case MSG_MYJOB:
                {
                    ipstream* client = ((myjob*)msg)->client;

                    // read the request line
                    string req = lowercase(client->line(maxtoken));
                    if (req == "hello")
                    {
                        // send our greeting to the client
                        client->putline("Hello, " + iptostring(client->get_ip()) + ", nice to see you!");
                        client->flush();

                        // log this request
                        pout.putf("%t  greeting received from %a, handled by thread %d\n",
                            now(), long(client->get_ip()), id);
                    }
                    client->close();
                }
                break;

            case MSG_QUIT:
                // MSG_QUIT is not used in our example
                quit = true;
                break;
            }
        }
        catch(exception*)
        {
            // the message object must be freed!
            delete msg;
            throw;
        }
        delete msg;
    }
}


void servermain(ipstmserver& svr)
{
    jobqueue jq;
    tobjlist<myjobthread> threads(true);

    // create the thread pool
    int i;
    for(i = 0; i < maxthreads; i++)
    {
        myjobthread* j = new myjobthread(i + 1, &jq);
        j->start();
        threads.add(j);
    }

    ipstream* client = new ipstream();

    pout.putf("Ready to answer queries on port %d\n", testport);

    while(true)
    {
        svr.serve(*client);

        if (client->get_active())
        {
            // post the job to the queue; the client object will be freed
            // automatically by the job object
            jq.post(new myjob(client));
            client = new ipstream();
        }
    }
}


int main()
{
    ipstmserver svr;

    try
    {
        // bind to all local addresses on port 8085
        svr.bindall(testport);

        // enter an infinite loop of serving requests
        servermain(svr);
    }
    catch(estream* e)
    {
        perr.putf("FATAL: %s\n", pconst(e->get_message()));
        delete e;
    }

    return 0;
}

#endif

