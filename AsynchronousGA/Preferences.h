/*
 *  Preferences.h
 *  AsynchronousGA
 *
 *  Created by Bill Sellers on 10/11/2010.
 *  Copyright 2010 Bill Sellers. All rights reserved.
 *
 */

#ifndef PREFERENCES_H
#define PREFERENCES_H

#include <iostream>
#include "DataFile.h"
#include "Population.h"
#include "Mating.h"
#include "Random.h"

namespace AsynchronousGA
{

const int kBufferSize = 1024;
const int kBigBufferSize = 1024 * 1024 * 64;

class Preferences
{
public:
    Preferences();
    ~Preferences();

#ifdef _WIN32
    int ReadPreferences(const wchar_t *filename);
#else
    int ReadPreferences(const char *filename);
#endif


    DataFile params;
    int genomeLength;
    int populationSize;
    int maxReproductions;
    double gaussianMutationChance;
    double frameShiftMutationChance;
    double duplicationMutationChance;
    double crossoverChance;
    int parentsToKeep;
    int saveBestEvery;
    int savePopEvery;
    int outputStatsEvery;
    bool onlyKeepBestGenome;
    bool onlyKeepBestPopulation;
    int improvementReproductions;
    double improvementThreshold;
    bool multipleGaussian;
    SelectionType parentSelection;
    CrossoverType crossoverType;
    char startingPopulation[kBufferSize];
    RANDOM_SEED_TYPE randomSeed;
    bool randomiseModel;
    double watchDogTimerLimit;
    int outputPopulationSize;
    double gamma;
    bool circularMutation;
    bool bounceMutation;
    ResizeControl resizeControl;
    int tcpPort;

};

std::ostream& operator<<(std::ostream &out, const Preferences &g);

}

#endif // PREFERENCES_H
