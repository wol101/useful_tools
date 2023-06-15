/*
 *  Preferences.cpp
 *  AsynchronousGA
 *
 *  Created by Bill Sellers on 10/11/2010.
 *  Copyright 2010 Bill Sellers. All rights reserved.
 *
 */

#include <unistd.h>
#include <string.h>

#include "Preferences.h"
#include "DataFile.h"

namespace AsynchronousGA
{

Preferences::Preferences()
{
    parentSelection = RankBasedSelection;
    randomSeed = (RANDOM_SEED_TYPE)time(0) * (RANDOM_SEED_TYPE)getpid();
    randomiseModel = false;
    watchDogTimerLimit = 60 * 5;
    gamma = 0.5;
    circularMutation = false;
    bounceMutation = true;
    resizeControl = MutateResize;
    tcpPort = 8086;
}

Preferences::~Preferences()
{
}

#ifdef _WIN32
int Preferences::ReadPreferences(const wchar_t *filename)
#else
int Preferences::ReadPreferences(const char *filename)
#endif
{

    char paramsBuffer[kBufferSize];

    try
    {
        if (params.ReadFile(filename)) throw __LINE__;

        // essential parameters
        if (params.RetrieveParameter("genomeLength", &genomeLength)) throw __LINE__;
        if (params.RetrieveParameter("populationSize", &populationSize)) throw __LINE__;
        if (params.RetrieveParameter("maxReproductions", &maxReproductions)) throw __LINE__;
        if (params.RetrieveParameter("gaussianMutationChance", &gaussianMutationChance)) throw __LINE__;
        if (params.RetrieveParameter("frameShiftMutationChance", &frameShiftMutationChance)) throw __LINE__;
        if (params.RetrieveParameter("duplicationMutationChance", &duplicationMutationChance)) throw __LINE__;
        if (params.RetrieveParameter("crossoverChance", &crossoverChance)) throw __LINE__;
        if (params.RetrieveParameter("parentsToKeep", &parentsToKeep)) throw __LINE__;
        if (params.RetrieveParameter("saveBestEvery", &saveBestEvery)) throw __LINE__;
        if (params.RetrieveParameter("savePopEvery", &savePopEvery)) throw __LINE__;
        if (params.RetrieveParameter("outputStatsEvery", &outputStatsEvery)) throw __LINE__;
        if (params.RetrieveParameter("onlyKeepBestGenome", &onlyKeepBestGenome)) throw __LINE__;
        if (params.RetrieveParameter("onlyKeepBestPopulation", &onlyKeepBestPopulation)) throw __LINE__;
        if (params.RetrieveParameter("improvementReproductions", &improvementReproductions)) throw __LINE__;
        if (params.RetrieveParameter("improvementThreshold", &improvementThreshold)) throw __LINE__;
        if (params.RetrieveParameter("multipleGaussian", &multipleGaussian)) throw __LINE__;
        if (params.RetrieveParameter("randomiseModel", &randomiseModel)) throw __LINE__;
        if (params.RetrieveParameter("startingPopulation", startingPopulation, sizeof(startingPopulation))) throw __LINE__;
        if (params.RetrieveParameter("outputPopulationSize", &outputPopulationSize)) throw __LINE__;
        if (params.RetrieveParameter("watchDogTimerLimit", &watchDogTimerLimit)) throw __LINE__;

        if (params.RetrieveParameter("parentSelection", paramsBuffer, sizeof(paramsBuffer))) throw __LINE__;
        if (strcmp(paramsBuffer, "UniformSelection") == 0) parentSelection = UniformSelection;
        if (strcmp(paramsBuffer, "RankBasedSelection") == 0) parentSelection = RankBasedSelection;
        if (strcmp(paramsBuffer, "SqrtBasedSelection") == 0) parentSelection = SqrtBasedSelection;
        if (strcmp(paramsBuffer, "GammaBasedSelection") == 0) parentSelection = GammaBasedSelection;

        if (parentSelection == GammaBasedSelection)
            if (params.RetrieveParameter("gamma", &gamma)) throw __LINE__;

        if (params.RetrieveParameter("crossoverType", paramsBuffer, sizeof(paramsBuffer))) throw __LINE__;
        if (strcmp(paramsBuffer, "OnePoint") == 0) crossoverType = OnePoint;
        if (strcmp(paramsBuffer, "Average") == 0) crossoverType = Average;

        // optional parameters
        params.RetrieveParameter("randomSeed", &randomSeed);
        params.RetrieveParameter("circularMutation", &circularMutation);
        params.RetrieveParameter("bounceMutation", &bounceMutation);
        params.RetrieveParameter("tcpPort", &tcpPort);

        params.RetrieveParameter("crossoverType", paramsBuffer, sizeof(paramsBuffer));
        if (strcmp(paramsBuffer, "DuplicateResize") == 0) resizeControl = DuplicateResize;
        if (strcmp(paramsBuffer, "RandomiseResize") == 0) resizeControl = RandomiseResize;
        if (strcmp(paramsBuffer, "MutateResize") == 0) resizeControl = MutateResize;

    }

    catch (int e)
    {
        std::cerr << "Preferences::ReadPreferences Parameter file error on line " << e << "\n";
        return e;
    }

    return 0;
}

// output to a stream
std::ostream& operator<<(std::ostream &out, const Preferences &g)
{

    out << "genomeLength " << g.genomeLength << "\n";
    out << "populationSize " << g.populationSize << "\n";
    out << "maxReproductions " << g.maxReproductions << "\n";
    out << "gaussianMutationChance " << g.gaussianMutationChance << "\n";
    out << "frameShiftMutationChance " << g.frameShiftMutationChance << "\n";
    out << "duplicationMutationChance " << g.duplicationMutationChance << "\n";
    out << "crossoverChance " << g.crossoverChance << "\n";
    out << "parentsToKeep " << g.parentsToKeep << "\n";
    out << "saveBestEvery " << g.saveBestEvery << "\n";
    out << "savePopEvery " << g.savePopEvery << "\n";
    out << "outputStatsEvery " << g.outputStatsEvery << "\n";
    out << "onlyKeepBestGenome " << g.onlyKeepBestGenome << "\n";
    out << "onlyKeepBestPopulation " << g.onlyKeepBestPopulation << "\n";
    out << "randomSeed " << g.randomSeed << "\n";
    out << "improvementReproductions " << g.improvementReproductions << "\n";
    out << "improvementThreshold " << g.improvementThreshold << "\n";
    out << "multipleGaussian " << g.multipleGaussian << "\n";
    out << "randomiseModel " << g.randomiseModel << "\n";
    out << "startingPopulation \"" << g.startingPopulation << "\"\n";
    out << "outputPopulationSize " << g.outputPopulationSize << "\n";
    out << "watchDogTimerLimit " << g.watchDogTimerLimit << "\n";
    out << "circularMutation " << g.circularMutation << "\n";
    out << "bounceMutation " << g.bounceMutation << "\n";
    out << "tcpPort " << g.tcpPort << "\n";

    switch (g.parentSelection)
    {
    case UniformSelection:
        out << "parentSelection " << "UniformSelection\n";
        break;
    case RankBasedSelection:
        out << "parentSelection " << "RankBasedSelection\n";
        break;
    case SqrtBasedSelection:
        out << "parentSelection " << "SqrtBasedSelection\n";
        break;
    case GammaBasedSelection:
        out << "parentSelection " << "GammaBasedSelection\n";
        out << "gamma " << g.gamma << "\n";
        break;
    }

    switch (g.crossoverType)
    {
    case OnePoint:
        out << "crossoverType " << "OnePoint\n";
        break;
    case Average:
        out << "crossoverType " << "Average\n";
        break;
    }

    switch (g.resizeControl)
    {
    case DuplicateResize:
        out << "resizeControl " << "DuplicateResize\n";
        break;
    case RandomiseResize:
        out << "resizeControl " << "RandomiseResize\n";
        break;
    case MutateResize:
        out << "resizeControl " << "MutateResize\n";
        break;
    }

    return out;
}

}
