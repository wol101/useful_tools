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
                              double lowBound,
                              double highBound,
                              bool randomFlag, 
                              double value,
                              double gaussianSD);
    
    Genome *GetGenome(int i) { return m_Population[i]; };
    int GetPopulationSize() { return m_PopulationSize; };
    void Sort();
    void UniqueResort();
    void SetSelectionType(SelectionType type) { m_SelectionType = type; };
    void InitialiseParentChoice();
    Genome * ChooseParent(int *parentRank);
    void Randomise(int from, int to);
    void SetMaxRunning(int i) { m_MaxRunning = i; };
    void CalculateFitness(int from, int to, char *programName, time_t timeLimit, int restartOnError);
    void Resize(int newSize);
    void OutputSubpopulation(const char *filename, int nBest);
    
    
    friend ostream& operator<<(ostream &out, Population &g);
    friend istream& operator>>(istream &in, Population &g);
    
protected:
    
    Genome **m_Population;
    int m_PopulationSize;
    SelectionType m_SelectionType;
    int m_MaxRunning;
    double *m_CumulativeFitness;
};

#ifdef USE_TCP
void *ThreadedCommunication(void *threadArg);
#endif

#endif // POPULATION_H
