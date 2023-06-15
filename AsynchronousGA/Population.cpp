/*
 *  Population.cpp
 *  AsynchronousGA
 *
 *  Created by Bill Sellers on 19/10/2010.
 *  Copyright 2010 Bill Sellers. All rights reserved.
 *
 */

#ifdef USE_MPI
#include <mpi.h>
#endif

#include <assert.h>
#include <iostream>
#include <fstream>
// #include <unistd.h>
#include <memory.h>
#include <float.h>
#include <map>
#include <list>

#include "Genome.h"
#include "Population.h"
#include "Random.h"
#include "DebugControl.h"
#include "Preferences.h"

#include "MPIStuff.h"
extern int gMPI_Comm_size;
extern int gMPI_Comm_rank;

namespace AsynchronousGA
{

extern Preferences gPrefs;

bool GenomeFitnessLessThan(Genome *g1, Genome *g2);

// constructor
Population::Population()
{
    m_SelectionType = RankBasedSelection;
    m_ParentsToKeep = 0;
    m_ResizeControl = MutateResize;
}

// destructor
Population::~Population()
{
    mkr::avl_array<Genome *>::iterator iter;
    for (iter = m_Population.begin(); iter != m_Population.end(); iter++)
        delete *iter;
}

// initialise the population
void Population::InitialisePopulation(int populationSize, Genome *genome)
{
    int i;
    Genome *g;
    for (i = 0; i < populationSize; i++)
    {
        g = new Genome();
        *g = *genome;
        m_Population.push_back(g);
    }
}

// choose a parent from a population
Genome * Population::ChooseParent(int *parentRank)
{
    // this type biases random choice to higher ranked individuals using the gamma function
    // this assumes a sorted genome
    if (m_SelectionType == GammaBasedSelection)
    {
        *parentRank = GammaBiasedRandomInt(0, m_Population.size() - 1, gPrefs.gamma);
        return m_Population[*parentRank];
    }

    // in this version we do uniform selection and just choose a parent
    // at random
    if (m_SelectionType == UniformSelection)
    {
        *parentRank = RandomInt(0, m_Population.size() - 1);
        return m_Population[*parentRank];
    }

    // this type biases random choice to higher ranked individuals
    // this assumes a sorted genome
    if (m_SelectionType == RankBasedSelection)
    {
        *parentRank = RankBiasedRandomInt(1, m_Population.size()) - 1;
        return m_Population[*parentRank];
    }

    // this type biases random choice to higher ranked individuals
    // this assumes a sorted genome
    if (m_SelectionType == SqrtBasedSelection)
    {
        *parentRank = SqrtBiasedRandomInt(0, m_Population.size() - 1);
        return m_Population[*parentRank];
    }

    // should never get here
    std::cerr << "Logic error Population::ChooseParent " << __LINE__ << "\n";
    return 0;
}

// insert a genome into the population
// the population is always kept sorted by fitness and the same size
// the oldest genome is deleted unless it is in the
// immortal list
int Population::InsertGenome(Genome *genome, int targetPopulationSize)
{
    // insert into master fitness sorted list
    int originalSize = m_Population.size();
    m_Population.insert_sorted(genome, false, GenomeFitnessLessThan);
    if (originalSize == m_Population.size())
    {
        if (gDebugControl == PopulationDebug) std::cerr << "InsertGenome not unique; m_Population.size() = " << m_Population.size() << "\n";
        delete genome;
        return 0; // nothing inserted because the fitness already exists
    }
    if (targetPopulationSize == 0) targetPopulationSize = originalSize; // not trying to change the size of the population

    // ok now insert into the other internal lists
    if (m_ParentsToKeep == 0)
    {
        m_AgeList.push_back(genome); // just add current genome to list by age
        if (gDebugControl == PopulationDebug) std::cerr << "InsertGenome adding to m_AgeList - no test; size = " << m_AgeList.size() << "\n";
    }
    else
    {
        // need to worry about immortality
        if (m_ImmortalList.size() < m_ParentsToKeep)
        {
            m_ImmortalList.insert_sorted(genome, false, GenomeFitnessLessThan);;
            if (gDebugControl == PopulationDebug) std::cerr << "InsertGenome adding to m_ImmortalList - no test; size = " << m_ImmortalList.size() << "\n";
        }
        else
        {
            if (genome->GetFitness() > m_ImmortalList.front()->GetFitness())
            {
                m_ImmortalList.insert_sorted(genome, false, GenomeFitnessLessThan);;
                m_AgeList.push_back(m_ImmortalList.front());
                m_ImmortalList.pop_front();
                if (gDebugControl == PopulationDebug) std::cerr << "InsertGenome m_ImmortalList to m_AgeList bump; m_AgeList.size() = " << m_AgeList.size() << " m_ImmortalList.size() = " << m_ImmortalList.size() << "\n";
            }
            else
            {
                m_AgeList.push_back(genome);
                if (gDebugControl == PopulationDebug) std::cerr << "InsertGenome adding to m_AgeList after test; size = " << m_AgeList.size() << "\n";
            }
        }
    }

    // check population sizes
    if (m_Population.size() > targetPopulationSize)
    {
        Genome *genomeToDelete = *m_AgeList.begin();
        m_AgeList.pop_front();
        mkr::avl_array<Genome *>::iterator iter;
        bool found = m_Population.binary_search(genomeToDelete, iter, GenomeFitnessLessThan);
        if (found)
        {
            m_Population.erase(iter);
            delete genomeToDelete;
            if (gDebugControl == PopulationDebug) std::cerr << "InsertGenome deleting from m_Population; size = " << m_Population.size() << "\n";
        }
        else // shouldn't actually ever get her but it's there just in case
        {
            std::cerr << "Logic error Population::InsertGenome genome not found" << __LINE__ << "\n";
            delete m_Population.front();
            m_Population.pop_front();
            if (gDebugControl == PopulationDebug) std::cerr << "InsertGenome deleting from m_Population (Logic Error); size = " << m_Population.size() << "\n";
        }

    }

    return 0;
}

// randomise the population
void Population::Randomise()
{
    mkr::avl_array<Genome *>::iterator iter;
    for (iter = m_Population.begin(); iter != m_Population.end(); iter++)
        (*iter)->Randomise();
}

// set the fitness of all the members of the population
void Population::ResetFitness(double fitness)
{
    mkr::avl_array<Genome *>::iterator iter;
    for (iter = m_Population.begin(); iter != m_Population.end(); iter++)
        (*iter)->SetFitness(fitness);
}

// reset the population size to a new value - needs at least one valid genome in population
int Population::ResizePopulation(int size)
{
    int i, j;
    if (m_Population.size() < 1) return __LINE__;
    if (m_Population.size() == (unsigned int)size) return 0;

    int delta = size - (int)m_Population.size();
    int startingSize = (int)m_Population.size();

    Genome *g;

    if (delta > 0)
    {
        switch (m_ResizeControl)
        {
        case DuplicateResize:
            // fill in with duplicate genomes
            j = 0;
            for (i = 0; i < delta; i++)
            {
                g = new Genome();
                if (j >= startingSize) j = 0;
                *g = *m_Population[j];
                m_Population.push_back(g);
                j++;
            }
            break;

        case RandomiseResize:
            // fill in with random genomes
            for (i = 0; i < delta; i++)
            {
                g = new Genome();
                *g = **m_Population.begin();
                g->Randomise();
                g->SetFitness(-DBL_MAX);
                m_Population.push_front(g);
            }
            break;

        case MutateResize:
            // fill in with mutated genomes
            j = 0;
            for (i = 0; i < delta; i++)
            {
                g = new Genome();
                if (j >= startingSize) j = 0;
                *g = *m_Population[j];
                while (GaussianMutate(g, 1.0) == 0);
                m_Population.push_back(g);
                j++;
            }
            break;

        }
    }
    else
    {
        for (i = 0; i < -delta; i++)
        {
            delete *m_Population.begin();
            m_Population.pop_front();

        }
    }

    return 0;
}

// set the circular flags for the genomes in the population
void Population::SetGlobalCircularMutation(bool circularMutation)
{
    mkr::avl_array<Genome *>::iterator iter;
    for (iter = m_Population.begin(); iter != m_Population.end(); ++iter)
        (*iter)->SetGlobalCircularMutationFlag(circularMutation);
}

// output a subpopulation as a new population
// note: outputs population with fittest first
int Population::WritePopulation(const char *filename, int nBest)
{
    if (nBest <= 0 || nBest > m_Population.size()) nBest = m_Population.size();

    std::ofstream outFile(filename);
    if(outFile.good() == false ) return __LINE__;

    outFile << nBest << "\n";

    int count = 0;
    mkr::avl_array<Genome *>::const_reverse_iterator iter;
    for (iter = m_Population.rbegin(); iter != m_Population.rend(); ++iter)
    {
        outFile << **iter;
        count++;

        if (count >= nBest) break;
    }
    outFile.close();
    return 0;
}

// read a population
// this assumes they are already in fitness order with the fittest first
int Population::ReadPopulation(const char *filename)
{
    std::ifstream inFile(filename);
    if(inFile.good() == false ) return __LINE__;

    if (m_Population.size() > 0)
    {
        mkr::avl_array<Genome *>::iterator iter;
        for (iter = m_Population.begin(); iter != m_Population.end(); ++iter)
            delete *iter;
        m_Population.clear();
    }

    int i;
    int populationSize;
    Genome *genome;
    inFile >> populationSize;
    for (i = 0; i < populationSize; i++)
    {
        genome = new Genome();
        inFile >> *genome;
        m_Population.push_front(genome);
    }
    return 0;
}

}
