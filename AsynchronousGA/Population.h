/*
 *  Population.h
 *  AsynchronousGA
 *
 *  Created by Bill Sellers on 19/10/2010.
 *  Copyright 2010 Bill Sellers. All rights reserved.
 *
 */

#ifndef POPULATION_H
#define POPULATION_H

#include <time.h>
#include <iostream>
#include <list>
#include <set>

#include "avl_array/avl_array.hpp"

#include "Genome.h"

namespace AsynchronousGA
{

enum SelectionType
{
    UniformSelection,
    RankBasedSelection,
    SqrtBasedSelection,
    GammaBasedSelection
};

enum ResizeControl
{
    DuplicateResize,
    RandomiseResize,
    MutateResize
};

class Population
{
public:
    Population();
    ~Population();

    void InitialisePopulation(int populationSize, Genome *genome);

    Genome *GetFirstGenome() { return m_Population.front(); }
    Genome *GetLastGenome() { return m_Population.back(); }
    Genome *GetGenome(int i) { return m_Population[i]; }
    int GetPopulationSize() { return m_Population.size(); }

    void SetSelectionType(SelectionType type) { m_SelectionType = type; }
    void SetParentsToKeep(int parentsToKeep) { m_ParentsToKeep = parentsToKeep; if (m_ParentsToKeep < 0) m_ParentsToKeep = 0; };
    void SetGlobalCircularMutation(bool circularMutation);
    void SetResizeControl(ResizeControl control) { m_ResizeControl = control; }


    Genome *ChooseParent(int *parentRank);
    void Randomise();
    int InsertGenome(Genome *genome, int targetPopulationSize);
    int ResizePopulation(int size);
    void ResetFitness(double fitness);

    int ReadPopulation(const char *filename);
    int WritePopulation(const char *filename, int nBest);

protected:

    mkr::avl_array<Genome *> m_Population;
    mkr::avl_array<Genome *> m_ImmortalList;
    std::list<Genome *> m_AgeList;
    SelectionType m_SelectionType;
    int m_ParentsToKeep;
    ResizeControl m_ResizeControl;

};

}

#endif // POPULATION_H
