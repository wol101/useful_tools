/*
 *  Preferences.cpp
 *  AsynchronousGA
 *
 *  Created by Bill Sellers on 10/11/2010.
 *  Copyright 2010 Bill Sellers. All rights reserved.
 *
 */

#include "Preferences.h"

#include <unistd.h>
#include <string.h>

using namespace std::string_literals;

Preferences *g_prefs;

Preferences::Preferences()
{
}

int Preferences::ReadPreferences(const std::string &filename)
{

    std::string paramsBuffer;

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
        if (params.RetrieveParameter("startingPopulation", &startingPopulation)) throw __LINE__;
        if (params.RetrieveParameter("outputPopulationSize", &outputPopulationSize)) throw __LINE__;
        if (params.RetrieveParameter("watchDogTimerLimit", &watchDogTimerLimit)) throw __LINE__;

        if (params.RetrieveParameter("parentSelection", &paramsBuffer)) throw __LINE__;
        if (paramsBuffer == "UniformSelection"s) parentSelection = UniformSelection;
        else if (paramsBuffer == "RankBasedSelection"s) parentSelection = RankBasedSelection;
        else if (paramsBuffer == "SqrtBasedSelection"s) parentSelection = SqrtBasedSelection;
        else if (paramsBuffer == "GammaBasedSelection"s) parentSelection = GammaBasedSelection;
        else throw __LINE__;

        if (parentSelection == GammaBasedSelection)
            if (params.RetrieveParameter("gamma", &gamma)) throw __LINE__;

        if (params.RetrieveParameter("crossoverType", &paramsBuffer)) throw __LINE__;
        else if (paramsBuffer == "OnePoint"s) crossoverType = OnePoint;
        else if (paramsBuffer == "Average"s) crossoverType = Average;
        else throw __LINE__;

        // optional parameters
        params.RetrieveParameter("circularMutation", &circularMutation);
        params.RetrieveParameter("bounceMutation", &bounceMutation);
        params.RetrieveParameter("tcpPort", &tcpPort);

        if (!params.RetrieveParameter("resizeControl", &paramsBuffer))
        {
            if (paramsBuffer == "DuplicateResize"s) resizeControl = DuplicateResize;
            else if (paramsBuffer == "RandomiseResize"s) resizeControl = RandomiseResize;
            else if (paramsBuffer == "MutateResize"s) resizeControl = MutateResize;
            else throw __LINE__;
        }
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


