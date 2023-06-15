// This is the wrapper to call GeneralGA to solve the gait bootstrap problem

// the basic outline is as follows

// 1. Set up the initial population files and config files
// 2. Run the GA
// 3. Generate a new config and population file
// 3. Repeat from 2 as required.

#include <mpi.h>
#include <stdio.h>
#include <limits.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <alloca.h>
#include <unistd.h>
#include <time.h>
#include <vector>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <string>

#include "../AsynchronousGA/Random.h"
#include "../GaitSym2016/src/DataFile.h"
#include "../GaitSym2016/src/MPIStuff.h"
#include "../GaitSym2016/src/XMLConverter.h"
#include "../GaitSym2016/src/Util.h"

extern int gMPI_Comm_size;
extern int gMPI_Comm_rank;


int GAMain(int argc, char *argv[]);
int GaitSymMain(int argc, char *argv[]);
int MergeXMLMain(int argc, char *argv[]);
void ApplyGenome(const char *inputXML, const char *inputGenome, const char *outputXML);
void GenerateNewKinematicPopulation(int runNumber, const char *bestGenomeFile, const char *modelPopulationFile, int kinematicIncrement, int kinematicStride, double kinematicRangeFraction, double kinematicSDFraction, const char *workingPopulationFile);

// returns zero on no erro, or the line number of an error
int Bootstrap(int argc, char *argv[])
{
    int nRuns, nOffsetCycles;
    int i, j;
    double CPUTime, IOTime;
    MPI_Status status;
    int mpi_buffer[16];
    double *ptr;
    char *mpi_big_buffer = 0;
    int mpi_big_buffer_size = 0;

    const char *ubFileName = "config.txt";
    const char *workingConfigFileName = "workingConfigFile.xml";
    const char *workingPopulationFileName = "workingPopulationFile.txt";

    char workingConfigFile[512];
    char workingConfigFilePremerge[512];
    char workingPopulationFile[512];

    DataFile file;
    file.SetExitOnError(true);

    char buffer[10000];
    char dirname[512];
    char *pServerArgs[256];
    int nServerArgs;

    DIR *dir;
    struct dirent* entry;
    std::vector<std::string> fileList;

    char bestPopulationFile[512];
    char bestGenomeFile[512];
    char bestGenomeFileXML[512];
    char modelPopulationFile[512];
    char modelConfigFile[512];
    char modelParameterFile[512];
    char mergeFile[512];
    char driverFile[512];

    file.ReadFile(ubFileName);
    file.RetrieveParameter("modelPopulationFile", modelPopulationFile, sizeof(modelPopulationFile) - 1, true);
    file.RetrieveParameter("modelConfigFile", modelConfigFile, sizeof(modelConfigFile) - 1, true);
    file.RetrieveParameter("modelParameterFile", modelParameterFile, sizeof(modelParameterFile) - 1, true);
    file.RetrieveParameter("mergeFile", mergeFile, sizeof(mergeFile) - 1, true);
    file.RetrieveParameter("driverFile", driverFile, sizeof(driverFile) - 1, true);
    file.RetrieveParameter("nRuns", &nRuns, true);
    file.RetrieveParameter("nOffsetCycles", &nOffsetCycles, true);

    // optional parameters for progressive kinematic match
    int kinematicIncrement = 0;
    int kinematicStride = 0;
    double kinematicRangeFraction = 0.1;
    double kinematicSDFraction = 0.1;
    double kinematicInitialTime = 0;
    double kinematicTimeIncrement = 0;
    file.SetExitOnError(false);
    if (file.RetrieveParameter("kinematicIncrement", &kinematicIncrement, true) == 0)
    {
        file.SetExitOnError(true);
        file.RetrieveParameter("kinematicStride", &kinematicStride, true);
        file.RetrieveParameter("kinematicRangeFraction", &kinematicRangeFraction, true);
        file.RetrieveParameter("kinematicSDFraction", &kinematicSDFraction, true);
        file.RetrieveParameter("kinematicInitialTime", &kinematicInitialTime, true);
        file.RetrieveParameter("kinematicTimeIncrement", &kinematicTimeIncrement, true);
    }

   // optional parameters for gait merge
    double gaitMergeStart = 0;
    double gaitMergeFinish = 0;
    double gaitMergeCurrent;
    double gaitMergeMinFitness = -DBL_MAX;
    char modelConfigFile2[512];
    file.SetExitOnError(false);
    file.RetrieveParameter("gaitMergeMinFitness", &gaitMergeMinFitness, true);
    if (file.RetrieveParameter("gaitMergeStart", &gaitMergeStart, true) == 0)
    {
        file.SetExitOnError(true);
        file.RetrieveParameter("gaitMergeFinish", &gaitMergeFinish, true);
        file.RetrieveParameter("modelConfigFile2", modelConfigFile2, sizeof(modelConfigFile2) - 1, true);
    }

    file.SetExitOnError(true);

    // create the output folder
    int err;
    time_t theTime;
    struct tm *theLocalTime;
    i = 0;
    do
    {
        theTime = time(0);
        theLocalTime = localtime(&theTime);
        sprintf(dirname, "%04d_Run_%04d-%02d-%02d_%02d.%02d.%02d",
                        i,
                        theLocalTime->tm_year + 1900,
                        theLocalTime->tm_mon + 1,
                        theLocalTime->tm_mday,
                        theLocalTime->tm_hour, theLocalTime->tm_min,
                        theLocalTime->tm_sec);
        std::cerr << "Creating folder " << dirname << "\n";
        err = mkdir(dirname, 0777);
        if (err != 0) sleep(10);
    }
    while (err != 0);

    // now set the destination file names
    sprintf(workingConfigFile, "%s/%s", dirname, workingConfigFileName);
    sprintf(workingPopulationFile, "%s/%s", dirname, workingPopulationFileName);

    // run the MergeXML command
    if (kinematicIncrement <= 0)
    {
        if (gaitMergeStart == gaitMergeFinish)
            sprintf(buffer, "mergexml -v -i %s --ignoreError -r INPUT_CONFIG_FILE %s -r DRIVER_FILE %s -r OUTPUT_CONFIG_FILE %s", mergeFile, modelConfigFile, driverFile, workingConfigFile);
        else
            sprintf(buffer, "mergexml -v -i %s --ignoreError -r INPUT_CONFIG_FILE %s -r DRIVER_FILE %s -r OUTPUT_CONFIG_FILE %s -r MERGE_CONFIG_FILE %s -r MERGE_VALUE %.17e",
                mergeFile, modelConfigFile, driverFile, workingConfigFile, modelConfigFile2, gaitMergeStart);
    }
    else
    {
        if (gaitMergeStart == gaitMergeFinish)
            sprintf(buffer, "mergexml -v -i %s --ignoreError -r INPUT_CONFIG_FILE %s -r DRIVER_FILE %s -r OUTPUT_CONFIG_FILE %s -r RUN_TIME %g", mergeFile, modelConfigFile, driverFile, workingConfigFile, kinematicInitialTime);
        else
            sprintf(buffer, "mergexml -v -i %s --ignoreError -r INPUT_CONFIG_FILE %s -r DRIVER_FILE %s -r OUTPUT_CONFIG_FILE %s -r RUN_TIME %g -r MERGE_CONFIG_FILE %s -r MERGE_VALUE %.17e",
                mergeFile, modelConfigFile, driverFile, workingConfigFile, kinematicInitialTime, modelConfigFile2, gaitMergeStart);
    }
    std::cerr << "Executing command:\n";
    std::cerr << buffer << "\n";
    nServerArgs = DataFile::ReturnTokens(buffer, pServerArgs, 256);
    MergeXMLMain(nServerArgs, pServerArgs);
    std::cerr << "MergeXMLMain finished\n";

    // prepare the populationFile
    std::cerr << "Reading modelPopulationFile: " << modelPopulationFile << "\n";
    file.ReadFile(modelPopulationFile);
    std::cerr << "Writing workingPopulationFile: " << workingPopulationFile << "\n";
    file.WriteFile(workingPopulationFile);

    for (i = 0; i < nRuns; i++)
    {
        // force a reload of the workingConfigFile
        std::cerr << "Reading workingConfigFile: " << workingConfigFile << "\n";
        file.ReadFile(workingConfigFile);
        std::cerr << "Force a reload of the workingConfigFile: " << workingConfigFile << "\n";
        int new_mpi_big_buffer_size = file.GetSize() + 1 + 2 * sizeof(int);
        if (new_mpi_big_buffer_size > mpi_big_buffer_size)
        {
            if (mpi_big_buffer) delete [] mpi_big_buffer;
            mpi_big_buffer_size = new_mpi_big_buffer_size;
            mpi_big_buffer = new char[mpi_big_buffer_size];
        }

        *((int *)mpi_big_buffer) = MPI_MESSAGE_ID_RELOAD_MODELCONFIG;
        *((int *)(mpi_big_buffer + sizeof(int))) = file.GetSize();
        strcpy(mpi_big_buffer + 2 * sizeof(int), file.GetRawData());

#define SYNCHRONOUS_MPI
#ifdef SYNCHRONOUS_MPI
        for (j = 1; j < gMPI_Comm_size; j++) // FIX ME - this should almost certainly be replaced with MPI_Barrier & MPI_Bcast
        {
            MPI_Send(mpi_big_buffer,            /* message buffer */
                     mpi_big_buffer_size,       /* number of data item */
                     MPI_BYTE,                  /* data items are bytes */
                     j,                         /* destination */
                     0,                         /* user chosen message tag */
                     MPI_COMM_WORLD);           /* default communicator */
        }
#else
        int count = gMPI_Comm_size - 1;
        MPI_Request *array_of_requests = new MPI_Request[count];
        MPI_Status *array_of_statuses = new MPI_Status[count];
        for (j = 1; j < gMPI_Comm_size; j++) // this should be quicker but not as fast as MPI_Barrier & MPI_Bcast
        {
            MPI_Request request;
            MPI_Isend(mpi_big_buffer,               /* message buffer */
                      mpi_big_buffer_size,          /* number of data item */
                      MPI_BYTE,                     /* data items are bytes */
                      j,                            /* destination */
                      0,                            /* user chosen message tag */
                      MPI_COMM_WORLD,               /* default communicator */
                      &(array_of_requests[j - 1])); /* communication request (output) */
        }
        MPI_Waitall(count,                      /* list length */
                    array_of_requests,          /* array of request handles */
                    array_of_statuses);         /* array of status objects (output but can be MPI_STATUSES_IGNORE) */
        delete [] array_of_requests;
        delete [] array_of_statuses;
#endif

        while (true)
        {
            // run the GA command
            sprintf(buffer, "ga -p %s -o %s -s %s", modelParameterFile, dirname, workingPopulationFile);
            for (j = 1; j < argc; j++)
            {
                strcat(buffer, " ");
                strcat(buffer, argv[j]);
            }
            std::cerr << "Executing command:\n";
            std::cerr << buffer << "\n";
            nServerArgs = DataFile::ReturnTokens(buffer, pServerArgs, 256);
            GAMain(nServerArgs, pServerArgs);
            std::cerr << "GAMain finished\n";

            // check how much CPU and IO time has been used
            std::cerr << "Checking IO Usage\n";
            CPUTime = 0;
            IOTime = 0;
            for (j = 1; j < gMPI_Comm_size; j++)
            {
                mpi_buffer[0] = MPI_MESSAGE_ID_SEND_TIMINGS;
                MPI_Send(mpi_buffer,                /* message buffer */
                         sizeof(int),               /* number of data item */
                         MPI_BYTE,                  /* data items are bytes */
                         j,                         /* destination */
                         0,                         /* user chosen message tag */
                         MPI_COMM_WORLD);           /* default communicator */

                MPI_Recv(mpi_buffer,            /* message buffer */
                         sizeof(mpi_buffer),    /* max number of data items */
                         MPI_BYTE,              /* of type BYTE */
                         j,                     /* receive from this sender */
                         MPI_ANY_TAG,           /* any type of message */
                         MPI_COMM_WORLD,        /* default communicator */
                         &status);              /* info about the received message */

                // std::cerr << "MPI target " << j << " Message ID " << ((int *)mpi_buffer)[0] << "\n";
                if (((int *)mpi_buffer)[0] == MPI_MESSAGE_ID_SEND_TIMINGS)
                {
                    ptr = (double *)&(mpi_buffer[2]);
                    CPUTime += *ptr++;
                    IOTime += *ptr;
                }
            }
            std::cerr << "CPUTime = " << CPUTime << " IOTime = " << IOTime << " CPU Use Proportion " << CPUTime / (CPUTime + IOTime) << "\n";

            // test to see whether we are above the minimum fitness
            sprintf(buffer, "%s/log.txt", dirname);
            DataFile logFile;
            logFile.ReadFile(buffer);
            int nTokens = DataFile::CountTokens(logFile.GetRawData());
            char **ptrs = new char *[nTokens];
            nTokens = DataFile::ReturnTokens(logFile.GetRawData(), ptrs, nTokens);
            double bestFitness = Util::Double(ptrs[nTokens - 1]);
            delete [] ptrs;
            std::cerr << "Best fitness = " << bestFitness << "\n";
            if (bestFitness >= gaitMergeMinFitness) break;

            // not above the minimum so we need to set up a new output folder and rerun
            std::cerr << "gaitMergeMinFitness = " << gaitMergeMinFitness << " so rerunning\n";
            do
            {
                theTime = time(0);
                theLocalTime = localtime(&theTime);
                sprintf(dirname, "%04d_Run_%04d-%02d-%02d_%02d.%02d.%02d",
                                i,
                                theLocalTime->tm_year + 1900,
                                theLocalTime->tm_mon + 1,
                                theLocalTime->tm_mday,
                                theLocalTime->tm_hour, theLocalTime->tm_min,
                                theLocalTime->tm_sec);
                std::cerr << "Creating folder " << dirname << "\n";
                err = mkdir(dirname, 0777);
                if (err != 0) sleep(10);
            }
            while (err != 0);

        }

        // read the output folder to find the best genome and the best population
        dir = opendir(dirname);
        fileList.clear();
        while( (entry = readdir(dir))) fileList.push_back(entry->d_name);
        closedir(dir);
        std::sort(fileList.begin(), fileList.end());

        for (j = fileList.size() - 1; j >= 0; j--)
        {
            if (fileList[j].find("Population") == 0) break;
        }
        if (j < 0) return __LINE__;
        sprintf(bestPopulationFile, "%s/%s", dirname, fileList[j].c_str());
        std::cerr << "bestPopulationFile is now " << bestPopulationFile << "\n";

        for (j = fileList.size() - 1; j >= 0; j--)
        {
            if (fileList[j].find("BestGenome") == 0)
            {
                if (fileList[j].find(".txt") != std::string::npos)
                    break;
            }
        }
        if (j < 0) return __LINE__;
        sprintf(bestGenomeFile, "%s/%s", dirname, fileList[j].c_str());
        std::cerr << "bestGenomeFile is now " << bestGenomeFile << "\n";

        // create the XML file from the best genome
        sprintf(bestGenomeFileXML, "%s.xml", bestGenomeFile);
        std::cerr << "Apply genome " <<  bestGenomeFile << " to " << workingConfigFile << " and create " << bestGenomeFileXML << "\n";
        ApplyGenome(workingConfigFile, bestGenomeFile, bestGenomeFileXML);

        // quit loop if this is last run
        if (i >= nRuns - 1) break;

        // create new output folder
        do
        {
            theTime = time(0);
            theLocalTime = localtime(&theTime);
            sprintf(dirname, "%04d_Run_%04d-%02d-%02d_%02d.%02d.%02d",
                            i + 1,
                            theLocalTime->tm_year + 1900,
                            theLocalTime->tm_mon + 1,
                            theLocalTime->tm_mday,
                            theLocalTime->tm_hour, theLocalTime->tm_min,
                            theLocalTime->tm_sec);
            std::cerr << "Creating folder " << dirname << "\n";
            err = mkdir(dirname, 0777);
            if (err != 0) sleep(10);
        }
        while (err != 0);

        // now set the destination file names
        sprintf(workingConfigFile, "%s/%s", dirname, workingConfigFileName);
        sprintf(workingConfigFilePremerge, "%s/%s_premerge", dirname, workingConfigFileName);
        sprintf(workingPopulationFile, "%s/%s", dirname, workingPopulationFileName);

        gaitMergeCurrent = (double(i + 1) / double(nRuns - 1)) * (gaitMergeFinish - gaitMergeStart) + gaitMergeStart;

        if (kinematicIncrement <= 0) // regular gait morph mode
        {
            // copy the population as the new population
            std::cerr << "Reading modelPopulationFile: " << bestPopulationFile << "\n";
            file.ReadFile(bestPopulationFile);
            std::cerr << "Writing workingPopulationFile: " << workingPopulationFile << "\n";
            file.WriteFile(workingPopulationFile);

            // run the GaitSym command to create the new config command
            std::cerr << "Create the new start conditions files: " << workingConfigFile << "\n";
            sprintf(buffer, "gaitsym -U -T %d -c %s -M %s", nOffsetCycles, bestGenomeFileXML, workingConfigFilePremerge);
            std::cerr << "Executing command:\n";
            std::cerr << buffer << "\n";
            nServerArgs = DataFile::ReturnTokens(buffer, pServerArgs, 256);
            GaitSymMain(nServerArgs, pServerArgs);
            std::cerr << "GaitSymMain finished\n";

            // check that the new workingConfigFilePremerge has been created
            struct stat fileStat;
            if (stat(workingConfigFilePremerge, &fileStat))
            {
                std::cerr << "Error reading " << workingConfigFilePremerge << "\nExiting\n";
                exit(1);
            }

            // run the MergeXML command
            if (gaitMergeStart == gaitMergeFinish)
                sprintf(buffer, "mergexml -v -i %s --ignoreError -r INPUT_CONFIG_FILE %s -r DRIVER_FILE %s -r OUTPUT_CONFIG_FILE %s", mergeFile, workingConfigFilePremerge, driverFile, workingConfigFile);
            else
                sprintf(buffer, "mergexml -v -i %s --ignoreError -r INPUT_CONFIG_FILE %s -r DRIVER_FILE %s -r OUTPUT_CONFIG_FILE %s -r MERGE_CONFIG_FILE %s -r MERGE_VALUE %.17e",
                    mergeFile, workingConfigFilePremerge, driverFile, workingConfigFile, modelConfigFile2, gaitMergeCurrent);
            std::cerr << "Executing command:\n";
            std::cerr << buffer << "\n";
            nServerArgs = DataFile::ReturnTokens(buffer, pServerArgs, 256);
            MergeXMLMain(nServerArgs, pServerArgs);
            std::cerr << "MergeXMLMain finished\n";
        }
        else // kinematic match mode
        {
            std::cerr << "GenerateNewKinematicPopulation\n";
            GenerateNewKinematicPopulation(i + 1, bestGenomeFile, modelPopulationFile, kinematicIncrement, kinematicStride, kinematicRangeFraction, kinematicSDFraction, workingPopulationFile);

            // run the MergeXML command
            kinematicInitialTime += kinematicTimeIncrement;
            if (gaitMergeStart == gaitMergeFinish)
                sprintf(buffer, "mergexml -v -i %s --ignoreError -r INPUT_CONFIG_FILE %s -r DRIVER_FILE %s -r OUTPUT_CONFIG_FILE %s -r RUN_TIME %g", mergeFile, modelConfigFile, driverFile, workingConfigFile, kinematicInitialTime);
            else
                sprintf(buffer, "mergexml -v -i %s --ignoreError -r INPUT_CONFIG_FILE %s -r DRIVER_FILE %s -r OUTPUT_CONFIG_FILE %s -r RUN_TIME %g -r MERGE_CONFIG_FILE %s -r MERGE_VALUE %.17e",
                    mergeFile, modelConfigFile, driverFile, workingConfigFile, kinematicInitialTime, modelConfigFile2, gaitMergeCurrent);
            std::cerr << "Executing command:\n";
            std::cerr << buffer << "\n";
            nServerArgs = DataFile::ReturnTokens(buffer, pServerArgs, 256);
            MergeXMLMain(nServerArgs, pServerArgs);
            std::cerr << "MergeXMLMain finished\n";
        }
    }

    if (mpi_big_buffer) delete [] mpi_big_buffer;

    return 0;
}

void ApplyGenome(const char *inputXML, const char *inputGenome, const char *outputXML)
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

    delete [] data;
}

void GenerateNewKinematicPopulation(int runNumber, const char *bestGenomeFile, const char *modelPopulationFile, int kinematicIncrement, int kinematicStride, double kinematicRangeFraction, double kinematicSDFraction, const char *workingPopulationFile)
{
    DataFile file;
    double val, cmin, cmax, newRange2;
    int ival, genomeSize, i, j;

    // get the best data from the bestGenomeFile
    file.ReadFile(bestGenomeFile);
    file.ReadNext(&ival);
    file.ReadNext(&genomeSize);
    double *bestData = new double[genomeSize];
    for (i = 0; i < genomeSize; i++)
    {
        file.ReadNext(&val); bestData[i] = val;
        file.ReadNext(&val); file.ReadNext(&val); file.ReadNext(&val);
    }

    // get the min-max ranges and SD from the modelPopulationFile
    file.ReadFile(modelPopulationFile);
    file.ReadNext(&ival); file.ReadNext(&ival);
    file.ReadNext(&genomeSize);
    double *minRange = new double[genomeSize];
    double *maxRange = new double[genomeSize];
    double *SD = new double[genomeSize];
    for (i = 0; i < genomeSize; i++)
    {
        file.ReadNext(&val);
        file.ReadNext(&val); minRange[i] = val;
        file.ReadNext(&val); maxRange[i] = val;
        file.ReadNext(&val); SD[i] = val;
    }

    // reassign the population values
    for (i = 0; i < runNumber * kinematicIncrement; i++)
    {
        for (j = i; j < genomeSize; j += kinematicStride)
        {
            cmin = minRange[j];
            cmax = maxRange[j];
            newRange2 = 0.5 * kinematicRangeFraction * (cmax - cmin);

            minRange[j] = bestData[j] - newRange2;
            if (minRange[j] < cmin) minRange[j] = cmin;

            maxRange[j] = bestData[j] + newRange2;
            if (maxRange[j] > cmax) maxRange[j] = cmax;

            SD[j] = SD[j] * kinematicSDFraction;
        }
    }

    // now create the new population
    std::ofstream ofs(workingPopulationFile);
    ofs << "1\n-1\n" << genomeSize << "\n";
    for (i = 0; i < genomeSize; i++)
    {
        ofs << bestData[i] << "\t" << minRange[i] << "\t" << maxRange[i] << "\t" << SD[i] << "\n";
    }
    ofs << "0\t0\t0\t0\t0\n";

    delete [] bestData;
    delete [] minRange;
    delete [] maxRange;
    delete [] SD;
}
