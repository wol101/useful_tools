/*
 *  GridSquare.cpp
 *  Primate Agent
 *
 *  Created by Bill Sellers on Wed Sep 22 2004.
 *  Copyright (c) 2004 Bill Sellers. All rights reserved.
 *
 */

using namespace std;

#include <stdlib.h>
#include <vector>
#include <iostream>
#include <math.h>
#include <float.h>
#include <assert.h>

#ifdef USE_OPENGL
#include <glut.h>
#include "GLShapes.h"
#endif

#include "GridSquare.h"
#include "DataFile.h"
#include "Simulation.h"
#include "Environment.h"
#include "HabitatType.h"
#include "DayLength.h"
#include "Random.h"

#define THROWIFZERO(a) if ((a) == 0) throw 0
#define THROWIF(a) if ((a) != 0) throw 0

extern Simulation g_Simulation;

// construct from an XML node
GridSquare::GridSquare(xmlNodePtr cur)
{
    xmlChar *buf;
    char *ptrs[256];
    int i, c;
    string s;

    m_YGrid = 0;
    m_XGrid = 0;
    m_XCentre = 0;
    m_YCentre = 0;
    m_MeanGroundSlope = 0;
    m_SleepingSite = false;
    m_CliffRefuge = false;
    m_OtherRefuge = false;
    m_WaterPresent = false;
    m_Risk = 0;
    m_Productivity = 0;
    m_MaxResource = 0;
    m_CurrentResource = 1e10;
    m_ForagingRateFactor = 0;
    m_MoveForagingRatePenalty = 0;
    m_RecoveryRate = 0;
    m_MeanEnergyValue = 0;
    m_MeanEnergyValue = 0;
    m_ForagingModel = HabitatType::FixedRate;
    m_FullResource = 100;
    
    ResetTotals();

#ifdef USE_OPENGL
    m_HiLite = false;
    m_RHiLite = 1.0;
    m_GHiLite = 1.0;
    m_BHiLite = 1.0;
#endif
    
    try
    {
        THROWIFZERO(buf = xmlGetProp(cur, (const xmlChar *)"YGRID"));
        m_YGrid = strtol((char *)buf, 0, 10);
        xmlFree(buf);

        THROWIFZERO(buf = xmlGetProp(cur, (const xmlChar *)"XGRID"));
        m_XGrid = strtol((char *)buf, 0, 10);
        xmlFree(buf);

        THROWIFZERO(buf = xmlGetProp(cur, (const xmlChar *)"MEAN_GROUND_SLOPE"));
        m_MeanGroundSlope = strtod((char *)buf, 0);
        xmlFree(buf);

        THROWIFZERO(buf = xmlGetProp(cur, (const xmlChar *)"HABITAT_TYPE"));
        c = DataFile::ReturnTokens((char *)buf, ptrs, 256);
        for (i = 0; i < c; i++)
        {
            s = ptrs[i];
            m_HabitatTypes.push_back(s);
        }

        if ((buf = xmlGetProp(cur, (const xmlChar *)"SLEEPING_SITE")) == 0)
        {
            m_SleepingSite = false;
        }
        else
        {
            m_SleepingSite = (bool)strtol((char *)buf, 0, 10);
            xmlFree(buf);
        }


        if ((buf = xmlGetProp(cur, (const xmlChar *)"CLIFF_REFUGE")) == 0)
        {
            m_CliffRefuge = false;
        }
        else
        {
            m_CliffRefuge = (bool)strtol((char *)buf, 0, 10);
            xmlFree(buf);
        }


        if ((buf = xmlGetProp(cur, (const xmlChar *)"OTHER_REFUGE")) == 0)
        {
            m_OtherRefuge = false;
        }
        else
        {
            m_OtherRefuge = (bool)strtol((char *)buf, 0, 10);
            xmlFree(buf);
        }


        if ((buf = xmlGetProp(cur, (const xmlChar *)"WATER_PRESENT")) == 0)
        {
            m_WaterPresent = false;
        }
        else
        {
            m_WaterPresent = (bool)strtol((char *)buf, 0, 10);
            xmlFree(buf);
        }
    }

    catch (int e)
    {
        cerr << "Error parsing GRID_SQUARE\n";
        throw 0;
    }

#ifdef USE_OPENGL
    // set the colour of the square
    m_R = m_G = m_B = 0;
    GLfloat r, g, b;
    unsigned int index;
    m_FullResource = 0;
    for (index = 0; index < m_HabitatTypes.size(); index++)
    {
        (*g_Simulation.GetEnvironment()->GetHabitatTypeList())[m_HabitatTypes[index]]->GetColour(&r, &g, &b);
        m_R += r;
        m_G += g;
        m_B += b;
        m_FullResource += (*g_Simulation.GetEnvironment()->GetHabitatTypeList())[m_HabitatTypes[index]]->GetFullResource();
    }
    m_R /= m_HabitatTypes.size();
    m_G /= m_HabitatTypes.size();
    m_B /= m_HabitatTypes.size();
    m_FullResource /= m_HabitatTypes.size();
    double area = g_Simulation.GetEnvironment()->GetGrid()->GetXSize() * g_Simulation.GetEnvironment()->GetGrid()->GetYSize();
    m_FullResource *= area;
#endif
}

double GridSquare::GetDistanceFrom(double x, double y)
{
    double dx = m_XCentre - x;
    double dy = m_YCentre - y;
    return sqrt(dx * dx + dy * dy);
}

double GridSquare::GetDirectionFrom(double x, double y)
{
    double dx = m_XCentre - x;
    double dy = m_YCentre - y;
    double angle = atan2(dy, dx);
    return angle;
}

int GridSquare::GetSegmentalDirectionFrom(double x, double y)
{
    double dx = m_XCentre - x;
    double dy = m_YCentre - y;
    double angle = atan2(dy, dx);
    if (angle < -(7.0/8.0) * M_PI) return 4;
    if (angle < -(5.0/8.0) * M_PI) return 5;
    if (angle < -(3.0/8.0) * M_PI) return 6;
    if (angle < -(1.0/8.0) * M_PI) return 7;
    if (angle < (1.0/8.0) * M_PI) return 0;
    if (angle < (3.0/8.0) * M_PI) return 1;
    if (angle < (5.0/8.0) * M_PI) return 2;
    return 3;
}

// note returns energy value of foraging
double GridSquare::Forage(double duration, bool move)
{
    double foragingFound;
    if (move) foragingFound = GetForagingRate() * m_MoveForagingRatePenalty * duration;
    else foragingFound = GetForagingRate() * duration;

    m_CurrentResource -= foragingFound;
    if (m_CurrentResource < 0)
    {
        foragingFound += m_CurrentResource;
        m_CurrentResource = 0;
    } 

    return foragingFound * m_MeanEnergyValue;
};

// note returns grams / second
double GridSquare::GetForagingRate()
{
    if (m_ForagingModel == HabitatType::Proportion)
        return m_ForagingRateFactor * m_CurrentResource;
    else if (m_ForagingModel == HabitatType::FixedRate)
        return m_ForagingRateFactor;
    else assert(false);        
}


void GridSquare::DoDailyRecovery()
{
    // this is also a good place to update habitat dependent factors

    int month = g_Simulation.GetMonth();
    unsigned int index;
    double visibility = 0;
    double maxResource = 0;
    double foragingRateFactor = 0;
    double moveForagingRatePenalty = 0;
    double recoveryRate = 0;
    double meanEnergyValue = 0;
    HabitatType *habitatType;
    double area = g_Simulation.GetEnvironment()->GetGrid()->GetXSize() * g_Simulation.GetEnvironment()->GetGrid()->GetYSize();
    for (index = 0; index < m_HabitatTypes.size(); index++)
    {
        habitatType = (*g_Simulation.GetEnvironment()->GetHabitatTypeList())[m_HabitatTypes[index]];
        visibility += habitatType->GetVisibility(month);
        maxResource += habitatType->GetProductivity(month) * area;
        foragingRateFactor += habitatType->GetForagingRateFactor(month);
        moveForagingRatePenalty += habitatType->GetMoveForagingRatePenalty(month);
        recoveryRate += habitatType->GetRecoveryRate(month);
        meanEnergyValue += habitatType->GetMeanEnergyValue(month);
    }
    visibility = visibility / (double)m_HabitatTypes.size();
    m_Risk = 1.0 / visibility;
    if (m_SleepingSite || m_CliffRefuge || m_OtherRefuge) m_Risk *= 0.1;
    m_MaxResource = maxResource / (double)m_HabitatTypes.size();
    m_ForagingRateFactor = foragingRateFactor / (double)m_HabitatTypes.size();
    m_MoveForagingRatePenalty = moveForagingRatePenalty / (double)m_HabitatTypes.size();
    m_RecoveryRate = recoveryRate / (double)m_HabitatTypes.size();
    m_MeanEnergyValue = meanEnergyValue / (double)m_HabitatTypes.size();

    m_CurrentResource += m_RecoveryRate * area;
    if (m_CurrentResource > m_MaxResource) m_CurrentResource = m_MaxResource;
    if (m_CurrentResource < 0) m_CurrentResource = 0;

    // add a small amout of noise to important values so not all grid squares the same
    m_Risk *= RandomDouble(1.0 - 1e-8, 1.0 + 1e-8);
    m_CurrentResource *= RandomDouble(1.0 - 1e-8, 1.0 + 1e-8);
}


void GridSquare::UpdateOccupancy(double duration)
{
    unsigned int index;
    double increment = duration / m_HabitatTypes.size();
    m_TotalOccupancy += duration;
    // and update the habitat scores too
    for (index = 0; index < m_HabitatTypes.size(); index++)
    {
        (*g_Simulation.GetEnvironment()->GetHabitatTypeList())[m_HabitatTypes[index]]->AddOccupancy(increment);
    }
}


#ifdef USE_OPENGL
// assumes that the transformation has been set up so that the square is a unit square
void GridSquare::Draw()
{
    GLfloat fraction10 = 10 * m_CurrentResource / m_FullResource;
    GLfloat f;
    int i;
    glPolygonMode(GL_FRONT, GL_FILL);
    for (i = 0; i < 10; i++)
    {
        f = fraction10 - i;
        if (f < 0) f = 0;
        else if (f > 1) f = 1;
        DrawRectangle((GLfloat)i / 10.0, 0, 0.1, f, m_R, m_G, m_B);
    }    
    glPolygonMode(GL_FRONT, GL_LINE);
    DrawRectangle(0, 0, 1, 1, 1, 0, 0);
    if (m_HiLite)
        DrawRectangle(0.1, 0.1, 0.8, 0.8, m_RHiLite, m_GHiLite, m_BHiLite);

    glPolygonMode(GL_FRONT, GL_FILL);
    if (m_SleepingSite)
        DrawRectangle(0.15, 0.65, 0.2, 0.2, 1, 0, 0);
    if (m_WaterPresent)
        DrawRectangle(0.65, 0.65, 0.2, 0.2, 0, 0, 1);
    if (m_CliffRefuge)
        DrawRectangle(0.15, 0.15, 0.2, 0.2, 1, 1, 0);
    if (m_OtherRefuge)
        DrawRectangle(0.65, 0.15, 0.2, 0.2, 0, 1, 1);
}
#endif


