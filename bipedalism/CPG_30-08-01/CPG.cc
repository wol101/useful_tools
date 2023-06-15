// CPG.cc - class to hold the central pattern generator

#include <stdio.h>
#include <iostream.h>
#include <iomanip.h>
#include <fstream.h>
#include <assert.h>

#include "CPG.h"
#include "DebugControl.h"

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
      delete [] m_Durations;
   }
}

// set up the controller data structures
void 
CPG::Initialise()
{
   // hardwired initialisation
   m_NumStartupPhases = 2;
   m_NumCyclicPhases = 6;
   m_NumPhases = m_NumStartupPhases + m_NumCyclicPhases;
   
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
   m_Durations = new double[m_NumPhases];
   
}

// read in the genome
void
CPG::ReadGenome(const char *filename)
{
  int i;
  // read the gene data
  
  ifstream inFile(filename);
  assert(inFile.good());
  inFile >> m_GenomeLength;
  m_Genome = new double[m_GenomeLength];
  for (i = 0; i < m_GenomeLength; i++)
    inFile >> m_Genome[i];
  inFile.close();
  
  // locked to this genome size
  assert(m_GenomeLength == 13 * (m_NumStartupPhases + m_NumCyclicPhases / 2)); 
  int genomeOffset = 0;
  for (i = 0; i < (m_NumStartupPhases + m_NumCyclicPhases / 2); i++)
  {
     m_Durations[i] = m_Genome[genomeOffset++];
     m_RightHipExtensorController[i] = m_Genome[genomeOffset++];
     m_RightHipFlexorController[i] = m_Genome[genomeOffset++];
     m_RightKneeExtensorController[i] = m_Genome[genomeOffset++];
     m_RightKneeFlexorController[i] = m_Genome[genomeOffset++];
     m_RightAnkleExtensorController[i] = m_Genome[genomeOffset++];
     m_RightAnkleFlexorController[i] = m_Genome[genomeOffset++];
     m_LeftHipExtensorController[i] = m_Genome[genomeOffset++];
     m_LeftHipFlexorController[i] = m_Genome[genomeOffset++];
     m_LeftKneeExtensorController[i] = m_Genome[genomeOffset++];
     m_LeftKneeFlexorController[i] = m_Genome[genomeOffset++];
     m_LeftAnkleExtensorController[i] = m_Genome[genomeOffset++];
     m_LeftAnkleFlexorController[i] = m_Genome[genomeOffset++];
  }
  // the last half of the cyclic phases are L/R symmetry of first half
  genomeOffset = m_NumStartupPhases * 13;
  for (i = (m_NumStartupPhases + m_NumCyclicPhases / 2); i < m_NumPhases; i++)
  {
     m_Durations[i] = m_Genome[genomeOffset++];
     m_LeftHipExtensorController[i] = m_Genome[genomeOffset++];
     m_LeftHipFlexorController[i] = m_Genome[genomeOffset++];
     m_LeftKneeExtensorController[i] = m_Genome[genomeOffset++];
     m_LeftKneeFlexorController[i] = m_Genome[genomeOffset++];
     m_LeftAnkleExtensorController[i] = m_Genome[genomeOffset++];
     m_LeftAnkleFlexorController[i] = m_Genome[genomeOffset++];
     m_RightHipExtensorController[i] = m_Genome[genomeOffset++];
     m_RightHipFlexorController[i] = m_Genome[genomeOffset++];
     m_RightKneeExtensorController[i] = m_Genome[genomeOffset++];
     m_RightKneeFlexorController[i] = m_Genome[genomeOffset++];
     m_RightAnkleExtensorController[i] = m_Genome[genomeOffset++];
     m_RightAnkleFlexorController[i] = m_Genome[genomeOffset++];
  } 
  
   if (gDebug == CPGDebug)
   {
      cerr << "CPG::ReadGenome\tfilename\t" << filename << "\n";
      for (i = 0; i < m_NumPhases; i ++)
      {
         cerr << "CPG::ReadGenome\tm_RightHipExtensorController[" << i << "]\t"
         << m_RightHipExtensorController[i] << "\n";
         cerr << "CPG::ReadGenome\tm_RightHipFlexorController[" << i << "]\t"
         << m_RightHipFlexorController[i] << "\n";
         cerr << "CPG::ReadGenome\tm_RightKneeExtensorController[" << i << "]\t"
         << m_RightKneeExtensorController[i] << "\n";
         cerr << "CPG::ReadGenome\tm_RightKneeFlexorController[" << i << "]\t"
         << m_RightKneeFlexorController[i] << "\n";
         cerr << "CPG::ReadGenome\tm_RightAnkleExtensorController[" << i << "]\t"
         << m_RightAnkleExtensorController[i] << "\n";
         cerr << "CPG::ReadGenome\tm_RightAnkleFlexorController[" << i << "]\t"
         << m_RightAnkleFlexorController[i] << "\n";
         cerr << "CPG::ReadGenome\tm_LeftHipExtensorController[" << i << "]\t"
         << m_LeftHipExtensorController[i] << "\n";
         cerr << "CPG::ReadGenome\tm_LeftHipFlexorController[" << i << "]\t"
         << m_LeftHipFlexorController[i] << "\n";
         cerr << "CPG::ReadGenome\tm_LeftKneeExtensorController[" << i << "]\t"
         << m_LeftKneeExtensorController[i] << "\n";
         cerr << "CPG::ReadGenome\tm_LeftKneeFlexorController[" << i << "]\t"
         << m_LeftKneeFlexorController[i] << "\n";
         cerr << "CPG::ReadGenome\tm_LeftAnkleExtensorController[" << i << "]\t"
         << m_LeftAnkleExtensorController[i] << "\n";
         cerr << "CPG::ReadGenome\tm_LeftAnkleFlexorController[" << i << "]\t"
         << m_LeftAnkleFlexorController[i] << "\n";
      }
   }
}

// set the controller values depending on the time
// and anything else if relevent
void
CPG::Update(double simTime)
{
   // calculate the phase
   
   m_TimeInPhase += (simTime - m_LastSimTime);
   
   if (m_TimeInPhase > m_Durations[m_Phase])
   {
      m_TimeInPhase = 0;
      m_Phase++;
   }
   
   if (m_Phase > m_NumPhases) m_Phase = m_NumStartupPhases;
   
   m_LastSimTime = simTime;
   
   if (gDebug == CPGDebug)
   {
      cerr << "CPG::Update\tsimTime\t" << simTime <<
      "\tm_TimeInPhase\t" << m_TimeInPhase <<
      "\tm_Phase\t" << m_Phase << "\n";
   }

/* old StrapForce version   
   // now set the forces in the StrapForces
   // * 1000 is a kludge - we need a proper muscle model
   m_LeftHipExtensor.SetTension(m_LeftHipExtensorController[m_Phase] * 1000);
   m_RightHipExtensor.SetTension(m_RightHipExtensorController[m_Phase] * 1000);
   m_LeftHipFlexor.SetTension(m_LeftHipFlexorController[m_Phase] * 1000);
   m_RightHipFlexor.SetTension(m_RightHipFlexorController[m_Phase] * 1000);
   m_LeftKneeExtensor.SetTension(m_LeftKneeExtensorController[m_Phase] * 1000);
   m_RightKneeExtensor.SetTension(m_RightKneeExtensorController[m_Phase] * 1000);
   m_LeftKneeFlexor.SetTension(m_LeftKneeFlexorController[m_Phase] * 1000);
   m_RightKneeFlexor.SetTension(m_RightKneeFlexorController[m_Phase] * 1000);
   m_LeftAnkleExtensor.SetTension(m_LeftAnkleExtensorController[m_Phase] * 1000);
   m_RightAnkleExtensor.SetTension(m_RightAnkleExtensorController[m_Phase] * 1000);
   m_LeftAnkleFlexor.SetTension(m_LeftAnkleFlexorController[m_Phase] * 1000);
   m_RightAnkleFlexor.SetTension(m_RightAnkleFlexorController[m_Phase] * 1000);
*/
   m_LeftHipExtensor.SetAlpha(m_LeftHipExtensorController[m_Phase]);
   m_RightHipExtensor.SetAlpha(m_RightHipExtensorController[m_Phase]);
   m_LeftHipFlexor.SetAlpha(m_LeftHipFlexorController[m_Phase]);
   m_RightHipFlexor.SetAlpha(m_RightHipFlexorController[m_Phase]);
   m_LeftKneeExtensor.SetAlpha(m_LeftKneeExtensorController[m_Phase]);
   m_RightKneeExtensor.SetAlpha(m_RightKneeExtensorController[m_Phase]);
   m_LeftKneeFlexor.SetAlpha(m_LeftKneeFlexorController[m_Phase]);
   m_RightKneeFlexor.SetAlpha(m_RightKneeFlexorController[m_Phase]);
   m_LeftAnkleExtensor.SetAlpha(m_LeftAnkleExtensorController[m_Phase]);
   m_RightAnkleExtensor.SetAlpha(m_RightAnkleExtensorController[m_Phase]);
   m_LeftAnkleFlexor.SetAlpha(m_LeftAnkleFlexorController[m_Phase]);
   m_RightAnkleFlexor.SetAlpha(m_RightAnkleFlexorController[m_Phase]);
   
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
}   
   
   
      
