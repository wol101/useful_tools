/*
 *  GridSquare.h
 *  Primate Agent
 *
 *  Created by Bill Sellers on Wed Sep 22 2004.
 *  Copyright (c) 2004 Bill Sellers. All rights reserved.
 *
 */

#ifndef GridSquare_h
#define GridSquare_h

#include <vector>
#include <string>
#include <libxml/xmlmemory.h>
#include <libxml/parser.h>
#include "HabitatType.h"

class GridSquare
{
public:
    GridSquare(xmlNodePtr cur);

    int GetYGrid() { return m_YGrid; };
    int GetXGrid() { return m_XGrid; };
    double GetMeanGroundSlope() { return m_MeanGroundSlope; };
    bool IsSleepingSite() { return m_SleepingSite; };
    bool IsCliffRefuge() { return m_CliffRefuge; };
    bool IsOtherRefuge() { return m_OtherRefuge; };
    bool IsWaterPresent() { return m_WaterPresent; };
    vector<string> &GetHabitatTypes() { return m_HabitatTypes; };
    void SetCentre(double x, double y) { m_XCentre = x; m_YCentre = y; };
    void GetCentre(double *x, double *y) { *x = m_XCentre; *y = m_YCentre; };    
    double GetDistanceFrom(double x, double y);
    double GridSquare::GetSquaredDistanceFrom(double x, double y)
    {
        double dx = m_XCentre - x;
        double dy = m_YCentre - y;
        return (dx * dx + dy * dy);
    };
    double GetDirectionFrom(double x, double y);
    int GetSegmentalDirectionFrom(double x, double y);
    double GetRisk() { return m_Risk; };
    double GetForagingRate();
    double Forage(double duration, bool move);
    HabitatType::ForagingModel GetForagingModel() { return m_ForagingModel; };
    double GetCurrentResource() { return m_CurrentResource; };
    void DoDailyRecovery();
    void UpdateOccupancy(double duration);
    
    double GetOccupancy() { return m_TotalOccupancy; };
    void ResetTotals() { m_TotalOccupancy = 0; };

#ifdef USE_OPENGL
    void Draw();
    void HiLite(bool f) { m_HiLite = f; };
    void HiLite(bool f, GLfloat r, GLfloat g, GLfloat b) { m_HiLite = f; m_RHiLite = r; m_GHiLite = g; m_BHiLite = b; };
#endif

protected:
    int m_YGrid;
    int m_XGrid;
    double m_XCentre;
    double m_YCentre;
    double m_MeanGroundSlope;
    vector<string> m_HabitatTypes;
    bool m_SleepingSite;
    bool m_CliffRefuge;
    bool m_OtherRefuge;
    bool m_WaterPresent;
    double m_Risk;
    double m_Productivity;
    double m_MaxResource;
    double m_CurrentResource;
    double m_ForagingRateFactor;
    double m_MoveForagingRatePenalty;
    double m_RecoveryRate;
    double m_MeanEnergyValue;
    HabitatType::ForagingModel m_ForagingModel;
    double m_FullResource;

    double m_TotalOccupancy;

#ifdef USE_OPENGL
    GLfloat m_R;
    GLfloat m_G;
    GLfloat m_B;
    GLfloat m_RHiLite;
    GLfloat m_GHiLite;
    GLfloat m_BHiLite;
    bool m_HiLite;
#endif
};


#endif

