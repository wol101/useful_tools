// Population.h - storage class for individual genomes

#ifndef POPULATION_H
#define POPULATION_H

#include <time.h>
#include <iostream>

class Genome;

enum SelectionType
{
  UniformSelection,
  RankBasedSelection,
  SqrtBasedSelection,
  RouletteWheelSelection
};


class Population
{
 public:
  Population();
  ~Population();

  void InitialisePopulation(int populationSize,
      GenomeType genomeType, 
      int genomeLength,
      double lowBound = 0.0,
      double highBound = 1.0,
      bool randomFlag = true, 
      double value = 0);

  Genome *GetGenome(int i) { return m_Population[i]; };
  int GetPopulationSize() { return m_PopulationSize; };
  void Sort();
  void UniqueResort();
  void SetSelectionType(SelectionType type) { m_SelectionType = type; };
  void InitialiseParentChoice();
  Genome * ChooseParent(int *parentRank);
  void Randomise(int from, int to);
#if defined(USE_FS)   
  void CalculateFitness(int from, int to, char *extraArgs, 
      char *programName, bool readGenomeFlag);
  void SetMaxRunning(int i) { m_MaxRunning = i; };
#elif defined(USE_PVM) || defined(USE_SOCKETS)
  void CalculateFitness(int from, int to, char *extraArgs, 
   char *programName, char *extraData, time_t timeLimit, 
       int restartOnError);
#endif  
  friend std::ostream& operator<<(std::ostream &out, Population &g);
  friend std::istream& operator>>(std::istream &in, Population &g);

 protected:

  Genome **m_Population;
  int m_PopulationSize;
  SelectionType m_SelectionType;
#if defined(USE_FS)   
  int m_MaxRunning;
#endif  
  double *m_CumulativeFitness;
};

#endif // POPULATION_H
