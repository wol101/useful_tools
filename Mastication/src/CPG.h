// CPG.h - class to hold the central pattern generator

#ifndef CPG_h
#define CPG_h

#include "StrapForce.h"

class CPG
{
   public:
   
      CPG();
      ~CPG();
      
      void Initialise(int numStartupPhases, int numCyclicPhases, int numAllelesPerPhase);
      void SetGenome(int genomeLength, const double *genome);
      void ParseGenome();
      void Update(double simTime);
      void draw();
      
      StrapForce *GetLeftTemporalis() { return &m_LeftTemporalis; };
      StrapForce *GetRightTemporalis() { return &m_RightTemporalis; };
      StrapForce *GetLeftMasseter() { return &m_LeftMasseter; };
      StrapForce *GetRightMasseter() { return &m_RightMasseter; };
      StrapForce *GetLeftMedialPterygoid() { return &m_LeftMedialPterygoid; };
      StrapForce *GetRightMedialPterygoid() { return &m_RightMedialPterygoid; };
      StrapForce *GetLeftLateralPterygoid() { return &m_LeftLateralPterygoid; };
      StrapForce *GetRightLateralPterygoid() { return &m_RightLateralPterygoid; };
      
      double GetDuration(int phaseNumber) { return m_Durations[phaseNumber]; };
      
   protected:
   
      int      m_Phase;
      int      m_NumPhases;
      int      m_NumStartupPhases;
      int      m_NumCyclicPhases;
      int      m_NumAllelesPerPhase;
      int      m_NumControllers;
      double   m_TimeInPhase;
      double   m_LastSimTime;
      
      double   *m_Genome;
      int      m_GenomeLength;
      
      double   *m_LeftTemporalisController;
      double   *m_RightTemporalisController;
      double   *m_LeftMasseterController;
      double   *m_RightMasseterController;
      double   *m_LeftMedialPterygoidController;
      double   *m_RightMedialPterygoidController;
      double   *m_LeftLateralPterygoidController;
      double   *m_RightLateralPterygoidController;
      
      double   *m_Durations;
      
      StrapForce  m_LeftTemporalis;
      StrapForce  m_RightTemporalis;
      StrapForce  m_LeftMasseter;
      StrapForce  m_RightMasseter;
      StrapForce  m_LeftMedialPterygoid;
      StrapForce  m_RightMedialPterygoid;
      StrapForce  m_LeftLateralPterygoid;
      StrapForce  m_RightLateralPterygoid;
};
      

#endif // CPG_h
