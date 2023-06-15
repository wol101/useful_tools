/*
 *  MonthlySummary.h
 *  Analysis
 *
 *  Created by Bill Sellers on Sun Mar 13 2005.
 *  Copyright (c) 2005 Bill Sellers. All rights reserved.
 *
 */

#ifndef MonthlySummary_h
#define MonthlySummary_h

#include "DataFile.h"

struct MonthlySummary
{
    // agent data
    double m_MeanDistanceTravelled;
    double m_MeanTimeForaging;
    double m_MeanTimeResting;
    double m_MeanTimeMoving;
    double m_MeanTimeSocialising;
    double m_MeanEnergyIntake;
    double m_MeanNormalisedForagingScore;
    double m_MeanNormalisedWaterScore;
    double m_MeanNormalisedSocialScore;

    // grid square data
    double m_SquaresEntered;

    // habitat data (daytime seconds)
    double m_ACACIA_WOODLAND;
    double m_BURNT_ACACIA_WOODLAND;
    double m_BURNT_FYNBOS;
    double m_CLIMAX_FYNBOS;
    double m_GRASSLAND;
    double m_VLEI;
                      
};

struct YearlySummary
{
    MonthlySummary m_MonthlySummary[12];

    // Monte-Carlo Parameters

    double m_MC_PROPORTIONTOWINVOTE;
    double m_MC_SEARCHRADIUS;
    double m_MC_RELATIVEFOODIMPORTANCE;
    double m_MC_RELATIVESOCIALIMPORTANCE;
    double m_MC_RELATIVEDRINKINGIMPORTANCE;
    double m_MC_FORAGINGMOVETHRESHOLD;
    double m_MC_SOCIALISINGMOVETHRESHOLD;
    double m_MC_RESTINGMOVETHRESHOLD;
    double m_MC_RESTSOCIALRISKTHRESHOLD;

    static YearlySummary* YearlySummary::ReadFiles(DataFile &agentLog,
                                                   DataFile &habitatTypeLog,
                                                   DataFile &gridSquareLog,
                                                   DataFile &monteCarloLog);
};

#endif




