// ga.cc implements a simple steady state GA system with the genome
// represented as an array of floating point numbers

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <iostream.h>
#include <fstream.h>
#include <string.h>
#include <time.h>
#include <float.h>
#include <unistd.h>
#include <sys/stat.h>

#include <pvm3.h>

#include "Genome.h"
#include "Mating.h"
#include "Statistics.h"
#include "Population.h"
#include "Random.h"

#include "../DataFile/DataFile.h"

#define gDebugDeclare
#include "Debug.h"

FILE *gPVMLogFile = 0;

int main(int argc, char *argv[])
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
  SelectionType parentSelection = RankBasedSelection;
  RANDOM_SEED_TYPE randomSeed = (RANDOM_SEED_TYPE)time(0) * (RANDOM_SEED_TYPE)getpid();
  int improvementGenerations = 0;
  double improvementThreshold = 0.1;
  char *modelGenome = 0;
  char *startingPopulation = 0;
  Genome theModelGenome;
  bool normalStats = false;
  double gaussianSD = 1.0;
  bool incrementalMutation = false;
  bool removeDuplicates = false;
  char *logNotes = 0;
  char *extraArguments = 0;
  char *genealogyFile = 0;
  GenomeType genomeType = DefaultGenome;
  char *parameterFile = 0;
  char *restartDirectory = 0;
  char restartLog[1024];
  DataFile params;
  bool status;
  char buffer[1024];
  struct stat sb;
  char filename[1024];
  time_t theTime = time(0);
  struct tm *theLocalTime = localtime(&theTime);
  char dirname[1024];
  char command[1024];
  int error;
  int startingGenerationNumber = 0;
  int maxGenerationsOveride = -1;
  CrossoverType crossoverType = OnePoint;
  DataFile extraData;
  char *extraDataFile = 0;
  time_t fitnessTimeLimit = 0;
  int fitnessRestartOnError = 1;
  
  int mytid = pvm_mytid();
  if (mytid < 0)
  {
     cerr << "PVM error: pvm_mytid()\n";
     return 1;
  }
  gPVMLogFile = fopen("pvm_error_log.txt", "a");
  pvm_catchout( gPVMLogFile );     
              
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
        if (i >= argc) throw(0);
        parameterFile = argv[i];
      }

      else
      if (strcmp(argv[i], "--restartDirectory") == 0 ||
               strcmp(argv[i], "-r") == 0)
      {
        i++;
        if (i >= argc) throw(0);
        restartDirectory = argv[i];
      }

      else
      if (strcmp(argv[i], "--debug") == 0 ||
               strcmp(argv[i], "-d") == 0)
      {
        i++;
        if (i >= argc) throw(0);
        if (sscanf(argv[i], "%d", &gDebug) != 1) throw(0);
      }

      else
      if (strcmp(argv[i], "--maxGenerationsOveride") == 0 ||
               strcmp(argv[i], "-g") == 0)
      {
        i++;
        if (i >= argc) throw(0);
        if (sscanf(argv[i], "%d", &maxGenerationsOveride) != 1) throw(0);
      }

      else
      {
        throw(0);
      }
    }
    if (parameterFile == 0 && restartDirectory == 0) throw(0);
  }
  
  // catch argument errors
  catch (...)
  {
    cerr << "\nGenetic Algorithm program\n";
    cerr << "Build " << __DATE__ << " " << __TIME__ << "\n";
    cerr << "Supported options:\n\n";
    cerr << "(-p) --parameterFile file_name\n";
    cerr << "(-r) --restartDirectory directory_name\n";
    cerr << "(-d) --debug n\n";
    cerr << "(-g) --maxGenerationsOveride n\n";
    cerr << "\nNote: --parameterFile or --restartDirectory required\n";
    cerr << "Short form is shown in brackets.\n";
    pvm_exit(); return (1);
  }
  
  if (restartDirectory) // set parameterFile to previous log file
  {
     sprintf(restartLog, "%s/log.txt", restartDirectory);
     parameterFile = restartLog;
  }
 
  if (params.ReadFile(parameterFile))
  {
     cerr << "Error reading " << parameterFile << "\n";
     pvm_exit(); return (1);
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
  status = params.RetrieveParameter("gaussianSD", &gaussianSD);
  status = params.RetrieveParameter("saveBestEvery", &saveBestEvery);
  status = params.RetrieveParameter("savePopEvery", &savePopEvery);
  status = params.RetrieveParameter("randomSeed", &randomSeed);
  status = params.RetrieveParameter("improvementGenerations", &improvementGenerations);
  status = params.RetrieveParameter("improvementThreshold", &improvementThreshold);

  status = params.RetrieveParameter("incrementalMutation", &incrementalMutation);
  status = params.RetrieveParameter("removeDuplicates", &removeDuplicates);
  status = params.RetrieveParameter("multipleGaussian", &multipleGaussian);
  status = params.RetrieveParameter("normalStats", &normalStats);
  status = params.RetrieveParameter("fitnessTimeLimit", &fitnessTimeLimit);
  status = params.RetrieveParameter("fitnessRestartOnError", &fitnessRestartOnError);
    
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
  
  if (params.RetrieveParameter("individualRanges", buffer, sizeof(buffer)) == 0)
  {
     if (strcmp(buffer, "IndividualRanges") == 0)
         genomeType = IndividualRanges;
     if (strcmp(buffer, "") == 0)
         genomeType = DefaultGenome;
  }
  
  if (params.RetrieveParameter("modelGenome", buffer, sizeof(buffer)) == 0)
  {
     modelGenome = new char[sizeof(buffer)];
     strcpy(modelGenome, buffer);
  }
     
  if (params.RetrieveParameter("startingPopulation", buffer, sizeof(buffer)) == 0)
  {
     startingPopulation = new char[sizeof(buffer)];
     strcpy(startingPopulation, buffer);
  }
     
  if (params.RetrieveParameter("logNotes", buffer, sizeof(buffer)) == 0)
  {
     logNotes = new char[sizeof(buffer)];
     strcpy(logNotes, buffer);
  }
     
  if (params.RetrieveParameter("extraArguments", buffer, sizeof(buffer)) == 0)
  {
     extraArguments = new char[sizeof(buffer)];
     strcpy(extraArguments, buffer);
  }
     
  if (params.RetrieveParameter("genealogyFile", buffer, sizeof(buffer)) == 0)
  {
     genealogyFile = new char[sizeof(buffer)];
     strcpy(genealogyFile, buffer);
  }
               
  if (params.RetrieveParameter("extraDataFile", buffer, sizeof(buffer)) == 0)
  {
     extraDataFile = new char[sizeof(buffer)];
     strcpy(extraDataFile, buffer);
     if (extraData.ReadFile(extraDataFile))
     {
        cerr << "Error reading " << buffer << "\n";
        pvm_exit(); return (1);
     }
  }
               
  if (modelGenome && startingPopulation)
  {
    cerr << "Cannot specify both --modelGenome and --startingPopulation\n";
    pvm_exit(); return 1;
  }
  
  if (populationSize < immigrations)
  {
    cerr << "Population size less than immigrations per generation\n";
    pvm_exit(); return 1;
  }
  
  if (populationSize < parentsToKeep)
  {
    cerr << "Population size less than parents to keep per generation\n";
    pvm_exit(); return 1;
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
      pvm_exit(); return 1;
    }
    in >> theModelGenome;
    in.close();
    
    if (theModelGenome.GetGenomeLength() != genomeLength)
    {
      cerr << "Model genome does not match specified genome length\n";
      pvm_exit(); return 1;
    }
    if (theModelGenome.GetGenomeType() != genomeType)
    {
      cerr << "Model genome does not match specified genome type\n";
      pvm_exit(); return 1;
    }
  }
  
  // read in last population if restarting
  
  if (restartDirectory)
  {
     if (savePopEvery == 0)
     {
        cerr << "No populations to restart from\n";
        pvm_exit(); return 1;
     }
     int maxPopulations = maxGenerations / savePopEvery;
     for (i = maxPopulations; i >= 0; i--)
     {
        sprintf(filename, "%s/Population_%07d.txt", restartDirectory, i * savePopEvery - 1);
        error = stat(filename, &sb);
        if (error == 0 && sb.st_size != 0)
        {
            startingPopulation = new char[1024];
            strcpy(startingPopulation, filename);
            startingGenerationNumber = i * savePopEvery - 1;
            break;
        }
     }
     if (i == 0) 
     {
        cerr << "No populations to restart from\n";
        pvm_exit(); return 1;
     }
  }            
                  
  // create a directory for all the output
  sprintf(dirname, "Run_%04d-%02d-%02d_%02d.%02d.%02d", 
      theLocalTime->tm_year + 1900, theLocalTime->tm_mon + 1, 
      theLocalTime->tm_mday,
      theLocalTime->tm_hour, theLocalTime->tm_min,
      theLocalTime->tm_sec);
  sprintf(command, "mkdir %s", dirname);
  error = system(command);
  if (error)
  {
    cerr << "Error creating directory " << dirname << "\n";
    pvm_exit(); return 1;
  }

  // write log
  sprintf(filename, "%s/log.txt", dirname);
  ofstream outFile(filename);
  outFile << "GA build " << __DATE__ << " " << __TIME__ "\n";
  outFile << "Log produced " << asctime(theLocalTime);
  
  outFile << "Command line:\n";
  for (i = 0; i < argc - 1; i++) outFile << argv[i] << " ";
  outFile << argv[i] << "\n";
  
  outFile << "PVM tid:\t" << mytid << "\n";
  
  
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
  outFile << "gaussianSD\t" << gaussianSD << "\n";
  outFile << "saveBestEvery\t" << saveBestEvery << "\n";
  outFile << "savePopEvery\t" << savePopEvery << "\n";
  outFile << "randomSeed\t" << randomSeed << "\n";
  outFile << "improvementGenerations\t" << improvementGenerations << "\n";
  outFile << "improvementThreshold\t" << improvementThreshold << "\n";
  outFile << "removeDuplicates\t" << removeDuplicates << "\n";
  outFile << "multipleGaussian\t" << multipleGaussian << "\n";
  outFile << "normalStats\t" << normalStats << "\n";
  outFile << "fitnessTimeLimit\t" << fitnessTimeLimit << "\n";
  outFile << "fitnessRestartOnError\t" << fitnessRestartOnError << "\n";

  if (modelGenome)
  {
    outFile << "modelGenome\t\"" << modelGenome << "\"\n";
    outFile << "incrementalMutation\t" << incrementalMutation << "\n";
  }
  
  if (startingPopulation)
  {
    outFile << "startingPopulation\t\"" << startingPopulation << "\"\n";
  }
  
  if (genealogyFile)
  {
    outFile << "genealogyFile\t\"" << genealogyFile << "\"\n";
  }
  
  if (extraArguments)
  {
    outFile << "extraArguments\t\"" << extraArguments << "\"\n";
  }
  
  if (extraDataFile)
  {
    outFile << "extraDataFile\t\"" << extraDataFile << "\"\n";
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
      pvm_exit(); return 1;
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
      pvm_exit(); return 1;
  }
  
  switch (genomeType)
  {
    case DefaultGenome: 
      outFile << "genomeType\t" << "DefaultGenome\n";
      break;
    case IndividualRanges: 
      outFile << "genomeType\t" << "IndividualRanges\n";
      break;
    default:
      cerr << "Unrecognised genomeType\n";
      pvm_exit(); return 1;
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
    ifstream in(startingPopulation);
    if (in.bad())
    {
      cerr << "Error reading starting population: " << startingPopulation << "\n";
      pvm_exit(); return 1;
    }
    in >> thePopulation;
    in.close();
    
    if (thePopulation.GetPopulationSize() != populationSize)
    {
      cerr << "Starting population size does not match specified population size\n";
      pvm_exit(); return 1;
    }
    if (thePopulation.GetGenome(0)->GetGenomeLength() != genomeLength)
    {
      cerr << "Starting population genome does not match specified genome length\n";
      pvm_exit(); return 1;
    }
    if (thePopulation.GetGenome(0)->GetGenomeType() != genomeType)
    {
      cerr << "Starting population genome does not match specified genome type\n";
      pvm_exit(); return 1;
    }
  }
  else
  {
    if (modelGenome)
    {
      // generate a population based on the model
      thePopulation.InitialisePopulation(populationSize, genomeType, 
                    genomeLength, 
                    lowBound, highBound, false, lowBound);
      *(thePopulation.GetGenome(0)) = theModelGenome;
      // keep mutating to generate a non-uniform population
      for (i = 1; i < populationSize; i++)
      {
        if (incrementalMutation)
        {      
          if (multipleGaussian)
            MultipleGaussianMutate(&theModelGenome, gaussianMutationChance, gaussianSD); 
          else
            GaussianMutate(&theModelGenome, gaussianMutationChance, gaussianSD); 
          FrameShiftMutate(&theModelGenome, frameShiftMutationChance); 
          DuplicationMutate(&theModelGenome, duplicationMutationChance); 
          *(thePopulation.GetGenome(i)) = theModelGenome;
        }
        else
        {
          *(thePopulation.GetGenome(i)) = theModelGenome;
          if (multipleGaussian)
            MultipleGaussianMutate(thePopulation.GetGenome(i), gaussianMutationChance, gaussianSD); 
          else
            GaussianMutate(thePopulation.GetGenome(i), gaussianMutationChance, gaussianSD); 
          FrameShiftMutate(thePopulation.GetGenome(i), frameShiftMutationChance); 
          DuplicationMutate(thePopulation.GetGenome(i), duplicationMutationChance); 
        }
      }
    }
    else
    {
      // generate a random population between the bounds
      thePopulation.InitialisePopulation(populationSize, genomeType,
                    genomeLength, 
                    lowBound, highBound);
    }
  }
    
  thePopulation.SetSelectionType(parentSelection);
  thePopulation.CalculateFitness(0, populationSize, extraArguments, 0, 
        extraData.GetRawData(), fitnessTimeLimit, fitnessRestartOnError);
  thePopulation.Sort();
  maxFitness = thePopulation.GetGenome(populationSize - 1)->GetFitness();
  lastMaxFitness = maxFitness;
  
  if (gDebug == Debug_GA)
    cerr << "main\tmaxFitness\t" << maxFitness << "\n";
  
  // set up the initial genealogy (clears any previous data)
  // and sets up each genome as its own single parent
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
  
  // do we need to save the best population?
  if (savePopEvery > 0)
  {
    sprintf(filename, "%s/Population_%07d.txt", dirname, startingGenerationNumber);
    ofstream bestPop(filename);
    bestPop << thePopulation;
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
                  false, lowBound);

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
*/
/* debugging code */
  for (i = 0; i < populationSize - 1; i++)
  {
    assert(thePopulation.GetGenome(i)->GetFitness() <= 
      thePopulation.GetGenome(i + 1)->GetFitness());
  }
  
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
        MultipleGaussianMutate(theOffspring.GetGenome(i), gaussianMutationChance, gaussianSD); 
      else
        GaussianMutate(theOffspring.GetGenome(i), gaussianMutationChance, gaussianSD); 
      FrameShiftMutate(theOffspring.GetGenome(i), frameShiftMutationChance); 
      DuplicationMutate(theOffspring.GetGenome(i), duplicationMutationChance); 
    }
    theOffspring.CalculateFitness(0, populationSize, extraArguments, 0, 
          extraData.GetRawData(), fitnessTimeLimit, fitnessRestartOnError);
    theOffspring.Sort();
    
    if (removeDuplicates)
    {
      // devalue duplicate members of the populations
      theOffspring.UniqueResort();
      thePopulation.UniqueResort();
    }
        
    // handle immigration
    thePopulation.Randomise(0, immigrations);
    thePopulation.CalculateFitness(0, immigrations, extraArguments, 0, 
          extraData.GetRawData(), fitnessTimeLimit, fitnessRestartOnError);

    // handle offspring replacing parents
    for (i = immigrations; i < populationSize - parentsToKeep; i++)
      *thePopulation.GetGenome(i) = 
      *theOffspring.GetGenome(i + parentsToKeep);

    // NB. thePopulation is now NOT sorted so resort
    thePopulation.Sort();

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
*/

    for (i = 0; i < populationSize - 1; i++)
    {
      assert(thePopulation.GetGenome(i)->GetFitness() <= 
          thePopulation.GetGenome(i + 1)->GetFitness());
    }
   
    // do we need to output Best Genome
    if (saveBestEvery < 0) // save anyway
    {
      sprintf(filename, "%s/BestGenome_%07d.txt", dirname, j);
      ofstream bestFile(filename);
      bestFile << *thePopulation.GetGenome(populationSize - 1);
    }
    else
    {
      if (thePopulation.GetGenome(populationSize - 1)->GetFitness() > maxFitness)
      {
        maxFitness = thePopulation.GetGenome(populationSize - 1)->GetFitness();
        if (saveBestEvery > 0)
        {
          if (j % saveBestEvery == saveBestEvery - 1)
          {
            sprintf(filename, "%s/BestGenome_%07d.txt", dirname, j);
            ofstream bestFile(filename);
            bestFile << *thePopulation.GetGenome(populationSize - 1);
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
        ofstream bestPop(filename);
        bestPop << thePopulation;
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
    
    // flush the pvm error log
    fflush(gPVMLogFile);
  }

  if (genealogyFile) genealogyStream.close();
    
  // and now the very best
  thePopulation.Sort();

  outFile << "\nBest Genome\n";
  outFile << *thePopulation.GetGenome(populationSize - 1);

  outFile.close();
  pvm_exit(); 
  fclose(gPVMLogFile);
  
  return 0;
}

