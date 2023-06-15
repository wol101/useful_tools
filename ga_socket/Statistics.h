// Statistics.h - calculate various statistics from a population

#ifndef STATISTICS_H
#define STATISTICS_H

class Genome;
class ostream;
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
ostream& operator<<(ostream &out, Statistics &s);
ostream& operator<<(ostream &out, TenPercentiles &s);

#endif // STATISTICS_H
