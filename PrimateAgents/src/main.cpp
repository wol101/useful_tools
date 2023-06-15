/*
 *  main.cpp
 *  Primate Agent
 *
 *  Created by Bill Sellers on Wed Sep 22 2004.
 *  Copyright (c) 2004 Bill Sellers. All rights reserved.
 *
 */

// program to read (and optionally verify) the XML file
// specifying the rnaging environment and output comma
// separated blocks of data that can be easily read into
// the agent modelling package

using namespace std;

#include <stdio.h>
#include <iostream>
#include <time.h>
#include <unistd.h>
#include <sys/stat.h>

#if defined(USE_OPENGL)
#include <glut.h>
#include "GLUIRoutines.h"
#endif

#include "Simulation.h"
#include "Environment.h"
#include "Agent.h"
#include "DataFile.h"
#include "Random.h"

#define DEBUG_MAIN
#include "DebugControl.h"

Simulation g_Simulation;

int main(int argc, char *argv[])
{
    int i;
    char *inFile = 0;
    char *outputDirectory = 0;
    struct stat sb;
    string dirname;
    time_t theTime = time(0);
    struct tm *theLocalTime = localtime(&theTime);
    string agentLogFilename;
    string habitatTypeLogFilename;
    string gridSquareLogFilename;
    int error;
    char buffer[256];
    RANDOM_SEED_TYPE randomSeed = (RANDOM_SEED_TYPE)time(0) * (RANDOM_SEED_TYPE)getpid();
    
#if defined(USE_OPENGL)
    // read any arguments relevent to OpenGL and remove them from the list
    // also intialises the windowing system
    glutInit(&argc, argv);
#endif

    // parse the command line
    try
    {
        for (i = 1; i < argc; i++)
        {
            // do something with arguments

            if (strcmp(argv[i], "-i") == 0)
            {
                i++;
                if (i >= argc) throw(1);
                inFile = argv[i];
            }

            else if (strcmp(argv[i], "-o") == 0)
            {
                i++;
                if (i >= argc) throw(1);
                outputDirectory = argv[i];
            }

            else if (strcmp(argv[i], "-s") == 0)
            {
                i++;
                if (i >= argc) throw(1);
                randomSeed = strtol(argv[i], 0, 10);
            }

            else if (strcmp(argv[i], "-d") == 0)
            {
                i++;
                if (i >= argc) throw(1);
                g_Debug = strtol(argv[i], 0, 10);
            }

            else
            {
                throw(1);
            }
        }
	
	if (inFile == 0) throw(1);
    }
        
    catch (int e)
    {
        cerr << "\nprimate_agent program\n";
        cerr << "Build " << __DATE__ << " " << __TIME__ << "\n";
        cerr << "Usage: primate_agent [options]\n\n";
        cerr << "-i inputFileName\n";
        cerr << "-o outputDirectory\n";
        cerr << "-s randomSeed\n";
        
        int nDebugLabels = sizeof(gDebugLabels) / sizeof(gDebugLabels[0]);
        cerr << "-d n\n";
        cerr << "Prints out a lot of extra debugging information on stderr if n is higher than 0.\n";
        cerr << "Debug numbers:\n";
        for (i = 0; i < nDebugLabels; i++)
            fprintf(stderr, "%3d %s\n", i, gDebugLabels[i]);
        cerr << "\n";
        return 1;
    }

    // set the random seed
    RandomSeed(randomSeed);    

    // create a directory for all the output
    if (outputDirectory)
    {
        dirname = outputDirectory;
        if (stat(dirname.c_str(), &sb) == -1)
        {
            error = mkdir(dirname.c_str(), 0777);
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
        sprintf(buffer, "Run_%04d-%02d-%02d_%02d.%02d.%02d",
                theLocalTime->tm_year + 1900, theLocalTime->tm_mon + 1,
                theLocalTime->tm_mday,
                theLocalTime->tm_hour, theLocalTime->tm_min,
                theLocalTime->tm_sec);
        dirname = buffer;
        error = mkdir(dirname.c_str(), 0777);
        if (error)
        {
            cerr << "Error creating directory " << dirname << "\n";
            return 1;
        }
    }

    DataFile theXMLFile;
    string inFileName = inFile;
    string outFileName = dirname + "/";
    if (theXMLFile.ReadFile(inFileName.c_str()))
    {
        cerr << "Error reading input document: " << inFileName << "\n";
        return 1;
    }
    string::size_type lastDirectorySeparator = inFileName.rfind( "/", inFileName.size());
    if (lastDirectorySeparator == string::npos) outFileName.append(inFileName);
    else outFileName.append(inFileName.substr(lastDirectorySeparator + 1, inFileName.size()));
    theXMLFile.WriteFile(outFileName.c_str());

    if (g_Simulation.LoadSimulation((xmlChar *)theXMLFile.GetRawData()))
    {
        cerr << "Error parsing XML document: " << inFileName << "\n";
    }

    agentLogFilename = dirname + "/AgentLog.txt";
    habitatTypeLogFilename = dirname + "/HabitatTypeLog.txt";
    gridSquareLogFilename = dirname + "/GridSquareLog.txt";
    g_Simulation.SetLogFiles(agentLogFilename, habitatTypeLogFilename, gridSquareLogFilename);
    
#if defined(USE_OPENGL)
    
    // now initialise the OpenGL bits
    StartGlut();

    // and enter the never to be returned loop
    glutMainLoop();
#else
    long count = 0;
    while (g_Simulation.ShouldFinish() == false)
    {
        g_Simulation.UpdateSimulation();
        count++;
        if (count % 10000 == 0) cerr << count << "\n";
    }
    
#endif
    

    return 0;
}


