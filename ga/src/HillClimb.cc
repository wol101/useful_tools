// HillClimb.cc - the hill climbing sub-program

// works very much like objective

#include <stdio.h>
#include <stdlib.h>
#include <iostream.h>
#include <fstream.h>
#include <string.h>
#include <unistd.h>

#include "Genome.h"

static char g_HillClimbGenomeName[256];
static char g_HillClimbScoreName[256];

static void HillClimb(Genome *genome);
static double CalculateFitness(Genome *genome);

int main(int argc, char *argv[])
{
  char *genomeName;
  char *scoreName;
  Genome genome;
  int i;
  
  // read command line arguments
  for (i = 1; i < argc; i++)
  {
    if (strcmp(argv[i], "--genome") == 0)
    {
      i++;
      if (i > argc) return 1;
      genomeName = argv[i];
    }
  
    if (strcmp(argv[i], "--score") == 0)
    {
      i++;
      if (i > argc) return 1;
      scoreName = argv[i];
    }
  }
  
  // read the genome
  ifstream in(genomeName);
  in >> genome;
  in.close();
  
  // create the hill climb names
  sprintf(g_HillClimbGenomeName, "hill_climb_%s", genomeName);
  sprintf(g_HillClimbScoreName, "hill_climb_%s", scoreName);
  
  // now do some hill climbing
  HillClimb(&genome);  
  
  // write out improved genome
  ofstream o(genomeName);
  o << genome;
  o.close();
  
  // write out the current best score
  double score = genome.GetFitness();
  FILE *out = fopen(scoreName, "wb");
  fwrite(&score, sizeof(double), 1, out);
  fclose(out);
}

void HillClimb(Genome *genome)
{
  double bestFitness;
  bool improved;
  int i;
  double v, vLow, vHigh;
  double fLow, fHigh;
  double delta;
  
  bestFitness = CalculateFitness(genome);
  genome->SetFitness(bestFitness);
  delta = (genome->GetHighBound() - genome->GetLowBound()) / 1000;
  do
  {
    improved = false;
    for (i = 0; i < genome->GetGenomeLength(); i++)
    {
      // get the gene to hill climb with
      v = genome->GetGene(i);
      vLow = v - delta;
      vHigh = v + delta;
      if (vHigh > genome->GetHighBound()) vHigh = genome->GetHighBound();
      if (vLow < genome->GetLowBound()) vLow = genome->GetLowBound();

      // calculate the bestFitnesses
      genome->SetGene(i, vLow);
      fLow = CalculateFitness(genome);
      genome->SetGene(i, vHigh);
      fHigh = CalculateFitness(genome);
      genome->SetGene(i, v);
      if (fLow > bestFitness && fLow >= fHigh)
      {
        bestFitness = fLow;
        genome->SetGene(i, vLow);
        genome->SetFitness(bestFitness);
        improved = true;
      }
      else
      {
        if (fHigh > bestFitness && fHigh > fLow)
        {
          bestFitness = fHigh;
          genome->SetGene(i, vHigh);
          genome->SetFitness(bestFitness);
          improved = true;
        }
      }
    }
  } while (improved);
}

double CalculateFitness(Genome *genome)
{
  char theCommand[256];
  FILE *in;
  double score;
  
  sprintf(theCommand, "./objective --genome %s --score %s", 
     g_HillClimbGenomeName, g_HillClimbScoreName);

  // write out the genome
  ofstream out(g_HillClimbGenomeName);
  out << *genome;
  out.close();
  
  // run the objective command
  system(theCommand);  
  
  in = fopen(g_HillClimbScoreName, "rb");
  fread(&score, sizeof(double), 1, in);
  fclose(in);
  
  unlink(g_HillClimbGenomeName);
  unlink(g_HillClimbScoreName);
  
  return score;
}

 
  
    

