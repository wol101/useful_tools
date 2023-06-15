// TestRandom.cc - utility to test the random number generator

#include <stdio.h>
#include <stdlib.h>
#include <iostream.h>
#include <time.h>
#include <unistd.h>
#include <float.h>
#include <fstream.h>

#include "Random.h"

void PrintFrequencies(double *list, int count, double low, double high, int nBins);

int main(int argc, char *argv[])
{
   int i;
   RANDOM_SEED_TYPE randomSeed = (RANDOM_SEED_TYPE)time(0) * (RANDOM_SEED_TYPE)getpid();
   int count = 1000;
   int type = 0;
   bool rawFlag = false;
   
   // read command line arguments

   for (i = 1; i < argc; i++)
   {
      if (strcmp(argv[i], "--randomSeed") == 0)
      {
         i++;
         if (i >= argc) return 1;
         long dummy; // hack to cope with different RANDOM_SEED_TYPE
         sscanf(argv[i], "%ld", &dummy);
         randomSeed = (RANDOM_SEED_TYPE)dummy;
      }
      
      else
      if (strcmp(argv[i], "--count") == 0)
      {
         i++;
         if (i >= argc) return 1;
         sscanf(argv[i], "%d", &count);
      }

      else
      if (strcmp(argv[i], "--type") == 0)
      {
         i++;
         if (i >= argc) return 1;
         sscanf(argv[i], "%d", &type);
      }

      else
      if (strcmp(argv[i], "--raw") == 0)
      {
         rawFlag = true;
      }

   }
   
   RandomSeed(randomSeed);
         
   // now for the tests
   
   if (rawFlag)
   {
      cout.precision(DBL_DIG);
      switch (type)
      {
         case 0:
         cout << "Random double between 0.0 and 1.0\n";
         for (i = 0; i < count; i++) cout << RandomDouble(0, 1) << "\n";
         break;

         case 1:
         cout << "Random unit Gaussian\n";
         for (i = 0; i < count; i++) cout << RandomUnitGaussian() << "\n";
         break;

         case 2:
         cout << "Random dice roll\n";   
         for (i = 0; i < count; i++) cout << RandomInt(1, 6) << "\n";
         break;
      }
   }
   
   else
   {   
      double *list = new double[count];
      switch (type)
      {
         case 0:
            for (i = 0; i < count; i++) list[i] = RandomDouble(0, 1);
            PrintFrequencies(list, count, 0.05, 0.95, 10);
            break;
            
         case 1:
            for (i = 0; i < count; i++) list[i] = RandomUnitGaussian();
            PrintFrequencies(list, count, -5, 5, 11);
            break;
            
         case 2:
            for (i = 0; i < count; i++) list[i] = (double)RandomInt(1, 6);
            PrintFrequencies(list, count, 1, 6, 6);
            break;
            
      }
      delete [] list;
   }
}

void PrintFrequencies(double *list, int count, double low, double high, int nBins)
{
   int *bins = new int[nBins + 2];
   double *ranges = new double [nBins + 1];
   int i, j;
   double binWidth = (high - low) / (double)(nBins - 1);
   
   for (i = 0; i < nBins + 2; i++) bins[i] = 0;
   for (i = 0; i < nBins + 1; i++) ranges[i] = 
      (low - binWidth / 2) + ((double)(i) * binWidth);
   
   for (i = 0; i < count; i++)
   {
      if (list[i] < ranges[0])
      {
         bins[0]++;
      }
      else
      {
         if (list[i] >= ranges[nBins])
         {
            bins[nBins + 1]++;
         }
         else
         {
            for (j = 1; j < nBins + 1; j++)
            {
               if (list[i] >= ranges[j - 1] && list[i] < ranges[j])
               {
                  bins[j]++;
                  break;
               }
            }
         }
      }
   }
   
   cout << "< " << ranges[0] << "\t" << bins[0] << "\n";
   for (i = 1; i < nBins + 1; i++)
   {
      cout << ranges[i - 1] << " to " << ranges[i] << "\t" << bins[i] << "\n";
   }
   cout << ">= " << ranges[nBins] << "\t" << bins[nBins + 1] << "\n";
}



   
