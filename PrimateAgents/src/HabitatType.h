/*
 *  HabitatType.h
 *  Primate Agent
 *
 *  Created by Bill Sellers on Wed Sep 22 2004.
 *  Copyright (c) 2004 Bill Sellers. All rights reserved.
 *
 */


#ifndef HabitatType_h
#define HabitatType_h

#include <map>
#include <string>
#include <libxml/xmlmemory.h>
#include <libxml/parser.h>

class HabitatType
{
public:
    HabitatType(xmlNodePtr cur);

    enum ForagingModel {FixedRate, Proportion};

    double GetProductivity(int month) { month = month % 12; return m_Productivity[month]; };
    string &GetName() { return m_Name; };
    double GetVisibility(int month) { month = month % 12; return m_Visibility[month]; };
    double GetForagingRateFactor(int month) { month = month % 12; return m_ForagingRateFactor[month]; };
    ForagingModel GetForagingModel() { return m_ForagingModel; };
    double GetMoveForagingRatePenalty(int month) { month = month % 12; return m_MoveForagingRatePenalty[month]; };
    double GetRecoveryRate(int month) { month = month % 12; return m_RecoveryRate[month]; };
    double GetMeanEnergyValue(int month) { month = month % 12; return m_MeanEnergyValue[month]; };
    double GetFullResource() { return m_FullResource; };

    double GetOccupancy() { return m_TotalOccupancy; };
    void AddOccupancy(double occupancy) { m_TotalOccupancy += occupancy; };
    void ResetTotals() { m_TotalOccupancy = 0; };
    
#ifdef USE_OPENGL
    void GetColour(GLfloat *r, GLfloat *g, GLfloat *b)
    {
        *r = m_R; *g = m_G; *b = m_B;
    }
#endif
    
protected:
    double m_Productivity[12];
    double m_Visibility[12];
    double m_ForagingRateFactor[12];
    double m_MoveForagingRatePenalty[12];
    double m_RecoveryRate[12];
    double m_MeanEnergyValue[12];
    string m_Name;
    ForagingModel m_ForagingModel;
    double m_FullResource;

    double m_TotalOccupancy;

#ifdef USE_OPENGL
    GLfloat m_R;
    GLfloat m_G;
    GLfloat m_B;
#endif
};

#endif
