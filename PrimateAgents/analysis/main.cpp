/*
 *  main.cpp
 *  Analysis
 *
 *  Created by Bill Sellers on Sun Mar 13 2005.
 *  Copyright (c) 2005 Bill Sellers. All rights reserved.
 *
 */

using namespace std;

#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#include <stdio.h>
#include <string.h>

#include <gsl/gsl_statistics.h>
#include <gsl/gsl_sort.h>

#include "DataFile.h"
#include "MonthlySummary.h"

#define OUTPUT_PARAMETER(p) \
summaryFile << #p "\n";\
for (month = startMonth; month <= endMonth; month++)\
{\
    j = month - 1;\
    for (i = 0; i < nValues; i++)\
    {\
        values[i] = yearlySummaryList[i]->m_MonthlySummary[j].p;\
    }\
    gsl_sort(values, stride, nValues);\
    median = gsl_stats_median_from_sorted_data(values, stride, nValues);\
    q975 = gsl_stats_quantile_from_sorted_data (values, stride, nValues, 0.975);\
    q025 = gsl_stats_quantile_from_sorted_data (values, stride, nValues, 0.025);\
    summaryFile << median << "\t" << q025 << "\t" << q975 << "\n";\
}

#define YEARMEAN(p,n) \
(( yearlySummaryList[i]->m_MonthlySummary[0].p + \
   yearlySummaryList[i]->m_MonthlySummary[1].p + \
   yearlySummaryList[i]->m_MonthlySummary[2].p + \
   yearlySummaryList[i]->m_MonthlySummary[3].p + \
   yearlySummaryList[i]->m_MonthlySummary[4].p + \
   yearlySummaryList[i]->m_MonthlySummary[5].p + \
   yearlySummaryList[i]->m_MonthlySummary[6].p + \
   yearlySummaryList[i]->m_MonthlySummary[7].p + \
   yearlySummaryList[i]->m_MonthlySummary[8].p + \
   yearlySummaryList[i]->m_MonthlySummary[9].p + \
   yearlySummaryList[i]->m_MonthlySummary[10].p + \
   yearlySummaryList[i]->m_MonthlySummary[11].p ) / n)
  
int main (int argc, const char * argv[])
{
    int i, j;
    string agentLogFilename("AgentLog.txt");
    string habitatTypeLogFilename("HabitatTypeLog.txt");
    string gridSquareLogFilename("GridSquareLog.txt");
    string monteCarloLogFilename("MonteCarloLog.txt");
    string rootDirectoryName;
    DataFile agentLog, habitatTypeLog, gridSquareLog, monteCarloLog;
    YearlySummary *yearlySummary;
    vector<YearlySummary *> yearlySummaryList;
    double *values;
    int nValues;
    int startMonth = 1;
    int endMonth = 12;
    int nMonths;
    int stride;
    int month;
    double median, q975, q025;
    ofstream summaryFile("MonthlySummary.txt");
    ofstream yearMeanMCValuesFile("YearMeanMCValues.txt");
    ofstream monthlyMCValuesFile("MonthlyMCValues.txt");
    
    // parse the command line
    try
    {
        for (i = 1; i < argc; i++)
        {
            // do something with arguments

            if (strcmp(argv[i], "-d") == 0)
            {
                i++;
                if (i >= argc) throw(1);
                rootDirectoryName = argv[i];
            }

            else if (strcmp(argv[i], "-s") == 0)
            {
                i++;
                if (i >= argc) throw(1);
                startMonth = strtol(argv[i], 0, 10);
            }

            else if (strcmp(argv[i], "-e") == 0)
            {
                i++;
                if (i >= argc) throw(1);
                endMonth = strtol(argv[i], 0, 10);
            }

            else
            {
                throw(1);
            }
            
        }
    }
    
    catch (int e)
    {
        cerr << "\nanalysis program\n";
        cerr << "Build " << __DATE__ << " " << __TIME__ << "\n";
        cerr << "Usage: monte_carlo inputXMLFile [options]\n\n";
        cerr << "-d rootDirectoryName\n";
        cerr << "-s startMonth (default 1)\n";
        cerr << "-e endMonth (default 12)\n";
        return 1;
    }
    
    nMonths = endMonth - startMonth + 1;

    if (rootDirectoryName.size() > 0)
    {
        rootDirectoryName.append("/");
        agentLogFilename.insert(0, rootDirectoryName);
        habitatTypeLogFilename.insert(0, rootDirectoryName);
        gridSquareLogFilename.insert(0, rootDirectoryName);
        monteCarloLogFilename.insert(0, rootDirectoryName);
    }

    agentLog.SetExitOnError(true);
    habitatTypeLog.SetExitOnError(true);
    gridSquareLog.SetExitOnError(true);
    monteCarloLog.SetExitOnError(true);

    cerr << "Reading " << agentLogFilename << "\n";
    agentLog.ReadFile(agentLogFilename.c_str());
    cerr << "Reading " << habitatTypeLogFilename << "\n";
    habitatTypeLog.ReadFile(habitatTypeLogFilename.c_str());
    cerr << "Reading " << gridSquareLogFilename << "\n";
    gridSquareLog.ReadFile(gridSquareLogFilename.c_str());
    cerr << "Reading " << monteCarloLogFilename << "\n";
    monteCarloLog.ReadFile(monteCarloLogFilename.c_str());

    i = 0;
    do
    {
        yearlySummary = YearlySummary::ReadFiles(agentLog, habitatTypeLog,
                                                 gridSquareLog, monteCarloLog);
        if (yearlySummary == 0) break;
        yearlySummaryList.push_back(yearlySummary);
        if (i % 100 == 0)
            cerr << "Parsed run " << i << "\n";
        i++;
    } while (yearlySummary);

    nValues = yearlySummaryList.size();
    values = new double[nValues];
    stride = 1;

    summaryFile << "Monthly Summaries\n";
    OUTPUT_PARAMETER(m_MeanDistanceTravelled);
    OUTPUT_PARAMETER(m_MeanTimeForaging);
    OUTPUT_PARAMETER(m_MeanTimeResting);
    OUTPUT_PARAMETER(m_MeanTimeMoving);
    OUTPUT_PARAMETER(m_MeanTimeSocialising);
    OUTPUT_PARAMETER(m_MeanEnergyIntake);
    OUTPUT_PARAMETER(m_MeanNormalisedForagingScore);
    OUTPUT_PARAMETER(m_MeanNormalisedWaterScore);
    OUTPUT_PARAMETER(m_MeanNormalisedSocialScore);

    OUTPUT_PARAMETER(m_SquaresEntered);

    OUTPUT_PARAMETER(m_ACACIA_WOODLAND);
    OUTPUT_PARAMETER(m_BURNT_ACACIA_WOODLAND);
    OUTPUT_PARAMETER(m_BURNT_FYNBOS);
    OUTPUT_PARAMETER(m_CLIMAX_FYNBOS);
    OUTPUT_PARAMETER(m_GRASSLAND);
    OUTPUT_PARAMETER(m_VLEI);

    yearMeanMCValuesFile << "MC_PROPORTIONTOWINVOTE\t" <<
        "MC_SEARCHRADIUS\t" <<
        "MC_RELATIVEFOODIMPORTANCE\t" <<
        "MC_RELATIVESOCIALIMPORTANCE\t" <<
        "MC_RELATIVEDRINKINGIMPORTANCE\t" <<
        "MC_FORAGINGMOVETHRESHOLD\t" <<
        "MC_SOCIALISINGMOVETHRESHOLD\t" <<
        "MC_RESTINGMOVETHRESHOLD\t" <<
        "MC_RESTSOCIALRISKTHRESHOLD\t";

    yearMeanMCValuesFile << "MeanDistanceTravelled\t" <<
        "MeanTimeForaging\t" <<
        "MeanTimeResting\t" <<
        "MeanTimeMoving\t" <<
        "MeanTimeSocialising\t" <<
        "MeanEnergyIntake\t" <<
        "MeanNormalisedForagingScore\t" <<
        "MeanNormalisedWaterScore\t" <<
        "MeanNormalisedSocialScore\t" <<
        "SquaresEntered\t" <<
        "ACACIA_WOODLAND\t" <<
        "BURNT_ACACIA_WOODLAND\t" <<
        "BURNT_FYNBOS\t" <<
        "CLIMAX_FYNBOS\t" <<
        "GRASSLAND\t" <<
        "VLEI\n";

    for (i = 0; i < nValues; i++)
    {
        yearMeanMCValuesFile << yearlySummaryList[i]->m_MC_PROPORTIONTOWINVOTE << "\t" <<
            yearlySummaryList[i]->m_MC_SEARCHRADIUS << "\t" <<
            yearlySummaryList[i]->m_MC_RELATIVEFOODIMPORTANCE << "\t" <<
            yearlySummaryList[i]->m_MC_RELATIVESOCIALIMPORTANCE << "\t" <<
            yearlySummaryList[i]->m_MC_RELATIVEDRINKINGIMPORTANCE << "\t" <<
            yearlySummaryList[i]->m_MC_FORAGINGMOVETHRESHOLD << "\t" <<
            yearlySummaryList[i]->m_MC_SOCIALISINGMOVETHRESHOLD << "\t" <<
            yearlySummaryList[i]->m_MC_RESTINGMOVETHRESHOLD << "\t" <<
            yearlySummaryList[i]->m_MC_RESTSOCIALRISKTHRESHOLD << "\t";

        yearMeanMCValuesFile << YEARMEAN(m_MeanDistanceTravelled, nMonths) << "\t" <<
            YEARMEAN(m_MeanTimeForaging, nMonths) << "\t" <<
            YEARMEAN(m_MeanTimeResting, nMonths) << "\t" <<
            YEARMEAN(m_MeanTimeMoving, nMonths) << "\t" <<
            YEARMEAN(m_MeanTimeSocialising, nMonths) << "\t" <<
            YEARMEAN(m_MeanEnergyIntake, nMonths) << "\t" <<
            YEARMEAN(m_MeanNormalisedForagingScore, nMonths) << "\t" <<
            YEARMEAN(m_MeanNormalisedWaterScore, nMonths) << "\t" <<
            YEARMEAN(m_MeanNormalisedSocialScore, nMonths) << "\t" <<
            YEARMEAN(m_SquaresEntered, nMonths) << "\t" <<
            YEARMEAN(m_ACACIA_WOODLAND, nMonths) << "\t" <<
            YEARMEAN(m_BURNT_ACACIA_WOODLAND, nMonths) << "\t" <<
            YEARMEAN(m_BURNT_FYNBOS, nMonths) << "\t" <<
            YEARMEAN(m_CLIMAX_FYNBOS, nMonths) << "\t" <<
            YEARMEAN(m_GRASSLAND, nMonths) << "\t" <<
            YEARMEAN(m_VLEI, nMonths) << "\n";
    }

    monthlyMCValuesFile << "MC_PROPORTIONTOWINVOTE\t" <<
        "MC_SEARCHRADIUS\t" <<
        "MC_RELATIVEFOODIMPORTANCE\t" <<
        "MC_RELATIVESOCIALIMPORTANCE\t" <<
        "MC_RELATIVEDRINKINGIMPORTANCE\t" <<
        "MC_FORAGINGMOVETHRESHOLD\t" <<
        "MC_SOCIALISINGMOVETHRESHOLD\t" <<
        "MC_RESTINGMOVETHRESHOLD\t" <<
        "MC_RESTSOCIALRISKTHRESHOLD\t";

    for (j = startMonth; j <= endMonth; j++)
    {
        monthlyMCValuesFile << "MeanDistanceTravelled" << j << "\t" <<
            "MeanTimeForaging" << j << "\t" <<
            "MeanTimeResting" << j << "\t" <<
            "MeanTimeMoving" << j << "\t" <<
            "MeanTimeSocialising" << j << "\t" <<
            "MeanEnergyIntake" << j << "\t" <<
            "MeanNormalisedForagingScore" << j << "\t" <<
            "MeanNormalisedWaterScore" << j << "\t" <<
            "MeanNormalisedSocialScore" << j << "\t" <<
            "SquaresEntered" << j << "\t" <<
            "ACACIA_WOODLAND" << j << "\t" <<
            "BURNT_ACACIA_WOODLAND" << j << "\t" <<
            "BURNT_FYNBOS" << j << "\t" <<
            "CLIMAX_FYNBOS" << j << "\t" <<
            "GRASSLAND" << j << "\t" <<
            "VLEI" << j;
        if (j < endMonth) monthlyMCValuesFile << "\t";
        else monthlyMCValuesFile << "\n";
    }

    for (i = 0; i < nValues; i++)
    {
        monthlyMCValuesFile << yearlySummaryList[i]->m_MC_PROPORTIONTOWINVOTE << "\t" <<
        yearlySummaryList[i]->m_MC_SEARCHRADIUS << "\t" <<
        yearlySummaryList[i]->m_MC_RELATIVEFOODIMPORTANCE << "\t" <<
        yearlySummaryList[i]->m_MC_RELATIVESOCIALIMPORTANCE << "\t" <<
        yearlySummaryList[i]->m_MC_RELATIVEDRINKINGIMPORTANCE << "\t" <<
        yearlySummaryList[i]->m_MC_FORAGINGMOVETHRESHOLD << "\t" <<
        yearlySummaryList[i]->m_MC_SOCIALISINGMOVETHRESHOLD << "\t" <<
        yearlySummaryList[i]->m_MC_RESTINGMOVETHRESHOLD << "\t" <<
        yearlySummaryList[i]->m_MC_RESTSOCIALRISKTHRESHOLD << "\t";

        for (j = startMonth - 1; j < endMonth; j++)
        {
            monthlyMCValuesFile << yearlySummaryList[i]->m_MonthlySummary[j].m_MeanDistanceTravelled << "\t" <<
                yearlySummaryList[i]->m_MonthlySummary[j].m_MeanTimeForaging << "\t" <<
                yearlySummaryList[i]->m_MonthlySummary[j].m_MeanTimeResting << "\t" <<
                yearlySummaryList[i]->m_MonthlySummary[j].m_MeanTimeMoving << "\t" <<
                yearlySummaryList[i]->m_MonthlySummary[j].m_MeanTimeSocialising << "\t" <<
                yearlySummaryList[i]->m_MonthlySummary[j].m_MeanEnergyIntake << "\t" <<
                yearlySummaryList[i]->m_MonthlySummary[j].m_MeanNormalisedForagingScore << "\t" <<
                yearlySummaryList[i]->m_MonthlySummary[j].m_MeanNormalisedWaterScore << "\t" <<
                yearlySummaryList[i]->m_MonthlySummary[j].m_MeanNormalisedSocialScore << "\t" <<
                yearlySummaryList[i]->m_MonthlySummary[j].m_SquaresEntered << "\t" <<
                yearlySummaryList[i]->m_MonthlySummary[j].m_ACACIA_WOODLAND << "\t" <<
                yearlySummaryList[i]->m_MonthlySummary[j].m_BURNT_ACACIA_WOODLAND << "\t" <<
                yearlySummaryList[i]->m_MonthlySummary[j].m_BURNT_FYNBOS << "\t" <<
                yearlySummaryList[i]->m_MonthlySummary[j].m_CLIMAX_FYNBOS << "\t" <<
                yearlySummaryList[i]->m_MonthlySummary[j].m_GRASSLAND << "\t" <<
                yearlySummaryList[i]->m_MonthlySummary[j].m_VLEI;
            if (j < endMonth - 1) monthlyMCValuesFile << "\t";
            else monthlyMCValuesFile << "\n";
        }
    }
    
    
    for (vector<YearlySummary *>::size_type index = 0; index < yearlySummaryList.size(); index++) delete yearlySummaryList[index];
    return 0;
}

