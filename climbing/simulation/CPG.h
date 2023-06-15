// CPG.h - class to hold the central pattern generator

#ifndef CPG_h
#define CPG_h

#include "MAMuscle.h"

class CPG
{
   public:
   
      CPG();
      ~CPG();
      
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
      
      MAMuscle *GetLeftShoulderAbductor() { return &m_LeftShoulderAbductor; };
      MAMuscle *GetRightShoulderAbductor() { return &m_RightShoulderAbductor; };
      MAMuscle *GetLeftShoulderExtensor() { return &m_LeftShoulderExtensor; };
      MAMuscle *GetRightShoulderExtensor() { return &m_RightShoulderExtensor; };
      MAMuscle *GetLeftShoulderFlexor() { return &m_LeftShoulderFlexor; };
      MAMuscle *GetRightShoulderFlexor() { return &m_RightShoulderFlexor; };
      MAMuscle *GetLeftElbowExtensor() { return &m_LeftElbowExtensor; };
      MAMuscle *GetRightElbowExtensor() { return &m_RightElbowExtensor; };
      MAMuscle *GetLeftElbowFlexor() { return &m_LeftElbowFlexor; };
      MAMuscle *GetRightElbowFlexor() { return &m_RightElbowFlexor; };
      MAMuscle *GetLeftWristExtensor() { return &m_LeftWristExtensor; };
      MAMuscle *GetRightWristExtensor() { return &m_RightWristExtensor; };
      MAMuscle *GetLeftWristFlexor() { return &m_LeftWristFlexor; };
      MAMuscle *GetRightWristFlexor() { return &m_RightWristFlexor; };
      
   protected:
   
      double FeedbackFunction(double x, double p1, double p2);
      void Initialise();
   
      int      m_Phase;
      int      m_NumPhases;
      int      m_NumControllers;
      double   m_TimeInPhase;
      double   m_LastSimTime;
      
      double   *m_Genome;
      int      m_GenomeLength;
      
      double   *m_Durations;
      
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

      double   *m_LeftShoulderAbductorController;
      double   *m_RightShoulderAbductorController;
      double   *m_LeftShoulderExtensorController;
      double   *m_RightShoulderExtensorController;
      double   *m_LeftShoulderFlexorController;
      double   *m_RightShoulderFlexorController;
      double   *m_LeftElbowExtensorController;
      double   *m_RightElbowExtensorController;
      double   *m_LeftElbowFlexorController;
      double   *m_RightElbowFlexorController;
      double   *m_LeftWristExtensorController;
      double   *m_RightWristExtensorController;
      double   *m_LeftWristFlexorController;
      double   *m_RightWristFlexorController;
      
      double   *m_LeftShoulderAbductorLengthFeedbackP1;
      double   *m_RightShoulderAbductorLengthFeedbackP1;
      double   *m_LeftShoulderExtensorLengthFeedbackP1;
      double   *m_RightShoulderExtensorLengthFeedbackP1;
      double   *m_LeftShoulderFlexorLengthFeedbackP1;
      double   *m_RightShoulderFlexorLengthFeedbackP1;
      double   *m_LeftElbowExtensorLengthFeedbackP1;
      double   *m_RightElbowExtensorLengthFeedbackP1;
      double   *m_LeftElbowFlexorLengthFeedbackP1;
      double   *m_RightElbowFlexorLengthFeedbackP1;
      double   *m_LeftWristExtensorLengthFeedbackP1;
      double   *m_RightWristExtensorLengthFeedbackP1;
      double   *m_LeftWristFlexorLengthFeedbackP1;
      double   *m_RightWristFlexorLengthFeedbackP1;
      
      double   *m_LeftShoulderAbductorLengthFeedbackP2;
      double   *m_RightShoulderAbductorLengthFeedbackP2;
      double   *m_LeftShoulderExtensorLengthFeedbackP2;
      double   *m_RightShoulderExtensorLengthFeedbackP2;
      double   *m_LeftShoulderFlexorLengthFeedbackP2;
      double   *m_RightShoulderFlexorLengthFeedbackP2;
      double   *m_LeftElbowExtensorLengthFeedbackP2;
      double   *m_RightElbowExtensorLengthFeedbackP2;
      double   *m_LeftElbowFlexorLengthFeedbackP2;
      double   *m_RightElbowFlexorLengthFeedbackP2;
      double   *m_LeftWristExtensorLengthFeedbackP2;
      double   *m_RightWristExtensorLengthFeedbackP2;
      double   *m_LeftWristFlexorLengthFeedbackP2;
      double   *m_RightWristFlexorLengthFeedbackP2;
      
      MAMuscle  m_LeftShoulderAbductor;
      MAMuscle  m_RightShoulderAbductor;
      MAMuscle  m_LeftShoulderExtensor;
      MAMuscle  m_RightShoulderExtensor;
      MAMuscle  m_LeftShoulderFlexor;
      MAMuscle  m_RightShoulderFlexor;
      MAMuscle  m_LeftElbowExtensor;
      MAMuscle  m_RightElbowExtensor;
      MAMuscle  m_LeftElbowFlexor;
      MAMuscle  m_RightElbowFlexor;
      MAMuscle  m_LeftWristExtensor;
      MAMuscle  m_RightWristExtensor;
      MAMuscle  m_LeftWristFlexor;
      MAMuscle  m_RightWristFlexor;

};
      

#endif // CPG_h
