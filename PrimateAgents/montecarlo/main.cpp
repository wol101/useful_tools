/*
 *  main.cpp
 *  MonteCarlo
 *
 *  Created by Bill Sellers on Sun Mar 06 2005.
 *  Copyright (c) 2005 Bill Sellers. All rights reserved.
 *
 */

using namespace std;

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <sys/stat.h>

#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#include "RangedVariable.h"
#include "DataFile.h"
#include "Random.h"

const int LINE_BUFFER_SIZE = 1024; // should be plenty big enouhg

int main (int argc, const char * argv[])
{
    const char *rangesFileName = 0;
    const char *inputXMLFileName = 0;
    const char *programToRun = 0;
    const char *outputDirectory = 0;
    char *outputXMLFileName = "CurrentMonteCarlo.xml";
    char *logFileName = "MonteCarloLog.txt";
    DataFile rangesFile;
    DataFile inputXMLFile;
    char buffer[LINE_BUFFER_SIZE];
    char *ptrs[LINE_BUFFER_SIZE];
    int count;
    vector<RangedVariable *> listOfRangedVariables;
    RangedVariable *rangedVariable;
    int numberOfTimesToRunProgram = 0;
    int i;
    unsigned int index;
    string xmlString;
    string::size_type pos;
    string sub;
    ofstream outputFile;
    double numberOfSDsInRange = 4.0;
    struct stat sb;
    time_t theTime = time(0);
    struct tm *theLocalTime = localtime(&theTime);
    int error;
    char dirname[LINE_BUFFER_SIZE];
    char path[LINE_BUFFER_SIZE];
    RANDOM_SEED_TYPE randomSeed = (RANDOM_SEED_TYPE)time(0) * (RANDOM_SEED_TYPE)getpid();

    rangesFile.SetExitOnError(true);
    inputXMLFile.SetExitOnError(true);
    
    // parse the command line
    try
    {
        for (i = 1; i < argc; i++)
        {
            // do something with arguments

            if (strcmp(argv[i], "-r") == 0)
            {
                i++;
                if (i >= argc) throw(1);
                rangesFileName = argv[i];
            }

            else if (strcmp(argv[i], "-i") == 0)
            {
                i++;
                if (i >= argc) throw(1);
                inputXMLFileName = argv[i];
            }

            else if (strcmp(argv[i], "-p") == 0)
            {
                i++;
                if (i >= argc) throw(1);
                programToRun = argv[i];
            }

            else if (strcmp(argv[i], "-n") == 0)
            {
                i++;
                if (i >= argc) throw(1);
                numberOfTimesToRunProgram = strtol(argv[i], 0, 10);
            }

            else if (strcmp(argv[i], "-s") == 0)
            {
                i++;
                if (i >= argc) throw(1);
                numberOfSDsInRange = strtod(argv[i], 0);
            }
            
            else
            {
                throw(1);
            }
        }

        if (rangesFileName == 0) throw(1);
        if (inputXMLFileName == 0) throw(1);
        if (programToRun == 0) throw(1);
        if (numberOfTimesToRunProgram < 1) throw(1);
    }

    catch (int e)
    {
        cerr << "\nmonte_carlo program\n";
        cerr << "Build " << __DATE__ << " " << __TIME__ << "\n";
        cerr << "Usage: monte_carlo [options]\n\n";
        cerr << "-r rangesFileName\n";
        cerr << "-i inputXMLFileName\n";
        cerr << "-p programToRun\n";
        cerr << "-n numberOfTimesToRunProgram\n";
        cerr << "-s numberOfSDsInRange (default=4.0)\n";
        return 1;
    }

    // set the random seed
    RandomSeed(randomSeed);
    
    // create a directory for all the output
    if (outputDirectory)
    {
        strcpy(dirname, outputDirectory);
        if (stat(dirname, &sb) == -1)
        {
            error = mkdir(dirname, 0777);
            if (error)
            {
                cerr << "Error creating directory " << dirname << "\n";
                return 1;
            }
        }
        else
        {
            if ((sb.st_mode & S_IFDIR) == 0)
            {
                cerr << "Error creating directory (exists but not a directory) " << dirname << "\n";
                return 1;
            }
        }
    }
    else
    {
        sprintf(dirname, "MC_Run_%04d-%02d-%02d_%02d.%02d.%02d",
                theLocalTime->tm_year + 1900, theLocalTime->tm_mon + 1,
                theLocalTime->tm_mday,
                theLocalTime->tm_hour, theLocalTime->tm_min,
                theLocalTime->tm_sec);
        error = mkdir(dirname, 0777);
        if (error)
        {
            cerr << "Error creating directory " << dirname << "\n";
            return 1;
        }
    }

    // create the log file
    sprintf(path, "%s/%s", dirname, logFileName);
    ofstream logFile(path);
    for (i = 0; i < argc; i++) // output the command line
    {
        logFile << argv[i];
        if (i < argc - 1) logFile << " ";
    }
    logFile << "\n";
    logFile.flush();

    // read the ranges file
    rangesFile.ReadFile(rangesFileName);
    sprintf(path, "%s/MonteCarloRangesFile.txt", dirname);
    rangesFile.WriteFile(path);

    while (rangesFile.ReadNextLine(buffer, LINE_BUFFER_SIZE, true, '#') == false)
    {
        count = DataFile::ReturnTokens(buffer, ptrs, LINE_BUFFER_SIZE);
        if (count != 3)
        {
            cerr << "Error parsing ranges file: " << buffer << "\n";
            return 1;
        }

        rangedVariable = new RangedVariable(ptrs[0], strtod(ptrs[1], 0), strtod(ptrs[2], 0), numberOfSDsInRange);
        listOfRangedVariables.push_back(rangedVariable);
    }
    // output the ranges names
    for (index = 0; index < listOfRangedVariables.size(); index++) // output the command line
    {
        logFile << listOfRangedVariables[index]->GetName();
        if (index < listOfRangedVariables.size() - 1) logFile << "\t";
    }
    logFile << "\n";
    logFile.flush();
    
    // read the XML file
    inputXMLFile.ReadFile(inputXMLFileName);
    sprintf(path, "%s/MonteCarloInputFile.xml", dirname);
    inputXMLFile.WriteFile(path);

    // enter main repeat loop
    for (i = 0; i < numberOfTimesToRunProgram; i++)
    {
        xmlString = inputXMLFile.GetRawData();
        for (index = 0; index < listOfRangedVariables.size(); index++)
        {
            pos = xmlString.find(listOfRangedVariables[index]->GetName());
            if (pos == string::npos)
            {
                cerr << "Error finding " << listOfRangedVariables[index]->GetName() << "\n";
                return 1;
            }
            sprintf(buffer, "%.17g", listOfRangedVariables[index]->GetRandomValue());
            sub = buffer;
            xmlString.replace(pos, listOfRangedVariables[index]->GetName().size(), sub);
            logFile << buffer;
            if (index < listOfRangedVariables.size() - 1) logFile << "\t";
        }
        logFile << "\n";
        logFile.flush();

        sprintf(path, "%s/%s", dirname, outputXMLFileName);
        outputFile.open(path);
        outputFile << xmlString;
        outputFile.close();

        sprintf(buffer, "%s -i \"%s\" -o \"%s\"", programToRun, path, dirname);
        cout << buffer << "\n";
        system(buffer);

        
    }

    for (index = 0; index < listOfRangedVariables.size(); index++)
        delete listOfRangedVariables[index];
    
    return 0;
}
