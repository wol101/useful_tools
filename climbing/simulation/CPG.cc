// CPG.cc - class to hold the central pattern generator

#include <stdio.h>
#include <iostream.h>
#include <iomanip.h>
#include <fstream.h>
#include <assert.h>
#include <math.h>

#include "CPG.h"
#include "DebugControl.h"
#include "SegmentParameters.h"

// default constructor
CPG::CPG()
{
   m_Phase = 0;
   m_NumPhases = 0;
   m_NumControllers = 0;
   m_TimeInPhase = 0;
   m_LastSimTime = 0;
   m_Genome = 0;
   
   m_LeftHipExtensorController = 0;
   m_RightHipExtensorController = 0;
   m_LeftHipFlexorController = 0;
   m_RightHipFlexorController = 0;
   m_LeftKneeExtensorController = 0;
   m_RightKneeExtensorController = 0;
   m_LeftKneeFlexorController = 0;
   m_RightKneeFlexorController = 0;
   m_LeftAnkleExtensorController = 0;
   m_RightAnkleExtensorController = 0;
   m_LeftAnkleFlexorController = 0;
   m_RightAnkleFlexorController = 0;
   
   m_LeftShoulderAbductorController = 0;
   m_RightShoulderAbductorController = 0;
   m_LeftShoulderExtensorController = 0;
   m_RightShoulderExtensorController = 0;
   m_LeftShoulderFlexorController = 0;
   m_RightShoulderFlexorController = 0;
   m_LeftElbowExtensorController = 0;
   m_RightElbowExtensorController = 0;
   m_LeftElbowFlexorController = 0;
   m_RightElbowFlexorController = 0;
   m_LeftWristExtensorController = 0;
   m_RightWristExtensorController = 0;
   m_LeftWristFlexorController = 0;
   m_RightWristFlexorController = 0;
   
   m_Durations = 0;
}

// default destructor
CPG::~CPG()
{
   if (m_Genome) delete [] m_Genome;
   if (m_NumPhases)
   {
      delete [] m_LeftHipExtensorController;
      delete [] m_RightHipExtensorController;
      delete [] m_LeftHipFlexorController;
      delete [] m_RightHipFlexorController;
      delete [] m_LeftKneeExtensorController;
      delete [] m_RightKneeExtensorController;
      delete [] m_LeftKneeFlexorController;
      delete [] m_RightKneeFlexorController;
      delete [] m_LeftAnkleExtensorController;
      delete [] m_RightAnkleExtensorController;
      delete [] m_LeftAnkleFlexorController;
      delete [] m_RightAnkleFlexorController;
      
      delete [] m_LeftHipExtensorLengthFeedbackP1;
      delete [] m_RightHipExtensorLengthFeedbackP1;
      delete [] m_LeftHipFlexorLengthFeedbackP1;
      delete [] m_RightHipFlexorLengthFeedbackP1;
      delete [] m_LeftKneeExtensorLengthFeedbackP1;
      delete [] m_RightKneeExtensorLengthFeedbackP1;
      delete [] m_LeftKneeFlexorLengthFeedbackP1;
      delete [] m_RightKneeFlexorLengthFeedbackP1;
      delete [] m_LeftAnkleExtensorLengthFeedbackP1;
      delete [] m_RightAnkleExtensorLengthFeedbackP1;
      delete [] m_LeftAnkleFlexorLengthFeedbackP1;
      delete [] m_RightAnkleFlexorLengthFeedbackP1;
      
      delete [] m_LeftHipExtensorLengthFeedbackP2;
      delete [] m_RightHipExtensorLengthFeedbackP2;
      delete [] m_LeftHipFlexorLengthFeedbackP2;
      delete [] m_RightHipFlexorLengthFeedbackP2;
      delete [] m_LeftKneeExtensorLengthFeedbackP2;
      delete [] m_RightKneeExtensorLengthFeedbackP2;
      delete [] m_LeftKneeFlexorLengthFeedbackP2;
      delete [] m_RightKneeFlexorLengthFeedbackP2;
      delete [] m_LeftAnkleExtensorLengthFeedbackP2;
      delete [] m_RightAnkleExtensorLengthFeedbackP2;
      delete [] m_LeftAnkleFlexorLengthFeedbackP2;
      delete [] m_RightAnkleFlexorLengthFeedbackP2;
      
      delete [] m_LeftShoulderAbductorController;
      delete [] m_RightShoulderAbductorController;
      delete [] m_LeftShoulderExtensorController;
      delete [] m_RightShoulderExtensorController;
      delete [] m_LeftShoulderFlexorController;
      delete [] m_RightShoulderFlexorController;
      delete [] m_LeftElbowExtensorController;
      delete [] m_RightElbowExtensorController;
      delete [] m_LeftElbowFlexorController;
      delete [] m_RightElbowFlexorController;
      delete [] m_LeftWristExtensorController;
      delete [] m_RightWristExtensorController;
      delete [] m_LeftWristFlexorController;
      delete [] m_RightWristFlexorController;
      
      delete [] m_LeftShoulderAbductorLengthFeedbackP1;
      delete [] m_RightShoulderAbductorLengthFeedbackP1;
      delete [] m_LeftShoulderExtensorLengthFeedbackP1;
      delete [] m_RightShoulderExtensorLengthFeedbackP1;
      delete [] m_LeftShoulderFlexorLengthFeedbackP1;
      delete [] m_RightShoulderFlexorLengthFeedbackP1;
      delete [] m_LeftElbowExtensorLengthFeedbackP1;
      delete [] m_RightElbowExtensorLengthFeedbackP1;
      delete [] m_LeftElbowFlexorLengthFeedbackP1;
      delete [] m_RightElbowFlexorLengthFeedbackP1;
      delete [] m_LeftWristExtensorLengthFeedbackP1;
      delete [] m_RightWristExtensorLengthFeedbackP1;
      delete [] m_LeftWristFlexorLengthFeedbackP1;
      delete [] m_RightWristFlexorLengthFeedbackP1;
      
      delete [] m_LeftShoulderAbductorLengthFeedbackP2;
      delete [] m_RightShoulderAbductorLengthFeedbackP2;
      delete [] m_LeftShoulderExtensorLengthFeedbackP2;
      delete [] m_RightShoulderExtensorLengthFeedbackP2;
      delete [] m_LeftShoulderFlexorLengthFeedbackP2;
      delete [] m_RightShoulderFlexorLengthFeedbackP2;
      delete [] m_LeftElbowExtensorLengthFeedbackP2;
      delete [] m_RightElbowExtensorLengthFeedbackP2;
      delete [] m_LeftElbowFlexorLengthFeedbackP2;
      delete [] m_RightElbowFlexorLengthFeedbackP2;
      delete [] m_LeftWristExtensorLengthFeedbackP2;
      delete [] m_RightWristExtensorLengthFeedbackP2;
      delete [] m_LeftWristFlexorLengthFeedbackP2;
      delete [] m_RightWristFlexorLengthFeedbackP2;
      
      delete [] m_Durations;
   }
}

// set up the controller data structures
void 
CPG::Initialise()
{
   assert(m_NumPhases != 0);
   
   m_LeftHipExtensorController = new double[m_NumPhases];
   m_RightHipExtensorController = new double[m_NumPhases];
   m_LeftHipFlexorController = new double[m_NumPhases];
   m_RightHipFlexorController = new double[m_NumPhases];
   m_LeftKneeExtensorController = new double[m_NumPhases];
   m_RightKneeExtensorController = new double[m_NumPhases];
   m_LeftKneeFlexorController = new double[m_NumPhases];
   m_RightKneeFlexorController = new double[m_NumPhases];
   m_LeftAnkleExtensorController = new double[m_NumPhases];
   m_RightAnkleExtensorController = new double[m_NumPhases];
   m_LeftAnkleFlexorController = new double[m_NumPhases];
   m_RightAnkleFlexorController = new double[m_NumPhases];
   
   m_LeftHipExtensorLengthFeedbackP1 = new double[m_NumPhases];
   m_RightHipExtensorLengthFeedbackP1 = new double[m_NumPhases];
   m_LeftHipFlexorLengthFeedbackP1 = new double[m_NumPhases];
   m_RightHipFlexorLengthFeedbackP1 = new double[m_NumPhases];
   m_LeftKneeExtensorLengthFeedbackP1 = new double[m_NumPhases];
   m_RightKneeExtensorLengthFeedbackP1 = new double[m_NumPhases];
   m_LeftKneeFlexorLengthFeedbackP1 = new double[m_NumPhases];
   m_RightKneeFlexorLengthFeedbackP1 = new double[m_NumPhases];
   m_LeftAnkleExtensorLengthFeedbackP1 = new double[m_NumPhases];
   m_RightAnkleExtensorLengthFeedbackP1 = new double[m_NumPhases];
   m_LeftAnkleFlexorLengthFeedbackP1 = new double[m_NumPhases];
   m_RightAnkleFlexorLengthFeedbackP1 = new double[m_NumPhases];
   
   m_LeftHipExtensorLengthFeedbackP2 = new double[m_NumPhases];
   m_RightHipExtensorLengthFeedbackP2 = new double[m_NumPhases];
   m_LeftHipFlexorLengthFeedbackP2 = new double[m_NumPhases];
   m_RightHipFlexorLengthFeedbackP2 = new double[m_NumPhases];
   m_LeftKneeExtensorLengthFeedbackP2 = new double[m_NumPhases];
   m_RightKneeExtensorLengthFeedbackP2 = new double[m_NumPhases];
   m_LeftKneeFlexorLengthFeedbackP2 = new double[m_NumPhases];
   m_RightKneeFlexorLengthFeedbackP2 = new double[m_NumPhases];
   m_LeftAnkleExtensorLengthFeedbackP2 = new double[m_NumPhases];
   m_RightAnkleExtensorLengthFeedbackP2 = new double[m_NumPhases];
   m_LeftAnkleFlexorLengthFeedbackP2 = new double[m_NumPhases];
   m_RightAnkleFlexorLengthFeedbackP2 = new double[m_NumPhases];
   
   m_LeftShoulderAbductorController = new double[m_NumPhases];
   m_RightShoulderAbductorController = new double[m_NumPhases];
   m_LeftShoulderExtensorController = new double[m_NumPhases];
   m_RightShoulderExtensorController = new double[m_NumPhases];
   m_LeftShoulderFlexorController = new double[m_NumPhases];
   m_RightShoulderFlexorController = new double[m_NumPhases];
   m_LeftElbowExtensorController = new double[m_NumPhases];
   m_RightElbowExtensorController = new double[m_NumPhases];
   m_LeftElbowFlexorController = new double[m_NumPhases];
   m_RightElbowFlexorController = new double[m_NumPhases];
   m_LeftWristExtensorController = new double[m_NumPhases];
   m_RightWristExtensorController = new double[m_NumPhases];
   m_LeftWristFlexorController = new double[m_NumPhases];
   m_RightWristFlexorController = new double[m_NumPhases];
   
   m_LeftShoulderAbductorLengthFeedbackP1 = new double[m_NumPhases];
   m_RightShoulderAbductorLengthFeedbackP1 = new double[m_NumPhases];
   m_LeftShoulderExtensorLengthFeedbackP1 = new double[m_NumPhases];
   m_RightShoulderExtensorLengthFeedbackP1 = new double[m_NumPhases];
   m_LeftShoulderFlexorLengthFeedbackP1 = new double[m_NumPhases];
   m_RightShoulderFlexorLengthFeedbackP1 = new double[m_NumPhases];
   m_LeftElbowExtensorLengthFeedbackP1 = new double[m_NumPhases];
   m_RightElbowExtensorLengthFeedbackP1 = new double[m_NumPhases];
   m_LeftElbowFlexorLengthFeedbackP1 = new double[m_NumPhases];
   m_RightElbowFlexorLengthFeedbackP1 = new double[m_NumPhases];
   m_LeftWristExtensorLengthFeedbackP1 = new double[m_NumPhases];
   m_RightWristExtensorLengthFeedbackP1 = new double[m_NumPhases];
   m_LeftWristFlexorLengthFeedbackP1 = new double[m_NumPhases];
   m_RightWristFlexorLengthFeedbackP1 = new double[m_NumPhases];
   
   m_LeftShoulderAbductorLengthFeedbackP2 = new double[m_NumPhases];
   m_RightShoulderAbductorLengthFeedbackP2 = new double[m_NumPhases];
   m_LeftShoulderExtensorLengthFeedbackP2 = new double[m_NumPhases];
   m_RightShoulderExtensorLengthFeedbackP2 = new double[m_NumPhases];
   m_LeftShoulderFlexorLengthFeedbackP2 = new double[m_NumPhases];
   m_RightShoulderFlexorLengthFeedbackP2 = new double[m_NumPhases];
   m_LeftElbowExtensorLengthFeedbackP2 = new double[m_NumPhases];
   m_RightElbowExtensorLengthFeedbackP2 = new double[m_NumPhases];
   m_LeftElbowFlexorLengthFeedbackP2 = new double[m_NumPhases];
   m_RightElbowFlexorLengthFeedbackP2 = new double[m_NumPhases];
   m_LeftWristExtensorLengthFeedbackP2 = new double[m_NumPhases];
   m_RightWristExtensorLengthFeedbackP2 = new double[m_NumPhases];
   m_LeftWristFlexorLengthFeedbackP2 = new double[m_NumPhases];
   m_RightWristFlexorLengthFeedbackP2 = new double[m_NumPhases];
   
   m_Durations = new double[m_NumPhases];
   
}

// read in the genome
void
CPG::ReadGenome(const char *filename)
{
  int i, j;
  int numAllelesPerPhase;
  
  // read the gene data
  
  ifstream inFile(filename);
  assert(inFile.good());
  inFile >> m_GenomeLength;
  m_Genome = new double[m_GenomeLength];
  for (i = 0; i < m_GenomeLength; i++)
    inFile >> m_Genome[i];
  inFile.close();
  
  // check for possible phase lengths
  if (m_GenomeLength % 27 == 0) numAllelesPerPhase = 27;
  else
  if (m_GenomeLength % 53 == 0) numAllelesPerPhase = 53;
  else
  if (m_GenomeLength % 79 == 0) numAllelesPerPhase = 79;
  else
  if (m_GenomeLength % 14 == 0) numAllelesPerPhase = 14;
  else
  {
     cerr << "Invalid genome length";
     exit(1);
  }
  
  m_NumPhases = m_GenomeLength / numAllelesPerPhase;
 
  // special case treatment for numAllelesPerPhase == 14
  if (numAllelesPerPhase == 14)
  {
     numAllelesPerPhase = 27;
     m_GenomeLength = m_NumPhases * numAllelesPerPhase;
     double *tempGenome = new double[m_GenomeLength];
     for (i = 0; i < m_NumPhases; i++)
     {
        tempGenome[i * 27] = m_Genome[i * 14];
        for (j = 0; j < 6; j++) tempGenome[i * 27 + j + 1] = m_Genome[i * 14 + j + 1];
        for (j = 0; j < 6; j++) tempGenome[i * 27 + j + 7] = m_Genome[i * 14 + j + 1];
        for (j = 0; j < 7; j++) tempGenome[i * 27 + j + 13] = m_Genome[i * 14 + j + 7];
        for (j = 0; j < 7; j++) tempGenome[i * 27 + j + 20] = m_Genome[i * 14 + j + 7];
     }

     if (gDebug == CPGDebug)
     {
       cerr << "CPG::ReadGenome\ttempGenome";
       for (i = 0; i < m_NumPhases; i++)
       {
         for (j = 0; j < numAllelesPerPhase; j++)
	   cerr << "\t" << tempGenome[i * numAllelesPerPhase + j];
       }
       cerr << "\n";
     }

     delete [] m_Genome;
     m_Genome = tempGenome;
  }
  
  // allocate memory for controllers
  Initialise();
          
  // assign controller values from genome
  int genomeOffset = 0;
  for (i = 0; i < m_NumPhases; i++)
  {
     m_Durations[i] = fabs(m_Genome[genomeOffset++]);
     
     m_RightHipFlexorController[i] = fabs(m_Genome[genomeOffset++]);
     m_RightHipExtensorController[i] = fabs(m_Genome[genomeOffset++]);
     m_RightKneeFlexorController[i] = fabs(m_Genome[genomeOffset++]);
     m_RightKneeExtensorController[i] = fabs(m_Genome[genomeOffset++]);
     m_RightAnkleFlexorController[i] = fabs(m_Genome[genomeOffset++]);
     m_RightAnkleExtensorController[i] = fabs(m_Genome[genomeOffset++]);
     m_LeftHipFlexorController[i] = fabs(m_Genome[genomeOffset++]);
     m_LeftHipExtensorController[i] = fabs(m_Genome[genomeOffset++]);
     m_LeftKneeFlexorController[i] = fabs(m_Genome[genomeOffset++]);
     m_LeftKneeExtensorController[i] = fabs(m_Genome[genomeOffset++]);
     m_LeftAnkleFlexorController[i] = fabs(m_Genome[genomeOffset++]);
     m_LeftAnkleExtensorController[i] = fabs(m_Genome[genomeOffset++]);
     
     m_RightShoulderAbductorController[i] = fabs(m_Genome[genomeOffset++]);
     m_RightShoulderFlexorController[i] = fabs(m_Genome[genomeOffset++]);
     m_RightShoulderExtensorController[i] = fabs(m_Genome[genomeOffset++]);
     m_RightElbowFlexorController[i] = fabs(m_Genome[genomeOffset++]);
     m_RightElbowExtensorController[i] = fabs(m_Genome[genomeOffset++]);
     m_RightWristFlexorController[i] = fabs(m_Genome[genomeOffset++]);
     m_RightWristExtensorController[i] = fabs(m_Genome[genomeOffset++]);
     m_LeftShoulderAbductorController[i] = fabs(m_Genome[genomeOffset++]);
     m_LeftShoulderFlexorController[i] = fabs(m_Genome[genomeOffset++]);
     m_LeftShoulderExtensorController[i] = fabs(m_Genome[genomeOffset++]);
     m_LeftElbowFlexorController[i] = fabs(m_Genome[genomeOffset++]);
     m_LeftElbowExtensorController[i] = fabs(m_Genome[genomeOffset++]);
     m_LeftWristFlexorController[i] = fabs(m_Genome[genomeOffset++]);
     m_LeftWristExtensorController[i] = fabs(m_Genome[genomeOffset++]);

     if (numAllelesPerPhase == 27 || numAllelesPerPhase == 53)
     {
        m_RightHipFlexorLengthFeedbackP1[i] = 1.0;
        m_RightHipExtensorLengthFeedbackP1[i] = 1.0;
        m_RightKneeFlexorLengthFeedbackP1[i] = 1.0;
        m_RightKneeExtensorLengthFeedbackP1[i] = 1.0;
        m_RightAnkleFlexorLengthFeedbackP1[i] = 1.0;
        m_RightAnkleExtensorLengthFeedbackP1[i] = 1.0;
        m_LeftHipFlexorLengthFeedbackP1[i] = 1.0;
        m_LeftHipExtensorLengthFeedbackP1[i] = 1.0;
        m_LeftKneeFlexorLengthFeedbackP1[i] = 1.0;
        m_LeftKneeExtensorLengthFeedbackP1[i] = 1.0;
        m_LeftAnkleFlexorLengthFeedbackP1[i] = 1.0;
        m_LeftAnkleExtensorLengthFeedbackP1[i] = 1.0;

        m_RightShoulderAbductorLengthFeedbackP1[i] = 1.0;
        m_RightShoulderFlexorLengthFeedbackP1[i] = 1.0;
        m_RightShoulderExtensorLengthFeedbackP1[i] = 1.0;
        m_RightElbowFlexorLengthFeedbackP1[i] = 1.0;
        m_RightElbowExtensorLengthFeedbackP1[i] = 1.0;
        m_RightWristFlexorLengthFeedbackP1[i] = 1.0;
        m_RightWristExtensorLengthFeedbackP1[i] = 1.0;
        m_LeftShoulderAbductorLengthFeedbackP1[i] = 1.0;
        m_LeftShoulderFlexorLengthFeedbackP1[i] = 1.0;
        m_LeftShoulderExtensorLengthFeedbackP1[i] = 1.0;
        m_LeftElbowFlexorLengthFeedbackP1[i] = 1.0;
        m_LeftElbowExtensorLengthFeedbackP1[i] = 1.0;
        m_LeftWristFlexorLengthFeedbackP1[i] = 1.0;
        m_LeftWristExtensorLengthFeedbackP1[i] = 1.0;
     }
     else
     {
        m_RightHipFlexorLengthFeedbackP1[i] = fabs(m_Genome[genomeOffset++]);
        m_RightHipExtensorLengthFeedbackP1[i] = fabs(m_Genome[genomeOffset++]);
        m_RightKneeFlexorLengthFeedbackP1[i] = fabs(m_Genome[genomeOffset++]);
        m_RightKneeExtensorLengthFeedbackP1[i] = fabs(m_Genome[genomeOffset++]);
        m_RightAnkleFlexorLengthFeedbackP1[i] = fabs(m_Genome[genomeOffset++]);
        m_RightAnkleExtensorLengthFeedbackP1[i] = fabs(m_Genome[genomeOffset++]);
        m_LeftHipFlexorLengthFeedbackP1[i] = fabs(m_Genome[genomeOffset++]);
        m_LeftHipExtensorLengthFeedbackP1[i] = fabs(m_Genome[genomeOffset++]);
        m_LeftKneeFlexorLengthFeedbackP1[i] = fabs(m_Genome[genomeOffset++]);
        m_LeftKneeExtensorLengthFeedbackP1[i] = fabs(m_Genome[genomeOffset++]);
        m_LeftAnkleFlexorLengthFeedbackP1[i] = fabs(m_Genome[genomeOffset++]);
        m_LeftAnkleExtensorLengthFeedbackP1[i] = fabs(m_Genome[genomeOffset++]);

        m_RightShoulderAbductorLengthFeedbackP1[i] = fabs(m_Genome[genomeOffset++]);
        m_RightShoulderFlexorLengthFeedbackP1[i] = fabs(m_Genome[genomeOffset++]);
        m_RightShoulderExtensorLengthFeedbackP1[i] = fabs(m_Genome[genomeOffset++]);
        m_RightElbowFlexorLengthFeedbackP1[i] = fabs(m_Genome[genomeOffset++]);
        m_RightElbowExtensorLengthFeedbackP1[i] = fabs(m_Genome[genomeOffset++]);
        m_RightWristFlexorLengthFeedbackP1[i] = fabs(m_Genome[genomeOffset++]);
        m_RightWristExtensorLengthFeedbackP1[i] = fabs(m_Genome[genomeOffset++]);
        m_LeftShoulderAbductorLengthFeedbackP1[i] = fabs(m_Genome[genomeOffset++]);
        m_LeftShoulderFlexorLengthFeedbackP1[i] = fabs(m_Genome[genomeOffset++]);
        m_LeftShoulderExtensorLengthFeedbackP1[i] = fabs(m_Genome[genomeOffset++]);
        m_LeftElbowFlexorLengthFeedbackP1[i] = fabs(m_Genome[genomeOffset++]);
        m_LeftElbowExtensorLengthFeedbackP1[i] = fabs(m_Genome[genomeOffset++]);
        m_LeftWristFlexorLengthFeedbackP1[i] = fabs(m_Genome[genomeOffset++]);
        m_LeftWristExtensorLengthFeedbackP1[i] = fabs(m_Genome[genomeOffset++]);
     } 
     
     if (numAllelesPerPhase == 27)
     {
        m_RightHipFlexorLengthFeedbackP2[i] = 0.5 * 1.0 * kRightHipFlexorLength + kRightHipFlexorLength;
        m_RightHipExtensorLengthFeedbackP2[i] = 0.5 * 1.0 * kRightHipExtensorLength + kRightHipExtensorLength;
        m_RightKneeFlexorLengthFeedbackP2[i] = 0.5 * 1.0 * kRightKneeFlexorLength + kRightKneeFlexorLength;
        m_RightKneeExtensorLengthFeedbackP2[i] = 0.5 * 1.0 * kRightKneeExtensorLength + kRightKneeExtensorLength;
        m_RightAnkleFlexorLengthFeedbackP2[i] = 0.5 * 1.0 * kRightAnkleFlexorLength + kRightAnkleFlexorLength;
        m_RightAnkleExtensorLengthFeedbackP2[i] = 0.5 * 1.0 * kRightAnkleExtensorLength + kRightAnkleExtensorLength;
        m_LeftHipFlexorLengthFeedbackP2[i] = 0.5 * 1.0 * kLeftHipFlexorLength + kLeftHipFlexorLength;
        m_LeftHipExtensorLengthFeedbackP2[i] = 0.5 * 1.0 * kLeftHipExtensorLength + kLeftHipExtensorLength;
        m_LeftKneeFlexorLengthFeedbackP2[i] = 0.5 * 1.0 * kLeftKneeFlexorLength + kLeftKneeFlexorLength;
        m_LeftKneeExtensorLengthFeedbackP2[i] = 0.5 * 1.0 * kLeftKneeExtensorLength + kLeftKneeExtensorLength;
        m_LeftAnkleFlexorLengthFeedbackP2[i] = 0.5 * 1.0 * kLeftAnkleFlexorLength + kLeftAnkleFlexorLength;
        m_LeftAnkleExtensorLengthFeedbackP2[i] = 0.5 * 1.0 * kLeftAnkleExtensorLength + kLeftAnkleExtensorLength;

        m_RightShoulderAbductorLengthFeedbackP2[i] = 0.5 * 1.0 * kRightShoulderAbductorLength + kRightShoulderAbductorLength;
        m_RightShoulderFlexorLengthFeedbackP2[i] = 0.5 * 1.0 * kRightShoulderFlexorLength + kRightShoulderFlexorLength;
        m_RightShoulderExtensorLengthFeedbackP2[i] = 0.5 * 1.0 * kRightShoulderExtensorLength + kRightShoulderExtensorLength;
        m_RightElbowFlexorLengthFeedbackP2[i] = 0.5 * 1.0 * kRightElbowFlexorLength + kRightElbowFlexorLength;
        m_RightElbowExtensorLengthFeedbackP2[i] = 0.5 * 1.0 * kRightElbowExtensorLength + kRightElbowExtensorLength;
        m_RightWristFlexorLengthFeedbackP2[i] = 0.5 * 1.0 * kRightWristFlexorLength + kRightWristFlexorLength;
        m_RightWristExtensorLengthFeedbackP2[i] = 0.5 * 1.0 * kRightWristExtensorLength + kRightWristExtensorLength;
        m_LeftShoulderAbductorLengthFeedbackP2[i] = 0.5 * 1.0 * kLeftShoulderAbductorLength + kLeftShoulderAbductorLength;
        m_LeftShoulderFlexorLengthFeedbackP2[i] = 0.5 * 1.0 * kLeftShoulderFlexorLength + kLeftShoulderFlexorLength;
        m_LeftShoulderExtensorLengthFeedbackP2[i] = 0.5 * 1.0 * kLeftShoulderExtensorLength + kLeftShoulderExtensorLength;
        m_LeftElbowFlexorLengthFeedbackP2[i] = 0.5 * 1.0 * kLeftElbowFlexorLength + kLeftElbowFlexorLength;
        m_LeftElbowExtensorLengthFeedbackP2[i] = 0.5 * 1.0 * kLeftElbowExtensorLength + kLeftElbowExtensorLength;
        m_LeftWristFlexorLengthFeedbackP2[i] = 0.5 * 1.0 * kLeftWristFlexorLength + kLeftWristFlexorLength;
        m_LeftWristExtensorLengthFeedbackP2[i] = 0.5 * 1.0 * kLeftWristExtensorLength + kLeftWristExtensorLength;
     }
     else
     {
        m_RightHipFlexorLengthFeedbackP2[i] = 0.5 * m_Genome[genomeOffset++] * kRightHipFlexorLength + kRightHipFlexorLength;
        m_RightHipExtensorLengthFeedbackP2[i] = 0.5 * m_Genome[genomeOffset++] * kRightHipExtensorLength + kRightHipExtensorLength;
        m_RightKneeFlexorLengthFeedbackP2[i] = 0.5 * m_Genome[genomeOffset++] * kRightKneeFlexorLength + kRightKneeFlexorLength;
        m_RightKneeExtensorLengthFeedbackP2[i] = 0.5 * m_Genome[genomeOffset++] * kRightKneeExtensorLength + kRightKneeExtensorLength;
        m_RightAnkleFlexorLengthFeedbackP2[i] = 0.5 * m_Genome[genomeOffset++] * kRightAnkleFlexorLength + kRightAnkleFlexorLength;
        m_RightAnkleExtensorLengthFeedbackP2[i] = 0.5 * m_Genome[genomeOffset++] * kRightAnkleExtensorLength + kRightAnkleExtensorLength;
        m_LeftHipFlexorLengthFeedbackP2[i] = 0.5 * m_Genome[genomeOffset++] * kLeftHipFlexorLength + kLeftHipFlexorLength;
        m_LeftHipExtensorLengthFeedbackP2[i] = 0.5 * m_Genome[genomeOffset++] * kLeftHipExtensorLength + kLeftHipExtensorLength;
        m_LeftKneeFlexorLengthFeedbackP2[i] = 0.5 * m_Genome[genomeOffset++] * kLeftKneeFlexorLength + kLeftKneeFlexorLength;
        m_LeftKneeExtensorLengthFeedbackP2[i] = 0.5 * m_Genome[genomeOffset++] * kLeftKneeExtensorLength + kLeftKneeExtensorLength;
        m_LeftAnkleFlexorLengthFeedbackP2[i] = 0.5 * m_Genome[genomeOffset++] * kLeftAnkleFlexorLength + kLeftAnkleFlexorLength;
        m_LeftAnkleExtensorLengthFeedbackP2[i] = 0.5 * m_Genome[genomeOffset++] * kLeftAnkleExtensorLength + kLeftAnkleExtensorLength;

        m_RightShoulderAbductorLengthFeedbackP2[i] = 0.5 * m_Genome[genomeOffset++] * kRightShoulderAbductorLength + kRightShoulderAbductorLength;
        m_RightShoulderFlexorLengthFeedbackP2[i] = 0.5 * m_Genome[genomeOffset++] * kRightShoulderFlexorLength + kRightShoulderFlexorLength;
        m_RightShoulderExtensorLengthFeedbackP2[i] = 0.5 * m_Genome[genomeOffset++] * kRightShoulderExtensorLength + kRightShoulderExtensorLength;
        m_RightElbowFlexorLengthFeedbackP2[i] = 0.5 * m_Genome[genomeOffset++] * kRightElbowFlexorLength + kRightElbowFlexorLength;
        m_RightElbowExtensorLengthFeedbackP2[i] = 0.5 * m_Genome[genomeOffset++] * kRightElbowExtensorLength + kRightElbowExtensorLength;
        m_RightWristFlexorLengthFeedbackP2[i] = 0.5 * m_Genome[genomeOffset++] * kRightWristFlexorLength + kRightWristFlexorLength;
        m_RightWristExtensorLengthFeedbackP2[i] = 0.5 * m_Genome[genomeOffset++] * kRightWristExtensorLength + kRightWristExtensorLength;
        m_LeftShoulderAbductorLengthFeedbackP2[i] = 0.5 * m_Genome[genomeOffset++] * kLeftShoulderAbductorLength + kLeftShoulderAbductorLength;
        m_LeftShoulderFlexorLengthFeedbackP2[i] = 0.5 * m_Genome[genomeOffset++] * kLeftShoulderFlexorLength + kLeftShoulderFlexorLength;
        m_LeftShoulderExtensorLengthFeedbackP2[i] = 0.5 * m_Genome[genomeOffset++] * kLeftShoulderExtensorLength + kLeftShoulderExtensorLength;
        m_LeftElbowFlexorLengthFeedbackP2[i] = 0.5 * m_Genome[genomeOffset++] * kLeftElbowFlexorLength + kLeftElbowFlexorLength;
        m_LeftElbowExtensorLengthFeedbackP2[i] = 0.5 * m_Genome[genomeOffset++] * kLeftElbowExtensorLength + kLeftElbowExtensorLength;
        m_LeftWristFlexorLengthFeedbackP2[i] = 0.5 * m_Genome[genomeOffset++] * kLeftWristFlexorLength + kLeftWristFlexorLength;
        m_LeftWristExtensorLengthFeedbackP2[i] = 0.5 * m_Genome[genomeOffset++] * kLeftWristExtensorLength + kLeftWristExtensorLength;
     } 
  }
     
  if (gDebug == CPGDebug)
  {
     cerr << "CPG::ReadGenome\tfilename\t" << filename << "\n";
     for (i = 0; i < m_NumPhases; i ++)
     {
        cerr << "CPG::ReadGenome\tm_Durations[" << i << "]\t"
        << m_Durations[i] << "\n";
        
        cerr << "CPG::ReadGenome\tm_RightHipFlexorController[" << i << "]\t"
        << m_RightHipFlexorController[i] << "\n";
        cerr << "CPG::ReadGenome\tm_RightHipExtensorController[" << i << "]\t"
        << m_RightHipExtensorController[i] << "\n";
        cerr << "CPG::ReadGenome\tm_RightKneeFlexorController[" << i << "]\t"
        << m_RightKneeFlexorController[i] << "\n";
        cerr << "CPG::ReadGenome\tm_RightKneeExtensorController[" << i << "]\t"
        << m_RightKneeExtensorController[i] << "\n";
        cerr << "CPG::ReadGenome\tm_RightAnkleFlexorController[" << i << "]\t"
        << m_RightAnkleFlexorController[i] << "\n";
        cerr << "CPG::ReadGenome\tm_RightAnkleExtensorController[" << i << "]\t"
        << m_RightAnkleExtensorController[i] << "\n";
        cerr << "CPG::ReadGenome\tm_LeftHipFlexorController[" << i << "]\t"
        << m_LeftHipFlexorController[i] << "\n";
        cerr << "CPG::ReadGenome\tm_LeftHipExtensorController[" << i << "]\t"
        << m_LeftHipExtensorController[i] << "\n";
        cerr << "CPG::ReadGenome\tm_LeftKneeFlexorController[" << i << "]\t"
        << m_LeftKneeFlexorController[i] << "\n";
        cerr << "CPG::ReadGenome\tm_LeftKneeExtensorController[" << i << "]\t"
        << m_LeftKneeExtensorController[i] << "\n";
        cerr << "CPG::ReadGenome\tm_LeftAnkleFlexorController[" << i << "]\t"
        << m_LeftAnkleFlexorController[i] << "\n";
        cerr << "CPG::ReadGenome\tm_LeftAnkleExtensorController[" << i << "]\t"
        << m_LeftAnkleExtensorController[i] << "\n";

        cerr << "CPG::ReadGenome\tm_RightShoulderAbductorController[" << i << "]\t"
        << m_RightShoulderAbductorController[i] << "\n";
        cerr << "CPG::ReadGenome\tm_RightShoulderFlexorController[" << i << "]\t"
        << m_RightShoulderFlexorController[i] << "\n";
        cerr << "CPG::ReadGenome\tm_RightShoulderExtensorController[" << i << "]\t"
        << m_RightShoulderExtensorController[i] << "\n";
        cerr << "CPG::ReadGenome\tm_RightElbowFlexorController[" << i << "]\t"
        << m_RightElbowFlexorController[i] << "\n";
        cerr << "CPG::ReadGenome\tm_RightElbowExtensorController[" << i << "]\t"
        << m_RightElbowExtensorController[i] << "\n";
        cerr << "CPG::ReadGenome\tm_RightWristFlexorController[" << i << "]\t"
        << m_RightWristFlexorController[i] << "\n";
        cerr << "CPG::ReadGenome\tm_RightWristExtensorController[" << i << "]\t"
        << m_RightWristExtensorController[i] << "\n";
        cerr << "CPG::ReadGenome\tm_LeftShoulderAbductorController[" << i << "]\t"
        << m_LeftShoulderAbductorController[i] << "\n";
        cerr << "CPG::ReadGenome\tm_LeftShoulderFlexorController[" << i << "]\t"
        << m_LeftShoulderFlexorController[i] << "\n";
        cerr << "CPG::ReadGenome\tm_LeftShoulderExtensorController[" << i << "]\t"
        << m_LeftShoulderExtensorController[i] << "\n";
        cerr << "CPG::ReadGenome\tm_LeftElbowFlexorController[" << i << "]\t"
        << m_LeftElbowFlexorController[i] << "\n";
        cerr << "CPG::ReadGenome\tm_LeftElbowExtensorController[" << i << "]\t"
        << m_LeftElbowExtensorController[i] << "\n";
        cerr << "CPG::ReadGenome\tm_LeftWristFlexorController[" << i << "]\t"
        << m_LeftWristFlexorController[i] << "\n";
        cerr << "CPG::ReadGenome\tm_LeftWristExtensorController[" << i << "]\t"
        << m_LeftWristExtensorController[i] << "\n";

        cerr << "CPG::ReadGenome\tm_RightHipFlexorLengthFeedbackP1[" << i << "]\t"
        << m_RightHipFlexorLengthFeedbackP1[i] << "\n";
        cerr << "CPG::ReadGenome\tm_RightHipExtensorLengthFeedbackP1[" << i << "]\t"
        << m_RightHipExtensorLengthFeedbackP1[i] << "\n";
        cerr << "CPG::ReadGenome\tm_RightKneeFlexorLengthFeedbackP1[" << i << "]\t"
        << m_RightKneeFlexorLengthFeedbackP1[i] << "\n";
        cerr << "CPG::ReadGenome\tm_RightKneeExtensorLengthFeedbackP1[" << i << "]\t"
        << m_RightKneeExtensorLengthFeedbackP1[i] << "\n";
        cerr << "CPG::ReadGenome\tm_RightAnkleFlexorLengthFeedbackP1[" << i << "]\t"
        << m_RightAnkleFlexorLengthFeedbackP1[i] << "\n";
        cerr << "CPG::ReadGenome\tm_RightAnkleExtensorLengthFeedbackP1[" << i << "]\t"
        << m_RightAnkleExtensorLengthFeedbackP1[i] << "\n";
        cerr << "CPG::ReadGenome\tm_LeftHipFlexorLengthFeedbackP1[" << i << "]\t"
        << m_LeftHipFlexorLengthFeedbackP1[i] << "\n";
        cerr << "CPG::ReadGenome\tm_LeftHipExtensorLengthFeedbackP1[" << i << "]\t"
        << m_LeftHipExtensorLengthFeedbackP1[i] << "\n";
        cerr << "CPG::ReadGenome\tm_LeftKneeFlexorLengthFeedbackP1[" << i << "]\t"
        << m_LeftKneeFlexorLengthFeedbackP1[i] << "\n";
        cerr << "CPG::ReadGenome\tm_LeftKneeExtensorLengthFeedbackP1[" << i << "]\t"
        << m_LeftKneeExtensorLengthFeedbackP1[i] << "\n";
        cerr << "CPG::ReadGenome\tm_LeftAnkleFlexorLengthFeedbackP1[" << i << "]\t"
        << m_LeftAnkleFlexorLengthFeedbackP1[i] << "\n";
        cerr << "CPG::ReadGenome\tm_LeftAnkleExtensorLengthFeedbackP1[" << i << "]\t"
        << m_LeftAnkleExtensorLengthFeedbackP1[i] << "\n";

        cerr << "CPG::ReadGenome\tm_RightShoulderAbductorLengthFeedbackP1[" << i << "]\t"
        << m_RightShoulderAbductorLengthFeedbackP1[i] << "\n";
        cerr << "CPG::ReadGenome\tm_RightShoulderFlexorLengthFeedbackP1[" << i << "]\t"
        << m_RightShoulderFlexorLengthFeedbackP1[i] << "\n";
        cerr << "CPG::ReadGenome\tm_RightShoulderExtensorLengthFeedbackP1[" << i << "]\t"
        << m_RightShoulderExtensorLengthFeedbackP1[i] << "\n";
        cerr << "CPG::ReadGenome\tm_RightElbowFlexorLengthFeedbackP1[" << i << "]\t"
        << m_RightElbowFlexorLengthFeedbackP1[i] << "\n";
        cerr << "CPG::ReadGenome\tm_RightElbowExtensorLengthFeedbackP1[" << i << "]\t"
        << m_RightElbowExtensorLengthFeedbackP1[i] << "\n";
        cerr << "CPG::ReadGenome\tm_RightWristFlexorLengthFeedbackP1[" << i << "]\t"
        << m_RightWristFlexorLengthFeedbackP1[i] << "\n";
        cerr << "CPG::ReadGenome\tm_RightWristExtensorLengthFeedbackP1[" << i << "]\t"
        << m_RightWristExtensorLengthFeedbackP1[i] << "\n";
        cerr << "CPG::ReadGenome\tm_LeftShoulderAbductorLengthFeedbackP1[" << i << "]\t"
        << m_LeftShoulderAbductorLengthFeedbackP1[i] << "\n";
        cerr << "CPG::ReadGenome\tm_LeftShoulderFlexorLengthFeedbackP1[" << i << "]\t"
        << m_LeftShoulderFlexorLengthFeedbackP1[i] << "\n";
        cerr << "CPG::ReadGenome\tm_LeftShoulderExtensorLengthFeedbackP1[" << i << "]\t"
        << m_LeftShoulderExtensorLengthFeedbackP1[i] << "\n";
        cerr << "CPG::ReadGenome\tm_LeftElbowFlexorLengthFeedbackP1[" << i << "]\t"
        << m_LeftElbowFlexorLengthFeedbackP1[i] << "\n";
        cerr << "CPG::ReadGenome\tm_LeftElbowExtensorLengthFeedbackP1[" << i << "]\t"
        << m_LeftElbowExtensorLengthFeedbackP1[i] << "\n";
        cerr << "CPG::ReadGenome\tm_LeftWristFlexorLengthFeedbackP1[" << i << "]\t"
        << m_LeftWristFlexorLengthFeedbackP1[i] << "\n";
        cerr << "CPG::ReadGenome\tm_LeftWristExtensorLengthFeedbackP1[" << i << "]\t"
        << m_LeftWristExtensorLengthFeedbackP1[i] << "\n";

        cerr << "CPG::ReadGenome\tm_RightHipFlexorLengthFeedbackP2[" << i << "]\t"
        << m_RightHipFlexorLengthFeedbackP2[i] << "\n";
        cerr << "CPG::ReadGenome\tm_RightHipExtensorLengthFeedbackP2[" << i << "]\t"
        << m_RightHipExtensorLengthFeedbackP2[i] << "\n";
        cerr << "CPG::ReadGenome\tm_RightKneeFlexorLengthFeedbackP2[" << i << "]\t"
        << m_RightKneeFlexorLengthFeedbackP2[i] << "\n";
        cerr << "CPG::ReadGenome\tm_RightKneeExtensorLengthFeedbackP2[" << i << "]\t"
        << m_RightKneeExtensorLengthFeedbackP2[i] << "\n";
        cerr << "CPG::ReadGenome\tm_RightAnkleFlexorLengthFeedbackP2[" << i << "]\t"
        << m_RightAnkleFlexorLengthFeedbackP2[i] << "\n";
        cerr << "CPG::ReadGenome\tm_RightAnkleExtensorLengthFeedbackP2[" << i << "]\t"
        << m_RightAnkleExtensorLengthFeedbackP2[i] << "\n";
        cerr << "CPG::ReadGenome\tm_LeftHipFlexorLengthFeedbackP2[" << i << "]\t"
        << m_LeftHipFlexorLengthFeedbackP2[i] << "\n";
        cerr << "CPG::ReadGenome\tm_LeftHipExtensorLengthFeedbackP2[" << i << "]\t"
        << m_LeftHipExtensorLengthFeedbackP2[i] << "\n";
        cerr << "CPG::ReadGenome\tm_LeftKneeFlexorLengthFeedbackP2[" << i << "]\t"
        << m_LeftKneeFlexorLengthFeedbackP2[i] << "\n";
        cerr << "CPG::ReadGenome\tm_LeftKneeExtensorLengthFeedbackP2[" << i << "]\t"
        << m_LeftKneeExtensorLengthFeedbackP2[i] << "\n";
        cerr << "CPG::ReadGenome\tm_LeftAnkleFlexorLengthFeedbackP2[" << i << "]\t"
        << m_LeftAnkleFlexorLengthFeedbackP2[i] << "\n";
        cerr << "CPG::ReadGenome\tm_LeftAnkleExtensorLengthFeedbackP2[" << i << "]\t"
        << m_LeftAnkleExtensorLengthFeedbackP2[i] << "\n";

        cerr << "CPG::ReadGenome\tm_RightShoulderAbductorLengthFeedbackP2[" << i << "]\t"
        << m_RightShoulderAbductorLengthFeedbackP2[i] << "\n";
        cerr << "CPG::ReadGenome\tm_RightShoulderFlexorLengthFeedbackP2[" << i << "]\t"
        << m_RightShoulderFlexorLengthFeedbackP2[i] << "\n";
        cerr << "CPG::ReadGenome\tm_RightShoulderExtensorLengthFeedbackP2[" << i << "]\t"
        << m_RightShoulderExtensorLengthFeedbackP2[i] << "\n";
        cerr << "CPG::ReadGenome\tm_RightElbowFlexorLengthFeedbackP2[" << i << "]\t"
        << m_RightElbowFlexorLengthFeedbackP2[i] << "\n";
        cerr << "CPG::ReadGenome\tm_RightElbowExtensorLengthFeedbackP2[" << i << "]\t"
        << m_RightElbowExtensorLengthFeedbackP2[i] << "\n";
        cerr << "CPG::ReadGenome\tm_RightWristFlexorLengthFeedbackP2[" << i << "]\t"
        << m_RightWristFlexorLengthFeedbackP2[i] << "\n";
        cerr << "CPG::ReadGenome\tm_RightWristExtensorLengthFeedbackP2[" << i << "]\t"
        << m_RightWristExtensorLengthFeedbackP2[i] << "\n";
        cerr << "CPG::ReadGenome\tm_LeftShoulderAbductorLengthFeedbackP2[" << i << "]\t"
        << m_LeftShoulderAbductorLengthFeedbackP2[i] << "\n";
        cerr << "CPG::ReadGenome\tm_LeftShoulderFlexorLengthFeedbackP2[" << i << "]\t"
        << m_LeftShoulderFlexorLengthFeedbackP2[i] << "\n";
        cerr << "CPG::ReadGenome\tm_LeftShoulderExtensorLengthFeedbackP2[" << i << "]\t"
        << m_LeftShoulderExtensorLengthFeedbackP2[i] << "\n";
        cerr << "CPG::ReadGenome\tm_LeftElbowFlexorLengthFeedbackP2[" << i << "]\t"
        << m_LeftElbowFlexorLengthFeedbackP2[i] << "\n";
        cerr << "CPG::ReadGenome\tm_LeftElbowExtensorLengthFeedbackP2[" << i << "]\t"
        << m_LeftElbowExtensorLengthFeedbackP2[i] << "\n";
        cerr << "CPG::ReadGenome\tm_LeftWristFlexorLengthFeedbackP2[" << i << "]\t"
        << m_LeftWristFlexorLengthFeedbackP2[i] << "\n";
        cerr << "CPG::ReadGenome\tm_LeftWristExtensorLengthFeedbackP2[" << i << "]\t"
        << m_LeftWristExtensorLengthFeedbackP2[i] << "\n";

     }
  }
}

// set the controller values depending on the time
// and anything else if relevent
void
CPG::Update(double simTime)
{
   assert(m_NumPhases);
   // calculate the phase
   
   m_TimeInPhase += (simTime - m_LastSimTime);
   
   if (m_TimeInPhase > m_Durations[m_Phase])
   {
      m_TimeInPhase = 0;
      m_Phase++;
   }
   
   if (m_Phase > m_NumPhases) m_Phase = 0;
   
   m_LastSimTime = simTime;
   
   if (gDebug == CPGDebug)
   {
      cerr << "CPG::Update\tsimTime\t" << simTime <<
      "\tm_TimeInPhase\t" << m_TimeInPhase <<
      "\tm_NumPhases\t" << m_NumPhases <<
      "\tm_Phase\t" << m_Phase << "\n";
   }

   m_RightHipExtensor.SetAlpha(m_RightHipExtensorController[m_Phase] +
       FeedbackFunction(m_RightHipExtensor.GetLength(), 
       m_RightHipExtensorLengthFeedbackP1[m_Phase],
       m_RightHipExtensorLengthFeedbackP2[m_Phase]));
   m_RightHipFlexor.SetAlpha(m_RightHipFlexorController[m_Phase] +
       FeedbackFunction(m_RightHipFlexor.GetLength(), 
       m_RightHipFlexorLengthFeedbackP1[m_Phase],
       m_RightHipFlexorLengthFeedbackP2[m_Phase]));
   m_RightKneeExtensor.SetAlpha(m_RightKneeExtensorController[m_Phase] +
       FeedbackFunction(m_RightKneeExtensor.GetLength(), 
       m_RightKneeExtensorLengthFeedbackP1[m_Phase],
       m_RightKneeExtensorLengthFeedbackP2[m_Phase]));
   m_RightKneeFlexor.SetAlpha(m_RightKneeFlexorController[m_Phase] +
       FeedbackFunction(m_RightKneeFlexor.GetLength(), 
       m_RightKneeFlexorLengthFeedbackP1[m_Phase],
       m_RightKneeFlexorLengthFeedbackP2[m_Phase]));
   m_RightAnkleExtensor.SetAlpha(m_RightAnkleExtensorController[m_Phase] +
       FeedbackFunction(m_RightAnkleExtensor.GetLength(), 
       m_RightAnkleExtensorLengthFeedbackP1[m_Phase],
       m_RightAnkleExtensorLengthFeedbackP2[m_Phase]));
   m_RightAnkleFlexor.SetAlpha(m_RightAnkleFlexorController[m_Phase] +
       FeedbackFunction(m_RightAnkleFlexor.GetLength(), 
       m_RightAnkleFlexorLengthFeedbackP1[m_Phase],
       m_RightAnkleFlexorLengthFeedbackP2[m_Phase]));
   m_LeftHipExtensor.SetAlpha(m_LeftHipExtensorController[m_Phase] +
       FeedbackFunction(m_LeftHipExtensor.GetLength(), 
       m_LeftHipExtensorLengthFeedbackP1[m_Phase],
       m_LeftHipExtensorLengthFeedbackP2[m_Phase]));
   m_LeftHipFlexor.SetAlpha(m_LeftHipFlexorController[m_Phase] +
       FeedbackFunction(m_LeftHipFlexor.GetLength(), 
       m_LeftHipFlexorLengthFeedbackP1[m_Phase],
       m_LeftHipFlexorLengthFeedbackP2[m_Phase]));
   m_LeftKneeExtensor.SetAlpha(m_LeftKneeExtensorController[m_Phase] +
       FeedbackFunction(m_LeftKneeExtensor.GetLength(), 
       m_LeftKneeExtensorLengthFeedbackP1[m_Phase],
       m_LeftKneeExtensorLengthFeedbackP2[m_Phase]));
   m_LeftKneeFlexor.SetAlpha(m_LeftKneeFlexorController[m_Phase] +
       FeedbackFunction(m_LeftKneeFlexor.GetLength(), 
       m_LeftKneeFlexorLengthFeedbackP1[m_Phase],
       m_LeftKneeFlexorLengthFeedbackP2[m_Phase]));
   m_LeftAnkleExtensor.SetAlpha(m_LeftAnkleExtensorController[m_Phase] +
       FeedbackFunction(m_LeftAnkleExtensor.GetLength(), 
       m_LeftAnkleExtensorLengthFeedbackP1[m_Phase],
       m_LeftAnkleExtensorLengthFeedbackP2[m_Phase]));
   m_LeftAnkleFlexor.SetAlpha(m_LeftAnkleFlexorController[m_Phase] +
       FeedbackFunction(m_LeftAnkleFlexor.GetLength(), 
       m_LeftAnkleFlexorLengthFeedbackP1[m_Phase],
       m_LeftAnkleFlexorLengthFeedbackP2[m_Phase]));
      
   m_RightShoulderAbductor.SetAlpha(m_RightShoulderAbductorController[m_Phase] +
       FeedbackFunction(m_RightShoulderAbductor.GetLength(), 
       m_RightShoulderAbductorLengthFeedbackP1[m_Phase],
       m_RightShoulderAbductorLengthFeedbackP2[m_Phase]));
   m_RightShoulderExtensor.SetAlpha(m_RightShoulderExtensorController[m_Phase] +
       FeedbackFunction(m_RightShoulderExtensor.GetLength(), 
       m_RightShoulderExtensorLengthFeedbackP1[m_Phase],
       m_RightShoulderExtensorLengthFeedbackP2[m_Phase]));
   m_RightShoulderFlexor.SetAlpha(m_RightShoulderFlexorController[m_Phase] +
       FeedbackFunction(m_RightShoulderFlexor.GetLength(), 
       m_RightShoulderFlexorLengthFeedbackP1[m_Phase],
       m_RightShoulderFlexorLengthFeedbackP2[m_Phase]));
   m_RightElbowExtensor.SetAlpha(m_RightElbowExtensorController[m_Phase] +
       FeedbackFunction(m_RightElbowExtensor.GetLength(), 
       m_RightElbowExtensorLengthFeedbackP1[m_Phase],
       m_RightElbowExtensorLengthFeedbackP2[m_Phase]));
   m_RightElbowFlexor.SetAlpha(m_RightElbowFlexorController[m_Phase] +
       FeedbackFunction(m_RightElbowFlexor.GetLength(), 
       m_RightElbowFlexorLengthFeedbackP1[m_Phase],
       m_RightElbowFlexorLengthFeedbackP2[m_Phase]));
   m_RightWristExtensor.SetAlpha(m_RightWristExtensorController[m_Phase] +
       FeedbackFunction(m_RightWristExtensor.GetLength(), 
       m_RightWristExtensorLengthFeedbackP1[m_Phase],
       m_RightWristExtensorLengthFeedbackP2[m_Phase]));
   m_RightWristFlexor.SetAlpha(m_RightWristFlexorController[m_Phase] +
       FeedbackFunction(m_RightWristFlexor.GetLength(), 
       m_RightWristFlexorLengthFeedbackP1[m_Phase],
       m_RightWristFlexorLengthFeedbackP2[m_Phase]));
   m_LeftShoulderAbductor.SetAlpha(m_LeftShoulderAbductorController[m_Phase] +
       FeedbackFunction(m_LeftShoulderAbductor.GetLength(), 
       m_LeftShoulderAbductorLengthFeedbackP1[m_Phase],
       m_LeftShoulderAbductorLengthFeedbackP2[m_Phase]));
   m_LeftShoulderExtensor.SetAlpha(m_LeftShoulderExtensorController[m_Phase] +
       FeedbackFunction(m_LeftShoulderExtensor.GetLength(), 
       m_LeftShoulderExtensorLengthFeedbackP1[m_Phase],
       m_LeftShoulderExtensorLengthFeedbackP2[m_Phase]));
   m_LeftShoulderFlexor.SetAlpha(m_LeftShoulderFlexorController[m_Phase] +
       FeedbackFunction(m_LeftShoulderFlexor.GetLength(), 
       m_LeftShoulderFlexorLengthFeedbackP1[m_Phase],
       m_LeftShoulderFlexorLengthFeedbackP2[m_Phase]));
   m_LeftElbowExtensor.SetAlpha(m_LeftElbowExtensorController[m_Phase] +
       FeedbackFunction(m_LeftElbowExtensor.GetLength(), 
       m_LeftElbowExtensorLengthFeedbackP1[m_Phase],
       m_LeftElbowExtensorLengthFeedbackP2[m_Phase]));
   m_LeftElbowFlexor.SetAlpha(m_LeftElbowFlexorController[m_Phase] +
       FeedbackFunction(m_LeftElbowFlexor.GetLength(), 
       m_LeftElbowFlexorLengthFeedbackP1[m_Phase],
       m_LeftElbowFlexorLengthFeedbackP2[m_Phase]));
   m_LeftWristExtensor.SetAlpha(m_LeftWristExtensorController[m_Phase] +
       FeedbackFunction(m_LeftWristExtensor.GetLength(), 
       m_LeftWristExtensorLengthFeedbackP1[m_Phase],
       m_LeftWristExtensorLengthFeedbackP2[m_Phase]));
   m_LeftWristFlexor.SetAlpha(m_LeftWristFlexorController[m_Phase] +
       FeedbackFunction(m_LeftWristFlexor.GetLength(), 
       m_LeftWristFlexorLengthFeedbackP1[m_Phase],
       m_LeftWristFlexorLengthFeedbackP2[m_Phase]));
      
   // and this is a good place to update the world positions    
   m_LeftHipExtensor.UpdateWorldPositions();
   m_RightHipExtensor.UpdateWorldPositions();
   m_LeftHipFlexor.UpdateWorldPositions();
   m_RightHipFlexor.UpdateWorldPositions();
   m_LeftKneeExtensor.UpdateWorldPositions();
   m_RightKneeExtensor.UpdateWorldPositions();
   m_LeftKneeFlexor.UpdateWorldPositions();
   m_RightKneeFlexor.UpdateWorldPositions();
   m_LeftAnkleExtensor.UpdateWorldPositions();
   m_RightAnkleExtensor.UpdateWorldPositions();
   m_LeftAnkleFlexor.UpdateWorldPositions();
   m_RightAnkleFlexor.UpdateWorldPositions();
   
   m_LeftShoulderAbductor.UpdateWorldPositions();
   m_RightShoulderAbductor.UpdateWorldPositions();
   m_LeftShoulderExtensor.UpdateWorldPositions();
   m_RightShoulderExtensor.UpdateWorldPositions();
   m_LeftShoulderFlexor.UpdateWorldPositions();
   m_RightShoulderFlexor.UpdateWorldPositions();
   m_LeftElbowExtensor.UpdateWorldPositions();
   m_RightElbowExtensor.UpdateWorldPositions();
   m_LeftElbowFlexor.UpdateWorldPositions();
   m_RightElbowFlexor.UpdateWorldPositions();
   m_LeftWristExtensor.UpdateWorldPositions();
   m_RightWristExtensor.UpdateWorldPositions();
   m_LeftWristFlexor.UpdateWorldPositions();
   m_RightWristFlexor.UpdateWorldPositions();
   
   
}

// draw the bits controlled by the CPG
void
CPG::draw()
{
   m_LeftHipExtensor.Draw();
   m_RightHipExtensor.Draw();
   m_LeftHipFlexor.Draw();
   m_RightHipFlexor.Draw();
   m_LeftKneeExtensor.Draw();
   m_RightKneeExtensor.Draw();
   m_LeftKneeFlexor.Draw();
   m_RightKneeFlexor.Draw();
   m_LeftAnkleExtensor.Draw();
   m_RightAnkleExtensor.Draw();
   m_LeftAnkleFlexor.Draw();
   m_RightAnkleFlexor.Draw();

   m_LeftShoulderAbductor.Draw();
   m_RightShoulderAbductor.Draw();
   m_LeftShoulderExtensor.Draw();
   m_RightShoulderExtensor.Draw();
   m_LeftShoulderFlexor.Draw();
   m_RightShoulderFlexor.Draw();
   m_LeftElbowExtensor.Draw();
   m_RightElbowExtensor.Draw();
   m_LeftElbowFlexor.Draw();
   m_RightElbowFlexor.Draw();
   m_LeftWristExtensor.Draw();
   m_RightWristExtensor.Draw();
   m_LeftWristFlexor.Draw();
   m_RightWristFlexor.Draw();
}   
   
// sigmoidal feedback function
double
 CPG::FeedbackFunction(double x, double p1, double p2)
{
  // added p1Scale to get p1 range to 0 to 100
  const double p1Scale = 100;
  double result = 1 / (1 + exp(-p1Scale * p1 * (x - p2)));
  if (gDebug == CPGDebug)
  {
     cerr << "CPG::FeedbackFunction\t" <<
     "\tx\t" << x <<
     "\tp1\t" << p1 <<
     "\tp2\t" << p2 <<
     "\tresult\t" << result << "\n";
  }
  
  return result;
}


      
