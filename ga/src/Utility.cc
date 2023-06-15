// Utility program to manipulate genomes and populations

#include <stdio.h>
#include <assert.h>
#include <iostream.h>
#include <fstream.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include "Genome.h"
#include "Population.h"
#include "Random.h"

#define gDebugDeclare
#include "Debug.h"

int main(int argc, char *argv[])
{
   int i, j;
   char buffer[256];
   RANDOM_SEED_TYPE randomSeed = (RANDOM_SEED_TYPE)time(0) * (RANDOM_SEED_TYPE)getpid();
   
   // set the random number seed
   RandomSeed(randomSeed);
   
   if (argc == 1)
   {
      cerr << "Usage:\n\nutility merge|resize|sample arglist\n\n";
      return 1;
   }
   
   char *command = argv[1];
         
   // merge a list of populations
   // assumes they are sensibly mergable!
   
   if (strcmp(command, "merge") == 0)
   {
      int nPopulations = argc - 2;
      if (nPopulations < 2)
      {
         cerr << "Need at least two populations\n";
         return 1;
      }
      Population **list = new Population *[nPopulations];
      ifstream *in = new ifstream();
      for (i = 2; i < argc; i++)
      {
         list[i - 2] = new Population();
         in->open(argv[i]);
         (*in) >> *list[i - 2];
         in->close();
      }
      int totalLength = 0;
      for (i = 0; i < nPopulations; i++)
         totalLength += list[i]->GetPopulationSize();
      Population mergedPopulation;
      mergedPopulation.InitialisePopulation(totalLength, 
            list[0]->GetGenome(0)->GetGenomeType(),
            list[0]->GetGenome(0)->GetGenomeLength(),
            list[0]->GetGenome(0)->GetLowBound(),
            list[0]->GetGenome(0)->GetHighBound(),
            false, list[0]->GetGenome(0)->GetLowBound());
      int insertPoint = 0;
      for (i = 0; i < nPopulations; i++)
      {
         for (j = 0; j < list[i]->GetPopulationSize(); j++)
         {
            *mergedPopulation.GetGenome(insertPoint) = 
               *list[i]->GetGenome(j);
            insertPoint++;
         }
      }
      mergedPopulation.Sort();
      sprintf(buffer, "MergedPopulation%05d.txt", totalLength);
      ofstream out(buffer);
      out << mergedPopulation;
      
      return 0;
   }
   
   // sample a population
   
   if (strcmp(command, "sample") == 0)
   {
      if (argc != 4)
      {
         cerr << "Need population name and new size\n";
         return 1;
      }
      
      Population inPop;
      ifstream in(argv[2]);
      in >> inPop;
      int newSize;
      sscanf(argv[3], "%d", &newSize);
      Population outPop;
      outPop.InitialisePopulation(newSize, 
            inPop.GetGenome(0)->GetGenomeType(),
            inPop.GetGenome(0)->GetGenomeLength(),
            inPop.GetGenome(0)->GetLowBound(),
            inPop.GetGenome(0)->GetHighBound(),
            false, inPop.GetGenome(0)->GetLowBound());
      
      for (i = 0; i < newSize; i++)
      {
         *outPop.GetGenome(i) = 
               *inPop.GetGenome(RandomInt(0, inPop.GetPopulationSize() - 1));
      }
      outPop.Sort();
      sprintf(buffer, "SampledPopulation%05d.txt", newSize);
      ofstream out(buffer);
      out << outPop;
      
      return 0;
   }
   
   // resize a population
   
   if (strcmp(command, "resize") == 0)
   {
      if (argc != 4)
      {
         cerr << "Need population name and new size\n";
         return 1;
      }
      
      Population inPop;
      ifstream in(argv[2]);
      in >> inPop;
      int newSize;
      sscanf(argv[3], "%d", &newSize);
      Population outPop;
      // perhaps this should be random rather than zero
      // only effects populations getting larger
      outPop.InitialisePopulation(newSize, 
            inPop.GetGenome(0)->GetGenomeType(),
            inPop.GetGenome(0)->GetGenomeLength(),
            inPop.GetGenome(0)->GetLowBound(),
            inPop.GetGenome(0)->GetHighBound(),
            false, inPop.GetGenome(0)->GetLowBound());
      if (newSize <= inPop.GetPopulationSize())
      {
         int origin = inPop.GetPopulationSize() - 1;
         for (i = newSize - 1; i >= 0; i--)
         {
            *outPop.GetGenome(i) = *inPop.GetGenome(origin);
            origin--;
         }
      }
      else
      {
         int insertion = outPop.GetPopulationSize() - 1;
         for (i = inPop.GetPopulationSize() - 1; i >= 0; i--)
         {
            *outPop.GetGenome(insertion) = *inPop.GetGenome(i);
            insertion--;
         }
      }
      sprintf(buffer, "ResizedPopulation%05d.txt", newSize);
      ofstream out(buffer);
      out << outPop;
      
      return 0;
   }
   
   return 0;
}
