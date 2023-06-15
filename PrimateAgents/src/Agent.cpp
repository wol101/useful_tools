/*
 *  Agent.cpp
 *  PrimateAgent
 *
 *  Created by Bill Sellers on Mon Jan 31 2005.
 *  Copyright (c) 2005 Bill Sellers. All rights reserved.
 *
 */

using namespace std;

#include <vector>
#include <iostream>
#include <float.h>
#include <math.h>
#include <float.h>
#include <assert.h>

#ifdef USE_OPENGL
#include <glut.h>
#include "GLShapes.h"
#endif

#include "Agent.h"
#include "Environment.h"
#include "Simulation.h"
#include "GridSquare.h"
#include "Random.h"

#include "DebugControl.h"

#define THROWIFZERO(a) if ((a) == 0) throw 0
#define THROWIF(a) if ((a) != 0) throw 0

extern Simulation g_Simulation;

Agent::Agent()
{
    m_ForagingScore = 0;
    m_WaterScore = 1; // set to m_DailyNominalWaterScoreTotal value
    m_SocialScore = 0;

    m_PreferredActivity = resting;
    m_WantToMove = false;
    m_PreferredDirection = 0;

    m_ComfortableSpeed = 0; 

    m_CurrentGridSquare = 0;
    m_X = 0;
    m_Y = 0;

    m_ClosestSleepingSite = 0;
    m_ClosestWaterPresent = 0;
    m_BestLocalForagingSite = 0;
    m_BestLocalRestingSite = 0;

    m_RestCost = 0;
    m_MoveCost = 0;
    m_SocialCost = 0;
    m_ForageCost = 0;
        
    m_ForagingDesire = 0;
    m_DrinkingDesire = 0;
    m_SocialisingDesire = 0;

    m_SearchRadius = 0; 

    m_LastTestedSleepingSiteX = 0;
    m_LastTestedSleepingSiteY = 0;
    m_LastTestedWaterPresentX = 0;
    m_LastTestedWaterPresentY = 0;
    m_LastTestedBestLocalForagingSite = 0;
    m_LastTestedBestLocalRestingSite = 0;
    m_LastTestedBestLocalForagingSiteRadius = 0;
    m_LastTestedBestLocalRestingSiteRadius = 0;

    m_DailyNominalForagingScoreTotal = 0; // daily energy budget
    m_DailyNominalSocialScoreTotal = 0; // seconds per day wanted
    m_DailyNominalWaterScoreTotal = 1; // arbitrary units
    m_NominalTimeBetweenDrinking = 1;

    m_ForagingImportance = 0;
    m_SocialImportance = 0;
    m_WaterImportance = 0;

    m_SocialScoreReductionRate = 0;
    m_WaterScoreReductionRate = 0;

    m_ForagingMoveThreshold = 1.5;
    m_SocialisingMoveThreshold = 1.5;
    m_RestingMoveThreshold = 1.5;

    m_RestSocialRiskThreshold = 0.5;

    ResetTotals();
    m_UpdateDuringDayTimeOnly = true;
}

Agent::~Agent()
{
}

void Agent::ResetTotals()
{
    m_TotalDistanceTravelled = 0;
    m_TotalTimeForaging = 0;
    m_TotalTimeResting = 0;
    m_TotalTimeMoving = 0;
    m_TotalTimeSocialising = 0;
    m_TotalEnergyIntake = 0;
}    

void Agent::ParseAgent(xmlNodePtr cur)
{
    xmlChar *buf;

    try
    {
        THROWIFZERO(buf = xmlGetProp(cur, (const xmlChar *)"AGENTNAME"));
        m_AgentName = (char *)buf;
        xmlFree(buf);

        THROWIFZERO(buf = xmlGetProp(cur, (const xmlChar *)"AGENTMULTIPLIER"));
        m_AgentMultiplier = strtol((char *)buf, 0, 10);
        xmlFree(buf);
        
        THROWIFZERO(buf = xmlGetProp(cur, (const xmlChar *)"COMFORTABLESPEED"));
        m_ComfortableSpeed = strtod((char *)buf, 0);
        xmlFree(buf);

        THROWIFZERO(buf = xmlGetProp(cur, (const xmlChar *)"RESTPOWER"));
        m_RestCost = strtod((char *)buf, 0);
        xmlFree(buf);

        THROWIFZERO(buf = xmlGetProp(cur, (const xmlChar *)"MOVEPOWER"));
        m_MoveCost = strtod((char *)buf, 0);
        xmlFree(buf);

        THROWIFZERO(buf = xmlGetProp(cur, (const xmlChar *)"SOCIALPOWER"));
        m_SocialCost = strtod((char *)buf, 0);
        xmlFree(buf);

        THROWIFZERO(buf = xmlGetProp(cur, (const xmlChar *)"FORAGEPOWER"));
        m_ForageCost = strtod((char *)buf, 0);
        xmlFree(buf);

        THROWIFZERO(buf = xmlGetProp(cur, (const xmlChar *)"SEARCHRADIUS"));
        m_SearchRadius = strtod((char *)buf, 0);
        xmlFree(buf);

        THROWIFZERO(buf = xmlGetProp(cur, (const xmlChar *)"DAILYNOMINALENERGYBUDGET"));
        m_DailyNominalForagingScoreTotal = strtod((char *)buf, 0);
        xmlFree(buf);

        THROWIFZERO(buf = xmlGetProp(cur, (const xmlChar *)"DAILYNOMINALSOCIALTIME"));
        m_DailyNominalSocialScoreTotal = strtod((char *)buf, 0);
        xmlFree(buf);
        m_SocialScoreReductionRate = m_DailyNominalSocialScoreTotal / (24.0 * 60 * 60);

        THROWIFZERO(buf = xmlGetProp(cur, (const xmlChar *)"NOMINALTIMEBETWEENDRINKING"));
        m_NominalTimeBetweenDrinking = strtod((char *)buf, 0);
        xmlFree(buf);
        m_WaterScoreReductionRate = m_DailyNominalWaterScoreTotal / m_NominalTimeBetweenDrinking;

        THROWIFZERO(buf = xmlGetProp(cur, (const xmlChar *)"RELATIVEFOODIMPORTANCE"));
        m_ForagingImportance = strtod((char *)buf, 0);
        xmlFree(buf);

        THROWIFZERO(buf = xmlGetProp(cur, (const xmlChar *)"RELATIVESOCIALIMPORTANCE"));
        m_SocialImportance = strtod((char *)buf, 0);
        xmlFree(buf);

        THROWIFZERO(buf = xmlGetProp(cur, (const xmlChar *)"RELATIVEDRINKINGIMPORTANCE"));
        m_WaterImportance = strtod((char *)buf, 0);
        xmlFree(buf);

        THROWIFZERO(buf = xmlGetProp(cur, (const xmlChar *)"FORAGINGMOVETHRESHOLD"));
        m_ForagingMoveThreshold = strtod((char *)buf, 0);
        xmlFree(buf);

        THROWIFZERO(buf = xmlGetProp(cur, (const xmlChar *)"SOCIALISINGMOVETHRESHOLD"));
        m_SocialisingMoveThreshold = strtod((char *)buf, 0);
        xmlFree(buf);

        THROWIFZERO(buf = xmlGetProp(cur, (const xmlChar *)"RESTINGMOVETHRESHOLD"));
        m_RestingMoveThreshold = strtod((char *)buf, 0);
        xmlFree(buf);

        THROWIFZERO(buf = xmlGetProp(cur, (const xmlChar *)"RESTSOCIALRISKTHRESHOLD"));
        m_RestSocialRiskThreshold = strtod((char *)buf, 0);
        xmlFree(buf);

    }

    catch (int e)
    {
        cerr << "Error parsing GLOBAL\n";
        throw 0;
    }
}

void Agent::SetGridSquare(GridSquare *gridSquare)
{
    m_CurrentGridSquare = gridSquare;
    m_CurrentGridSquare->GetCentre(&m_X, &m_Y);

    // now it should be safe to do these
    FindClosestSleepingSite();
    FindClosestWaterPresent();
    FindBestLocalForagingSite(m_SearchRadius);
    FindBestLocalRestingSite(m_SearchRadius);
}

// do necessary but effectively zero time activities
// currently this is just drinking
void Agent::PerformAutomaticActions()
{
    if (m_CurrentGridSquare->IsWaterPresent())
    {
        m_WaterScore = m_DailyNominalWaterScoreTotal;
    }
}

// work out what the agent would like to do
void Agent::CalculatePreferredAction()
{
    m_WantToMove = false;

    // house keeping to make sure knowledge is up to date
    // these are cached so are very quick if there is no movement
    FindClosestSleepingSite();
    FindClosestWaterPresent();
    double maxTravelDistance = (g_Simulation.GetDusk() - g_Simulation.GetTimeOfDay()) * m_ComfortableSpeed;
    if (maxTravelDistance < 0) maxTravelDistance = 0;
    if (maxTravelDistance > m_SearchRadius)
    {
        FindBestLocalForagingSite(m_SearchRadius);
        FindBestLocalRestingSite(m_SearchRadius);
    }
    else
    {
        FindBestLocalForagingSite(maxTravelDistance);
        FindBestLocalRestingSite(maxTravelDistance);
    }
    
    
    // check the hard constraints
    
    // at night time go to sleep
    if (g_Simulation.IsNightTime())
    {
        if (m_CurrentGridSquare->IsSleepingSite() == false) 
        {
            m_PreferredActivity = goingToSleepingSite;
            m_WantToMove = true;
            return;
        }
        m_PreferredActivity = resting;
        return;
    }
    
    // check whether I can get back to a sleeping site
    if (m_ClosestSleepingSite->GetDistanceFrom(m_X, m_Y) >= maxTravelDistance)
    {
        m_PreferredActivity = goingToSleepingSite;
        m_WantToMove = true;
        return;
    }

    // now onto the soft constraints

    // there are 4 things I might want to do
    // foraging, drinking, socialising, resting

    // set all the desires to values depending on the 'bliss point' functions
    m_ForagingDesire = -m_ForagingScore / m_DailyNominalForagingScoreTotal;
    m_DrinkingDesire = -m_WaterScore / m_DailyNominalWaterScoreTotal;
    m_SocialisingDesire = -m_SocialScore / m_DailyNominalSocialScoreTotal;

    // now modify by their importance ratings (and change sign)
    m_ForagingDesire *=  m_ForagingImportance;
    m_DrinkingDesire *= m_WaterImportance;
    m_SocialisingDesire *= m_SocialImportance;

    if (m_ForagingDesire < 0) m_ForagingDesire = 0;
    if (m_DrinkingDesire < 0) m_DrinkingDesire = 0;
    if (m_SocialisingDesire < 0) m_SocialisingDesire = 0;

    double total;
    double roll;
    if (m_ForagingDesire == 0 && m_DrinkingDesire == 0 && m_SocialisingDesire == 0)
    {
        m_PreferredActivity = resting;
    }
    else
    {
        // roll for desire
        total = m_ForagingDesire + m_DrinkingDesire + m_SocialisingDesire;
        roll = RandomDouble(0, total);
        if (roll <= m_ForagingDesire) m_PreferredActivity = foraging;
        else if (roll <= m_ForagingDesire + m_DrinkingDesire) m_PreferredActivity = drinking;
        else m_PreferredActivity = socialising;
    }
    
    // now check to see if I'd really rather move
    switch (m_PreferredActivity)
    {
        case foraging:
#ifdef USING_RATE_BASED_DECISIONS
            if (m_BestLocalForagingSite->GetForagingRate() > m_CurrentGridSquare->GetForagingRate() * m_ForagingMoveThreshold)
                m_WantToMove = true;
#else
            if (m_BestLocalForagingSite->GetCurrentResource() > m_CurrentGridSquare->GetCurrentResource() * m_ForagingMoveThreshold)
                m_WantToMove = true;
#endif
            break;

        case drinking:
            if (m_CurrentGridSquare->IsWaterPresent() == false)
                m_WantToMove = true;
            break;

        case socialising:
            if (m_BestLocalRestingSite->GetRisk() < m_CurrentGridSquare->GetRisk() / m_SocialisingMoveThreshold)
                m_WantToMove = true;
            break;

        case resting:
            if (m_BestLocalRestingSite->GetRisk() < m_CurrentGridSquare->GetRisk() / m_RestingMoveThreshold)
                m_WantToMove = true;
            break;
            
        default:
            assert(false);
    }
}

void Agent::CheckPreferredAction()
{
    if (g_Simulation.IsNightTime()) return;

    // do we need to choose another activity?

    // foraging is always OK
    if (m_PreferredActivity == foraging) return;

    // but resting and socialising only in safe areas
    if ((m_PreferredActivity == socialising || m_PreferredActivity == resting)
        && m_CurrentGridSquare->GetRisk() > m_RestSocialRiskThreshold)
    {
        m_PreferredActivity = foraging;
        return;
    }

    // drinking or goingToSleepingSite need to reroll
    double total;
    double roll;
    if (m_PreferredActivity == drinking || m_PreferredActivity == goingToSleepingSite)
    {
        if (m_CurrentGridSquare->GetRisk() > m_RestSocialRiskThreshold)
        {
            m_PreferredActivity = foraging;
            return;
        }
        if (m_ForagingDesire == 0 && m_SocialisingDesire == 0)
        {
            m_PreferredActivity = resting;
        }
        else
        {
            // roll for desire
            total = m_ForagingDesire + m_SocialisingDesire;
            roll = RandomDouble(0, total);
            if (roll <= m_ForagingDesire) m_PreferredActivity = foraging;
            else m_PreferredActivity = socialising;
        }
    }
}

void Agent::DoPreferredAction(double duration)
{
    double energyIntake;
    switch (m_PreferredActivity)
    {
        case foraging:
            energyIntake = m_CurrentGridSquare->Forage(duration, false);
            m_ForagingScore = m_ForagingScore - m_ForageCost * duration + energyIntake;
            m_WaterScore = m_WaterScore - m_WaterScoreReductionRate * duration;
            m_SocialScore = m_SocialScore - m_SocialScoreReductionRate * duration;

            m_TotalEnergyIntake += energyIntake;
            m_TotalTimeForaging += duration;
            m_CurrentGridSquare->UpdateOccupancy(duration);
            break;

        case socialising:
            m_ForagingScore = m_ForagingScore - m_SocialCost * duration;
            m_WaterScore = m_WaterScore - m_WaterScoreReductionRate * duration;
            m_SocialScore = m_SocialScore - m_SocialScoreReductionRate * duration + duration;

            m_TotalTimeSocialising += duration;
            m_CurrentGridSquare->UpdateOccupancy(duration);
            break;

        case resting:
            m_ForagingScore = m_ForagingScore - m_RestCost * duration;
            m_WaterScore = m_WaterScore - m_WaterScoreReductionRate * duration;
            m_SocialScore = m_SocialScore - m_SocialScoreReductionRate * duration;

            // only resting occurs at night
            if (m_UpdateDuringDayTimeOnly == false || g_Simulation.IsNightTime() == false)
            {
                m_TotalTimeResting += duration;
                m_CurrentGridSquare->UpdateOccupancy(duration);
            }
            break;

        default:
            assert(false);
    }
}

int Agent::GetPreferredDirection()
{
    int direction;
    
    switch (m_PreferredActivity)
    {
        case foraging:
            direction = m_BestLocalForagingSite->GetSegmentalDirectionFrom(m_X, m_Y);
            break;

        case drinking:
            direction = m_ClosestWaterPresent->GetSegmentalDirectionFrom(m_X, m_Y);
            break;

        case socialising:
            direction = m_BestLocalRestingSite->GetSegmentalDirectionFrom(m_X, m_Y);
            break;

        case resting:
            direction = m_BestLocalRestingSite->GetSegmentalDirectionFrom(m_X, m_Y);
            break;

        case goingToSleepingSite:
            direction = m_ClosestSleepingSite->GetSegmentalDirectionFrom(m_X, m_Y);
            break;

        default:
            assert(false);
    }

    return direction;
}

void Agent::Move(int preferredDirection, double duration)
{
    double angle = preferredDirection * M_PI / 4;
    double distance = duration * m_ComfortableSpeed;

    m_X += distance * cos(angle);
    m_Y += distance * sin(angle);

    Grid *grid = g_Simulation.GetEnvironment()->GetGrid();
    grid->LimitRange(&m_X, &m_Y);
    GridData *gridData = grid->GetGridDataPtr(m_X, m_Y);
    if (gridData)
    {
        if (gridData->valid)
        {
            if (m_CurrentGridSquare != gridData->gridSquare)
            {
                m_CurrentGridSquare = gridData->gridSquare;
            }
        }
    }

    // spending time
    double energyIntake = m_CurrentGridSquare->Forage(duration, true);

    m_ForagingScore =  m_ForagingScore - m_MoveCost * duration + energyIntake;
    m_WaterScore = m_WaterScore - m_WaterScoreReductionRate * duration;
    m_SocialScore = m_SocialScore - m_SocialScoreReductionRate * duration;

    m_TotalEnergyIntake += energyIntake;
    m_TotalTimeMoving += duration;
    m_TotalDistanceTravelled += distance;
    m_CurrentGridSquare->UpdateOccupancy(duration);
}


void Agent::FindClosestSleepingSite()
{
    // can we use old value?
    if (m_X == m_LastTestedSleepingSiteX && m_Y == m_LastTestedSleepingSiteY) return;
    m_LastTestedSleepingSiteX = m_X;
    m_LastTestedSleepingSiteY = m_Y;
    
    unsigned int index;
    vector<GridSquare *> *sleepingSiteList = g_Simulation.GetEnvironment()->GetSleepingSiteList();
    GridSquare *sleepingSite;
    double distanceSquared;
    double minDistance = DBL_MAX;

    for (index = 0; index < sleepingSiteList->size(); index++)
    {
        sleepingSite = (*sleepingSiteList)[index];
        distanceSquared = sleepingSite->GetSquaredDistanceFrom(m_X, m_Y);
        if (distanceSquared < minDistance)
        {
            minDistance = distanceSquared;
            m_ClosestSleepingSite = sleepingSite;
        }
    }
}

void Agent::FindClosestWaterPresent()
{
    // can we use old value?
    if (m_X == m_LastTestedWaterPresentX && m_Y == m_LastTestedWaterPresentY) return;
    m_LastTestedWaterPresentX = m_X;
    m_LastTestedWaterPresentY = m_Y;

    unsigned int index;
    vector<GridSquare *> *waterPresentListList = g_Simulation.GetEnvironment()->GetWaterPresentList();
    GridSquare *waterPresent;
    double distanceSquared;
    double minDistance = DBL_MAX;

    for (index = 0; index < waterPresentListList->size(); index++)
    {
        waterPresent = (*waterPresentListList)[index];
        distanceSquared = waterPresent->GetSquaredDistanceFrom(m_X, m_Y);
        if (distanceSquared < minDistance)
        {
            minDistance = distanceSquared;
            m_ClosestWaterPresent = waterPresent;
        }
    }
}

void Agent::FindBestLocalForagingSite(double radius)
{
    // can we use old value?
    //if (m_CurrentGridSquare == m_LastTestedBestLocalForagingSite && radius == m_LastTestedBestLocalForagingSiteRadius) return;
    //m_LastTestedBestLocalForagingSite = m_CurrentGridSquare;
    //m_LastTestedBestLocalForagingSiteRadius = radius;

    GridData *gridData;
    int i, j;
    double foragingRate;
    double bestForagingRate = -1;
    GridSquare *foragingSite;
    double distanceSquared;
    m_BestLocalForagingSite = m_CurrentGridSquare;
    
    int x = m_CurrentGridSquare->GetXGrid();
    int y = m_CurrentGridSquare->GetYGrid();

    Grid *grid = g_Simulation.GetEnvironment()->GetGrid();
    int width = (int)(0.5 + radius / grid->GetXSize());
    int height = (int)(0.5 + radius / grid->GetYSize());
    double radiusSquared = radius * radius;

    for (i = -width; i <= width; i++)
    {
        for (j = -height; j <= height; j++)
        {
            gridData = grid->GetGridDataPtr(x + i, y +j);
            if (gridData)
            {
                if (gridData->valid)
                {
                    foragingSite = gridData->gridSquare;
                    distanceSquared = foragingSite->GetSquaredDistanceFrom(m_X, m_Y);
                    if (distanceSquared <= radiusSquared)
                    {
#ifdef USING_RATE_BASED_DECISIONS
                        foragingRate = foragingSite->GetForagingRate();
#else
                        foragingRate = foragingSite->GetCurrentResource();
#endif
                        if (foragingRate > bestForagingRate)
                        {
                            bestForagingRate = foragingRate;
                            m_BestLocalForagingSite = foragingSite;
                        }
                        else if (foragingRate == bestForagingRate)
                        {
                            // choose closest
                            if (distanceSquared < m_BestLocalForagingSite->GetSquaredDistanceFrom(m_X, m_Y))
                                m_BestLocalForagingSite = foragingSite;
                        }
                    }
                }
            }
        }
    }
    if (g_Debug == AgentDebug)
    {
        *g_DebugStream << "Agent::FindBestLocalForagingSite m_CurrentGridSquare->GetCurrentResource() "
        << m_CurrentGridSquare->GetCurrentResource()
        << " m_BestLocalForagingSite->GetCurrentResource() "
        << m_BestLocalForagingSite->GetCurrentResource() << "\n";
    }
}

void Agent::FindBestLocalRestingSite(double radius)
{
    // can we use old value?
    //if (m_CurrentGridSquare == m_LastTestedBestLocalRestingSite && radius == m_LastTestedBestLocalRestingSiteRadius) return;
    //m_LastTestedBestLocalRestingSite = m_CurrentGridSquare;
    //m_LastTestedBestLocalRestingSiteRadius = radius;

    GridData *gridData;
    int i, j;
    double risk;
    double lowestRisk = DBL_MAX;
    GridSquare *restingSite;
    double distanceSquared;
    m_BestLocalRestingSite = m_CurrentGridSquare;

    int x = m_CurrentGridSquare->GetXGrid();
    int y = m_CurrentGridSquare->GetYGrid();

    Grid *grid = g_Simulation.GetEnvironment()->GetGrid();
    int width = (int)(0.5 + radius / grid->GetXSize());
    int height = (int)(0.5 + radius / grid->GetYSize());
    double radiusSquared = radius * radius;

    for (i = -width; i <= width; i++)
    {
        for (j = -height; j <= height; j++)
        {
            gridData = grid->GetGridDataPtr(x + i, y +j);
            if (gridData)
            {
                if (gridData->valid)
                {
                    restingSite = gridData->gridSquare;
                    distanceSquared = restingSite->GetSquaredDistanceFrom(m_X, m_Y);
                    if (distanceSquared <= radiusSquared)
                    {
                        risk = restingSite->GetRisk();
                        if (risk < lowestRisk)
                        {
                            lowestRisk = risk;
                            m_BestLocalRestingSite = restingSite;
                        }
                        else if (risk == lowestRisk) // all things being equal pick the one with most foraging
                        {
                            if (restingSite->GetForagingRate() > m_BestLocalRestingSite->GetForagingRate())
                                m_BestLocalRestingSite = restingSite;
                            else if (restingSite->GetForagingRate() == m_BestLocalRestingSite->GetForagingRate())
                            {
                                // choose closest
                                if (distanceSquared < m_BestLocalRestingSite->GetSquaredDistanceFrom(m_X, m_Y))
                                    m_BestLocalForagingSite = restingSite;
                            }
                        }
                    }
                }
            }
        }
    }
}

#ifdef USE_OPENGL
// assumes that the transformation has been set up so that the grid is in world coordinates
void Agent::Draw()
{
    glPushMatrix();

    Grid *grid = g_Simulation.GetEnvironment()->GetGrid();
    grid->ConvertToGridCoordinates();
    GLfloat w = (GLfloat)grid->GetXSize();
    GLfloat h = (GLfloat)grid->GetYSize();
    GLfloat w2 = w / 2;
    GLfloat h2 = h / 2;    
    
    // draw the agent
    DrawCross(m_X - w2, m_Y - h2, w, h, 1, 1, 1);

    // draw the various desire targets
    double x, y;
    GLfloat xm = w * 0.05;
    GLfloat ym = h * 0.05;
    glPolygonMode(GL_FRONT, GL_LINE);
    m_ClosestSleepingSite->GetCentre(&x, &y);
    DrawRectangle(x + xm - w2 , y + ym - h2, w - 2 * xm, h - 2 * ym, 0, 1, 1);

    m_ClosestWaterPresent->GetCentre(&x, &y);
    DrawRectangle(x + xm - w2 , y + ym - h2, w - 2 * xm, h - 2 * ym, 0, 0, 1);

    m_BestLocalForagingSite->GetCentre(&x, &y);
    DrawRectangle(x + xm - w2 , y + ym - h2, w - 2 * xm, h - 2 * ym, 0, 1, 0);

    m_BestLocalRestingSite->GetCentre(&x, &y);
    xm = w * 0.1;
    ym = h * 0.1;
    DrawRectangle(x + xm - w2 , y + ym - h2, w - 2 * xm, h - 2 * ym, 1, 0, 1);
        
    glPopMatrix();
}
#endif




