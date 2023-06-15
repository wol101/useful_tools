/*
 *  MonthlySummary.cpp
 *  Analysis
 *
 *  Created by Bill Sellers on Sun Mar 13 2005.
 *  Copyright (c) 2005 Bill Sellers. All rights reserved.
 *
 */

using namespace std;

#include <stdlib.h>

#include <iostream>

#include "MonthlySummary.h"

// read in the yearly summary
// assumes already at the data
YearlySummary *YearlySummary::ReadFiles(DataFile &agentLog,
                          DataFile &habitatTypeLog,
                          DataFile &gridSquareLog,
                          DataFile &monteCarloLog)
{
    char buffer[1024];
    char *ptrs[1024];
    int count;
    int counts[12];
    static bool firstTime = true;
    YearlySummary *yearlySummary;
    int month;

    if (firstTime)
    {
        firstTime = false;
        agentLog.FindParameter("MeanNormalisedSocialScore");
        habitatTypeLog.FindParameter("HabitatsUsed");
        gridSquareLog.FindParameter("SquaresEntered");
        monteCarloLog.FindParameter("MC_RESTSOCIALRISKTHRESHOLD"); 
    }
    
    // read the Monte Carlo data

    // read next line checking for end of file
    if (monteCarloLog.ReadNextLine(buffer, sizeof(buffer), true, '#'))
    {
    	cerr << "Finished parsing files\n";
        return 0;
    }

    count = DataFile::ReturnTokens(buffer, ptrs, sizeof(buffer));
    if (count != 9)
    {
        cerr << "Error parsing monte carlo log\n";
        exit(1);
    }
    
    yearlySummary = new YearlySummary();

    yearlySummary->m_MC_PROPORTIONTOWINVOTE = strtod(ptrs[0], 0);
    yearlySummary->m_MC_SEARCHRADIUS = strtod(ptrs[1], 0);
    yearlySummary->m_MC_RELATIVEFOODIMPORTANCE = strtod(ptrs[2], 0);
    yearlySummary->m_MC_RELATIVESOCIALIMPORTANCE = strtod(ptrs[3], 0);
    yearlySummary->m_MC_RELATIVEDRINKINGIMPORTANCE = strtod(ptrs[4], 0);
    yearlySummary->m_MC_FORAGINGMOVETHRESHOLD = strtod(ptrs[5], 0);
    yearlySummary->m_MC_SOCIALISINGMOVETHRESHOLD = strtod(ptrs[6], 0);
    yearlySummary->m_MC_RESTINGMOVETHRESHOLD = strtod(ptrs[7], 0);
    yearlySummary->m_MC_RESTSOCIALRISKTHRESHOLD = strtod(ptrs[8], 0);

    // read the agent data
    for (month = 0; month < 12; month++)
    {
        counts[month] = 0;
        yearlySummary->m_MonthlySummary[month].m_MeanDistanceTravelled = 0;
        yearlySummary->m_MonthlySummary[month].m_MeanTimeForaging = 0;
        yearlySummary->m_MonthlySummary[month].m_MeanTimeResting = 0;
        yearlySummary->m_MonthlySummary[month].m_MeanTimeMoving = 0;
        yearlySummary->m_MonthlySummary[month].m_MeanTimeSocialising = 0;
        yearlySummary->m_MonthlySummary[month].m_MeanEnergyIntake = 0;
        yearlySummary->m_MonthlySummary[month].m_MeanNormalisedForagingScore = 0;
        yearlySummary->m_MonthlySummary[month].m_MeanNormalisedWaterScore = 0;
        yearlySummary->m_MonthlySummary[month].m_MeanNormalisedSocialScore = 0;
    }
    do
    {
        if (agentLog.ReadNextLine(buffer, sizeof(buffer), true, '#')) break;
        if (strncmp(buffer, "Month", 3) == 0) break;
        count = DataFile::ReturnTokens(buffer, ptrs, sizeof(buffer));
        month = strtol(ptrs[0], 0, 10) - 1;
        counts[month]++;
        yearlySummary->m_MonthlySummary[month].m_MeanDistanceTravelled += strtod(ptrs[2], 0);
        yearlySummary->m_MonthlySummary[month].m_MeanTimeForaging += strtod(ptrs[3], 0);
        yearlySummary->m_MonthlySummary[month].m_MeanTimeResting += strtod(ptrs[4], 0);
        yearlySummary->m_MonthlySummary[month].m_MeanTimeMoving += strtod(ptrs[5], 0);
        yearlySummary->m_MonthlySummary[month].m_MeanTimeSocialising += strtod(ptrs[6], 0);
        yearlySummary->m_MonthlySummary[month].m_MeanEnergyIntake += strtod(ptrs[7], 0);
        yearlySummary->m_MonthlySummary[month].m_MeanNormalisedForagingScore += strtod(ptrs[8], 0);
        yearlySummary->m_MonthlySummary[month].m_MeanNormalisedWaterScore += strtod(ptrs[9], 0);
        yearlySummary->m_MonthlySummary[month].m_MeanNormalisedSocialScore += strtod(ptrs[10], 0);
    } while (true);
    for (month = 0; month < 12; month++)
    {
        if (counts[month])
        {
            yearlySummary->m_MonthlySummary[month].m_MeanDistanceTravelled /= counts[month];
            yearlySummary->m_MonthlySummary[month].m_MeanTimeForaging /= counts[month];
            yearlySummary->m_MonthlySummary[month].m_MeanTimeResting /= counts[month];
            yearlySummary->m_MonthlySummary[month].m_MeanTimeMoving /= counts[month];
            yearlySummary->m_MonthlySummary[month].m_MeanTimeSocialising /= counts[month];
            yearlySummary->m_MonthlySummary[month].m_MeanEnergyIntake /= counts[month];
            yearlySummary->m_MonthlySummary[month].m_MeanNormalisedForagingScore /= counts[month];
            yearlySummary->m_MonthlySummary[month].m_MeanNormalisedWaterScore /= counts[month];
            yearlySummary->m_MonthlySummary[month].m_MeanNormalisedSocialScore /= counts[month];
        }
    }

    // read the grid data
    for (month = 0; month < 12; month++)
    {
        counts[month] = 0;
        yearlySummary->m_MonthlySummary[month].m_SquaresEntered = 0;
    }
    do
    {
        if (gridSquareLog.ReadNextLine(buffer, sizeof(buffer), true, '#')) break;
        if (strncmp(buffer, "Month", 3) == 0) break;
        count = DataFile::ReturnTokens(buffer, ptrs, sizeof(buffer));
        month = strtol(ptrs[0], 0, 10) - 1;
        counts[month]++;
        yearlySummary->m_MonthlySummary[month].m_SquaresEntered += strtod(ptrs[2], 0);
    }  while (true);
    for (month = 0; month < 12; month++)
    {
        if (counts[month])
        {
            yearlySummary->m_MonthlySummary[month].m_SquaresEntered /= counts[month];
        }
    }

    // read the habitat data
    for (month = 0; month < 12; month++)
    {
        counts[month] = 0;
        yearlySummary->m_MonthlySummary[month].m_ACACIA_WOODLAND = 0;
        yearlySummary->m_MonthlySummary[month].m_BURNT_ACACIA_WOODLAND = 0;
        yearlySummary->m_MonthlySummary[month].m_BURNT_FYNBOS = 0;
        yearlySummary->m_MonthlySummary[month].m_CLIMAX_FYNBOS = 0;
        yearlySummary->m_MonthlySummary[month].m_GRASSLAND = 0;
        yearlySummary->m_MonthlySummary[month].m_VLEI = 0;
    }
    do
    {
        if (habitatTypeLog.ReadNextLine(buffer, sizeof(buffer), true, '#')) break;
        if (strncmp(buffer, "Month", 3) == 0) break;
        count = DataFile::ReturnTokens(buffer, ptrs, sizeof(buffer));
        month = strtol(ptrs[0], 0, 10) - 1;
        counts[month]++;
        yearlySummary->m_MonthlySummary[month].m_ACACIA_WOODLAND += strtod(ptrs[2], 0);
        yearlySummary->m_MonthlySummary[month].m_BURNT_ACACIA_WOODLAND += strtod(ptrs[3], 0);
        yearlySummary->m_MonthlySummary[month].m_BURNT_FYNBOS += strtod(ptrs[4], 0);
        yearlySummary->m_MonthlySummary[month].m_CLIMAX_FYNBOS += strtod(ptrs[5], 0);
        yearlySummary->m_MonthlySummary[month].m_GRASSLAND += strtod(ptrs[6], 0);
        yearlySummary->m_MonthlySummary[month].m_VLEI += strtod(ptrs[7], 0);
    }  while (true);
    for (month = 0; month < 12; month++)
    {
        if (counts[month])
        {
            yearlySummary->m_MonthlySummary[month].m_ACACIA_WOODLAND /= counts[month];
            yearlySummary->m_MonthlySummary[month].m_BURNT_ACACIA_WOODLAND /= counts[month];
            yearlySummary->m_MonthlySummary[month].m_BURNT_FYNBOS /= counts[month];
            yearlySummary->m_MonthlySummary[month].m_CLIMAX_FYNBOS /= counts[month];
            yearlySummary->m_MonthlySummary[month].m_GRASSLAND /= counts[month];
            yearlySummary->m_MonthlySummary[month].m_VLEI /= counts[month];
        }
    }

    return yearlySummary;
}
    






        
    






