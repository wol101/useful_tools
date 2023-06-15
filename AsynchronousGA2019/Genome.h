/*
 *  Genome.h
 *  AsynchronousGA
 *
 *  Created by Bill Sellers on 19/10/2010.
 *  Copyright 2010 Bill Sellers. All rights reserved.
 *
 */

#ifndef GENOME_H
#define GENOME_H

#include "Random.h"

#include <iostream>

class Genome
{
public:

    Genome();
    Genome(Genome &g);
    ~Genome();

    enum GenomeType
    {
        IndividualRanges = -1,
        IndividualCircularMutation = -2
    };

    double GetGene(int i) const { return mGenes[i]; }
    int GetGenomeLength() const { return mGenomeLength; }
    double GetHighBound(int i) const { return mHighBounds[i]; }
    double GetLowBound(int i) const { return mLowBounds[i]; }
    double GetGaussianSD(int i) const { return mGaussianSDs[i]; }
    double GetFitness() const { return mFitness; }
    GenomeType GetGenomeType() const { return mGenomeType; }
    double *GetGenes() const { return mGenes; }
    bool GetCircularMutation(int i);
    bool GetGlobalCircularMutationFlag() { return mGlobalCircularMutationFlag; }

    void Randomise();
    void SetGene(int i, double value) { mGenes[i] = value; }
    void SetFitness(double fitness) { mFitness = fitness; }
    void SetCircularMutation(int i, bool circularFlag);
    void SetGlobalCircularMutationFlag(bool circularFlag) { mGlobalCircularMutationFlag = circularFlag; }

    Genome& operator=(const Genome &in);
    bool operator<(const Genome &in);

    friend std::ostream& operator<<(std::ostream &out, const Genome &g);
    friend std::istream& operator>>(std::istream &in, Genome &g);

private:

    int mGenomeLength;
    double *mGenes;
    double mFitness;
    GenomeType mGenomeType;
    double *mLowBounds;
    double *mHighBounds;
    double *mGaussianSDs;
    bool *mCircularMutationFlags;
    bool mGlobalCircularMutationFlag;
};

bool GenomeFitnessLessThan(Genome *g1, Genome *g2);

#endif // GENOME_H
