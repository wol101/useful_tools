// Statistics.h - calculate various statistics from a population

#ifndef STATISTICS_H
#define STATISTICS_H

#include <iostream>

class Genome;
class Population;

struct Statistics
{
  double min;
  double max;
  double mean;
  double sd;
};

struct TenPercentiles
{
  double values[11];
};
    
void CalculateStatistics(Population *thePopulation, Statistics *stats);
void CalculateTenPercentiles(Population *thePopulation, TenPercentiles *perc);
std::ostream& operator<<(std::ostream &out, Statistics &s);
std::ostream& operator<<(std::ostream &out, TenPercentiles &s);

#endif // STATISTICS_H
