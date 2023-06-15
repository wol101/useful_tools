/*
 *  Simulation.h
 *  PrimateAgent
 *
 *  Created by Bill Sellers on Mon Jan 31 2005.
 *  Copyright (c) 2005 Bill Sellers. All rights reserved.
 *
 */

#ifndef Simulation_h
#define Simulation_h

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <libxml/xmlmemory.h>
#include <libxml/parser.h>

#ifdef USE_OPENGL
#include "Key.h"
#endif

class Agent;
class Environment;
class ostream;

class Simulation
{
public:
    Simulation();
    ~Simulation();

    void UpdateSimulation();
    int LoadSimulation(xmlChar *data);
    bool ShouldFinish()
    {
        if (m_CurrentTime > m_EndTime) return true;
        return false;
    };
    Environment *GetEnvironment() { return m_Environment; };
    double GetCurrentTime() { return m_CurrentTime; };
    double GetTimeOfDay() { return m_TimeOfDay; };
    int GetMonth() { return m_Month; };
    int GetDay() { return m_Day; };
    bool IsNightTime() { return m_NightTime; };
    double GetDayLength() { return m_DayLength; };
    double GetDawn() { return m_Dawn; };
    double GetDusk() { return m_Dusk; };
    long long GetStepCounter() { return m_StepCounter; };
    int GetNumberOfAgentsMoving() { return m_NumberOfAgentsMoving; }
    int GetNumberOfAgentsResting() { return m_NumberOfAgentsResting; }
    int GetNumberOfAgentsForaging() { return m_NumberOfAgentsForaging; }
    int GetNumberOfAgentsSocialising() { return m_NumberOfAgentsSocialising; }

    void SetLogFiles(string &agentLogFilename, string &habitatTypeLogFilename, string &gridSquareLogFilename);

#ifdef USE_OPENGL
    void Draw();
    int GetWindowWidth() { return m_WindowWidth; };
    int GetWindowHeight() { return m_WindowHeight; };
    int GetDayTimeSpeedUp() { return m_DayTimeSpeedUp; };
    int GetNightTimeSpeedUp() { return m_NightTimeSpeedUp; };
#endif
    
protected:

    void ParseGlobal(xmlNodePtr cur);
#ifdef USE_OPENGL
    void ParseDisplay(xmlNodePtr cur);
#endif

    bool m_DTDValidateFlag;
    xmlDocPtr m_Doc;
    
    Environment *m_Environment;
    vector<Agent *> m_AgentList;

    double m_ProportionToWinVote;
    
    double m_StartTime;
    double m_EndTime;
    double m_TimeIncrement;
    double m_CurrentTime;
    long long m_StepCounter;

    bool m_NightTime;
    double m_TimeOfDay;
    double m_LastTimeOfDay;
    int m_Month;
    int m_Day;
    double m_DayLength;
    double m_Dawn;
    double m_Dusk;

    ofstream m_AgentLog;
    ofstream m_HabitatTypeLog;
    ofstream m_GridSquareLog;
    bool m_AgentSummariesOnly;
    bool m_GridSquareTotalsOnly;

    int m_NumberOfAgentsMoving;
    int m_NumberOfAgentsResting;
    int m_NumberOfAgentsForaging;
    int m_NumberOfAgentsSocialising;

#ifdef USE_OPENGL
    int m_WindowWidth;
    int m_WindowHeight;
    int m_DayTimeSpeedUp;
    int m_NightTimeSpeedUp;
    Key m_Key;
#endif

};



#endif



