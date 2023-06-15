// Population.cc - storage class for individual genomes

#include <assert.h>
#include <iostream.h>
#include <unistd.h>

#include "Genome.h"
#include "Population.h"
#include "Random.h"
#include "Objective.h"
#include "QuickSort.h"
#include "Debug.h"

// constructor
Population::Population()
{
  m_Population = 0;
  m_PopulationSize = 0;
  m_SelectionType = RankBasedSelection;
  m_CumulativeFitness = 0;
}

// destructor
Population::~Population()
{
  int i;
  if (m_Population) 
  {
    for (i = 0; i < m_PopulationSize; i++)
    delete m_Population[i];
    delete [] m_Population;
  }
  if (m_CumulativeFitness) delete [] m_CumulativeFitness;
}

// initialise the population 
void Population::InitialisePopulation(int populationSize,
      GenomeType genomeType, 
      int genomeLength,
      double lowBound,
      double highBound,
      bool randomFlag, 
      double value)
{
  int i;

  m_PopulationSize = populationSize;
  
  m_CumulativeFitness = new double[m_PopulationSize];
  m_Population = new Genome *[m_PopulationSize];
  for (i = 0; i < m_PopulationSize; i++)
  {
    m_Population[i] = new Genome();
    m_Population[i]->InitialiseGenome(genomeType, genomeLength, 
                    lowBound, highBound,
                    randomFlag, value);
  }
}

// choose a parent from a population
Genome * Population::ChooseParent(int *parentRank)
{
  double r;
  int l, u, p, d;

  // in this version we do uniform selection and just choose a parent
  // at random
  if (m_SelectionType == UniformSelection)
  {
    *parentRank = RandomInt(0, m_PopulationSize - 1);
    return m_Population[*parentRank];
  }

  // this type biases random choice to higher ranked individuals
  // this assumes a sorted genome
  if (m_SelectionType == RankBasedSelection)
  {
    *parentRank = RankBiasedRandomInt(1, m_PopulationSize) - 1;
    return m_Population[*parentRank];
  }
  
  // this type biases random choice to higher ranked individuals
  // this assumes a sorted genome
  if (m_SelectionType == SqrtBasedSelection)
  {
    *parentRank = SqrtBiasedRandomInt(0, m_PopulationSize - 1);
    return m_Population[*parentRank];
  }

  // this type biases random choice to higher ranked individuals
  // again assumes a sorted genome since m_CumulativeFitness is
  // allocated at sort time
  // MUST HAVE POSITIVE FITNESS
  if (m_SelectionType == RouletteWheelSelection)
  {
    // if fitness total is zero fall back on uniform selection
    if (m_CumulativeFitness[m_PopulationSize - 1] == 0)
    {
      *parentRank = RandomInt(0, m_PopulationSize - 1);
      return m_Population[*parentRank];
    }
      
    // first need to get a suitable random number
    r = RandomDouble(0, m_CumulativeFitness[m_PopulationSize - 1]);
    // now find the index where: 
    // m_CumulativeFitness[index - 1] < r <= m_CumulativeFitness[index]
    l = 0;
    u = m_PopulationSize - 1;
    p = (u - l) / 2;
    while (true)
    {
      if (m_CumulativeFitness[p - 1] < r && m_CumulativeFitness[p] >= r) break;
      if (r > m_CumulativeFitness[p]) 
      {
        l = p;
        d = (u - l) / 2;
        if (d == 0) d = 1;
        p += d;
      }
      else 
      {
        u = p;
        d = (u - l) / 2;
        if (d == 0) d = 1;
        p -= d;
      }
    }
    *parentRank = p;
    
    if (gDebug == Debug_Population)
      cerr << "Population::ChooseParent\tRouletteWheelSelection\tr\t" <<
      r << "\tparentRank\t" << *parentRank << "\tm_CumulativeFitness\t" <<
      m_CumulativeFitness[p] << "\n";
      
    return m_Population[*parentRank];
  }
  
  // should never get here
  return 0;
}

// sort by fitness and fill in the m_CumulativeFitness if we
// are using it
void Population::Sort()
{
  double total;
  double fitness;
  int i;
  
  QuickSort(m_Population, m_PopulationSize);
  
  if (m_SelectionType == RouletteWheelSelection)
  {
    // need to fill m_CumulativeFitness
    total = 0;
    for (i = 0; i < m_PopulationSize; i++)
    {
      fitness = m_Population[i]->GetFitness();
      if (fitness < 0) fitness = 0;
      m_CumulativeFitness[i] = total + fitness;
      total += fitness;
      
      if (gDebug == Debug_Population)
        cerr << "Population::Sort\tm_CumulativeFitness[\t" << i <<
        "\t]\t" << m_CumulativeFitness[i] << "\n";
    }
  }
}

// randomise a subset of the population
void Population::Randomise(int from, int to)
{
  int i;
  
  for (i = from; i < to; i++)
    m_Population[i]->Randomise();
}
    
// calculate the fitness for a subset of the population    
void Population::CalculateFitness(int from, int to, char *extraArgs,
    char *programName, char *extraData, time_t timeLimit, 
    int restartOnError)
{
  int i, j;
  bool stillWaiting;
  
  int runListSize = to - from;
  Objective *runList = new Objective[runListSize];
  
  // loop through runList starting processes
  for (i = 0; i < runListSize; i++)
  {
    // set up the data
    runList[i].SetID(from + i);
    runList[i].SetGenome(m_Population[from + i]);
    runList[i].SetTimeLimit(timeLimit);
    if (programName) runList[i].SetProgramName(programName);
    if (extraArgs) runList[i].SetExtraArguments(extraArgs);
    if (extraData) runList[i].SetExtraData(extraData);

    // and wait until the process starts up
    while (runList[i].Run() == false) 
    {
      for (j = 0; j < i; j++) runList[j].CheckRunning();
      usleep(10000);
    }
  }
  
  // now loop until all the processes have finished
  do
  {
    stillWaiting = false;
    for (j = 0; j < runListSize; j++) 
    {
      if (runList[j].CheckRunning()) stillWaiting = true;
    }
  } while (stillWaiting);

  // all finished now so read off the fitness values
  for (i = 0; i < runListSize; i++)
  {
    m_Population[runList[i].GetID()]->SetFitness(runList[i].GetScore());
  }
  
  // and delete the run list  
  delete [] runList;
}

    
// Resort a previously sorted genome so that only unique fitness elements
// are represented (duplicates are put to the end of the list)
void Population::UniqueResort()
{
  int i, j;
  Genome *t;
  
  for (i = m_PopulationSize - 1; i >= 1; i--)
  {
    if (m_Population[i]->GetFitness() <= m_Population[i - 1]->GetFitness())
    {
      // got a match so search down list for first non-match
      for (j = i - 2; j >= 0; j--)
      {
        if (m_Population[i]->GetFitness() > m_Population[j]->GetFitness()) break;
      }
      if (j < 0) break; // we've done the best we can
      t = m_Population[i - 1];
      m_Population[i - 1] = m_Population[j];
      m_Population[j] = t;
    }
  }
}
          
/* slower (I'm guessing) untested version     
void Population::UniqueResort()
{
  int i, j;
  Genome *t;
  int count;
  
  for (i = m_PopulationSize - 1; i >= 2; i--)
  {
    count = i - 1;
    while (m_Population[i]->GetFitness() == m_Population[i - 1]->GetFitness())
    {
      // got a match so move to the beginning and shuffle everything up
      t = m_Population[i - 1];
      for (j = i - 1; j >= 1; j--)
      {
        m_Population[j] = m_Population[j - 1];
      }
      m_Population[0] = t;
      count--;
      if (count == 0) return; // done the best we can
    }
  }
}          
*/       

ostream& operator<<(ostream &out, Population &g)
{
  int i;
  out << g.m_PopulationSize << "\n";
  for (i = 0; i < g.m_PopulationSize; i++)
  out << *(g.m_Population[i]);
  return out;
}

istream& operator>>(istream &in, Population &g)
{
  int i;
  int populationSize;

  in >> populationSize;
  if (populationSize !=g.m_PopulationSize)
  {
    if (g.m_Population) 
    {
      for (i = 0; i < g.m_PopulationSize; i++)
        delete g.m_Population[i];
      delete [] g.m_Population;
    }
    if (g.m_CumulativeFitness) delete [] g.m_CumulativeFitness;
    
    g.m_PopulationSize = populationSize;
    g.m_CumulativeFitness = new double[g.m_PopulationSize];
    g.m_Population = new Genome *[g.m_PopulationSize];
    for (i = 0; i < g.m_PopulationSize; i++)
      g.m_Population[i] = new Genome();
  }

  for (i = 0; i < g.m_PopulationSize; i++)
    in >> *(g.m_Population[i]);
  return in;
}  


