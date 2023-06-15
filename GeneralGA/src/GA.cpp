// ga.cc implements a simple steady state GA system with the genome
// represented as an array of floating point numbers

#ifdef USE_MPI
#include <mpi.h>
#endif

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <iostream>
#include <fstream>
#include <string.h>
#include <time.h>
#include <float.h>
#include <unistd.h>
#include <sys/stat.h>

using namespace std;

#include "Genome.h"
#include "Mating.h"
#include "Statistics.h"
#include "Population.h"
#include "Random.h"
#include "DataFile.h"
#include "XMLConverter.h"

#ifndef USE_MPI
#define gDebugDeclare
#endif
#include "Debug.h"

#if defined(USE_SOCKETS)
#include <pinet.h>
pt::ipstmserver g_svr;
ofstream gSocketLogFile;

#elif defined(USE_UDP)
#include "UDP.h"
UDP gUDP;

#elif defined(USE_TCP)
#include "TCP.h"
TCP gTCP;

#elif defined(USE_MPI)
extern int gMPI_Comm_size;
extern int gMPI_Comm_rank;

#endif 

const int MAX_PATH_LENGTH = 1024;

int gUsleep = 100;
int gRedundancyPercent = 0;
int gTCPRetryCount = 10;

XMLConverterGA *gXMLConverter = 0;

int gCircularMutation = 0;


#ifndef USE_MPI
int main(int argc, char *argv[])
#else
int GAMain(int argc, char *argv[])
#endif
{
    int i, j;
    double maxFitness;
    double lastMaxFitness;
    int parent1Rank;
    int parent2Rank;
    
    // set the run parameters
    int genomeLength = 10;
    int populationSize = 500;
    int parentsToKeep = 10;
    int immigrations = 0;
    int maxGenerations = 100;
    double lowBound = 0;
    double highBound = 1.0;
    double gaussianMutationChance = 1.0;
    double frameShiftMutationChance = 1.0;
    double duplicationMutationChance = 0.001;
    double crossoverChance = 0.5;
    bool multipleGaussian = false;
    int saveBestEvery = 10;
    int savePopEvery = 100;
    bool onlyKeepBestGenome = false;
    bool onlyKeepBestPopulation = false;
    SelectionType parentSelection = RankBasedSelection;
    RANDOM_SEED_TYPE randomSeed = (RANDOM_SEED_TYPE)time(0) * (RANDOM_SEED_TYPE)getpid();
    int parallelObjectives = 1;
    int improvementGenerations = 0;
    double improvementThreshold = 0.1;
    char *modelGenome = 0;
    char *startingPopulation = 0;
    char *parameterStartingPopulation = 0;
    Genome theModelGenome;
    bool normalStats = false;
    double defaultGaussianSD = 1.0;
    bool incrementalMutation = false;
    bool removeDuplicates = false;
    char *logNotes = 0;
    char *genealogyFile = 0;
    GenomeType genomeType = IndividualRanges;
    char *parameterFile = 0;
    char *restartDirectory = 0;
    char restartLog[MAX_PATH_LENGTH];
    DataFile params;
    bool status;
    char buffer[MAX_PATH_LENGTH];
    struct stat sb;
    char filename[MAX_PATH_LENGTH];
    time_t theTime = time(0);
    struct tm *theLocalTime = localtime(&theTime);
    char dirname[MAX_PATH_LENGTH];
    int error;
    int startingGenerationNumber = 0;
    int maxGenerationsOveride = -1;
    CrossoverType crossoverType = OnePoint;
    time_t fitnessTimeLimit = 0;
    int fitnessRestartOnError = 1;
    char *extraDataFile = 0;
    int testport = 8086;
    char *bestGenomeFile = 0;
    char *bestPopulationFile = 0;
    char *bestGenomeFileXML = 0;
    char *outputDirectory = 0;
    bool randomiseModel = false;
    bool steadyState = false;
    char *objectiveCommand = 0;
    char *ptr;
    ConversionType conversionType = SixMuscle;
    char lastBestGenomeFile[MAX_PATH_LENGTH] = "";
    char lastBestGenomeFileXML[MAX_PATH_LENGTH] = "";
    char lastBestPopulationFile[MAX_PATH_LENGTH] = "";
    gXMLConverter = new XMLConverterGA();
    int outputPopulationSize = 0;

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
                if (strcmp(argv[i], "--restartDirectory") == 0 ||
                    strcmp(argv[i], "-r") == 0)
                {
                    i++;
                    if (i >= argc) throw(1);
                    restartDirectory = argv[i];
                }
            
            else
                if (strcmp(argv[i], "--bestGenome") == 0 ||
                    strcmp(argv[i], "-b") == 0)
                {
                    i++;
                    if (i >= argc) throw(1);
                    bestGenomeFile = argv[i];
                }
            
            else
                if (strcmp(argv[i], "--bestGenomeFileXML") == 0 ||
                    strcmp(argv[i], "-x") == 0)
                {
                    i++;
                    if (i >= argc) throw(1);
                    bestGenomeFileXML = argv[i];
                }
            
            else
                if (strcmp(argv[i], "--bestPopulation") == 0 ||
                    strcmp(argv[i], "-l") == 0)
                {
                    i++;
                    if (i >= argc) throw(1);
                    bestPopulationFile = argv[i];
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
                    if (sscanf(argv[i], "%d", &gDebug) != 1) throw(1);
                }
            
            else
                if (strcmp(argv[i], "--maxGenerationsOveride") == 0 ||
                    strcmp(argv[i], "-g") == 0)
                {
                    i++;
                    if (i >= argc) throw(1);
                    if (sscanf(argv[i], "%d", &maxGenerationsOveride) != 1) throw(1);
                }
            
            else
                if (strcmp(argv[i], "--port") == 0 ||
                    strcmp(argv[i], "-t") == 0)
                {
                    i++;
                    if (i >= argc) throw(1);
                    if (sscanf(argv[i], "%d", &testport) != 1) throw(1);
                }
            
            else
                if (strcmp(argv[i], "--usleep") == 0 ||
                    strcmp(argv[i], "-u") == 0)
                {
                    i++;
                    if (i >= argc) throw(1);
                    if (sscanf(argv[i], "%d", &gUsleep) != 1) throw(1);
                }
            else
                if (strcmp(argv[i], "--redundancyPercent") == 0 ||
                    strcmp(argv[i], "-R") == 0)
                {
                    i++;
                    if (i >= argc)
                    {
                        std::cerr << "Error parsing --redundancyPercent\n";
                        exit(1);
                    }
                    gRedundancyPercent = strtol(argv[i], 0, 10);
                }
            else
            {
                throw(1);
            }
        }
        if (parameterFile == 0 && restartDirectory == 0) throw(2);
    }
    
    // catch argument errors
    catch (int e)
    {
        cerr << "\nGenetic Algorithm program\n";
        cerr << "Build " << __DATE__ << " " << __TIME__ << "\n";
        cerr << "Supported options:\n\n";
        cerr << "(-p) --parameterFile file_name\n";
        cerr << "(-r) --restartDirectory directory_name\n";
        cerr << "(-b) --bestGenome file_name\n";
        cerr << "(-x) --bestGenomeFileXML file_name\n";
        cerr << "(-l) --bestPopulation file_name\n";
        cerr << "(-o) --outputDirectory directory_name\n";
        cerr << "(-s) --startingPopulation population_file";
        cerr << "(-d) --debug n\n";
        cerr << "(-g) --maxGenerationsOveride n\n";
        cerr << "(-t) --port n (default 8086)\n";
        cerr << "(-u) --usleep n (default 100)\n";
        cerr << "(-R) --redundancyPercent n (default 0)\n";
        cerr << "\nNote: --parameterFile or --restartDirectory required\n";
        cerr << "Short form is shown in brackets.\n";
        cerr << "\n\n";
        cerr << "Debug_GA 1\n";
        cerr << "Debug_Genome 2\n";
        cerr << "Debug_HillClimb 3\n";
        cerr << "Debug_Mating 4\n";
        cerr << "Debug_Objective 5\n";
        cerr << "Debug_ParameterFile 6\n";
        cerr << "Debug_Population 7\n";
        cerr << "Debug_Random 8\n";
        cerr << "Debug_StartupScreen 9\n";
        cerr << "Debug_Statistics 10";
        cerr << "Debug_TestObjective 11\n";
        cerr << "Debug_TestRandom 12\n";
        cerr << "Debug_Utility 13\n";
        cerr << "Debug_Sockets 14\n";
        
        return (e);
    }
    
    if (gDebug == Debug_GA)
    {
        for (i = 0; i < argc; i++)
            cerr << i << " " << argv[i] << "\n";
    }

    if (restartDirectory) // set parameterFile to previous log file
    {
        sprintf(restartLog, "%s/log.txt", restartDirectory);
        parameterFile = restartLog;
    }
    
    if (params.ReadFile(parameterFile))
    {
        cerr << "Error reading " << parameterFile << "\n";
        return (1);
    }
    
    status = params.RetrieveParameter("genomeLength", &genomeLength);
    status = params.RetrieveParameter("populationSize", &populationSize);
    status = params.RetrieveParameter("parentsToKeep", &parentsToKeep);
    status = params.RetrieveParameter("immigrations", &immigrations);
    status = params.RetrieveParameter("maxGenerations", &maxGenerations);
    status = params.RetrieveParameter("lowBound", &lowBound);
    status = params.RetrieveParameter("highBound", &highBound);
    status = params.RetrieveParameter("gaussianMutationChance", &gaussianMutationChance);
    status = params.RetrieveParameter("frameShiftMutationChance", &frameShiftMutationChance);
    status = params.RetrieveParameter("duplicationMutationChance", &duplicationMutationChance);
    status = params.RetrieveParameter("crossoverChance", &crossoverChance);
    status = params.RetrieveParameter("defaultGaussianSD", &defaultGaussianSD);
    status = params.RetrieveParameter("saveBestEvery", &saveBestEvery);
    status = params.RetrieveParameter("savePopEvery", &savePopEvery);
    status = params.RetrieveParameter("onlyKeepBestGenome", &onlyKeepBestGenome);
    status = params.RetrieveParameter("onlyKeepBestPopulation", &onlyKeepBestPopulation);
    status = params.RetrieveParameter("randomSeed", &randomSeed);
    status = params.RetrieveParameter("parallelObjectives", &parallelObjectives);
    status = params.RetrieveParameter("improvementGenerations", &improvementGenerations);
    status = params.RetrieveParameter("improvementThreshold", &improvementThreshold);
    status = params.RetrieveParameter("incrementalMutation", &incrementalMutation);
    status = params.RetrieveParameter("removeDuplicates", &removeDuplicates);
    status = params.RetrieveParameter("multipleGaussian", &multipleGaussian);
    status = params.RetrieveParameter("normalStats", &normalStats);
    status = params.RetrieveParameter("fitnessTimeLimit", &fitnessTimeLimit);
    status = params.RetrieveParameter("fitnessRestartOnError", &fitnessRestartOnError);
    status = params.RetrieveParameter("randomiseModel", &randomiseModel);
    status = params.RetrieveParameter("steadyState", &steadyState);
    
    if (params.RetrieveParameter("parentSelection", buffer, sizeof(buffer)) == 0)
    {
        if (strcmp(buffer, "UniformSelection") == 0)
            parentSelection = UniformSelection;
        if (strcmp(buffer, "RankBasedSelection") == 0)
            parentSelection = RankBasedSelection;
        if (strcmp(buffer, "SqrtBasedSelection") == 0)
            parentSelection = SqrtBasedSelection;
        if (strcmp(buffer, "RouletteWheelSelection") == 0)
            parentSelection = RouletteWheelSelection;
    }
    
    if (params.RetrieveParameter("crossoverType", buffer, sizeof(buffer)) == 0)
    {
        if (strcmp(buffer, "OnePoint") == 0)
            crossoverType = OnePoint;
        if (strcmp(buffer, "Average") == 0)
            crossoverType = Average;
    }
    
    if (params.RetrieveParameter("genomeType", buffer, sizeof(buffer)) == 0)
    {
        if (strcmp(buffer, "IndividualRanges") == 0)
            genomeType = IndividualRanges;
    }
    
    if (params.RetrieveParameter("conversionType", buffer, sizeof(buffer)) == 0)
    {
        if (strcmp(buffer, "SixMuscle") == 0)
            conversionType = SixMuscle;
        if (strcmp(buffer, "EightMuscle") == 0)
            conversionType = EightMuscle;
        if (strcmp(buffer, "TwelveMuscleQuad") == 0)
            conversionType = TwelveMuscleQuad;
        if (strcmp(buffer, "TwentyFourMuscleQuadKinematic") == 0)
            conversionType = TwentyFourMuscleQuadKinematic;
        if (strcmp(buffer, "TwelveMuscleQuadKinematic") == 0)
            conversionType = TwelveMuscleQuadKinematic;
        if (strcmp(buffer, "SmartSubstitution") == 0)
            conversionType = SmartSubstitution;
        gXMLConverter->SetConversionType(conversionType);
    }
    
    if (params.RetrieveParameter("modelGenome", buffer, sizeof(buffer)) == 0)
    {
        modelGenome = new char[sizeof(buffer)];
        strcpy(modelGenome, buffer);
    }
    
    if (startingPopulation == 0)
    {
        if (params.RetrieveParameter("startingPopulation", buffer, sizeof(buffer)) == 0)
        {
            parameterStartingPopulation = new char[sizeof(buffer)];
            strcpy(parameterStartingPopulation, buffer);
            startingPopulation = parameterStartingPopulation;
        }
    }
    
    if (params.RetrieveParameter("logNotes", buffer, sizeof(buffer)) == 0)
    {
        logNotes = new char[sizeof(buffer)];
        strcpy(logNotes, buffer);
    }
    
    if (params.RetrieveParameter("genealogyFile", buffer, sizeof(buffer)) == 0)
    {
        genealogyFile = new char[sizeof(buffer)];
        strcpy(genealogyFile, buffer);
    }
    
    if (params.RetrieveParameter("objectiveCommand", buffer, sizeof(buffer)) == 0)
    {
        objectiveCommand = new char[sizeof(buffer)];
        strcpy(objectiveCommand, buffer);
    }
    
    if (params.RetrieveParameter("outputPopulationSize", &outputPopulationSize))
    {
        outputPopulationSize = populationSize; // defaults to whole population
    }
    if (outputPopulationSize > populationSize) outputPopulationSize = populationSize;
    
    params.RetrieveParameter("circularMutation", &gCircularMutation);
    
    if (modelGenome && startingPopulation)
    {
        cerr << "Cannot specify both --modelGenome and --startingPopulation\n";
        return 1;
    }
    
    if (populationSize < immigrations)
    {
        cerr << "Population size less than immigrations per generation\n";
        return 1;
    }
    
    if (populationSize < parentsToKeep)
    {
        cerr << "Population size less than parents to keep per generation\n";
        return 1;
    }
    
    if (params.RetrieveParameter("extraDataFile", buffer, sizeof(buffer)) == 0)
    {
        extraDataFile = new char[sizeof(buffer)];
        strcpy(extraDataFile, buffer);
        if (gXMLConverter->LoadBaseXMLFile(extraDataFile))
        {
            cerr << "Error reading " << buffer << "\n";
            return (1);
        }
    }
    else
    {
        if (conversionType == SmartSubstitution)
        {
            cerr << "Warning extraDataFile may be required with SmartSubstitution\n";
        }
    }
    
    // overide max generations if necessary
    if (maxGenerationsOveride > 0) maxGenerations = maxGenerationsOveride;
    
    // read in the model genome
    if (modelGenome)
    {
        ifstream in(modelGenome);
        if (in.bad())
        {
            cerr << "Error reading model genome: " << modelGenome << "\n";
            return 1;
        }
        in >> theModelGenome;
        in.close();
        
        if (theModelGenome.GetGenomeLength() != genomeLength)
        {
            cerr << "Model genome does not match specified genome length\n";
            return 1;
        }
        if (theModelGenome.GetGenomeType() != genomeType)
        {
            cerr << "Model genome does not match specified genome type\n";
            return 1;
        }
    }
    
    // read in last population if restarting
    
    if (restartDirectory)
    {
        if (savePopEvery == 0)
        {
            cerr << "No populations to restart from\n";
            return 1;
        }
        int maxPopulations = maxGenerations / savePopEvery;
        for (i = maxPopulations; i >= 0; i--)
        {
            sprintf(filename, "%s/Population_%07d.txt", restartDirectory, i * savePopEvery - 1);
            error = stat(filename, &sb);
            if (error == 0 && sb.st_size != 0)
            {
                if (startingPopulation) delete [] startingPopulation;
                startingPopulation = new char[MAX_PATH_LENGTH];
                strcpy(startingPopulation, filename);
                startingGenerationNumber = i * savePopEvery - 1;
                break;
            }
        }
        if (i == 0) 
        {
            cerr << "No populations to restart from\n";
            return 1;
        }
    }            
    
#if defined(USE_SOCKETS)                    
    // set up the server
    try
    {
        g_svr.bindall(testport);
    }
    catch(pt::estream* e)
    {
        pt::perr.putf("Error: %s\n", pt::pconst(e->get_message()));
        cerr << "Error setting up server - port probably in use\n";
        delete e;
        return 1;
    }
#elif defined(USE_UDP)  
    if (gUDP.StartListener(testport) != 0)
    {
        cerr << "Error setting up server - port probably in use\n";
        return 1;
    }
    if (gUDP.StartTalker() != 0)
    {
        cerr << "Error setting up server\n";
        return 1;
    }
#elif defined(USE_TCP)  
    if (gTCP.StartServer(testport) != 0)
    {
        cerr << "Error setting up server - port probably in use\n";
        return 1;
    }
#endif
    
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
        sprintf(dirname, "Run_%04d-%02d-%02d_%02d.%02d.%02d", 
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
    
#if defined(USE_SOCKETS)
    if (gDebug == Debug_Sockets)
    {
        // open socket log file
        sprintf(filename, "%s/socket_log.txt", dirname);
        gSocketLogFile.open(filename);
    }
#endif
    
    // write log
    sprintf(filename, "%s/log.txt", dirname);
    ofstream outFile(filename);
    outFile << "GA build " << __DATE__ << " " << __TIME__ "\n";
    outFile << "Log produced " << asctime(theLocalTime);
    
    outFile << "Command line:\n";
    for (i = 0; i < argc - 1; i++) outFile << argv[i] << " ";
    outFile << argv[i] << "\n";
    
    if (restartDirectory)
    {
        outFile << "restartDirectory\t\"" << restartDirectory << "\"\n";
        outFile << "startingGenerationNumber\t" << startingGenerationNumber << "\n";
    }
    
    outFile << "parameterFile\t\"" << parameterFile << "\"\n";
    
    if (logNotes)
    {
        outFile << "logNotes\n\"" << logNotes << "\"\n";
    }
    
    outFile << "genomeLength\t" << genomeLength << "\n";
    outFile << "populationSize\t" << populationSize << "\n";
    outFile << "parentsToKeep\t" << parentsToKeep << "\n";
    outFile << "immigrations\t" << immigrations << "\n";
    outFile << "maxGenerations\t" << maxGenerations << "\n";
    outFile << "lowBound\t" << lowBound << "\n";
    outFile << "highBound\t" << highBound << "\n";
    outFile << "gaussianMutationChance\t" << gaussianMutationChance << "\n";
    outFile << "frameShiftMutationChance\t" << frameShiftMutationChance << "\n";
    outFile << "duplicationMutationChance\t" << duplicationMutationChance << "\n";
    outFile << "crossoverChance\t" << crossoverChance << "\n";
    outFile << "defaultGaussianSD\t" << defaultGaussianSD << "\n";
    outFile << "circularMutation\t" << gCircularMutation << "\n";
    outFile << "saveBestEvery\t" << saveBestEvery << "\n";
    outFile << "savePopEvery\t" << savePopEvery << "\n";
    outFile << "onlyKeepBestGenome\t" << onlyKeepBestGenome << "\n";
    outFile << "onlyKeepBestPopulation\t" << onlyKeepBestPopulation << "\n";
    outFile << "randomSeed\t" << randomSeed << "\n";
    outFile << "parallelObjectives\t" << parallelObjectives << "\n";
    outFile << "improvementGenerations\t" << improvementGenerations << "\n";
    outFile << "improvementThreshold\t" << improvementThreshold << "\n";
    outFile << "removeDuplicates\t" << removeDuplicates << "\n";
    outFile << "multipleGaussian\t" << multipleGaussian << "\n";
    outFile << "normalStats\t" << normalStats << "\n";
    outFile << "fitnessTimeLimit\t" << fitnessTimeLimit << "\n";
    outFile << "fitnessRestartOnError\t" << fitnessRestartOnError << "\n";
    outFile << "steadyState\t" << steadyState << "\n";
    outFile << "randomiseModel\t" << randomiseModel << "\n";
    outFile << "outputPopulationSize\t" << outputPopulationSize << "\n";
    
    if (modelGenome)
    {
        outFile << "modelGenome\t\"" << modelGenome << "\"\n";
        outFile << "incrementalMutation\t" << incrementalMutation << "\n";
    }
    
    if (startingPopulation)
    {
        outFile << "startingPopulation\t\"" << startingPopulation << "\"\n";
    }
    
    if (extraDataFile)
    {
        outFile << "extraDataFile\t\"" << extraDataFile << "\"\n";
    }
    
    if (genealogyFile)
    {
        outFile << "genealogyFile\t\"" << genealogyFile << "\"\n";
    }
    
    if (objectiveCommand)
    {
        outFile << "objectiveCommand\t\"" << objectiveCommand << "\"\n";
    }
    
    switch (parentSelection)
    {
        case UniformSelection: 
            outFile << "parentSelection\t" << "UniformSelection\n";
            break;
        case RankBasedSelection: 
            outFile << "parentSelection\t" << "RankBasedSelection\n";
            break;
        case SqrtBasedSelection: 
            outFile << "parentSelection\t" << "SqrtBasedSelection\n";
            break;
        case RouletteWheelSelection: 
            outFile << "parentSelection\t" << "RouletteWheelSelection\n";
            break;
        default:
            cerr << "Unrecognised parentSelection\n";
            return 1;
    }
    
    switch (crossoverType)
    {
        case OnePoint:
            outFile << "crossoverType\t" << "OnePoint\n";
            break;
        case Average:
            outFile << "crossoverType\t" << "Average\n";
            break;
        default:
            cerr << "Unrecognised crossoverType\n";
            return 1;
    }
    
    switch (genomeType)
    {
        case IndividualRanges: 
            outFile << "genomeType\t" << "IndividualRanges\n";
            break;
        default:
            cerr << "Unrecognised genomeType\n";
            return 1;
    }
    
    switch (conversionType)
    {
        case SixMuscle:
            outFile << "conversionType\t" << "SixMuscle\n";
            break;
        case EightMuscle:
            outFile << "conversionType\t" << "EightMuscle\n";
            break;
        case TwelveMuscleQuad:
            outFile << "conversionType\t" << "TwelveMuscleQuad\n";
            break;
        case TwentyFourMuscleQuadKinematic:
            outFile << "conversionType\t" << "TwentyFourMuscleQuadKinematic\n";
            break;
        case TwelveMuscleQuadKinematic:
            outFile << "conversionType\t" << "TwelveMuscleQuadKinematic\n";
            break;
        case SmartSubstitution:
            outFile << "conversionType\t" << "SmartSubstitution\n";
            break;
        default:
            cerr << "Unrecognised conversionType\n";
            return 1;
    }
    
    outFile.flush();
    
    // set the random seed
    RandomSeed(randomSeed);
    
    Population thePopulation;
    Population theOffspring;
    Genome *parent1;
    Genome *parent2;
    
    Statistics stats;
    TenPercentiles tenPercentiles;
    
    // initialise the population
    if (startingPopulation)
    {
        thePopulation.InitialisePopulation(populationSize, genomeType, 
                                           genomeLength, 
                                           lowBound, highBound, false, lowBound, defaultGaussianSD);
        ifstream in(startingPopulation);
        if (in.bad())
        {
            cerr << "Error reading starting population: " << startingPopulation << "\n";
            return 1;
        }
        in >> thePopulation;
        in.close();
        
        if (thePopulation.GetPopulationSize() != populationSize)
        {
            cerr << "Starting population size does not match specified population size - RESIZING\n";
            thePopulation.Resize(populationSize);
        }
        if (thePopulation.GetGenome(0)->GetGenomeLength() != genomeLength)
        {
            cerr << "Starting population genome does not match specified genome length\n";
            return 1;
        }
        if (thePopulation.GetGenome(0)->GetGenomeType() != genomeType)
        {
            cerr << "Starting population genome does not match specified genome type\n";
            return 1;
        }
        if (randomiseModel)
        {
            thePopulation.Randomise(0, populationSize);
        }
    }
    else
    {
        if (modelGenome)
        {
            // generate a population based on the model
            thePopulation.InitialisePopulation(populationSize, genomeType, 
                                               genomeLength, 
                                               lowBound, highBound, false, lowBound, defaultGaussianSD);
            *(thePopulation.GetGenome(0)) = theModelGenome;
            if (randomiseModel)
            {
                for (i = 0; i < populationSize; i++)
                    *(thePopulation.GetGenome(i)) = theModelGenome;
                thePopulation.Randomise(0, populationSize);
            }
            else
            {
                *(thePopulation.GetGenome(0)) = theModelGenome;
                // keep mutating to generate a non-uniform population
                for (i = 1; i < populationSize; i++)
                {
                    if (incrementalMutation)
                    {      
                        if (multipleGaussian)
                            MultipleGaussianMutate(&theModelGenome, gaussianMutationChance); 
                        else
                            GaussianMutate(&theModelGenome, gaussianMutationChance); 
                        FrameShiftMutate(&theModelGenome, frameShiftMutationChance); 
                        DuplicationMutate(&theModelGenome, duplicationMutationChance); 
                        *(thePopulation.GetGenome(i)) = theModelGenome;
                    }
                    else
                    {
                        *(thePopulation.GetGenome(i)) = theModelGenome;
                        if (multipleGaussian)
                            MultipleGaussianMutate(thePopulation.GetGenome(i), gaussianMutationChance); 
                        else
                            GaussianMutate(thePopulation.GetGenome(i), gaussianMutationChance); 
                        FrameShiftMutate(thePopulation.GetGenome(i), frameShiftMutationChance); 
                        DuplicationMutate(thePopulation.GetGenome(i), duplicationMutationChance); 
                    }
                }
            }
        }
        else
        {
            // generate a random population between the bounds
            thePopulation.InitialisePopulation(populationSize, genomeType,
                                               genomeLength, 
                                               lowBound, highBound, true, 0, defaultGaussianSD);
        }
    }
    
    thePopulation.SetSelectionType(parentSelection);
    thePopulation.SetMaxRunning(parallelObjectives);
    thePopulation.CalculateFitness(0, populationSize, objectiveCommand, 
                                   fitnessTimeLimit, fitnessRestartOnError);
    
    thePopulation.Sort();
    maxFitness = thePopulation.GetGenome(populationSize - 1)->GetFitness();
    lastMaxFitness = maxFitness;
    
    if (gDebug == Debug_GA)
        cerr << "main\tmaxFitness\t" << maxFitness << "\n";
    
    // set up the initial genealogy
    for (i = 0; i < populationSize; i++)
    {
        thePopulation.GetGenome(i)->SetParent1(0, i);
        thePopulation.GetGenome(i)->SetParent2(0, -1);
    }       
    
    // output best initial genome + population
    sprintf(filename, "%s/BestGenome_%07d.txt", dirname, startingGenerationNumber);
    ofstream initialBestFile(filename);
    initialBestFile << *thePopulation.GetGenome(populationSize - 1);
    initialBestFile.close();
    strcpy(lastBestGenomeFile, filename);
    
    sprintf(filename, "%s/BestGenome_%07d.xml", dirname, startingGenerationNumber);
    gXMLConverter->ApplyGenome(thePopulation.GetGenome(populationSize - 1));
    ptr = (char *)gXMLConverter->GetFormattedXML(&i);
    ofstream initialBestFileXML(filename);
    initialBestFileXML.write(ptr, i);
    initialBestFileXML.close();
    strcpy(lastBestGenomeFileXML, filename);
    
    // do we need to save the best population?
    if (savePopEvery > 0)
    {
        sprintf(filename, "%s/Population_%07d.txt", dirname, startingGenerationNumber);
        thePopulation.OutputSubpopulation(filename, outputPopulationSize);
        strcpy(lastBestPopulationFile, filename);
    }
    
    // genealogy file
    ofstream genealogyStream;
    if (genealogyFile)
    {
        sprintf(filename, "%s/%s", dirname, genealogyFile);
        genealogyStream.open(filename);
        genealogyStream << startingGenerationNumber << "\t";
        for (i = 0; i < populationSize; i++)
        {
            if (i != populationSize - 1)
                genealogyStream 
                    << thePopulation.GetGenome(i)->GetParent1()->generationNumber << "\t"
                    << thePopulation.GetGenome(i)->GetParent1()->rank << "\t"
                    << thePopulation.GetGenome(i)->GetParent2()->generationNumber << "\t"
                    << thePopulation.GetGenome(i)->GetParent2()->rank << "\t"
                    << thePopulation.GetGenome(i)->GetFitness() << "\t";
            else
                genealogyStream 
                    << thePopulation.GetGenome(i)->GetParent1()->generationNumber << "\t"
                    << thePopulation.GetGenome(i)->GetParent1()->rank << "\t"
                    << thePopulation.GetGenome(i)->GetParent2()->generationNumber << "\t"
                    << thePopulation.GetGenome(i)->GetParent2()->rank << "\t"
                    << thePopulation.GetGenome(i)->GetFitness() << "\n";
        }
        genealogyStream.flush();
    }
    
    // initialise the offspring population
    theOffspring.InitialisePopulation(populationSize, genomeType,
                                      genomeLength, 
                                      lowBound, highBound,
                                      false, lowBound, defaultGaussianSD);
    // make sure the bounds are copied over into the offspring
    for (i = 0; i < populationSize; i++)
        *theOffspring.GetGenome(i) = *thePopulation.GetGenome(i);
    
#if defined(USE_FS)  
    theOffspring.SetMaxRunning(parallelObjectives);
#endif
    
    // create steady state population if necessary
    Population steadyStatePopulation;
    if (steadyState) steadyStatePopulation.InitialisePopulation(populationSize * 2, genomeType,
                                                                genomeLength, 
                                                                lowBound, highBound,
                                                                false, lowBound, defaultGaussianSD);
    
    // column headings
    if (normalStats)
    {
        outFile << "\nGen\tMax\tMean\tMin\tSD\n";
        CalculateStatistics(&thePopulation, &stats);
        outFile << startingGenerationNumber << "\t";
        outFile << stats << "\n";
    }
    else
    {
        outFile << "\nGen\t0%\t10%\t20%\t30%\t40%\t50%\t60%\t70%\t80%\t90%\t100%\n";
        CalculateTenPercentiles(&thePopulation, &tenPercentiles);
        outFile << startingGenerationNumber << "\t";
        outFile << tenPercentiles << "\n";
    }
    outFile.flush();
    
    /* debugging code
        for (i = 0; i < populationSize; i++)
        cerr << thePopulation.GetGenome(i)->GetFitness() << "\t";
    cerr << "\n";
    
    for (i = 0; i < populationSize - 1; i++)
    {
        assert(thePopulation.GetGenome(i)->GetFitness() <= 
               thePopulation.GetGenome(i + 1)->GetFitness());
    }
    */
    
    // evolve the population
    for (j = startingGenerationNumber + 1; j < maxGenerations; j++)
    {
        // produce the next generation
        for (i = 0; i < populationSize; i++)
        {
            parent1 = thePopulation.ChooseParent(&parent1Rank);
            if (CoinFlip(crossoverChance))
            {
                parent2 = thePopulation.ChooseParent(&parent2Rank);
                Mate(parent1, parent2, theOffspring.GetGenome(i), crossoverType);
                theOffspring.GetGenome(i)->SetParent1(j - 1, parent1Rank);
                theOffspring.GetGenome(i)->SetParent2(j - 1, parent2Rank);
            }
            else
            {
                *(theOffspring.GetGenome(i)) = *parent1;
                theOffspring.GetGenome(i)->SetParent1(j - 1, parent1Rank);
                theOffspring.GetGenome(i)->SetParent2(-1, -1);
            }
            if (multipleGaussian)
                MultipleGaussianMutate(theOffspring.GetGenome(i), gaussianMutationChance); 
            else
                GaussianMutate(theOffspring.GetGenome(i), gaussianMutationChance); 
            FrameShiftMutate(theOffspring.GetGenome(i), frameShiftMutationChance); 
            DuplicationMutate(theOffspring.GetGenome(i), duplicationMutationChance); 
        }
        theOffspring.CalculateFitness(0, populationSize, objectiveCommand, 
                                      fitnessTimeLimit, fitnessRestartOnError);
        
        if (steadyState)
        {
            // merge offspring and parents
            for (i = 0; i < populationSize; i++)
            {
                *steadyStatePopulation.GetGenome(i) = *theOffspring.GetGenome(i);
                *steadyStatePopulation.GetGenome(i + populationSize) = *thePopulation.GetGenome(i);
            }
            // sort merged population
            steadyStatePopulation.Sort();
            if (removeDuplicates) steadyStatePopulation.UniqueResort();
            // put best genomes back in the population
            for (i = 0; i < populationSize; i++)
            {
                *thePopulation.GetGenome(i) = *steadyStatePopulation.GetGenome(i + populationSize);
            }
            // handle immigration
            if (immigrations)
            {
                thePopulation.Randomise(0, immigrations);
                thePopulation.CalculateFitness(0, immigrations, objectiveCommand, 
                                               fitnessTimeLimit, fitnessRestartOnError);
                thePopulation.Sort();
            }
        }
        else
        {
            theOffspring.Sort();
            
            if (removeDuplicates)
            {
                // devalue duplicate members of the populations
                theOffspring.UniqueResort();
                thePopulation.UniqueResort();
            }
            
            // handle immigration
            thePopulation.Randomise(0, immigrations);
            thePopulation.CalculateFitness(0, immigrations, objectiveCommand, 
                                           fitnessTimeLimit, fitnessRestartOnError);
            
            // handle offspring replacing parents
            for (i = immigrations; i < populationSize - parentsToKeep; i++)
                *thePopulation.GetGenome(i) = 
                    *theOffspring.GetGenome(i + parentsToKeep);
            
            // NB. thePopulation is now NOT sorted so resort
            thePopulation.Sort();
        }
        
        // output some statistics
        if (normalStats)
        {
            CalculateStatistics(&thePopulation, &stats);
            outFile << j << "\t";
            outFile << stats << "\n";
            outFile.flush();
        }
        else
        {
            CalculateTenPercentiles(&thePopulation, &tenPercentiles);
            outFile << j << "\t";
            outFile << tenPercentiles << "\n";
        }
        outFile.flush();
        
        /* debugging code 
            for (i = 0; i < populationSize; i++)
            cerr << thePopulation.GetGenome(i)->GetFitness() << "\t";
        cerr << "\n";
        
        for (i = 0; i < populationSize - 1; i++)
        {
            assert(thePopulation.GetGenome(i)->GetFitness() <= 
                   thePopulation.GetGenome(i + 1)->GetFitness());
        }
        */
        
        // do we need to output Best Genome
        if (saveBestEvery < 0) // save anyway
        {
            sprintf(filename, "%s/BestGenome_%07d.txt", dirname, j);
            ofstream bestFile(filename);
            bestFile << *thePopulation.GetGenome(populationSize - 1);
            sprintf(filename, "%s/BestGenome_%07d.xml", dirname, j);
            gXMLConverter->ApplyGenome(thePopulation.GetGenome(populationSize - 1));
            ptr = (char *)gXMLConverter->GetFormattedXML(&i);
            ofstream bestFileXML(filename);
            bestFileXML.write(ptr, i);
            bestFileXML.close();
        }
        else
        {
            if (saveBestEvery > 0)
            {
                if (j % saveBestEvery == saveBestEvery - 1)
                {
                    if (thePopulation.GetGenome(populationSize - 1)->GetFitness() > maxFitness)
                    {
                        maxFitness = thePopulation.GetGenome(populationSize - 1)->GetFitness();
                        
                        sprintf(filename, "%s/BestGenome_%07d.txt", dirname, j);
                        ofstream bestFile(filename);
                        bestFile << *thePopulation.GetGenome(populationSize - 1);
                        bestFile.close();
                        if (onlyKeepBestGenome)
                        {
                            if (*lastBestGenomeFile) unlink(lastBestGenomeFile);
                            strcpy(lastBestGenomeFile, filename);
                        }
                        
                        sprintf(filename, "%s/BestGenome_%07d.xml", dirname, j);
                        gXMLConverter->ApplyGenome(thePopulation.GetGenome(populationSize - 1));
                        ptr = (char *)gXMLConverter->GetFormattedXML(&i);
                        ofstream bestFileXML(filename);
                        bestFileXML.write(ptr, i);
                        bestFileXML.close();
                        
                        if (onlyKeepBestGenome)
                        {
                            if (*lastBestGenomeFileXML) unlink(lastBestGenomeFileXML);
                            strcpy(lastBestGenomeFileXML, filename);
                        }
                    }
                }
            }
        }
        // do we need to save the best population?
        if (savePopEvery > 0)
        {
            if (j % savePopEvery == savePopEvery - 1)
            {
                sprintf(filename, "%s/Population_%07d.txt", dirname, j);
                thePopulation.OutputSubpopulation(filename, outputPopulationSize);
                if (onlyKeepBestPopulation)
                {
                    if (*lastBestPopulationFile) unlink(lastBestPopulationFile);
                    strcpy(lastBestPopulationFile, filename);
                }
            }
        }
        
        // do we need to save genealogy information?
        if (genealogyFile)
        {
            genealogyStream << j << "\t";
            for (i = 0; i < populationSize; i++)
            {
                if (i != populationSize - 1)
                    genealogyStream 
                        << thePopulation.GetGenome(i)->GetParent1()->generationNumber << "\t"
                        << thePopulation.GetGenome(i)->GetParent1()->rank << "\t"
                        << thePopulation.GetGenome(i)->GetParent2()->generationNumber << "\t"
                        << thePopulation.GetGenome(i)->GetParent2()->rank << "\t"
                        << thePopulation.GetGenome(i)->GetFitness() << "\t";
                else
                    genealogyStream 
                        << thePopulation.GetGenome(i)->GetParent1()->generationNumber << "\t"
                        << thePopulation.GetGenome(i)->GetParent1()->rank << "\t"
                        << thePopulation.GetGenome(i)->GetParent2()->generationNumber << "\t"
                        << thePopulation.GetGenome(i)->GetParent2()->rank << "\t"
                        << thePopulation.GetGenome(i)->GetFitness() << "\n";
            }
            genealogyStream.flush();
        }
        
        // check for finish criteria
        if (improvementGenerations > 0)
        {
            if ( j % improvementGenerations == improvementGenerations - 1)
            {
                if ( maxFitness - lastMaxFitness < improvementThreshold )
                    break;
                lastMaxFitness = maxFitness;
            }
        }
    }
    
    if (genealogyFile) genealogyStream.close();
    
    // and now the very best
    thePopulation.Sort();
    
    outFile << "\nBest Genome\n";
    outFile << *thePopulation.GetGenome(populationSize - 1);
    
    outFile.close();
    
    if (bestGenomeFile)
    {
        outFile.open(bestGenomeFile);
        outFile << *thePopulation.GetGenome(populationSize - 1);
        outFile.close();
    }
    
    if (bestGenomeFileXML)
    {
        gXMLConverter->ApplyGenome(thePopulation.GetGenome(populationSize - 1));
        ptr = (char *)gXMLConverter->GetFormattedXML(&i);
        outFile.open(bestGenomeFileXML);
        outFile.write(ptr, i);
        outFile.close();
    }
    
    if (bestPopulationFile)
    {
        thePopulation.OutputSubpopulation(bestPopulationFile, outputPopulationSize);
    }  
    
#if defined(USE_UDP)  
    gUDP.StopListener();
    gUDP.StopTalker();
#elif defined(USE_TCP)  
    gTCP.StopServer();
#elif defined(USE_MPI)
#if defined(USE_MPI_QUIT)
    for (i =  1; i < gMPI_Comm_size; i++)
    {
        MPI_Send(0,                      /* message buffer */ 
                 0,                      /* no data item */ 
                 MPI_BYTE,               /* data items are bytes */ 
                 i,                      /* destination */ 
                 MPI_TAG_QUIT,           /* abort message tag (needs to be defined at compile time) */ 
                 MPI_COMM_WORLD);        /* default communicator */
    }
#endif
#endif
    
    // delete allocated memory
    if (modelGenome) delete modelGenome;
    if (parameterStartingPopulation) delete parameterStartingPopulation;
    if (logNotes) delete [] logNotes;
    if (genealogyFile) delete [] genealogyFile;
    if (objectiveCommand) delete [] objectiveCommand;
    if (extraDataFile) delete [] extraDataFile;
    delete gXMLConverter;
    
    return 0;
}

