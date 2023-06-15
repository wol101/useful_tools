// Statistics.cc - calculate various statistics from a population

#include <assert.h>
#include <iostream.h>
#include <math.h>
#include <float.h>

#include "Statistics.h"
#include "Genome.h"
#include "Population.h"

// calculate standard statistics
void CalculateStatistics(Population *thePopulation, Statistics *stats)
{
  int i;
  double min = DBL_MAX;
  double max = -DBL_MAX;
  double sum = 0;
  double sumSquareDiff = 0;
  double variance;
  double mean;
  double fitness;
  double diff;
  double sd;
  int populationSize = thePopulation->GetPopulationSize();

  for (i = 0; i < populationSize; i++)
  {
    fitness = thePopulation->GetGenome(i)->GetFitness();
    if (fitness > max) max = fitness;
    if (fitness < min) min = fitness;
    sum += fitness;
  }

  mean = sum / populationSize;
  for (i = 0; i < populationSize; i++)
  {
    fitness = thePopulation->GetGenome(i)->GetFitness();
    diff = fitness - mean;
    sumSquareDiff += diff * diff;
  }
  variance = sumSquareDiff / populationSize;
  sd = sqrt(variance);

  stats->min = min;
  stats->max = max;
  stats->mean = mean;
  stats->sd = sd;
}

// output to a stream
ostream& operator<<(ostream &out, Statistics &s)
{
  out << s.max << "\t" << s.mean << "\t"
	  << s.min << "\t" << s.sd;
  
  return out;
}

// calculate percentiles at 10% intervals
// ASSUMES POPULATION IS SORTED!
void CalculateTenPercentiles(Population *thePopulation, TenPercentiles *perc)
{
  int lastGenome = thePopulation->GetPopulationSize() - 1;
  int i;
  
  for (i = 0; i < 11; i ++)
  {
    perc->values[i] = 
        thePopulation->GetGenome((i * lastGenome) / 10)->GetFitness();
  }
}

// output to a stream
ostream& operator<<(ostream &out, TenPercentiles &s)
{
  int i;
  for (i = 0; i < 11; i++)
    out << s.values[i] << "\t";
  
  return out;
}
