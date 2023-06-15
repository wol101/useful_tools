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
    m_UsingFeedbackFunction = false;

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
    if (m_Genome) delete m_Genome;
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

        if (m_UsingFeedbackFunction)
        {
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
        }

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

    if (m_NumAllelesPerPhase != 7) m_UsingFeedbackFunction = true;

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

    if (m_UsingFeedbackFunction)
    {
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
    }

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
        cerr << "Invalid genome length\n";
        exit(1);
    }

    switch (m_NumAllelesPerPhase)
    {
        case 7:
        case 9:
        case 19:
        case 31:
            break;

        default:
            cerr << "Invalid m_NumAllelesPerPhase\n";
            exit(1);
    };

    int genomeOffset = 0;
    for (i = 0; i < (m_NumStartupPhases + m_NumCyclicPhases / 2); i++)
    {
        m_Durations[i] = fabs(m_Genome[genomeOffset++]);

        if (m_Genome[genomeOffset] > 0)
        {
            m_RightHipFlexorController[i] = m_Genome[genomeOffset++];
            m_RightHipExtensorController[i] = 0;
        }
        else
        {
            m_RightHipExtensorController[i] = -m_Genome[genomeOffset++];
            m_RightHipFlexorController[i] = 0;
        }
        if (m_Genome[genomeOffset] > 0)
        {
            m_RightKneeFlexorController[i] = m_Genome[genomeOffset++];
            m_RightKneeExtensorController[i] = 0;
        }
        else
        {
            m_RightKneeExtensorController[i] = -m_Genome[genomeOffset++];
            m_RightKneeFlexorController[i] = 0;
        }
        if (m_Genome[genomeOffset] > 0)
        {
            m_RightAnkleFlexorController[i] = m_Genome[genomeOffset++];
            m_RightAnkleExtensorController[i] = 0;
        }
        else
        {
            m_RightAnkleExtensorController[i] = -m_Genome[genomeOffset++];
            m_RightAnkleFlexorController[i] = 0;
        }
        if (m_Genome[genomeOffset] > 0)
        {
            m_LeftHipFlexorController[i] = m_Genome[genomeOffset++];
            m_LeftHipExtensorController[i] = 0;
        }
        else
        {
            m_LeftHipExtensorController[i] = -m_Genome[genomeOffset++];
            m_LeftHipFlexorController[i] = 0;
        }
        if (m_Genome[genomeOffset] > 0)
        {
            m_LeftKneeFlexorController[i] = m_Genome[genomeOffset++];
            m_LeftKneeExtensorController[i] = 0;
        }
        else
        {
            m_LeftKneeExtensorController[i] = -m_Genome[genomeOffset++];
            m_LeftKneeFlexorController[i] = 0;
        }
        if (m_Genome[genomeOffset] > 0)
        {
            m_LeftAnkleFlexorController[i] = m_Genome[genomeOffset++];
            m_LeftAnkleExtensorController[i] = 0;
        }
        else
        {
            m_LeftAnkleExtensorController[i] = -m_Genome[genomeOffset++];
            m_LeftAnkleFlexorController[i] = 0;
        }

        if (m_UsingFeedbackFunction)
        {

            if (m_NumAllelesPerPhase == 31)
            {
                m_RightHipExtensorLengthFeedbackP1[i] = fabs(m_Genome[genomeOffset++]);
                m_RightHipFlexorLengthFeedbackP1[i] = fabs(m_Genome[genomeOffset++]);
                m_RightKneeExtensorLengthFeedbackP1[i] = fabs(m_Genome[genomeOffset++]);
                m_RightKneeFlexorLengthFeedbackP1[i] = fabs(m_Genome[genomeOffset++]);
                m_RightAnkleExtensorLengthFeedbackP1[i] = fabs(m_Genome[genomeOffset++]);
                m_RightAnkleFlexorLengthFeedbackP1[i] = fabs(m_Genome[genomeOffset++]);
                m_LeftHipExtensorLengthFeedbackP1[i] = fabs(m_Genome[genomeOffset++]);
                m_LeftHipFlexorLengthFeedbackP1[i] = fabs(m_Genome[genomeOffset++]);
                m_LeftKneeExtensorLengthFeedbackP1[i] = fabs(m_Genome[genomeOffset++]);
                m_LeftKneeFlexorLengthFeedbackP1[i] = fabs(m_Genome[genomeOffset++]);
                m_LeftAnkleExtensorLengthFeedbackP1[i] = fabs(m_Genome[genomeOffset++]);
                m_LeftAnkleFlexorLengthFeedbackP1[i] = fabs(m_Genome[genomeOffset++]);
            }
            else
            {
                m_RightHipExtensorLengthFeedbackP1[i] = 1.0;
                m_RightHipFlexorLengthFeedbackP1[i] = 1.0;
                m_RightKneeExtensorLengthFeedbackP1[i] = 1.0;
                m_RightKneeFlexorLengthFeedbackP1[i] = 1.0;
                m_RightAnkleExtensorLengthFeedbackP1[i] = 1.0;
                m_RightAnkleFlexorLengthFeedbackP1[i] = 1.0;
                m_LeftHipExtensorLengthFeedbackP1[i] = 1.0;
                m_LeftHipFlexorLengthFeedbackP1[i] = 1.0;
                m_LeftKneeExtensorLengthFeedbackP1[i] = 1.0;
                m_LeftKneeFlexorLengthFeedbackP1[i] = 1.0;
                m_LeftAnkleExtensorLengthFeedbackP1[i] = 1.0;
                m_LeftAnkleFlexorLengthFeedbackP1[i] = 1.0;
            }

            if (m_NumAllelesPerPhase == 31 || m_NumAllelesPerPhase == 19)
            {
                m_RightHipExtensorLengthFeedbackP2[i] = 0.5 * m_Genome[genomeOffset++]
                * kRightHipExtensorLength + kRightHipExtensorLength;
                m_RightHipFlexorLengthFeedbackP2[i] = 0.5 * m_Genome[genomeOffset++]
                    * kRightHipFlexorLength + kRightHipFlexorLength;
                m_RightKneeExtensorLengthFeedbackP2[i] = 0.5 * m_Genome[genomeOffset++]
                    * kRightKneeExtensorLength + kRightKneeExtensorLength;
                m_RightKneeFlexorLengthFeedbackP2[i] = 0.5 * m_Genome[genomeOffset++]
                    * kRightKneeFlexorLength + kRightKneeFlexorLength;
                m_RightAnkleExtensorLengthFeedbackP2[i] = 0.5 * m_Genome[genomeOffset++]
                    * kRightAnkleExtensorLength + kRightAnkleExtensorLength;
                m_RightAnkleFlexorLengthFeedbackP2[i] = 0.5 * m_Genome[genomeOffset++]
                    * kRightAnkleFlexorLength + kRightAnkleFlexorLength;
                m_LeftHipExtensorLengthFeedbackP2[i] = 0.5 * m_Genome[genomeOffset++]
                    * kLeftHipExtensorLength + kLeftHipExtensorLength;
                m_LeftHipFlexorLengthFeedbackP2[i] = 0.5 * m_Genome[genomeOffset++]
                    * kLeftHipFlexorLength + kLeftHipFlexorLength;
                m_LeftKneeExtensorLengthFeedbackP2[i] = 0.5 * m_Genome[genomeOffset++]
                    * kLeftKneeExtensorLength + kLeftKneeExtensorLength;
                m_LeftKneeFlexorLengthFeedbackP2[i] = 0.5 * m_Genome[genomeOffset++]
                    * kLeftKneeFlexorLength + kLeftKneeFlexorLength;
                m_LeftAnkleExtensorLengthFeedbackP2[i] = 0.5 * m_Genome[genomeOffset++]
                    * kLeftAnkleExtensorLength + kLeftAnkleExtensorLength;
                m_LeftAnkleFlexorLengthFeedbackP2[i] = 0.5 * m_Genome[genomeOffset++]
                    * kLeftAnkleFlexorLength + kLeftAnkleFlexorLength;
            }
            else
            {
                m_RightHipExtensorLengthFeedbackP2[i] = 1000;
                m_RightHipFlexorLengthFeedbackP2[i] = 1000;
                m_RightKneeExtensorLengthFeedbackP2[i] = 1000;
                m_RightKneeFlexorLengthFeedbackP2[i] = 1000;
                m_RightAnkleExtensorLengthFeedbackP2[i] = 1000;
                m_RightAnkleFlexorLengthFeedbackP2[i] = 1000;
                m_LeftHipExtensorLengthFeedbackP2[i] = 1000;
                m_LeftHipFlexorLengthFeedbackP2[i] = 1000;
                m_LeftKneeExtensorLengthFeedbackP2[i] = 1000;
                m_LeftKneeFlexorLengthFeedbackP2[i] = 1000;
                m_LeftAnkleExtensorLengthFeedbackP2[i] = 1000;
                m_LeftAnkleFlexorLengthFeedbackP2[i] = 1000;
            }
            // special case for m_GenomeLength == 45
            if (m_NumAllelesPerPhase == 9)
            {
                m_RightHipExtensorLengthFeedbackP2[i] = 0.5 * m_Genome[genomeOffset++]
                * kRightHipExtensorLength + kRightHipExtensorLength;
                m_LeftHipExtensorLengthFeedbackP2[i] = 0.5 * m_Genome[genomeOffset++]
                    * kLeftHipExtensorLength + kLeftHipExtensorLength;
            }
        }

    }
    // the last half of the cyclic phases are L/R symmetry of first half
    genomeOffset = m_NumStartupPhases * m_NumAllelesPerPhase;
    for (i = (m_NumStartupPhases + m_NumCyclicPhases / 2); i < m_NumPhases; i++)
    {
        m_Durations[i] = fabs(m_Genome[genomeOffset++]);

        if (m_Genome[genomeOffset] > 0)
        {
            m_LeftHipFlexorController[i] = m_Genome[genomeOffset++];
            m_LeftHipExtensorController[i] = 0;
        }
        else
        {
            m_LeftHipExtensorController[i] = -m_Genome[genomeOffset++];
            m_LeftHipFlexorController[i] = 0;
        }
        if (m_Genome[genomeOffset] > 0)
        {
            m_LeftKneeFlexorController[i] = m_Genome[genomeOffset++];
            m_LeftKneeExtensorController[i] = 0;
        }
        else
        {
            m_LeftKneeExtensorController[i] = -m_Genome[genomeOffset++];
            m_LeftKneeFlexorController[i] = 0;
        }
        if (m_Genome[genomeOffset] > 0)
        {
            m_LeftAnkleFlexorController[i] = m_Genome[genomeOffset++];
            m_LeftAnkleExtensorController[i] = 0;
        }
        else
        {
            m_LeftAnkleExtensorController[i] = -m_Genome[genomeOffset++];
            m_LeftAnkleFlexorController[i] = 0;
        }
        if (m_Genome[genomeOffset] > 0)
        {
            m_RightHipFlexorController[i] = m_Genome[genomeOffset++];
            m_RightHipExtensorController[i] = 0;
        }
        else
        {
            m_RightHipExtensorController[i] = -m_Genome[genomeOffset++];
            m_RightHipFlexorController[i] = 0;
        }
        if (m_Genome[genomeOffset] > 0)
        {
            m_RightKneeFlexorController[i] = m_Genome[genomeOffset++];
            m_RightKneeExtensorController[i] = 0;
        }
        else
        {
            m_RightKneeExtensorController[i] = -m_Genome[genomeOffset++];
            m_RightKneeFlexorController[i] = 0;
        }
        if (m_Genome[genomeOffset] > 0)
        {
            m_RightAnkleFlexorController[i] = m_Genome[genomeOffset++];
            m_RightAnkleExtensorController[i] = 0;
        }
        else
        {
            m_RightAnkleExtensorController[i] = -m_Genome[genomeOffset++];
            m_RightAnkleFlexorController[i] = 0;
        }

        if (m_UsingFeedbackFunction)
        {

            if (m_NumAllelesPerPhase == 31)
            {
                m_LeftHipExtensorLengthFeedbackP1[i] = fabs(m_Genome[genomeOffset++]);
                m_LeftHipFlexorLengthFeedbackP1[i] = fabs(m_Genome[genomeOffset++]);
                m_LeftKneeExtensorLengthFeedbackP1[i] = fabs(m_Genome[genomeOffset++]);
                m_LeftKneeFlexorLengthFeedbackP1[i] = fabs(m_Genome[genomeOffset++]);
                m_LeftAnkleExtensorLengthFeedbackP1[i] = fabs(m_Genome[genomeOffset++]);
                m_LeftAnkleFlexorLengthFeedbackP1[i] = fabs(m_Genome[genomeOffset++]);
                m_RightHipExtensorLengthFeedbackP1[i] = fabs(m_Genome[genomeOffset++]);
                m_RightHipFlexorLengthFeedbackP1[i] = fabs(m_Genome[genomeOffset++]);
                m_RightKneeExtensorLengthFeedbackP1[i] = fabs(m_Genome[genomeOffset++]);
                m_RightKneeFlexorLengthFeedbackP1[i] = fabs(m_Genome[genomeOffset++]);
                m_RightAnkleExtensorLengthFeedbackP1[i] = fabs(m_Genome[genomeOffset++]);
                m_RightAnkleFlexorLengthFeedbackP1[i] = fabs(m_Genome[genomeOffset++]);
            }
            else
            {
                m_LeftHipExtensorLengthFeedbackP1[i] = 1.0;
                m_LeftHipFlexorLengthFeedbackP1[i] = 1.0;
                m_LeftKneeExtensorLengthFeedbackP1[i] = 1.0;
                m_LeftKneeFlexorLengthFeedbackP1[i] = 1.0;
                m_LeftAnkleExtensorLengthFeedbackP1[i] = 1.0;
                m_LeftAnkleFlexorLengthFeedbackP1[i] = 1.0;
                m_RightHipExtensorLengthFeedbackP1[i] = 1.0;
                m_RightHipFlexorLengthFeedbackP1[i] = 1.0;
                m_RightKneeExtensorLengthFeedbackP1[i] = 1.0;
                m_RightKneeFlexorLengthFeedbackP1[i] = 1.0;
                m_RightAnkleExtensorLengthFeedbackP1[i] = 1.0;
                m_RightAnkleFlexorLengthFeedbackP1[i] = 1.0;
            }

            if (m_NumAllelesPerPhase == 31 || m_NumAllelesPerPhase == 19)
            {
                m_LeftHipExtensorLengthFeedbackP2[i] = 0.5 * m_Genome[genomeOffset++]
                * kLeftHipExtensorLength + kLeftHipExtensorLength;
                m_LeftHipFlexorLengthFeedbackP2[i] = 0.5 * m_Genome[genomeOffset++]
                    * kLeftHipFlexorLength + kLeftHipFlexorLength;
                m_LeftKneeExtensorLengthFeedbackP2[i] = 0.5 * m_Genome[genomeOffset++]
                    * kLeftKneeExtensorLength + kLeftKneeExtensorLength;
                m_LeftKneeFlexorLengthFeedbackP2[i] = 0.5 * m_Genome[genomeOffset++]
                    * kLeftKneeFlexorLength + kLeftKneeFlexorLength;
                m_LeftAnkleExtensorLengthFeedbackP2[i] = 0.5 * m_Genome[genomeOffset++]
                    * kLeftAnkleExtensorLength + kLeftAnkleExtensorLength;
                m_LeftAnkleFlexorLengthFeedbackP2[i] = 0.5 * m_Genome[genomeOffset++]
                    * kLeftAnkleFlexorLength + kLeftAnkleFlexorLength;
                m_RightHipExtensorLengthFeedbackP2[i] = 0.5 * m_Genome[genomeOffset++]
                    * kRightHipExtensorLength + kRightHipExtensorLength;
                m_RightHipFlexorLengthFeedbackP2[i] = 0.5 * m_Genome[genomeOffset++]
                    * kRightHipFlexorLength + kRightHipFlexorLength;
                m_RightKneeExtensorLengthFeedbackP2[i] = 0.5 * m_Genome[genomeOffset++]
                    * kRightKneeExtensorLength + kRightKneeExtensorLength;
                m_RightKneeFlexorLengthFeedbackP2[i] = 0.5 * m_Genome[genomeOffset++]
                    * kRightKneeFlexorLength + kRightKneeFlexorLength;
                m_RightAnkleExtensorLengthFeedbackP2[i] = 0.5 * m_Genome[genomeOffset++]
                    * kRightAnkleExtensorLength + kRightAnkleExtensorLength;
                m_RightAnkleFlexorLengthFeedbackP2[i] = 0.5 * m_Genome[genomeOffset++]
                    * kRightAnkleFlexorLength + kRightAnkleFlexorLength;
            }
            else
            {
                m_LeftHipExtensorLengthFeedbackP2[i] = 1000;
                m_LeftHipFlexorLengthFeedbackP2[i] = 1000;
                m_LeftKneeExtensorLengthFeedbackP2[i] = 1000;
                m_LeftKneeFlexorLengthFeedbackP2[i] = 1000;
                m_LeftAnkleExtensorLengthFeedbackP2[i] = 1000;
                m_LeftAnkleFlexorLengthFeedbackP2[i] = 1000;
                m_RightHipExtensorLengthFeedbackP2[i] = 1000;
                m_RightHipFlexorLengthFeedbackP2[i] = 1000;
                m_RightKneeExtensorLengthFeedbackP2[i] = 1000;
                m_RightKneeFlexorLengthFeedbackP2[i] = 1000;
                m_RightAnkleExtensorLengthFeedbackP2[i] = 1000;
                m_RightAnkleFlexorLengthFeedbackP2[i] = 1000;
            }

            // special case for m_GenomeLength == 45
            if (m_NumAllelesPerPhase == 9)
            {
                m_LeftHipExtensorLengthFeedbackP2[i] = 0.5 * m_Genome[genomeOffset++]
                * kLeftHipExtensorLength + kLeftHipExtensorLength;
                m_RightHipExtensorLengthFeedbackP2[i] = 0.5 * m_Genome[genomeOffset++]
                    * kRightHipExtensorLength + kRightHipExtensorLength;
            }
        }
    }

    if (gDebug == CPGDebug)
    {
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

            cerr << "CPG::ReadGenome\tm_RightHipExtensorLengthFeedbackP1[" << i << "]\t"
                << m_RightHipExtensorLengthFeedbackP1[i] << "\n";
            cerr << "CPG::ReadGenome\tm_RightHipFlexorLengthFeedbackP1[" << i << "]\t"
                << m_RightHipFlexorLengthFeedbackP1[i] << "\n";
            cerr << "CPG::ReadGenome\tm_RightKneeExtensorLengthFeedbackP1[" << i << "]\t"
                << m_RightKneeExtensorLengthFeedbackP1[i] << "\n";
            cerr << "CPG::ReadGenome\tm_RightKneeFlexorLengthFeedbackP1[" << i << "]\t"
                << m_RightKneeFlexorLengthFeedbackP1[i] << "\n";
            cerr << "CPG::ReadGenome\tm_RightAnkleExtensorLengthFeedbackP1[" << i << "]\t"
                << m_RightAnkleExtensorLengthFeedbackP1[i] << "\n";
            cerr << "CPG::ReadGenome\tm_RightAnkleFlexorLengthFeedbackP1[" << i << "]\t"
                << m_RightAnkleFlexorLengthFeedbackP1[i] << "\n";
            cerr << "CPG::ReadGenome\tm_LeftHipExtensorLengthFeedbackP1[" << i << "]\t"
                << m_LeftHipExtensorLengthFeedbackP1[i] << "\n";
            cerr << "CPG::ReadGenome\tm_LeftHipFlexorLengthFeedbackP1[" << i << "]\t"
                << m_LeftHipFlexorLengthFeedbackP1[i] << "\n";
            cerr << "CPG::ReadGenome\tm_LeftKneeExtensorLengthFeedbackP1[" << i << "]\t"
                << m_LeftKneeExtensorLengthFeedbackP1[i] << "\n";
            cerr << "CPG::ReadGenome\tm_LeftKneeFlexorLengthFeedbackP1[" << i << "]\t"
                << m_LeftKneeFlexorLengthFeedbackP1[i] << "\n";
            cerr << "CPG::ReadGenome\tm_LeftAnkleExtensorLengthFeedbackP1[" << i << "]\t"
                << m_LeftAnkleExtensorLengthFeedbackP1[i] << "\n";
            cerr << "CPG::ReadGenome\tm_LeftAnkleFlexorLengthFeedbackP1[" << i << "]\t"
                << m_LeftAnkleFlexorLengthFeedbackP1[i] << "\n";

            cerr << "CPG::ReadGenome\tm_RightHipExtensorLengthFeedbackP2[" << i << "]\t"
                << m_RightHipExtensorLengthFeedbackP2[i] << "\n";
            cerr << "CPG::ReadGenome\tm_RightHipFlexorLengthFeedbackP2[" << i << "]\t"
                << m_RightHipFlexorLengthFeedbackP2[i] << "\n";
            cerr << "CPG::ReadGenome\tm_RightKneeExtensorLengthFeedbackP2[" << i << "]\t"
                << m_RightKneeExtensorLengthFeedbackP2[i] << "\n";
            cerr << "CPG::ReadGenome\tm_RightKneeFlexorLengthFeedbackP2[" << i << "]\t"
                << m_RightKneeFlexorLengthFeedbackP2[i] << "\n";
            cerr << "CPG::ReadGenome\tm_RightAnkleExtensorLengthFeedbackP2[" << i << "]\t"
                << m_RightAnkleExtensorLengthFeedbackP2[i] << "\n";
            cerr << "CPG::ReadGenome\tm_RightAnkleFlexorLengthFeedbackP2[" << i << "]\t"
                << m_RightAnkleFlexorLengthFeedbackP2[i] << "\n";
            cerr << "CPG::ReadGenome\tm_LeftHipExtensorLengthFeedbackP2[" << i << "]\t"
                << m_LeftHipExtensorLengthFeedbackP2[i] << "\n";
            cerr << "CPG::ReadGenome\tm_LeftHipFlexorLengthFeedbackP2[" << i << "]\t"
                << m_LeftHipFlexorLengthFeedbackP2[i] << "\n";
            cerr << "CPG::ReadGenome\tm_LeftKneeExtensorLengthFeedbackP2[" << i << "]\t"
                << m_LeftKneeExtensorLengthFeedbackP2[i] << "\n";
            cerr << "CPG::ReadGenome\tm_LeftKneeFlexorLengthFeedbackP2[" << i << "]\t"
                << m_LeftKneeFlexorLengthFeedbackP2[i] << "\n";
            cerr << "CPG::ReadGenome\tm_LeftAnkleExtensorLengthFeedbackP2[" << i << "]\t"
                << m_LeftAnkleExtensorLengthFeedbackP2[i] << "\n";
            cerr << "CPG::ReadGenome\tm_LeftAnkleFlexorLengthFeedbackP2[" << i << "]\t"
                << m_LeftAnkleFlexorLengthFeedbackP2[i] << "\n";

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

    if (m_UsingFeedbackFunction)
    {
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
    }
    else
    {
        m_RightHipExtensor.SetAlpha(m_RightHipExtensorController[m_Phase]);
        m_RightHipFlexor.SetAlpha(m_RightHipFlexorController[m_Phase]);
        m_RightKneeExtensor.SetAlpha(m_RightKneeExtensorController[m_Phase]);
        m_RightKneeFlexor.SetAlpha(m_RightKneeFlexorController[m_Phase]);
        m_RightAnkleExtensor.SetAlpha(m_RightAnkleExtensorController[m_Phase]);
        m_RightAnkleFlexor.SetAlpha(m_RightAnkleFlexorController[m_Phase]);
        m_LeftHipExtensor.SetAlpha(m_LeftHipExtensorController[m_Phase]);
        m_LeftHipFlexor.SetAlpha(m_LeftHipFlexorController[m_Phase]);
        m_LeftKneeExtensor.SetAlpha(m_LeftKneeExtensorController[m_Phase]);
        m_LeftKneeFlexor.SetAlpha(m_LeftKneeFlexorController[m_Phase]);
        m_LeftAnkleExtensor.SetAlpha(m_LeftAnkleExtensorController[m_Phase]);
        m_LeftAnkleFlexor.SetAlpha(m_LeftAnkleFlexorController[m_Phase]);
    }

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



