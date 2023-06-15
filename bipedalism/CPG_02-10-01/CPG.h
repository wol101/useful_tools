// CPG.h - class to hold the central pattern generator

#ifndef CPG_h
#define CPG_h

#include "MAMuscle.h"

class CPG
{
   public:
   
      CPG();
      ~CPG();
      
      void Initialise(int numStartupPhases, int numCyclicPhases, int numAllelesPerPhase);
      void ReadGenome(const char *filename);
      void Update(double simTime);
      void draw();
      
      MAMuscle *GetLeftHipExtensor() { return &m_LeftHipExtensor; };
      MAMuscle *GetRightHipExtensor() { return &m_RightHipExtensor; };
      MAMuscle *GetLeftHipFlexor() { return &m_LeftHipFlexor; };
      MAMuscle *GetRightHipFlexor() { return &m_RightHipFlexor; };
      MAMuscle *GetLeftKneeExtensor() { return &m_LeftKneeExtensor; };
      MAMuscle *GetRightKneeExtensor() { return &m_RightKneeExtensor; };
      MAMuscle *GetLeftKneeFlexor() { return &m_LeftKneeFlexor; };
      MAMuscle *GetRightKneeFlexor() { return &m_RightKneeFlexor; };
      MAMuscle *GetLeftAnkleExtensor() { return &m_LeftAnkleExtensor; };
      MAMuscle *GetRightAnkleExtensor() { return &m_RightAnkleExtensor; };
      MAMuscle *GetLeftAnkleFlexor() { return &m_LeftAnkleFlexor; };
      MAMuscle *GetRightAnkleFlexor() { return &m_RightAnkleFlexor; };
      
      double GetDuration(int phaseNumber) { return m_Durations[phaseNumber]; };
      
      friend int main(int argc, char ** argv);
      
   protected:
   
      double FeedbackFunction(double x, double p1, double p2);
   
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
      
      double   *m_LeftHipExtensorController;
      double   *m_RightHipExtensorController;
      double   *m_LeftHipFlexorController;
      double   *m_RightHipFlexorController;
      double   *m_LeftKneeExtensorController;
      double   *m_RightKneeExtensorController;
      double   *m_LeftKneeFlexorController;
      double   *m_RightKneeFlexorController;
      double   *m_LeftAnkleExtensorController;
      double   *m_RightAnkleExtensorController;
      double   *m_LeftAnkleFlexorController;
      double   *m_RightAnkleFlexorController;
      
      double   *m_LeftHipExtensorLengthFeedbackP1;
      double   *m_RightHipExtensorLengthFeedbackP1;
      double   *m_LeftHipFlexorLengthFeedbackP1;
      double   *m_RightHipFlexorLengthFeedbackP1;
      double   *m_LeftKneeExtensorLengthFeedbackP1;
      double   *m_RightKneeExtensorLengthFeedbackP1;
      double   *m_LeftKneeFlexorLengthFeedbackP1;
      double   *m_RightKneeFlexorLengthFeedbackP1;
      double   *m_LeftAnkleExtensorLengthFeedbackP1;
      double   *m_RightAnkleExtensorLengthFeedbackP1;
      double   *m_LeftAnkleFlexorLengthFeedbackP1;
      double   *m_RightAnkleFlexorLengthFeedbackP1;
      
      double   *m_LeftHipExtensorLengthFeedbackP2;
      double   *m_RightHipExtensorLengthFeedbackP2;
      double   *m_LeftHipFlexorLengthFeedbackP2;
      double   *m_RightHipFlexorLengthFeedbackP2;
      double   *m_LeftKneeExtensorLengthFeedbackP2;
      double   *m_RightKneeExtensorLengthFeedbackP2;
      double   *m_LeftKneeFlexorLengthFeedbackP2;
      double   *m_RightKneeFlexorLengthFeedbackP2;
      double   *m_LeftAnkleExtensorLengthFeedbackP2;
      double   *m_RightAnkleExtensorLengthFeedbackP2;
      double   *m_LeftAnkleFlexorLengthFeedbackP2;
      double   *m_RightAnkleFlexorLengthFeedbackP2;
      
      double   *m_Durations;
      
      MAMuscle  m_LeftHipExtensor;
      MAMuscle  m_RightHipExtensor;
      MAMuscle  m_LeftHipFlexor;
      MAMuscle  m_RightHipFlexor;
      MAMuscle  m_LeftKneeExtensor;
      MAMuscle  m_RightKneeExtensor;
      MAMuscle  m_LeftKneeFlexor;
      MAMuscle  m_RightKneeFlexor;
      MAMuscle  m_LeftAnkleExtensor;
      MAMuscle  m_RightAnkleExtensor;
      MAMuscle  m_LeftAnkleFlexor;
      MAMuscle  m_RightAnkleFlexor;

};
      

#endif // CPG_h
