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
#include <iomanip>
#include <fstream>
#include <sys/stat.h>
#include <sys/timeb.h>
#include <queue>
#include <map>
#include <float.h>
#include <unistd.h>

#ifdef USE_QT_AGA
#include "AsynchronousGAQtWidget.h"
#include <QMetaObject>
#endif

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

#define DEBUG_MAIN
#include "DebugControl.h"

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
                if (strcmp(argv[i], "--debug") == 0 ||
                    strcmp(argv[i], "-d") == 0)
                {
                    i++;
                    if (i >= argc) throw(1);
                    AsynchronousGA::gDebugControl = (AsynchronousGA::DebugControl)strtol(argv[i], 0, 10);
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
        int nDebugLabels = sizeof(AsynchronousGA::gDebugLabels) / sizeof(AsynchronousGA::gDebugLabels[0]);
        for (i = 0; i < nDebugLabels; i++)
            fprintf(stderr, "%3d %s\n", i, AsynchronousGA::gDebugLabels[i]);
        std::cerr << "\n";
        return (e);
    }

    AsynchronousGA::GAMain ga;
    ga.SetLogLevel(logLevel);
    ga.LoadBaseXMLFile(baseXMLFile);
    return ga.Process(parameterFile, outputDirectory, startingPopulation);
}
#endif

namespace AsynchronousGA
{

#define OUT_VAR(x) std::cerr << #x " = " << (x) << "\n";

Preferences gPrefs;

GAMain::GAMain()
{
    m_baseXMLFile = 0;
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

    if (gPrefs.ReadPreferences(parameterFile))
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
    if (startingPopulation && strlen(wg.ToUTF8Tmp(startingPopulation))) strcpy(gPrefs.startingPopulation, wg.ToUTF8Tmp(startingPopulation));

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
    outFile << gPrefs;
    outFile.flush();
#ifdef USE_QT_AGA
    if (m_logLevel > 0)
    {
        message = QString("\"%1\" opened").arg(QString::fromWCharArray(wg.ToWideTmp(filename)));
        QMetaObject::invokeMethod(m_callingWidget, "appendProgress", Qt::QueuedConnection, Q_ARG(QString, message));
    }
#endif

    // set the random seed
    RandomSeed(gPrefs.randomSeed);

    // initialise the population
    if (startPopulation->ReadPopulation(gPrefs.startingPopulation))
    {
        std::cerr << "Error reading starting population: " << gPrefs.startingPopulation << "\n";
        return __LINE__;
    }
#ifdef USE_QT_AGA
    if (m_logLevel > 0)
    {
        message = QString("\"%1\" read").arg(QString::fromWCharArray(wg.ToWideTmp(gPrefs.startingPopulation)));
        QMetaObject::invokeMethod(m_callingWidget, "appendProgress", Qt::QueuedConnection, Q_ARG(QString, message));
    }
#endif

    if (startPopulation->GetPopulationSize() != gPrefs.populationSize)
    {
        std::cerr << "Warning: Starting population size does not match specified population size - using standard fixup\n";
        if (startPopulation->ResizePopulation(gPrefs.populationSize))
        {
            std::cerr << "Error: Starting population size does not match specified population size - unable to fix\n";
            return __LINE__;
        }
    }
    if (startPopulation->GetGenome(0)->GetGenomeLength() != gPrefs.genomeLength)
    {
        std::cerr << "Error: Starting population genome does not match specified genome length\n";
        return __LINE__;
    }
    if (gPrefs.randomiseModel)
    {
        startPopulation->Randomise();
    }

    startPopulation->SetGlobalCircularMutation(gPrefs.circularMutation);
    startPopulation->SetResizeControl(gPrefs.resizeControl);

    if (gDebugControl == MainDebug) std::cerr << "main read startingPopulation " << gPrefs.startingPopulation << "\n";

    evolvePopulation->SetSelectionType(gPrefs.parentSelection);
    evolvePopulation->SetParentsToKeep(gPrefs.parentsToKeep);

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
        return 1;
    }
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
    const int kStandardMessageSize = 32;
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
    TCP server;
    if ( server.GetConstructorFailed() || server.StartServer(gPrefs.tcpPort) != 0)
    {
        std::cerr << "Evolve: Error setting up server - port " << gPrefs.tcpPort << " probably in use\n";
        return __LINE__;
    }
#ifdef USE_QT_AGA
    if (m_logLevel > 0)
    {
        message = QString("TCP/IP port %1 open").arg(gPrefs.tcpPort);
        QMetaObject::invokeMethod(m_callingWidget, "appendProgress", Qt::QueuedConnection, Q_ARG(QString, message));
    }
#endif

    int progressValue = 0;
    int lastProgressValue = -1;
    time_t lastTime = time(0);
    int periodicTaskInterval = 1;
    while (returnCount < gPrefs.maxReproductions)
    {
        if (time(0) >= lastTime + periodicTaskInterval) // this part of the loop is for things that don't need to be done all that often
        {
            lastTime = time(0);
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
            progressValue = int(100 * returnCount / gPrefs.maxReproductions);
            if (progressValue != lastProgressValue)
            {
                lastProgressValue = progressValue;
#ifdef USE_QT_AGA
                QMetaObject::invokeMethod(m_callingWidget, "setProgressValue", Qt::QueuedConnection, Q_ARG(int, progressValue));
#endif
            }
        }
        // first check to see if there are connection requests
        if (server.CheckReceiver(0, 1000))
        {
            TCP connection;
            if (connection.GetConstructorFailed() || connection.StartAcceptor(server.GetSocket()) == -1)
            {
                continue;
            }
            struct sockaddr_in *sock = connection.GetSenderAddress();
            char *ip = inet_ntoa(sock->sin_addr);
            if (hostTime.find(ip) == hostTime.end())
            {
#ifdef USE_QT_AGA
                if (m_logLevel > 0)
                {
                    message = QString("Client %1 has connected").arg(ip);
                    QMetaObject::invokeMethod(m_callingWidget, "appendProgress", Qt::QueuedConnection, Q_ARG(QString, message));
                }
#endif
            }
            hostTime[ip] = time(0);
            if (connection.CheckReceiver(1, 0)) // anything to receive?
            {
                // read the command from the sender
                numBytes = connection.ReceiveData(commsBuffer.get(), kStandardMessageSize, 1, 0);
#ifdef USE_QT_AGA
                if (m_logLevel > 1)
                {
                    message = QString("Read %1 bytes from %2: %3").arg(numBytes).arg(ip).arg(commsBuffer.get());
                    QMetaObject::invokeMethod(m_callingWidget, "appendProgress", Qt::QueuedConnection, Q_ARG(QString, message));
                }
#endif

                // a client wants some data to work with
                if (numBytes == kStandardMessageSize && (strcmp(commsBuffer.get(), "req_send_length") == 0 || strcmp(commsBuffer.get(), "req_xml_length") == 0))
                {
                    offspring = new Genome();
                    // if we are still working from the start population, just get the next one
                    if (startPopulationIndex < startPopulation->GetPopulationSize())
                    {
                        if (gDebugControl == MainDebug) OUT_VAR(startPopulationIndex);
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
                            if (CoinFlip(gPrefs.crossoverChance))
                            {
                                if (thePopulation->GetPopulationSize()) parent2 = thePopulation->ChooseParent(&parent2Rank);
                                else parent2 = startPopulation->ChooseParent(&parent2Rank);
                                *offspring = *parent1;
                                mutationCount += Mate(parent1, parent2, offspring, gPrefs.crossoverType);
                            }
                            else
                            {
                                *offspring = *parent1;
                            }
                            if (gPrefs.multipleGaussian)  mutationCount += MultipleGaussianMutate(offspring, gPrefs.gaussianMutationChance);
                            else mutationCount += GaussianMutate(offspring, gPrefs.gaussianMutationChance);

                            mutationCount += FrameShiftMutate(offspring, gPrefs.frameShiftMutationChance);
                            mutationCount += DuplicationMutate(offspring, gPrefs.duplicationMutationChance);
                        }
                    }
                    // is xml wanted?
                    if (strcmp(commsBuffer.get(), "req_xml_length") == 0 && m_baseXMLFile)
                    {
                        int len = m_baseXMLFile->GetSize() + 1; // because we want the terminating 0 too
                        *(int *)commsBuffer.get() = len;
                        connection.SendData(commsBuffer.get(), kStandardMessageSize);
                        numBytes = connection.ReceiveData(commsBuffer.get(), kStandardMessageSize, 5, 0);
#ifdef USE_QT_AGA
                        if (m_logLevel > 1)
                        {
                            message = QString("Read %1 bytes from %2: %3").arg(numBytes).arg(ip).arg(commsBuffer.get());
                            QMetaObject::invokeMethod(m_callingWidget, "appendProgress", Qt::QueuedConnection, Q_ARG(QString, message));
                        }
#endif
                        if (numBytes == kStandardMessageSize && strcmp(commsBuffer.get(), "req_xml_data") == 0)
                        {
                            numBytes = connection.SendData(m_baseXMLFile->GetRawData(), m_baseXMLFile->GetSize() + 1);
                            if (numBytes != m_baseXMLFile->GetSize() + 1) continue;
                        else
                            continue;
                        }
                    }
                    // this should be the genome data request
                    if (strcmp(commsBuffer.get(), "req_send_length") == 0)
                    {
                        int len = offspring->GetGenomeLength() * sizeof(double);
                        int *ptr = (int *)commsBuffer.get();
                        *ptr++ = len;
                        *ptr++ = submitCount;
                        connection.SendData(commsBuffer.get(), kStandardMessageSize);
                        numBytes = connection.ReceiveData(commsBuffer.get(), kStandardMessageSize, 5, 0);
#ifdef USE_QT_AGA
                        if (m_logLevel > 1)
                        {
                            message = QString("Read %1 bytes from %2: %3").arg(numBytes).arg(ip).arg(commsBuffer.get());
                            QMetaObject::invokeMethod(m_callingWidget, "appendProgress", Qt::QueuedConnection, Q_ARG(QString, message));
                        }
#endif
                        if (numBytes == kStandardMessageSize && strcmp(commsBuffer.get(), "req_send_data") == 0)
                        {
                            numBytes = connection.SendData((char *)offspring->GetGenes(), len);
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
                        message = QString("Sample %1 sent to %2").arg(submitCount).arg(ip);
                        QMetaObject::invokeMethod(m_callingWidget, "appendProgress", Qt::QueuedConnection, Q_ARG(QString, message));
                    }
#endif
                    submitCount++;
                    if (gDebugControl == MainDebug) std::cerr << "Evolve submitCount = " << submitCount << "\n";
                }

                // a client has an answer for us
                if (numBytes == kStandardMessageSize && strcmp(commsBuffer.get(), "result") == 0)
                {
                    int index = *(int *)(commsBuffer.get() + 8 + sizeof(double));
                    double result = *(double *)(commsBuffer.get() + 8);
                    std::map<int, RunSpecifier *>::iterator iter = runningList.find(index);
                    if (iter != runningList.end())
                    {
#ifdef USE_QT_AGA
                        if (m_logLevel > 1)
                        {
                            message = QString("Sample %1 score %2 from %3").arg(index).arg(result).arg(ip);
                            QMetaObject::invokeMethod(m_callingWidget, "appendProgress", Qt::QueuedConnection, Q_ARG(QString, message));
                        }
#endif
                        iter->second->genome->SetFitness(result);
                        thePopulation->InsertGenome(iter->second->genome, gPrefs.populationSize);
                        delete iter->second;
                        runningList.erase(iter);
                        // freeList.push(status.MPI_SOURCE);
                        if (gDebugControl == MainDebug) OUT_VAR(thePopulation->GetPopulationSize());

                        if (returnCount % gPrefs.outputStatsEvery == gPrefs.outputStatsEvery - 1)
                        {
                            if (gDebugControl == MainDebug) std::cerr << "main returnCount = " << returnCount << " outputStatsEvery = " << gPrefs.outputStatsEvery << "\n";
                            CalculateTenPercentiles(thePopulation, &tenPercentiles);
                            *outFile << std::setw(10) << returnCount << " ";
                            *outFile << tenPercentiles << "\n";
                            outFile->flush();
                        }

                        if (returnCount % gPrefs.saveBestEvery == gPrefs.saveBestEvery - 1)
                        {
                            if (gDebugControl == MainDebug) std::cerr << "main returnCount = " << returnCount << " saveBestEvery = " << gPrefs.saveBestEvery << "\n";
                            if (thePopulation->GetLastGenome()->GetFitness() > maxFitness)
                            {
                                maxFitness = thePopulation->GetLastGenome()->GetFitness();
                                sprintf(filename, "%s/BestGenome_%012d.txt", dirname, returnCount);
                                std::ofstream bestFile(filename);
                                bestFile << *thePopulation->GetLastGenome();
                                bestFile.close();
                                if (gPrefs.onlyKeepBestGenome)
                                {
                                    if (*lastBestGenomeFile) unlink(lastBestGenomeFile);
                                }
                                strcpy(lastBestGenomeFile, filename);
                            }
                        }

                        if (returnCount % gPrefs.savePopEvery == gPrefs.savePopEvery - 1)
                        {
                            if (gDebugControl == MainDebug) std::cerr << "main returnCount = " << returnCount << " savePopEvery = " << gPrefs.savePopEvery << "\n";
                            sprintf(filename, "%s/Population_%012d.txt", dirname, returnCount);
                            thePopulation->WritePopulation(filename, gPrefs.outputPopulationSize);
                            if (gPrefs.onlyKeepBestPopulation)
                            {
                                if (*lastBestPopulationFile) unlink(lastBestPopulationFile);
                            }
                            strcpy(lastBestPopulationFile, filename);
                        }

                        if (returnCount % gPrefs.improvementReproductions == gPrefs.improvementReproductions - 1)
                        {
                            if (gDebugControl == MainDebug) std::cerr << "main returnCount = " << returnCount << " improvementReproductions = " << gPrefs.improvementReproductions << "\n";
                            if ( maxFitness - lastMaxFitness < gPrefs.improvementThreshold ) stopSendingFlag = true; // it will now quit
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
            if (gPrefs.onlyKeepBestGenome)
            {
                if (*lastBestGenomeFile) unlink(lastBestGenomeFile);
            }
        }
    }

    sprintf(filename, "%s/Population_%012d.txt", dirname, returnCount);
    if (strcmp(filename, lastBestPopulationFile) != 0)
    {
        thePopulation->WritePopulation(filename, gPrefs.outputPopulationSize);
        if (gPrefs.onlyKeepBestPopulation)
        {
            if (*lastBestPopulationFile) unlink(lastBestPopulationFile);
        }
    }

    return 0;
}



}

#endif

