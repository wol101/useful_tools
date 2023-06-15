/*
 *  Genome.cpp
 *  AsynchronousGA
 *
 *  Created by Bill Sellers on 19/10/2010.
 *  Copyright 2010 Bill Sellers. All rights reserved.
 *
 */

#include <assert.h>
#include <iostream>
#include <float.h>
#include <iomanip>
#include <ios>
#include <limits>

#include "Genome.h"
#include "Random.h"

// constructor
Genome::Genome()
{
    mGenes = 0;
    mGenomeLength = 0;
    mFitness = 0;
    mLowBounds = 0;
    mHighBounds = 0;
    mGaussianSDs = 0;
    mCircularMutationFlags = 0;
    mGenomeType = IndividualRanges;
    mGlobalCircularMutationFlag = false;
}

Genome::Genome(Genome &in)
{
    int i;

    mGenomeLength = in.mGenomeLength;
    mGenomeType = in.mGenomeType;
    mGlobalCircularMutationFlag = in.mGlobalCircularMutationFlag;

    mGenes = new double[mGenomeLength];
    for (i = 0; i < mGenomeLength; i++) mGenes[i] = in.mGenes[i];
    mFitness = in.mFitness;

    switch (mGenomeType)
    {
    case IndividualRanges:
        mLowBounds = new double[mGenomeLength];
        mHighBounds = new double[mGenomeLength];
        mGaussianSDs = new double[mGenomeLength];
        mCircularMutationFlags = 0;
        for (i = 0; i < mGenomeLength; i++)
        {
            mLowBounds[i] = in.mLowBounds[i];
            mHighBounds[i] = in.mHighBounds[i];
            mGaussianSDs[i] = in.mGaussianSDs[i];
        }
        break;

    case IndividualCircularMutation:
        mLowBounds = new double[mGenomeLength];
        mHighBounds = new double[mGenomeLength];
        mGaussianSDs = new double[mGenomeLength];
        mCircularMutationFlags = new bool[mGenomeLength];
        for (i = 0; i < mGenomeLength; i++)
        {
            mLowBounds[i] = in.mLowBounds[i];
            mHighBounds[i] = in.mHighBounds[i];
            mGaussianSDs[i] = in.mGaussianSDs[i];
            mCircularMutationFlags[i] = in.mCircularMutationFlags[i];
        }
        break;
    }
}


// destructor - deallocates memory
Genome::~Genome()
{
    if (mGenes) delete [] mGenes;
    if (mLowBounds) delete [] mLowBounds;
    if (mHighBounds) delete [] mHighBounds;
    if (mGaussianSDs) delete [] mGaussianSDs;
    if (mCircularMutationFlags) delete [] mCircularMutationFlags;
}

// randomise the genome
void Genome::Randomise()
{
    int i;

    switch (mGenomeType)
    {
    case IndividualRanges:
    case IndividualCircularMutation:
        for (i = 0; i < mGenomeLength; i++)
        {
            if (mGaussianSDs[i] != 0)
                mGenes[i] = g_random->RandomDouble(mLowBounds[i], mHighBounds[i]);
        }
        break;
    }
}

// define = operator
Genome & Genome::operator=(const Genome &in)
{
    int i;

    // check for mismatch
    if (mGenomeLength != in.mGenomeLength || mGenomeType != in.mGenomeType)
    {
        if (mGenes) delete [] mGenes;
        if (mLowBounds) delete [] mLowBounds;
        if (mHighBounds) delete [] mHighBounds;
        if (mGaussianSDs) delete [] mGaussianSDs;
        if (mCircularMutationFlags)
        {
            delete [] mCircularMutationFlags;
            mCircularMutationFlags = 0;
        }

        mGenomeLength = in.mGenomeLength;
        mGenomeType = in.mGenomeType;

        mGenes = new double[mGenomeLength];
        switch (mGenomeType)
        {
        case IndividualRanges:
            mLowBounds = new double[mGenomeLength];
            mHighBounds = new double[mGenomeLength];
            mGaussianSDs = new double[mGenomeLength];
            break;

        case IndividualCircularMutation:
            mLowBounds = new double[mGenomeLength];
            mHighBounds = new double[mGenomeLength];
            mGaussianSDs = new double[mGenomeLength];
            mCircularMutationFlags = new bool[mGenomeLength];
            break;
        }
    }

    for (i = 0; i < mGenomeLength; i++) mGenes[i] = in.mGenes[i];
    mFitness = in.mFitness;
    mGlobalCircularMutationFlag = in.mGlobalCircularMutationFlag;

    switch (mGenomeType)
    {
    case IndividualRanges:
        for (i = 0; i < mGenomeLength; i++)
        {
            mLowBounds[i] = in.mLowBounds[i];
            mHighBounds[i] = in.mHighBounds[i];
            mGaussianSDs[i] = in.mGaussianSDs[i];
        }
        break;

    case IndividualCircularMutation:
        for (i = 0; i < mGenomeLength; i++)
        {
            mLowBounds[i] = in.mLowBounds[i];
            mHighBounds[i] = in.mHighBounds[i];
            mGaussianSDs[i] = in.mGaussianSDs[i];
            mCircularMutationFlags[i] = in.mCircularMutationFlags[i];
        }
        break;
    }

    return *this;
}

// define < operator
bool Genome::operator<(const Genome &in)
{
    std::cerr << "Fitness test " << this->GetFitness() << " " << in.GetFitness() << "\n";
    if (this->GetFitness() < in.GetFitness()) return true;
    return false;
}

// get the value of the circular mutation flag for a gene
bool Genome::GetCircularMutation(int i)
{
    bool v = 0;

    switch (mGenomeType)
    {
    case IndividualRanges:
        v = mGlobalCircularMutationFlag;
        break;

    case IndividualCircularMutation:
        v = mCircularMutationFlags[i];
        break;
    }

    return v;
}

// set the value of the circular mutation flag for a gene
void Genome::SetCircularMutation(int i, bool circularFlag)
{
    switch (mGenomeType)
    {
    case IndividualRanges:
        mGlobalCircularMutationFlag = circularFlag;
        break;

    case IndividualCircularMutation:
        mCircularMutationFlags[i] = circularFlag;
        break;
    }
}


// output to a stream
std::ostream& operator<<(std::ostream &out, const Genome &g)
{
    int i;

    switch (g.mGenomeType)
    {
    case Genome::IndividualRanges:
        out << g.mGenomeType << "\n";
        out << g.mGenomeLength << "\n";
        for (i = 0; i < g.mGenomeLength; i++)
        {
            out << std::scientific << std::setprecision(18) << g.mGenes[i] << "\t" <<
                   std::defaultfloat << std::setprecision(8) << g.mLowBounds[i] << "\t" <<
                   g.mHighBounds[i] << "\t" << g.mGaussianSDs[i] << "\n";
        }
        out << std::scientific << std::setprecision(18) << g.mFitness << "\t0\t0\t0\t0\n";
        break;

    case Genome::IndividualCircularMutation:
        out << g.mGenomeType << "\n";
        out << g.mGenomeLength << "\n";
        for (i = 0; i < g.mGenomeLength; i++)
        {
            out << std::scientific << std::setprecision(18) << g.mGenes[i] << "\t" <<
                   std::defaultfloat << std::setprecision(8) << g.mLowBounds[i] << "\t" <<
                   g.mHighBounds[i] << "\t" << g.mGaussianSDs[i] << "\t" << g.mCircularMutationFlags[i] << "\n";
        }
        out << std::scientific << std::setprecision(18) << g.mFitness << std::defaultfloat << "\t0\t0\t0\t0\n";
        break;
    }

    return out;
}

// input from a stream
std::istream& operator>>(std::istream &in, Genome &g)
{
    int i;
    int genomeLength;
    Genome::GenomeType genomeType;
    int dummy;

    in >> i;
    genomeType = (Genome::GenomeType)i;
    in >> genomeLength;

    // check for mismatch
    if (genomeLength != g.mGenomeLength || genomeType != g.mGenomeType)
    {
        if (g.mGenes) delete [] g.mGenes;
        if (g.mLowBounds) delete [] g.mLowBounds;
        if (g.mHighBounds) delete [] g.mHighBounds;
        if (g.mGaussianSDs) delete [] g.mGaussianSDs;
        if (g.mCircularMutationFlags)
        {
            delete [] g.mCircularMutationFlags;
            g.mCircularMutationFlags = nullptr;
        }

        g.mGenomeLength = genomeLength;
        g.mGenomeType = genomeType;
        g.mGenes = new double[g.mGenomeLength];
        switch (g.mGenomeType)
        {
        case Genome::IndividualRanges:
            g.mLowBounds = new double[g.mGenomeLength];
            g.mHighBounds = new double[g.mGenomeLength];
            g.mGaussianSDs = new double[g.mGenomeLength];
            break;

        case Genome::IndividualCircularMutation:
            g.mLowBounds = new double[g.mGenomeLength];
            g.mHighBounds = new double[g.mGenomeLength];
            g.mGaussianSDs = new double[g.mGenomeLength];
            g.mCircularMutationFlags = new bool[g.mGenomeLength];
            break;
        }
    }


    switch (g.mGenomeType)
    {
    case Genome::IndividualRanges:
        for (i = 0; i < g.mGenomeLength; i++)
            in >> g.mGenes[i] >> g.mLowBounds[i] >> g.mHighBounds[i] >> g.mGaussianSDs[i];
        in >> g.mFitness >> dummy >> dummy >> dummy >> dummy;
        break;

    case Genome::IndividualCircularMutation:
        for (i = 0; i < g.mGenomeLength; i++)
            in >> g.mGenes[i] >> g.mLowBounds[i] >> g.mHighBounds[i] >> g.mGaussianSDs[i] >> g.mCircularMutationFlags[i];
        in >> g.mFitness >> dummy >> dummy >> dummy >> dummy;
        break;

    }

    return in;
}

bool GenomeFitnessLessThan(Genome *g1, Genome *g2)
{
    if (g1->GetFitness() < g2->GetFitness()) return true;
    return false;
}


