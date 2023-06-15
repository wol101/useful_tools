/*
 *  Agent.h
 *  PrimateAgent
 *
 *  Created by Bill Sellers on Mon Jan 31 2005.
 *  Copyright (c) 2005 Bill Sellers. All rights reserved.
 *
 */

#ifndef Agent_h
#define Agent_h

#include <libxml/xmlmemory.h>
#include <libxml/parser.h>
#include <string>

class GridSquare;

class Agent
{
public:
    Agent();
    ~Agent();

    void Agent::ParseAgent(xmlNodePtr cur);
    int GetAgentMultiplier() { return m_AgentMultiplier; };
    void SetAgentCounter(int c) { m_AgentCounter = c; };
    int GetAgentCounter() { return m_AgentCounter; };
    
    enum Activity {foraging, drinking, socialising, resting, moving, goingToSleepingSite };
    
    void SetGridSquare(GridSquare *gridSquare);
    void ResetTotals();

    void PerformAutomaticActions();
    void CalculatePreferredAction();
    void CheckPreferredAction();
    void DoPreferredAction(double duration);

    bool WantToMove() { return m_WantToMove; };
    Activity GetPreferredActivity() { return m_PreferredActivity; };
    int GetPreferredDirection();
    void Move(int preferredDirection, double duration);

    double GetForagingScore() { return m_ForagingScore; };
    double GetWaterScore() { return m_WaterScore; };
    double GetSocialScore() { return m_SocialScore; };
    double GetNormalisedForagingScore() { return m_ForagingScore / m_DailyNominalForagingScoreTotal; };
    double GetNormalisedWaterScore() { return m_WaterScore / m_DailyNominalWaterScoreTotal; };
    double GetNormalisedSocialScore() { return m_SocialScore / m_DailyNominalSocialScoreTotal; };
    
    double GetTotalDistanceTravelled() { return m_TotalDistanceTravelled; };
    double GetTotalTimeForaging() { return m_TotalTimeForaging; };
    double GetTotalTimeResting() { return m_TotalTimeResting; };
    double GetTotalTimeMoving() { return m_TotalTimeMoving; };
    double GetTotalTimeSocialising() { return m_TotalTimeSocialising; };
    double GetTotalEnergyIntake() { return m_TotalEnergyIntake; };
    
#ifdef USE_OPENGL
    void Draw();
#endif
        
protected:

    void FindClosestSleepingSite();              // returns the distance
    void FindClosestWaterPresent();              // returns the distance
    void FindBestLocalForagingSite(double radius);   // returns the foraging availability
    void FindBestLocalRestingSite(double radius);   // returns the risk

    string m_AgentName;
    int m_AgentMultiplier;
    int m_AgentCounter;

    double m_ForagingScore;      // Foraging Score (Joules - energy balance -  carries over day to day)
    double m_WaterScore;     // Water Score - arbitrary units
    double m_SocialScore;    // Social Score - arbitrary units

    Activity m_PreferredActivity;
    bool m_WantToMove;
    int m_PreferredDirection;
    
    double m_ComfortableSpeed;

    GridSquare *m_CurrentGridSquare;
    double m_X;
    double m_Y;

    GridSquare *m_ClosestSleepingSite;
    GridSquare *m_ClosestWaterPresent;
    GridSquare *m_BestLocalForagingSite;
    GridSquare *m_BestLocalRestingSite;

    double m_MoveCost;
    double m_RestCost;
    double m_SocialCost;
    double m_ForageCost;

    double m_ForagingDesire;
    double m_DrinkingDesire;
    double m_SocialisingDesire;

    double m_SearchRadius;

    double m_LastTestedSleepingSiteX;
    double m_LastTestedSleepingSiteY;
    double m_LastTestedWaterPresentX;
    double m_LastTestedWaterPresentY;
    GridSquare *m_LastTestedBestLocalForagingSite;
    double m_LastTestedBestLocalForagingSiteRadius;
    GridSquare *m_LastTestedBestLocalRestingSite;
    double m_LastTestedBestLocalRestingSiteRadius;

    double m_SocialScoreReductionRate; 
    double m_WaterScoreReductionRate; 

    double m_DailyNominalForagingScoreTotal; 
    double m_DailyNominalSocialScoreTotal; 
    double m_DailyNominalWaterScoreTotal;
    double m_NominalTimeBetweenDrinking;

    double m_ForagingImportance;
    double m_SocialImportance;
    double m_WaterImportance;

    double m_ForagingMoveThreshold;
    double m_SocialisingMoveThreshold;
    double m_RestingMoveThreshold;

    double m_RestSocialRiskThreshold;

    double m_TotalDistanceTravelled;
    double m_TotalTimeForaging;
    double m_TotalTimeResting;
    double m_TotalTimeMoving;
    double m_TotalTimeSocialising;
    double m_TotalEnergyIntake;

    bool m_UpdateDuringDayTimeOnly;
};



#endif
