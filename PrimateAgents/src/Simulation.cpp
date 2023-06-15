/*
 *  Simulation.cpp
 *  PrimateAgent
 *
 *  Created by Bill Sellers on Mon Jan 31 2005.
 *  Copyright (c) 2005 Bill Sellers. All rights reserved.
 *
 */

using namespace std;

#include <vector>
#include <math.h>
#include <iostream>
#include <sys/types.h>
#include <unistd.h>
#include <assert.h>
#include <time.h>

#ifdef USE_OPENGL
#include <glut.h>
#include "GLShapes.h"
#include "Key.h"
#endif

#include "Simulation.h"
#include "Environment.h"
#include "DayLength.h"
#include "Agent.h"
#include "GridSquare.h"
#include "Random.h"
#include "HabitatType.h"
#include "DataFile.h"

#include "DebugControl.h"

#define THROWIFZERO(a) if ((a) == 0) throw 0
#define THROWIF(a) if ((a) != 0) throw 0

Simulation::Simulation()
{
    m_DTDValidateFlag = false;
    m_Environment = 0;
    m_StartTime = 0;
    m_EndTime = 0;
    m_TimeIncrement = 0;
    m_CurrentTime = 0;
    m_NightTime = true;
    m_TimeOfDay = 0;
    m_LastTimeOfDay = 1e10;
    m_Month = 0;
    m_DayLength = 0;
    m_Dawn = 0;
    m_Dusk = 0;
    m_StepCounter = 0;
    m_ProportionToWinVote = 0.5;
    m_NumberOfAgentsMoving = 0;
    m_NumberOfAgentsResting = 0;
    m_NumberOfAgentsForaging = 0;
    m_NumberOfAgentsSocialising = 0;
    
    m_AgentSummariesOnly = true;
    m_GridSquareTotalsOnly = true;

#ifdef USE_OPENGL
    m_WindowWidth = 100;
    m_WindowHeight = 100;
#endif
}


Simulation::~Simulation()
{
    unsigned int index;
    if (m_Environment) delete m_Environment;
    for (index = 0; index < m_AgentList.size(); index++) delete m_AgentList[index];
}

void Simulation::UpdateSimulation()
{
    unsigned int index;
    struct tm *tm;
    time_t clock;

    m_TimeOfDay = remainder(m_CurrentTime, (24 * 60 * 60));
    if (m_TimeOfDay < 0) m_TimeOfDay += (24 * 60 * 60); // because I want the positive remainder
    
    if (m_TimeOfDay < m_LastTimeOfDay) // midnight so do housekeeping
    {
        if (m_CurrentTime != m_StartTime)
        {
            // and this is a good place to do my reporting
            double totalDistanceTravelled = 0;
            double totalTimeForaging = 0;
            double totalTimeResting = 0;
            double totalTimeMoving = 0;
            double totalTimeSocialising = 0;
            double totalEnergyIntake = 0;
            double totalForagingScore = 0;
            double totalWaterScore = 0;
            double totalSocialScore = 0;
            double numberUsed = 0;

            m_AgentLog << m_Month + 1 << "\t" << m_Day + 1 << "\t";
            for (index = 0; index < m_AgentList.size(); index++)
            {
                if (m_AgentSummariesOnly == false)
                {
                    m_AgentLog
                    << m_AgentList[index]->GetTotalDistanceTravelled() << "\t"
                    << m_AgentList[index]->GetTotalTimeForaging() << "\t"
                    << m_AgentList[index]->GetTotalTimeResting() << "\t"
                    << m_AgentList[index]->GetTotalTimeMoving() << "\t"
                    << m_AgentList[index]->GetTotalTimeSocialising() << "\t"
                    << m_AgentList[index]->GetTotalEnergyIntake() << "\t"
                    << m_AgentList[index]->GetNormalisedForagingScore() << "\t"
                    << m_AgentList[index]->GetNormalisedWaterScore() << "\t"
                    << m_AgentList[index]->GetNormalisedSocialScore() << "\t";
                }
                totalDistanceTravelled += m_AgentList[index]->GetTotalDistanceTravelled();
                totalTimeForaging += m_AgentList[index]->GetTotalTimeForaging();
                totalTimeResting += m_AgentList[index]->GetTotalTimeResting();
                totalTimeMoving += m_AgentList[index]->GetTotalTimeMoving();
                totalTimeSocialising += m_AgentList[index]->GetTotalTimeSocialising();
                totalEnergyIntake += m_AgentList[index]->GetTotalEnergyIntake();
                totalForagingScore += m_AgentList[index]->GetNormalisedForagingScore();
                totalWaterScore += m_AgentList[index]->GetNormalisedWaterScore();
                totalSocialScore += m_AgentList[index]->GetNormalisedSocialScore();
            }
            // note scores are instantaneous values and it makes no sense to multiply by factor
            m_AgentLog
                << totalDistanceTravelled / m_AgentList.size() << "\t"
                << totalTimeForaging / m_AgentList.size() << "\t"
                << totalTimeResting / m_AgentList.size() << "\t"
                << totalTimeMoving / m_AgentList.size() << "\t"
                << totalTimeSocialising / m_AgentList.size() << "\t"
                << totalEnergyIntake / m_AgentList.size() << "\t"
                << totalForagingScore / m_AgentList.size() << "\t"
                << totalWaterScore / m_AgentList.size() << "\t"
                << totalSocialScore / m_AgentList.size() << "\n";
            m_AgentLog.flush();

            m_HabitatTypeLog << m_Month + 1 << "\t" << m_Day + 1 << "\t";
            map<string, HabitatType *>::const_iterator iter;
            for (iter=m_Environment->GetHabitatTypeList()->begin(); iter != m_Environment->GetHabitatTypeList()->end(); iter++)
            {
                m_HabitatTypeLog << iter->second->GetOccupancy() << "\t";
                if (iter->second->GetOccupancy() > 0) numberUsed++;
            }
            m_HabitatTypeLog << numberUsed << "\n";
            m_HabitatTypeLog.flush();

            m_GridSquareLog << m_Month + 1 << "\t" << m_Day + 1 << "\t";
            numberUsed = 0;
            if (m_GridSquareTotalsOnly)
            {
                for (index = 0; index < m_Environment->GetGridSquareList()->size(); index++)
                {
                    if ((*m_Environment->GetGridSquareList())[index]->GetOccupancy() > 0) numberUsed++;
                }
                m_GridSquareLog << numberUsed << "\n";
            }
            else
            {
                for (index = 0; index < m_Environment->GetGridSquareList()->size(); index++)
                {
                    m_GridSquareLog << (*m_Environment->GetGridSquareList())[index]->GetOccupancy() << "\t";
                    if ((*m_Environment->GetGridSquareList())[index]->GetOccupancy() > 0) numberUsed++;
                }
                m_GridSquareLog << numberUsed << "\n";
            }
            m_GridSquareLog.flush();

            // and reset the totals
            for (index = 0; index < m_AgentList.size(); index++)
                m_AgentList[index]->ResetTotals();
            for (iter=m_Environment->GetHabitatTypeList()->begin(); iter != m_Environment->GetHabitatTypeList()->end(); iter++)
                iter->second->ResetTotals();
            for (index = 0; index < m_Environment->GetGridSquareList()->size(); index++)
                (*m_Environment->GetGridSquareList())[index]->ResetTotals();

        }
        
        // time housekeeping
        clock = (time_t)(m_CurrentTime + 0.5);
#ifdef USE_LOCALTIME
        tm = gmtime(&clock);
#else
        tm = localtime(&clock);
#endif
        m_Month = tm->tm_mon; // already from 0 to 11
        m_Day = tm->tm_mday - 1; // 0 to 27,28,29,30 as appropriate
        m_DayLength = (60.0 * 60.) * m_Environment->GetDayLength()->GetHoursAtMonth(m_Month);
        m_Dawn = (12.0 * 60.0 * 60.0) - (m_DayLength / 2);
        m_Dusk = (12.0 * 60.0 * 60.0) + (m_DayLength / 2);

        // simulation housekeeping
        for (index = 0; index < m_Environment->GetGridSquareList()->size(); index++)
        {
            (*m_Environment->GetGridSquareList())[index]->DoDailyRecovery();
        }

    }
    
    m_LastTimeOfDay = m_TimeOfDay;
    if (m_TimeOfDay > m_Dawn && m_TimeOfDay < m_Dusk) m_NightTime = false;
    else m_NightTime = true;

    int voteToMoveForaging = 0;
    int voteToMoveSocialRest = 0;
    int voteToMoveWater = 0;
    int voteToMoveToSleepSite = 0;
    int directionCount[8];
    int preferredDirectionCount = -1;
    int preferredDirection = 0;
    int i;
    int voteThreshold;
    Agent::Activity movementActivity;
    bool moveFlag = false;

    for (index = 0; index < m_AgentList.size(); index++)
    {
        // automatic actions first
        m_AgentList[index]->PerformAutomaticActions();
        // then calculate preferred action
        m_AgentList[index]->CalculatePreferredAction();
    }

    // test for movement
    for (index = 0; index < m_AgentList.size(); index++)
    {
        if (m_AgentList[index]->WantToMove())
        {
            switch (m_AgentList[index]->GetPreferredActivity())
            {
                case Agent::foraging:
                    voteToMoveForaging++;
                    break;
                case Agent::drinking:
                    voteToMoveWater++;
                    break;
                case Agent::socialising:
                case Agent::resting:
                    voteToMoveSocialRest++;
                    break;
                case Agent::goingToSleepingSite:
                    voteToMoveToSleepSite++;
                    break;
                default:
                    assert(false);
            }
        }
    }
    voteThreshold = (int)(m_AgentList.size() * m_ProportionToWinVote);
    if (voteToMoveForaging >= voteThreshold)
    {
        movementActivity = Agent::foraging;
        moveFlag = true;
    }
    else if (voteToMoveWater >= voteThreshold)
    {
        movementActivity = Agent::drinking;
        moveFlag = true;
    }
    else if (voteToMoveSocialRest >= voteThreshold)
    {
        movementActivity = Agent::socialising;
        moveFlag = true;
    }
    else if (voteToMoveToSleepSite >= voteThreshold)
    {
        movementActivity = Agent::goingToSleepingSite;
        moveFlag = true;
    }
    

    if (moveFlag)
    {
        memset(directionCount, 0, sizeof(directionCount));
        for (index = 0; index < m_AgentList.size(); index++)
        {
            switch (m_AgentList[index]->GetPreferredActivity())
            {
                case Agent::foraging:
                    if (movementActivity == Agent::foraging) directionCount[m_AgentList[index]->GetPreferredDirection()]++;
                    break;
                case Agent::drinking:
                    if (movementActivity == Agent::drinking) directionCount[m_AgentList[index]->GetPreferredDirection()]++;
                    break;
                case Agent::socialising:
                case Agent::resting:
                    if (movementActivity == Agent::socialising) directionCount[m_AgentList[index]->GetPreferredDirection()]++;
                    break;
                case Agent::goingToSleepingSite:
                    if (movementActivity == Agent::goingToSleepingSite) directionCount[m_AgentList[index]->GetPreferredDirection()]++;
                    break;
                default:
                    assert(false);
            }
        }
        // choose the modal value for preferred direction
        for (i = 0; i < 8; i++)
        {
            if (directionCount[i] > preferredDirectionCount)
            {
                preferredDirectionCount = directionCount[i];
                preferredDirection = i;
            }
        }
        // and do the move
        for (index = 0; index < m_AgentList.size(); index++)
        {
            m_AgentList[index]->Move(preferredDirection, m_TimeIncrement);
        }
        m_NumberOfAgentsMoving = m_AgentList.size();
        m_NumberOfAgentsResting = 0;
        m_NumberOfAgentsForaging = 0;
        m_NumberOfAgentsSocialising = 0;
    }
    else
    {
        m_NumberOfAgentsMoving = 0;
        m_NumberOfAgentsResting = 0;
        m_NumberOfAgentsForaging = 0;
        m_NumberOfAgentsSocialising = 0;
        for (index = 0; index < m_AgentList.size(); index++)
        {
            m_AgentList[index]->CheckPreferredAction();
            m_AgentList[index]->DoPreferredAction(m_TimeIncrement);
            switch (m_AgentList[index]->GetPreferredActivity())
            {
                case Agent::foraging:
                    m_NumberOfAgentsForaging++;
                    break;
                case Agent::resting:
                    m_NumberOfAgentsResting++;
                    break;
                case Agent::socialising:
                    m_NumberOfAgentsSocialising++;
                    break;
            }
        }
    }
    
    m_CurrentTime += m_TimeIncrement;
    m_StepCounter++;
}

int Simulation::LoadSimulation(xmlChar *data)
{
    Agent *agent;
    xmlNodePtr agentXMLPtr;
    int i;
    unsigned int index;
    xmlNodePtr cur;
    xmlParserCtxtPtr ctxt = 0;
    int agentMultiplier;

    if (g_Debug == DTDValidateDebug) m_DTDValidateFlag = true;
    
    try
    {
        // do the basic XML parsing

        /* create a parser context */
        ctxt = xmlNewParserCtxt();
        if (ctxt == 0)
        {
            fprintf(stderr, "Failed to allocate parser context\n");
            throw 1;
        }

        if (m_DTDValidateFlag)
            /* parse the file, activating the DTD validation option */
            m_Doc = xmlCtxtReadDoc(ctxt, data, "", NULL, XML_PARSE_DTDVALID);
        else
            m_Doc = xmlCtxtReadDoc(ctxt, data, "", NULL, 0);

        /* check if parsing suceeded */
        if (m_Doc == 0)
        {
            fprintf(stderr, "Failed to parse document\n");
            throw 1;
        }

        /* check if validation suceeded */
        if (ctxt->valid == 0)
        {
            fprintf(stderr, "Failed to validate document\n");
            throw 1;
        }

        cur = xmlDocGetRootElement(m_Doc);

        if (cur == NULL)
        {
            fprintf(stderr,"Empty document\n");
            throw 1;
        }

        if (xmlStrcmp(cur->name, (const xmlChar *) "PRIMATE_SIMULATION"))
        {
            fprintf(stderr,"Document of the wrong type, root node != PRIMATE_SIMULATION");
            throw 1;
        }

        // now parse the elements in the file

        cur = cur->xmlChildrenNode;
        while (cur != NULL)
        {
            if ((!xmlStrcmp(cur->name, (const xmlChar *)"GLOBAL")))
            {
                ParseGlobal(cur);
            }
#ifdef USE_OPENGL
            else if ((!xmlStrcmp(cur->name, (const xmlChar *)"DISPLAY")))
            {
                ParseDisplay(cur);
            }
#endif
            else if ((!xmlStrcmp(cur->name, (const xmlChar *)"AGENT")))
            {
                agentXMLPtr = cur;
                agent = new Agent();
                agent->ParseAgent(agentXMLPtr);
                agentMultiplier = agent->GetAgentMultiplier();
                agent->SetAgentCounter(0);
                m_AgentList.push_back(agent);
                
                for (i = 1; i < agentMultiplier; i++)
                {
                    agent = new Agent();
                    agent->ParseAgent(agentXMLPtr);
                    agent->SetAgentCounter(i);
                    m_AgentList.push_back(agent);
                }
            }
            else if ((!xmlStrcmp(cur->name, (const xmlChar *)"ENVIRONMENT")))
            {
                m_Environment = new Environment();
                m_Environment->ParseEnvironment(cur);
            }
            cur = cur->next;
        }
    }

    catch(...)
    {
        if (ctxt) xmlFreeParserCtxt(ctxt);
        if (m_Doc) xmlFreeDoc(m_Doc);
        m_Doc = 0;
        return 1;
    }

    xmlFreeParserCtxt(ctxt);

    // finish off the agents;

    unsigned int numSleepSites = m_Environment->GetSleepingSiteList()->size();
    GridSquare *startSquare =
        (*m_Environment->GetSleepingSiteList())[RandomInt(0, numSleepSites - 1)];

    for (index = 0; index < m_AgentList.size(); index++)
    {
        m_AgentList[index]->SetGridSquare(startSquare);
    }

    // time housekeeping
    struct tm *tm;
    time_t clock;
    clock = (time_t)(m_CurrentTime + 0.5);
#ifdef USE_LOCALTIME
        tm = gmtime(&clock);
#else
        tm = localtime(&clock);
#endif
    m_Month = tm->tm_mon; // already from 0 to 11
    m_Day = tm->tm_mday - 1; // 0 to 27,28,29,30 as appropriate
    m_DayLength = (60.0 * 60.) * m_Environment->GetDayLength()->GetHoursAtMonth(m_Month);
    m_Dawn = (12.0 * 60.0 * 60.0) - (m_DayLength / 2);
    m_Dusk = (12.0 * 60.0 * 60.0) + (m_DayLength / 2);
    
    // simulation housekeeping
    for (index = 0; index < m_Environment->GetGridSquareList()->size(); index++)
    {
        (*m_Environment->GetGridSquareList())[index]->DoDailyRecovery();
    }
    
    return 0;
}

#ifdef USE_OPENGL
void Simulation::ParseDisplay(xmlNodePtr cur)
{
    xmlChar *buf;
    char *ptrs[2];
    GLfloat keyOriginX;
    GLfloat keyOriginY;
    GLfloat keyWidth;
    GLfloat keyHeight;
    GLfloat keyTextSize;
    
    try
    {
        THROWIFZERO(buf = xmlGetProp(cur, (const xmlChar *)"WINDOWSIZE"));
        THROWIF(DataFile::ReturnTokens((char *)buf, ptrs, 2) != 2);
        m_WindowWidth = strtol(ptrs[0], 0, 10);
        m_WindowHeight = strtol(ptrs[1], 0, 10);
        xmlFree(buf);

        THROWIFZERO(buf = xmlGetProp(cur, (const xmlChar *)"DAYTIMESPEEDUP"));
        m_DayTimeSpeedUp = strtol((char *)buf, 0, 10);
        xmlFree(buf);

        THROWIFZERO(buf = xmlGetProp(cur, (const xmlChar *)"NIGHTTIMESPEEDUP"));
        m_NightTimeSpeedUp = strtol((char *)buf, 0, 10);
        xmlFree(buf);

        THROWIFZERO(buf = xmlGetProp(cur, (const xmlChar *)"KEYORIGIN"));
        THROWIF(DataFile::ReturnTokens((char *)buf, ptrs, 2) != 2);
        keyOriginX = strtod(ptrs[0], 0);
        keyOriginY = strtod(ptrs[1], 0);
        xmlFree(buf);
        
        THROWIFZERO(buf = xmlGetProp(cur, (const xmlChar *)"KEYSIZE"));
        THROWIF(DataFile::ReturnTokens((char *)buf, ptrs, 2) != 2);
        keyWidth = strtod(ptrs[0], 0);
        keyHeight = strtod(ptrs[1], 0);
        xmlFree(buf);
        
        THROWIFZERO(buf = xmlGetProp(cur, (const xmlChar *)"KEYTEXTSIZE"));
        keyTextSize = strtod((char *)buf, 0);
        xmlFree(buf);
        
        m_Key.SetSize(keyOriginX, keyOriginY, keyWidth, keyHeight);
        m_Key.SetTextSize(keyTextSize);
    }

    catch (int e)
    {
        cerr << "Error parsing DISPLAY\n";
        throw 0;
    }
}
#endif

void Simulation::ParseGlobal(xmlNodePtr cur)
{
    xmlChar *buf;
    struct tm tm;
    int day, month, year;

    try
    {
        THROWIFZERO(buf = xmlGetProp(cur, (const xmlChar *)"STARTTIME"));
        memset(&tm, 0, sizeof(tm));
        sscanf((char *)buf, "%02d/%02d/%04d", &day, &month, &year);
        tm.tm_mday = day;
        tm.tm_mon = month - 1;
        tm.tm_year = year - 1900;
#ifdef USE_LOCALTIME
        m_StartTime = mktime(&tm);
#else
        m_StartTime = timegm(&tm);
#endif
        xmlFree(buf);
        m_CurrentTime = m_StartTime;

        THROWIFZERO(buf = xmlGetProp(cur, (const xmlChar *)"ENDTIME"));
        memset(&tm, 0, sizeof(tm));
        sscanf((char *)buf, "%02d/%02d/%04d", &day, &month, &year);
        tm.tm_mday = day;
        tm.tm_mon = month - 1;
        tm.tm_year = year - 1900;
#ifdef USE_LOCALTIME
        m_EndTime = mktime(&tm);
#else
        m_EndTime = timegm(&tm);
#endif
        xmlFree(buf);

        THROWIFZERO(buf = xmlGetProp(cur, (const xmlChar *)"STEPTIME"));
        m_TimeIncrement = strtod((char *)buf, 0);
        xmlFree(buf);

        THROWIFZERO(buf = xmlGetProp(cur, (const xmlChar *)"PROPORTIONTOWINVOTE"));
        m_ProportionToWinVote = strtod((char *)buf, 0);
        xmlFree(buf);

        THROWIFZERO(buf = xmlGetProp(cur, (const xmlChar *)"AGENTSUMMARIESONLY"));
        if(strtol((char *)buf, 0, 10) != 0) m_AgentSummariesOnly = true;
        else m_AgentSummariesOnly = false;
        xmlFree(buf);

        THROWIFZERO(buf = xmlGetProp(cur, (const xmlChar *)"GRIDSQUARETOTALSONLY"));
        if(strtol((char *)buf, 0, 10) != 0) m_GridSquareTotalsOnly = true;
        else m_GridSquareTotalsOnly = false;
        xmlFree(buf);
    }

    catch (int e)
    {
        cerr << "Error parsing GLOBAL\n";
        throw 0;
    }
}

void Simulation::SetLogFiles(string &agentLogFilename,
                             string &habitatTypeLogFilename,
                             string &gridSquareLogFilename)
{
    m_AgentLog.open(agentLogFilename.c_str(), ios::app);
    m_HabitatTypeLog.open(habitatTypeLogFilename.c_str(), ios::app);
    m_GridSquareLog.open(gridSquareLogFilename.c_str(), ios::app);

    unsigned int index;
    m_AgentLog << "Month" << "\t" << "Day" << "\t";
    for (index = 0; index < m_AgentList.size(); index++)
    {
        if (m_AgentSummariesOnly == false)
        {
            m_AgentLog
            << "TotalDistanceTravelled" << index << "\t"
            << "TotalTimeForaging" << index << "\t"
            << "TotalTimeResting" << index << "\t"
            << "TotalTimeMoving" << index << "\t"
            << "TotalTimeSocialising" << index << "\t"
            << "TotalEnergyIntake" << index << "\t"
            << "NormalisedForagingScore" << index << "\t"
            << "NormalisedWaterScore" << index << "\t"
            << "NormalisedSocialScore" << index << "\t";
        }
    }
    m_AgentLog
        << "MeanDistanceTravelled" << "\t"
        << "MeanTimeForaging" << "\t"
        << "MeanTimeResting" << "\t"
        << "MeanTimeMoving" << "\t"
        << "MeanTimeSocialising" << "\t"
        << "MeanEnergyIntake" << "\t"
        << "MeanNormalisedForagingScore" << "\t"
        << "MeanNormalisedWaterScore" << "\t"
        << "MeanNormalisedSocialScore" << "\n";
    m_AgentLog.flush();

    m_HabitatTypeLog << "Month" << "\t" << "Day" << "\t";
    map<string, HabitatType *>::const_iterator iter;
    for (iter=m_Environment->GetHabitatTypeList()->begin(); iter != m_Environment->GetHabitatTypeList()->end(); iter++)
    {
        m_HabitatTypeLog << iter->second->GetName() << "\t";
    }
    m_HabitatTypeLog << "HabitatsUsed" << "\n";
    m_HabitatTypeLog.flush();

    m_GridSquareLog << "Month" << "\t" << "Day" << "\t";
    if (m_GridSquareTotalsOnly)
    {
        m_GridSquareLog << "SquaresEntered" << "\n";
    }
    else
    {
        for (index = 0; index < m_Environment->GetGridSquareList()->size(); index++)
        {
            m_GridSquareLog << (*m_Environment->GetGridSquareList())[index]->GetXGrid() <<
            "_" << (*m_Environment->GetGridSquareList())[index]->GetYGrid() << "\t";
        }
        m_GridSquareLog << "SquaresEntered" << "\n";
    }
    m_GridSquareLog.flush();
    
}

#ifdef USE_OPENGL
void Simulation::Draw()
{
    unsigned int index;

    // draw background
    glPolygonMode(GL_FRONT, GL_FILL);
    DrawRectangle(0, 0, 1, 1, 0, 0, 0);
        
    m_Environment->Draw();

    for (index = 0; index < m_AgentList.size(); index++)
        m_AgentList[index]->Draw();

    // draw frame
    glPolygonMode(GL_FRONT, GL_LINE);
    DrawRectangle(0, 0, 1, 1, 1, 0, 0);

    // draw key
    m_Key.Draw();
}
#endif

