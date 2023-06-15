// Genome.h - basic class containing the genome for an individual

#ifndef GENOME_H
#define GENOME_H

#include <iostream>

struct Parent
{
  int generationNumber;
  int rank;
};

enum GenomeType
{
  DefaultGenome = 0,
  IndividualRanges = -1
};

class Genome
{
public:

  Genome();
  ~Genome();

  void InitialiseGenome(GenomeType genomeType,
                        int genomeLength,
                        double lowBound = 0.0,
                        double highBound = 1.0,
                        bool randomFlag = true, 
                        double value = 0,
                        double gaussianSD = 1.0);
  double GetGene(int i);
  int GetGenomeLength() { return mGenomeLength; };
  double GetHighBound() { return mHighBound; };
  double GetLowBound() { return mLowBound; };
  bool GetFitnessValid() { return mFitnessValid; };
  double GetFitness(); 
  Parent *GetParent1() { return &mParent1; };
  Parent *GetParent2() { return &mParent2; };
  GenomeType GetGenomeType() { return mGenomeType; };

  void Randomise();
  void SetGene(int i, double value);
  void SetFitness(double fitness) 
      { 
        mFitness = fitness; 
        mFitnessValid = true;
      };
  void SetParent1(int generationNumber, int parentRank) 
      { 
        mParent1.generationNumber = generationNumber;
        mParent1.rank = parentRank;
      };
  void SetParent2(int generationNumber, int parentRank) 
      { 
        mParent2.generationNumber = generationNumber;
        mParent2.rank = parentRank;
      };

  Genome& operator=(Genome &in);

  friend std::ostream& operator<<(std::ostream &out, Genome &g);
  friend std::istream& operator>>(std::istream &in, Genome &g);
  
  friend bool GreaterThan(Genome *g1, Genome *g2);

#if defined(USE_PVM)
  friend class Objective;
#endif  
  
protected:

  int mGenomeLength;
  double *mGenes;
  double mLowBound;
  double mHighBound;
  double mFitness;
  Parent mParent1;
  Parent mParent2;
  GenomeType mGenomeType;
  double *mLowBounds;
  double *mHighBounds;
  double *mGaussianSDs;
  bool mFitnessValid;
};

#endif // GENOME_H
