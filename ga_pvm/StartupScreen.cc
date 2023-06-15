// StartupScreen.cc - program to screen a large number of random
// genomes to produce a starting population

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <iostream.h>
#include <fstream.h>
#include <string.h>
#include <time.h>
#include <float.h>
#include <unistd.h>

#include "Genome.h"
#include "Population.h"
#include "Random.h"

#define gDebugDeclare
#include "Debug.h"

int main(int argc, char *argv[])
{
   int i, j, k;
   int genomeLength = 10;
   int populationSize = 500;
   int maxTrials = 100;
   double lowBound = 0;
   double highBound = 1.0;
   char *extraArguments = 0;
   unsigned int randomSeed = (unsigned int)time(0) * (unsigned int)getpid();
   char *logNotes = 0;
   GenomeType genomeType = DefaultGenome;
   
   for (i = 1; i < argc; i++)
   {
      if (strcmp(argv[i], "--genomeLength") == 0)
      {
         i++;
         if (i >= argc) return 1;
         sscanf(argv[i], "%d", &genomeLength);
      }
      else
      if (strcmp(argv[i], "--populationSize") == 0)
      {
         i++;
         if (i >= argc) return 1;
         sscanf(argv[i], "%d", &populationSize);
      }
      else
      if (strcmp(argv[i], "--maxTrials") == 0)
      {
         i++;
         if (i >= argc) return 1;
         sscanf(argv[i], "%d", &maxTrials);
      }
      else
      if (strcmp(argv[i], "--lowBound") == 0)
      {
         i++;
         if (i >= argc) return 1;
         sscanf(argv[i], "%lf", &lowBound);
      }
      else
      if (strcmp(argv[i], "--highBound") == 0)
      {
         i++;
         if (i >= argc) return 1;
         sscanf(argv[i], "%lf", &highBound);
      }
      else
      if (strcmp(argv[i], "--extraArguments") == 0)
      {
         i++;
         if (i >= argc) return 1;
         extraArguments = argv[i];
      }
      else
      if (strcmp(argv[i], "--logNotes") == 0)
      {
         i++;
         if (i >= argc) return 1;
         logNotes = argv[i];
      }
      else
      if (strcmp(argv[i], "--individualRanges") == 0)
      {
         genomeType = IndividualRanges;
      }
      else
      if (strcmp(argv[i], "--help") == 0 || 
         strcmp(argv[i], "-h") == 0 ||
         strcmp(argv[i], "-?") == 0)
      {
         cerr << "\nStartupScreen program\n";
         cerr << "Build " << __DATE__ << " " << __TIME__ << "\n";
         cerr << "Supported options:\n\n";
         cerr << "--genomeLength n\n";
         cerr << "--populationSize n\n";
         cerr << "--maxTrials n\n";
         cerr << "--lowBound n\n";
         cerr << "--highBound n\n";
         cerr << "--individualRanges\n";
         cerr << "--extraArguments arguments\n";
         cerr << "--logNotes notes\n";
         cerr << "--randomSeed n\n";
         cerr << "\n";
         return 1;
      }
      else
      {
        cerr << argv[i] << " unrecognised argument. Use --help to get options list\n";
        return 1;
      }
   }
   
   // create a directory for all the output
   time_t theTime = time(0);
   struct tm *theLocalTime = localtime(&theTime);
   char dirname[256];
   sprintf(dirname, "Screen_%04d-%02d-%02d_%02d.%02d.%02d", 
       theLocalTime->tm_year + 1900, theLocalTime->tm_mon + 1, 
       theLocalTime->tm_mday,
       theLocalTime->tm_hour, theLocalTime->tm_min,
       theLocalTime->tm_sec);
   char command[256];
   sprintf(command, "mkdir %s", dirname);
   int error = system(command);
   if (error)
   {
      cerr << "Error creating directory " << dirname << "\n";
      return 1;
   }

   // write log
   char filename[256];
   sprintf(filename, "%s/log.txt", dirname);
   ofstream outFile(filename);
   outFile << "StartupScreen build " << __DATE__ << " " << __TIME__ "\n";
   outFile << "Log produced " << asctime(theLocalTime) << "\n";

   if (logNotes)
   {
      outFile << logNotes << "\n";
   }

   outFile << "genomeLength\t" << genomeLength << "\n";
   outFile << "populationSize\t" << populationSize << "\n";
   outFile << "maxTrials\t" << maxTrials << "\n";
   outFile << "lowBound\t" << lowBound << "\n";
   outFile << "highBound\t" << highBound << "\n";
   outFile << "randomSeed\t" << randomSeed << "\n";

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
       return 1;
   }
  
   if (extraArguments)
   {
     outFile << "extraArguments\t" << extraArguments << "\n";
   }
    
   outFile.flush();   
   
   // set the random number seed
   RandomSeed(randomSeed);
  
   // set up the populations
   Population bestPopulation;
   bestPopulation.InitialisePopulation(populationSize, genomeType,
       genomeLength,
      lowBound, highBound, true);
   bestPopulation.CalculateFitness(0, populationSize, extraArguments, 0);
   bestPopulation.Sort();
   outFile << "Trial\tMin Fitness\t Max Fitness\n";
   double maxFitness = bestPopulation.GetGenome(populationSize - 1)->GetFitness();
   double minFitness = bestPopulation.GetGenome(0)->GetFitness();
   outFile << "0\t" << minFitness << "\t" << maxFitness << "\n";
   outFile.flush();  
    
   // and save the current best population 
   {  
      sprintf(filename, "%s/Population_%07d.txt", dirname, 0);
      ofstream bestPop(filename);
      bestPop << bestPopulation;
   }
            
   Population newPopulation;
   newPopulation.InitialisePopulation(populationSize, genomeType,
      genomeLength,
      lowBound, highBound, false, lowBound);
   
   Genome *genome;
   int insertPosition;
   bool insertedOrFinished;
        
   // loop the required number of trials
   
   for (i = 1; i < maxTrials; i++)
   {
      // produce a new random population
      newPopulation.Randomise(0, populationSize);
      newPopulation.CalculateFitness(0, populationSize, extraArguments, 0);
      newPopulation.Sort();
      
      // insert the best of the new population into the bestPopulation
      
      insertPosition = populationSize - 1;
      for (j = populationSize - 1; j >= 0; j--)
      {
         genome = newPopulation.GetGenome(j);
         insertedOrFinished = false;
         while (insertedOrFinished == false)
         {
            if (genome->GetFitness() > 
                  bestPopulation.GetGenome(insertPosition)->GetFitness())
            {
               // note: this does a lot of copying - pointer swapping would be quicker
               for (k = 1; k <= insertPosition; k++)
                  *bestPopulation.GetGenome(k - 1) = *bestPopulation.GetGenome(k);
               *bestPopulation.GetGenome(insertPosition) = *genome;
               insertedOrFinished = true;
            }
            insertPosition--;
            if (insertPosition < 0) insertedOrFinished = true;
         }
         if (insertPosition < 0) break;
      }
      maxFitness = bestPopulation.GetGenome(populationSize - 1)->GetFitness();
      minFitness = bestPopulation.GetGenome(0)->GetFitness();
      outFile << i << "\t" << minFitness << "\t" << maxFitness << "\n";
      outFile.flush();
      
      // and save the current best population 
      {  
         sprintf(filename, "%s/Population_%07d.txt", dirname, i);
         ofstream bestPop(filename);
         bestPop << bestPopulation;
      }
   }
}
