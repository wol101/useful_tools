/*
 *  main.cpp
 *  AsynchronousGA
 *
 *  Created by Bill Sellers on 19/10/2010.
 *  Copyright 2010 Bill Sellers. All rights reserved.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <sys/stat.h>
#include <queue>
#include <map>
#include <float.h>
#include <unistd.h>

#include <mpi.h>
extern int gMPI_Comm_size;
extern int gMPI_Comm_rank;

#include "Preferences.h"
#include "Genome.h"
#include "Population.h"
#include "Mating.h"
#include "Random.h"
#include "Statistics.h"
#include "MPIStuff.h"

#define DEBUG_MAIN
#include "DebugControl.h"

#define OUT_VAR(x) std::cerr << #x " = " << (x) << "\n";

using namespace AsynchronousGA;

//int InitialiseFitness(Population *startPopulation, Population *evolvePopulation, double timeout);
int Evolve(Population *startPopulation, Population *evolvePopulation, std::ostream *outFile, const char *dirname);


namespace AsynchronousGA
{
    Preferences gPrefs;
}

int GAMain(int argc, char *argv[])
{
    int i;
    char *parameterFile = 0;
    char *outputDirectory = 0;
    char *startingPopulation = 0;
    char dirname[kBufferSize];
    char filename[kBufferSize];
    int error;
    struct stat sb;
    time_t theTime = time(0);
    struct tm *theLocalTime = localtime(&theTime);
    Population *startPopulation = new Population();
    Population *evolvePopulation = new Population();
    std::ofstream outFile;
    std::queue<int>freeList;

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
                if (strcmp(argv[i], "--debug") == 0 ||
                    strcmp(argv[i], "-d") == 0)
                {
                    i++;
                    if (i >= argc) throw(1);
                    gDebugControl = (DebugControl)strtol(argv[i], 0, 10);
                }

            else
            {
                throw(__LINE__);
            }
        }
        if (parameterFile == 0) throw(__LINE__);
    }

    // catch argument errors
    catch (int e)
    {
        std::cerr << "\nGenetic Algorithm program\n";
        std::cerr << "Build " << __DATE__ << " " << __TIME__ << "\n";
        std::cerr << "Supported options:\n\n";
        std::cerr << "(-p) --parameterFile file_name\n";
        std::cerr << "(-o) --outputDirectory directory_name\n";
        std::cerr << "(-s) --startingPopulation population_file";
        std::cerr << "(-d) --debug n\n";
        int nDebugLabels = sizeof(gDebugLabels) / sizeof(gDebugLabels[0]);
        for (i = 0; i < nDebugLabels; i++)
            fprintf(stderr, "%3d %s\n", i, gDebugLabels[i]);
        std::cerr << "\n";
        return (e);
    }

    if (gPrefs.ReadPreferences(parameterFile))
    {
        std::cerr << "Error reading " << parameterFile << "\n";
        return (__LINE__);
    }
    // overwrite starting population if defined
    if (startingPopulation) strcpy(gPrefs.startingPopulation, startingPopulation);


    // create a directory for all the output
    if (outputDirectory)
    {
        strcpy(dirname, outputDirectory);
        if (stat(dirname, &sb) == -1)
        {
            error = mkdir(dirname, 0777);
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
        error = mkdir(dirname, 0777);
        if (error)
        {
            std::cerr << "Error creating directory " << dirname << "\n";
            return __LINE__;
        }
    }

    // write log
    sprintf(filename, "%s/log.txt", dirname);
    outFile.open(filename);
    outFile << "GA build " << __DATE__ << " " << __TIME__ "\n";
    outFile << "Log produced " << asctime(theLocalTime);

    outFile << "Command line:\n";
    for (i = 0; i < argc - 1; i++) outFile << argv[i] << " ";
    outFile << argv[i] << "\n";
    outFile << "parameterFile \"" << parameterFile << "\"\n";


    outFile << gPrefs;

    outFile.flush();

    // set the random seed
    RandomSeed(gPrefs.randomSeed);

    // initialise the population
    if (startPopulation->ReadPopulation(gPrefs.startingPopulation))
    {
        std::cerr << "Error reading starting population: " << gPrefs.startingPopulation << "\n";
        return __LINE__;
    }

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

#if 0
    if (InitialiseFitness(startPopulation, evolvePopulation, gPrefs.watchDogTimerLimit))
    {
        std::cerr << "Terminated due to InitialiseFitness failure\n";
    }
#endif

    if (Evolve(startPopulation, evolvePopulation, &outFile, dirname))
    {
        std::cerr << "Terminated due to Evolve failure\n";
    }

    delete startPopulation;
    delete evolvePopulation;

    return 0;
}

#if 0
int InitialiseFitness(Population *startPopulation, Population *evolvePopulation, double timeLimit)
{
    // check which values need calculating
    std::map<int, MPIRunSpecifier *>toSendList;
    std::map<int, MPIRunSpecifier *>::iterator iter;
    int i;
    for (i = 0; i < startPopulation->GetPopulationSize(); i++)
    {
        toSendList[i] = new MPIRunSpecifier();
    }

    // get the list of possible clients
    if (gDebugControl == MainDebug) std::cerr << "InitialiseFitness read gMPI_Comm_size " << gMPI_Comm_size << "\n";
    std::queue<int>freeList;
    for (i = 1; i < gMPI_Comm_size; i++)
    {
        freeList.push(i);
    }

    // start server loop
    int genomeLength = startPopulation->GetGenome(0)->GetGenomeLength();
    bool finished = false;
    std::map<int, MPIRunSpecifier *>runningList;
    MPI_Status status;
    char buffer[256];
    int flag;
    int len = (genomeLength) * sizeof(double) + 3 * sizeof(int);
    char *buf = new char[len];
    while (finished == false)
    {
        if (freeList.empty() == false)
        {
            if (toSendList.empty() == false)
            {
                iter = toSendList.begin();
                int *iPtr = (int *)buf;
                iPtr[0] = MPI_MESSAGE_ID_SEND_GENOME_DATA;
                iPtr[1] = iter->first;
                iPtr[2] = genomeLength;
                memcpy((double *)(&iPtr[3]), startPopulation->GetGenome(iter->first)->GetGenes(), genomeLength * sizeof(double));

                iter->second->startTime = time(0);
                iter->second->mpiSource = freeList.front();
                iter->second->genome = new Genome(*startPopulation->GetGenome(iter->first));

                if (gDebugControl == MainDebug)
                    std::cerr << "InitialiseFitness index " << iter->first << " startTime " << iter->second->startTime
                            << " MPI_SOURCE " << iter->second->mpiSource << "\n";


                MPI_Send(buf,                    /* message buffer */
                         len,                    /* 'len' data item */
                         MPI_BYTE,               /* data items are bytes */
                         iter->second->mpiSource,/* destination */
                         0,                      /* user chosen message tag */
                         MPI_COMM_WORLD);        /* default communicator */

                runningList[iter->first] = iter->second;
                toSendList.erase(iter);
                freeList.pop();
                continue; // this means that stuffing the clients takes priority
            }
        }

        MPI_Iprobe(MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &flag, &status);
        if (flag)
        {
            MPI_Recv(buffer,            /* message buffer */
                     sizeof(buffer),    /* max number of data items */
                     MPI_BYTE,          /* of type BYTE */
                     MPI_ANY_SOURCE,    /* receive from any sender */
                     MPI_ANY_TAG,       /* any type of message */
                     MPI_COMM_WORLD,    /* default communicator */
                     &status);          /* info about the received message */

            if (((int *)buffer)[0] == MPI_MESSAGE_ID_SEND_RESULTS)
            {
                int index = ((int *)buffer)[1];
                double result = *(double *)(&(((int *)buffer)[2]));
                if (gDebugControl == MainDebug)
                    std::cerr << "InitialiseFitness index " << index << " result " << result << "\n";
                iter = runningList.find(index);
                if (iter != runningList.end())
                {
                    if (iter->second->mpiSource == status.MPI_SOURCE)
                    {
                        if (gDebugControl == MainDebug)
                            std::cerr << "InitialiseFitness fitness " << result << "\n";
                        iter->second->genome->SetFitness(result);
                        evolvePopulation->InsertGenome(iter->second->genome, gPrefs.populationSize);
                        delete iter->second;
                        runningList.erase(iter);
                        freeList.push(status.MPI_SOURCE);
                    }
                }

            }
        }

        if (toSendList.empty())
        {
            if (runningList.empty())
            {
                finished = true;
            }
            else
            {
                for (iter = runningList.begin(); iter != runningList.end(); iter++)
                {
                    if ((time(0) - iter->second->startTime) > timeLimit)
                    {
                        std::cerr << "InitialiseFitness Timeout on " << iter->second->mpiSource << "\n";
                        toSendList[iter->first] = iter->second;
                        runningList.erase(iter);
                    }
                }
            }
        }

    }

    return 0;
}
#endif

int Evolve(Population *startPopulation, Population *thePopulation, std::ostream *outFile, const char *dirname)
{
    // This is the asynchronous evolution loop
    int startPopulationIndex = 0;
    int bufferLen = (startPopulation->GetGenome(0)->GetGenomeLength()) * sizeof(double) + 3 * sizeof(int);
    char *buffer = new char[bufferLen];
    int submitCount = 0;
    int returnCount = 0;
    int parent1Rank, parent2Rank;
    MPI_Status status;
    int flag;
    int mutationCount;
    time_t watchDogTimer = time(0);
    Genome *offspring;
    Genome *parent1;
    Genome *parent2;
    TenPercentiles tenPercentiles;
    std::map<int, MPIRunSpecifier *>runningList;
    std::map<int, MPIRunSpecifier *>::iterator iter;
    char lastBestGenomeFile[kBufferSize] = "";
    char lastBestPopulationFile[kBufferSize] = "";
    double maxFitness = -DBL_MAX;
    double lastMaxFitness = -DBL_MAX;
    char filename[kBufferSize];
    int *iPtr;
    bool stopSendingFlag = false;
    int i;

    // get the list of possible clients
    if (gDebugControl == MainDebug) std::cerr << "Evolve read gMPI_Comm_size " << gMPI_Comm_size << "\n";
    std::queue<int>freeList;
    for (i = 1; i < gMPI_Comm_size; i++)
    {
        freeList.push(i);
    }

    while (returnCount < gPrefs.maxReproductions && (time(0) - watchDogTimer) < gPrefs.watchDogTimerLimit)
    {
        if (submitCount < gPrefs.maxReproductions && freeList.size() > 0 && stopSendingFlag == false)
        {
            if (gDebugControl == MainDebug) std::cerr << "Evolve freeList.size() = " << freeList.size() << "\n";

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
            // and send if off for processing
            iPtr = (int *)buffer;
            iPtr[0] = MPI_MESSAGE_ID_SEND_GENOME_DATA;
            iPtr[1] = submitCount;
            iPtr[2] = offspring->GetGenomeLength();
            memcpy((double *)(&iPtr[3]), offspring->GetGenes(), offspring->GetGenomeLength() * sizeof(double));
            MPI_Send(buffer,                 /* message buffer */
                     bufferLen,              /* number of data item */
                     MPI_BYTE,               /* data items are bytes */
                     freeList.front(),       /* destination */
                     0,                      /* user chosen message tag */
                     MPI_COMM_WORLD);        /* default communicator */

            MPIRunSpecifier *runSpecifier = new MPIRunSpecifier();
            runSpecifier->mpiSource = freeList.front();
            runSpecifier->genome = offspring;
            runningList[submitCount] = runSpecifier;
            submitCount++;
            freeList.pop();
            watchDogTimer = time(0); // reset watchDogTimer
            if (gDebugControl == MainDebug) std::cerr << "Evolve submitCount = " << submitCount << "\n";
        }

        MPI_Iprobe(MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &flag, &status);
        if (flag)
        {
            MPI_Recv(buffer,            /* message buffer */
                     bufferLen,         /* max number of data items */
                     MPI_BYTE,          /* of type BYTE */
                     MPI_ANY_SOURCE,    /* receive from any sender */
                     MPI_ANY_TAG,       /* any type of message */
                     MPI_COMM_WORLD,    /* default communicator */
                     &status);          /* info about the received message */

            if (gDebugControl == MainDebug) std::cerr << "Evolve MPI_Recv MPI_MESSAGE_ID = " << ((int *)buffer)[0] << "\n";

            if (((int *)buffer)[0] == MPI_MESSAGE_ID_SEND_RESULTS)
            {
                int index = ((int *)buffer)[1];
                double result = *(double *)(&(((int *)buffer)[2]));
                iter = runningList.find(index);
                if (iter != runningList.end())
                {
                    if (iter->second->mpiSource == status.MPI_SOURCE)
                    {
                        iter->second->genome->SetFitness(result);
                        thePopulation->InsertGenome(iter->second->genome, gPrefs.populationSize);
                        delete iter->second;
                        runningList.erase(iter);
                        freeList.push(status.MPI_SOURCE);
                        watchDogTimer = time(0); // reset watchDogTimer
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
                if (gDebugControl == MainDebug) std::cerr << "main returnCount = " << returnCount << " maxReproductions = " << gPrefs.maxReproductions << "\n";
            }
        }
        if (stopSendingFlag)
        {
            if (returnCount == submitCount) break;
        }
    }
    if (stopSendingFlag == false && returnCount < gPrefs.maxReproductions)
    {
        std::cerr << "Terminated due to watchdog timeout: returnCount = " << returnCount << " watchdogTime = " << time(0) - watchDogTimer << "\n";
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

    delete [] buffer;
    return 0;
}


