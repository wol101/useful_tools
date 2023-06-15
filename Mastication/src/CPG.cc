// CPG.cc - class to hold the central pattern generator

#include <stdio.h>
#include <iostream>
#include <iomanip>
#include <fstream>
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

    m_LeftTemporalisController = 0;
    m_RightTemporalisController = 0;
    m_LeftMasseterController = 0;
    m_RightMasseterController = 0;
    m_LeftMedialPterygoidController = 0;
    m_RightMedialPterygoidController = 0;
    m_LeftLateralPterygoidController = 0;
    m_RightLateralPterygoidController = 0;
    m_Durations = 0;
}

// default destructor
CPG::~CPG()
{
    if (m_Genome) delete m_Genome;
    if (m_NumPhases)
    {
        delete [] m_LeftTemporalisController;
        delete [] m_RightTemporalisController;
        delete [] m_LeftMasseterController;
        delete [] m_RightMasseterController;
        delete [] m_LeftMedialPterygoidController;
        delete [] m_RightMedialPterygoidController;
        delete [] m_LeftLateralPterygoidController;
        delete [] m_RightLateralPterygoidController;

        delete [] m_Durations;
    }
}

// set up the controller data structures
void
CPG::Initialise(int numStartupPhases, int numCyclicPhases, int numAllelesPerPhase)
{
    // hardwired initialisation
    m_NumStartupPhases = numStartupPhases;
    m_NumCyclicPhases = numCyclicPhases;
    m_NumPhases = m_NumStartupPhases + m_NumCyclicPhases;
    m_NumAllelesPerPhase = numAllelesPerPhase;

    m_LeftTemporalisController = new double[m_NumPhases];
    m_RightTemporalisController = new double[m_NumPhases];
    m_LeftMasseterController = new double[m_NumPhases];
    m_RightMasseterController = new double[m_NumPhases];
    m_LeftMedialPterygoidController = new double[m_NumPhases];
    m_RightMedialPterygoidController = new double[m_NumPhases];
    m_LeftLateralPterygoidController = new double[m_NumPhases];
    m_RightLateralPterygoidController = new double[m_NumPhases];

    m_Durations = new double[m_NumPhases];

}

// set the genome data
void
CPG::SetGenome(int genomeLength, const double *genome)
{
    int i;
    m_GenomeLength = genomeLength;
    if (m_Genome) delete [] m_Genome;
    m_Genome = new double [m_GenomeLength];
    for (i = 0; i < m_GenomeLength; i++)
        m_Genome[i] = genome[i];
}

// parse the genome
void
CPG::ParseGenome()
{
    int i;

    if (m_GenomeLength != m_NumAllelesPerPhase * (m_NumStartupPhases + m_NumCyclicPhases / 2))
    {
        cerr << "Invalid genome length:" << m_GenomeLength << "\n";
        exit(1);
    }

    switch (m_NumAllelesPerPhase)
    {
        case 9:
            break;

        default:
            cerr << "Invalid m_NumAllelesPerPhase" << m_NumAllelesPerPhase << "\n";
            exit(1);
    };

    int genomeOffset = 0;
    for (i = 0; i < (m_NumStartupPhases + m_NumCyclicPhases / 2); i++)
    {
        m_Durations[i] = m_Genome[genomeOffset++];
        m_LeftTemporalisController[i] = m_Genome[genomeOffset++];
        m_LeftMasseterController[i] = m_Genome[genomeOffset++];
        m_LeftMedialPterygoidController[i] = m_Genome[genomeOffset++];
        m_LeftLateralPterygoidController[i] = m_Genome[genomeOffset++];
        m_RightTemporalisController[i] = m_Genome[genomeOffset++];
        m_RightMasseterController[i] = m_Genome[genomeOffset++];
        m_RightMedialPterygoidController[i] = m_Genome[genomeOffset++];
        m_RightLateralPterygoidController[i] = m_Genome[genomeOffset++];

    }
    // the last half of the cyclic phases are L/R symmetry of first half
    genomeOffset = m_NumStartupPhases * m_NumAllelesPerPhase;
    for (i = (m_NumStartupPhases + m_NumCyclicPhases / 2); i < m_NumPhases; i++)
    {
        m_Durations[i] = m_Genome[genomeOffset++];
        m_RightTemporalisController[i] = m_Genome[genomeOffset++];
        m_RightMasseterController[i] = m_Genome[genomeOffset++];
        m_RightMedialPterygoidController[i] = m_Genome[genomeOffset++];
        m_RightLateralPterygoidController[i] = m_Genome[genomeOffset++];
        m_LeftTemporalisController[i] = m_Genome[genomeOffset++];
        m_LeftMasseterController[i] = m_Genome[genomeOffset++];
        m_LeftMedialPterygoidController[i] = m_Genome[genomeOffset++];
        m_LeftLateralPterygoidController[i] = m_Genome[genomeOffset++];
   }

    if (gDebug == CPGDebug)
    {
        for (i = 0; i < m_NumPhases; i ++)
        {
            cerr << "CPG::ReadGenome\tm_RightTemporalisController[" << i << "]\t"
            << m_RightTemporalisController[i] << "\n";
            cerr << "CPG::ReadGenome\tm_RightMasseterController[" << i << "]\t"
                << m_RightMasseterController[i] << "\n";
            cerr << "CPG::ReadGenome\tm_RightMedialPterygoidController[" << i << "]\t"
                << m_RightMedialPterygoidController[i] << "\n";
            cerr << "CPG::ReadGenome\tm_RightLateralPterygoidController[" << i << "]\t"
                << m_RightLateralPterygoidController[i] << "\n";
            cerr << "CPG::ReadGenome\tm_LeftTemporalisController[" << i << "]\t"
                << m_LeftTemporalisController[i] << "\n";
            cerr << "CPG::ReadGenome\tm_LeftMasseterController[" << i << "]\t"
                << m_LeftMasseterController[i] << "\n";
            cerr << "CPG::ReadGenome\tm_LeftMedialPterygoidController[" << i << "]\t"
                << m_LeftMedialPterygoidController[i] << "\n";
            cerr << "CPG::ReadGenome\tm_LeftLateralPterygoidController[" << i << "]\t"
                << m_LeftLateralPterygoidController[i] << "\n";

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

    m_RightTemporalis.SetTension(m_RightTemporalisController[m_Phase]);
    m_RightMasseter.SetTension(m_RightMasseterController[m_Phase]);
    m_RightMedialPterygoid.SetTension(m_RightMedialPterygoidController[m_Phase]);
    m_RightLateralPterygoid.SetTension(m_RightLateralPterygoidController[m_Phase]);
    m_LeftTemporalis.SetTension(m_LeftTemporalisController[m_Phase]);
    m_LeftMasseter.SetTension(m_LeftMasseterController[m_Phase]);
    m_LeftMedialPterygoid.SetTension(m_LeftMedialPterygoidController[m_Phase]);
    m_LeftLateralPterygoid.SetTension(m_LeftLateralPterygoidController[m_Phase]);
    
    // and this is a good place to update the world positions
    m_LeftTemporalis.UpdateWorldPositions();
    m_RightTemporalis.UpdateWorldPositions();
    m_LeftMasseter.UpdateWorldPositions();
    m_RightMasseter.UpdateWorldPositions();
    m_LeftMedialPterygoid.UpdateWorldPositions();
    m_RightMedialPterygoid.UpdateWorldPositions();
    m_LeftLateralPterygoid.UpdateWorldPositions();
    m_RightLateralPterygoid.UpdateWorldPositions();
}

// draw the bits controlled by the CPG
void
CPG::draw()
{
    m_LeftTemporalis.Draw();
    m_RightTemporalis.Draw();
    m_LeftMasseter.Draw();
    m_RightMasseter.Draw();
    m_LeftMedialPterygoid.Draw();
    m_RightMedialPterygoid.Draw();
    m_LeftLateralPterygoid.Draw();
    m_RightLateralPterygoid.Draw();
}




