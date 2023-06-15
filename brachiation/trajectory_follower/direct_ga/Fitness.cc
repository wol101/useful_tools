// Fitness.cc

// holds the objective function for GA use

#include <stdio.h>
#include <iostream.h>
#include <iomanip.h>
#include <fstream.h>
#include <assert.h>
#include <vector>
#include <math.h>

#include <glut.h>

#include <dmGLTime.h>
#include <dmGLMouse.h>
#include <dmGLPolarCamera_zup.h>
#include <dmEnvironment.h>
#include <dmArticulation.h>
#include <dmMobileBaseLink.h>
#include <dmu.h>

#include <ga/ga.h>
#include <ga/GARealGenome.h>
#include <ga/GAPopulation.h>

#include "Bar.h"
#include "ForceList.h"
#include "Simulation.h"
#include "Controller.h"
#include "ControllerList.h"
#include "GripModel.h"
#include "LoggedForce.h"
#include "Util.h"
#include "Fitness.h"

// Simulation - must be a global instantiated somewhere
Simulation *gSimulation;

// 2.506628275 is sqrt(2 * M_PI)
#define GAUSSIAN(x, mu, sigma) ((1/((sigma)*2.506628275)) * exp(-((x)-(mu))*((x)-(mu)) / (2*(sigma)*(sigma))))
#define UNIT_HEIGHT_GAUSSIAN(x, mu, sigma) (exp(-((x)-(mu))*((x)-(mu)) / (2*(sigma)*(sigma))))

Fitness::Fitness()
{
	gSimulation = 0;
	m_ControlFileName = "ControlFile.dat";
	m_GeneMappingFileName = "GeneMapping.dat";
	m_ConfigFileName = "gibbon.cfg";
	m_TrajectoryFileName = "TargetKinetics.dat";
	m_KineticsFileName = 0;
	m_TargetAndAchievedFileName = 0;
	m_LeftHandX = 0;
	m_RightHandX = 0;
	m_LeftHandY = 0;
	m_RightHandY = 0;
	m_LeftGrip = 0;
	m_RightGrip = 0;	
}

Fitness::~Fitness()
{
	if (gSimulation) delete gSimulation;
	if (m_LeftHandX) delete [] m_LeftHandX;
	if (m_RightHandX) delete [] m_RightHandX;
	if (m_LeftHandY) delete [] m_LeftHandY;
	if (m_RightHandY) delete [] m_RightHandY;
	if (m_LeftGrip) delete [] m_LeftGrip;
	if (m_RightGrip) delete [] m_RightGrip;
}

void
Fitness::ReadInitialisationData()
{
	bool err;
	GeneMapping myGeneMapping;
	int i;
	
	// read the control file
	ifstream controlGenomeFile(m_ControlFileName);
	err = ReadParameter(controlGenomeFile, "genomeLength", m_GenomeLength);
	assert(err == false);
	err = ReadParameter(controlGenomeFile, "runLimit", m_RunLimit);
	if (err)
		m_RunLimit = 5;
	err = ReadParameter(controlGenomeFile, "xPositionWeight", m_XPositionWeight);
	if (err)
		m_XPositionWeight = 1;
	err = ReadParameter(controlGenomeFile, "yPositionWeight", m_YPositionWeight);
	if (err)
		m_YPositionWeight = 1;
	err = ReadParameter(controlGenomeFile, "zPositionWeight", m_ZPositionWeight);
	if (err)
		m_ZPositionWeight = 1;
	err = ReadParameter(controlGenomeFile, "gripWeight", m_GripWeight);
	if (err)
		m_GripWeight = 1;
	err = ReadParameter(controlGenomeFile, "fatalHeightThreshold", m_FatalHeightThreshold);
	if (err)
		m_FatalHeightThreshold = 0;
	err = ReadParameter(controlGenomeFile, "positionTolerance", m_PositionTolerance);
	if (err)
		m_PositionTolerance = 0.1;
	err = ReadParameter(controlGenomeFile, "gripTolerance", m_GripTolerance);
	if (err)
		m_GripTolerance = 10;

	gSimulation = new Simulation();

	// turn off graphics

	gSimulation->SetGraphics(false);

	// turn off SetControllerDataFromFile

	gSimulation->SetControllerDataFromFile(false);

	// load the simulation data file
	gSimulation->Load("gibbon.cfg");
	
	// load the gene mapping file
	ifstream geneMapingFile(m_GeneMappingFileName);
	assert(geneMapingFile.good());
	while (GetQuotedString(geneMapingFile, myGeneMapping.name))
	{
		geneMapingFile >> myGeneMapping.n >> myGeneMapping.start;
		for (i = 0; i < gSimulation->GetControllerList()->GetSize(); i++)
		{
			if (strcmp(gSimulation->GetControllerList()->GetEntry(i)->getName(),
					myGeneMapping.name) == 0)
			{
				myGeneMapping.index = i;
				break;
			}
		}
		assert(i < gSimulation->GetControllerList()->GetSize());
		m_GeneMapping.push_back(myGeneMapping);
	}

	// open the output kinetics file
	if (m_KineticsFileName) m_OutputKinetics.open(m_KineticsFileName);
	
	// read the trajectory file
	if (m_TrajectoryFileName)
	{
		int numRuns = (int)(0.5 + m_RunLimit / gSimulation->GetTimeIncrement());
		m_LeftHandX = new double[numRuns];
		m_RightHandX = new double[numRuns];
		m_LeftHandY = new double[numRuns];
		m_RightHandY = new double[numRuns];
		m_LeftGrip = new double[numRuns];
		m_RightGrip = new double[numRuns];
		double myTime, mySin;
		char buffer[256];
		ifstream trajectoryFile(m_TrajectoryFileName);
		assert(trajectoryFile.good());
		trajectoryFile.getline(buffer, sizeof(buffer) - 1, '\n');
		for (i = 0; i < numRuns; i++)
		{
			trajectoryFile >> myTime >> mySin >> m_LeftHandX[i] >> m_RightHandX[i]
					>> m_LeftHandY[i] >> m_RightHandY[i] 
					>> m_LeftGrip[i] >> m_RightGrip[i];
		}
		m_LeftHandZ = 20;
		m_RightHandZ = 20;
	}
}

double
Fitness::CalculateFitness()
{
	ControllerList *myControllerList = gSimulation->GetControllerList();
	Controller *myController;
	double simData[100]; // can't cope with more than 100 parameters per controller :->
	GARealGenome &myGenome = (GARealGenome &)(*m_Genome);
        dmLink *theLink;
        unsigned int leftHandLinkIndex, rightHandLinkIndex;
	int i, j;
	double score;
	int runCount;
	double error;
        const dmABForKinStruct *theLeftDmABForKinStruct;
        const dmABForKinStruct *theRightDmABForKinStruct;
	ofstream targetAndAchievedFile;
	
#ifdef INLINE_OBJECTIVE
	// reinitialise the model - NB fragile since it hardwires the integrator type
	gSimulation->Reset();
#endif
			
	// assign the controller values based on the current genome
	
	for (i = 0; i < m_GeneMapping.size(); i++)
	{
		for (j = 0; j < m_GeneMapping[i].n; j++)
			simData[j] = myGenome.gene(j);
		myController = myControllerList->GetEntry(m_GeneMapping[i].index);
		myController->SetParameters(m_GeneMapping[i].n, simData);
	}

	if (m_TargetAndAchievedFileName)
	{
		targetAndAchievedFile.open(m_TargetAndAchievedFileName);
		targetAndAchievedFile << "TargetLeftHandX" << "\t" 
				<< "TargetLeftHandY" << "\t"
				<< "TargetLeftHandZ" << "\t" 
				<< "TargetRightHandX" << "\t" 
				<< "TargetRightHandY" << "\t"
				<< "TargetRightHandZ" << "\t" 
				<< "TargetLeftHandGrip" << "\t" 
				<< "TargetRightHandGrip" << "\t" 
				<< "AchievedLeftHandX" << "\t" 
				<< "AchievedLeftHandY" << "\t"
				<< "AchievedLeftHandZ" << "\t" 
				<< "AchievedRightHandX" << "\t" 
				<< "AchievedRightHandY" << "\t"
				<< "AchievedRightHandZ" << "\t"
				<< "AchievedLeftHandGrip" << "\t" 
				<< "AchievedRightHandGrip" << "\n" ;
	}

        // get the indices of the hands
        theLink = (dmLink *) dmuFindObject("left_hand", gSimulation->GetRobot());
        leftHandLinkIndex = gSimulation->GetRobot()->getLinkIndex(theLink);
        theLink = (dmLink *) dmuFindObject("right_hand", gSimulation->GetRobot());
        rightHandLinkIndex = gSimulation->GetRobot()->getLinkIndex(theLink);

	// get the grip forces for scoring
	GripModel *leftHandGrip = (GripModel *) gSimulation->GetForceList()->FindByName("left_hand_grip");
	GripModel *rightHandGrip = (GripModel *) gSimulation->GetForceList()->FindByName("right_hand_grip");

	gErrorCode = 0;
	score = 0;
	runCount = 0;
	while (gSimulation->GetTime() < m_RunLimit)
	{
		gSimulation->UpdateSimulation();
		if (gErrorCode)
			break;

		// get locations and calculate error 
		theLeftDmABForKinStruct = gSimulation->GetRobot()->getForKinStruct(leftHandLinkIndex);
		error = theLeftDmABForKinStruct->p_ICS[XC] - m_LeftHandX[runCount];
		score += m_XPositionWeight * UNIT_HEIGHT_GAUSSIAN(sqrt(error * error), 0, m_PositionTolerance);
		error = theLeftDmABForKinStruct->p_ICS[YC] - m_LeftHandY[runCount];
		score += m_YPositionWeight * UNIT_HEIGHT_GAUSSIAN(sqrt(error * error), 0, m_PositionTolerance);
		error = theLeftDmABForKinStruct->p_ICS[ZC] - m_LeftHandZ;
		score += m_ZPositionWeight * UNIT_HEIGHT_GAUSSIAN(sqrt(error * error), 0, m_PositionTolerance);
		
		theRightDmABForKinStruct = gSimulation->GetRobot()->getForKinStruct(rightHandLinkIndex);
		error = theRightDmABForKinStruct->p_ICS[XC] - m_RightHandX[runCount];
		score += m_XPositionWeight * UNIT_HEIGHT_GAUSSIAN(sqrt(error * error), 0, m_PositionTolerance);
		error = theRightDmABForKinStruct->p_ICS[YC] - m_RightHandY[runCount];
		score += m_YPositionWeight * UNIT_HEIGHT_GAUSSIAN(sqrt(error * error), 0, m_PositionTolerance);
		error = theRightDmABForKinStruct->p_ICS[ZC] - m_RightHandZ;
		score += m_ZPositionWeight * UNIT_HEIGHT_GAUSSIAN(sqrt(error * error), 0, m_PositionTolerance);

		// and match grip
		error = leftHandGrip->getGrip() - m_LeftGrip[runCount];
		score += m_GripWeight * UNIT_HEIGHT_GAUSSIAN(sqrt(error * error), 0, m_GripTolerance);
		error = rightHandGrip->getGrip() - m_RightGrip[runCount];
		score += m_GripWeight * UNIT_HEIGHT_GAUSSIAN(sqrt(error * error), 0, m_GripTolerance);		
		
		if (m_TargetAndAchievedFileName)
		{
			targetAndAchievedFile << m_LeftHandX[runCount] << "\t" 
					<< m_LeftHandY[runCount] << "\t"
					<< m_LeftHandZ << "\t" 
					<< m_RightHandX[runCount] << "\t" 
					<< m_RightHandY[runCount] << "\t"
					<< m_RightHandZ << "\t" 
					<< m_LeftGrip[runCount] << "\t" 
					<< m_RightGrip[runCount] << "\t" 
					<< theLeftDmABForKinStruct->p_ICS[XC] << "\t" 
					<< theLeftDmABForKinStruct->p_ICS[YC] << "\t" 
					<< theLeftDmABForKinStruct->p_ICS[ZC] << "\t" 
					<< theRightDmABForKinStruct->p_ICS[XC] << "\t" 
					<< theRightDmABForKinStruct->p_ICS[YC] << "\t" 
					<< theRightDmABForKinStruct->p_ICS[ZC] << "\t" 
					<< leftHandGrip->getGrip() << "\t" 
					<< rightHandGrip->getGrip() << "\n" ;
		}
		
		// some sanity checking so we can end when things have gone wrong :->
		if (theLeftDmABForKinStruct->p_ICS[ZC] < m_FatalHeightThreshold) break;
		if (theRightDmABForKinStruct->p_ICS[ZC] < m_FatalHeightThreshold) break;

		runCount++;
		if (m_KineticsFileName) OutputKinetics();
	}
	// cerr << score << "\n";
	return score;
}

// function to produce a file of link kinetics

void 
Fitness::OutputKinetics()
{
	int myNumLinks = gSimulation->GetRobot()->getNumLinks();
	const dmABForKinStruct *myDmABForKinStruct;
	unsigned int i;
	int j, k;
	static bool firstTimeFlag = true;
	
	// first time through output the column headings
	if (firstTimeFlag)
	{
		m_OutputKinetics << "time\t";
		for (i = 0; i < myNumLinks; i++)
		{
			m_OutputKinetics << 
				gSimulation->GetRobot()->getLink(i)->getName() 
				<< "_X\t";
			m_OutputKinetics << 
				gSimulation->GetRobot()->getLink(i)->getName() 
				<< "_Y\t";
			m_OutputKinetics << 
				gSimulation->GetRobot()->getLink(i)->getName() 
				<< "_Z\t";
			m_OutputKinetics << 
				gSimulation->GetRobot()->getLink(i)->getName() 
				<< "_R00\t";
			m_OutputKinetics << 
				gSimulation->GetRobot()->getLink(i)->getName() 
				<< "_R01\t";
			m_OutputKinetics << 
				gSimulation->GetRobot()->getLink(i)->getName() 
				<< "_R02\t";
			m_OutputKinetics << 
				gSimulation->GetRobot()->getLink(i)->getName() 
				<< "_R10\t";
			m_OutputKinetics << 
				gSimulation->GetRobot()->getLink(i)->getName() 
				<< "_R11\t";
			m_OutputKinetics << 
				gSimulation->GetRobot()->getLink(i)->getName() 
				<< "_R12\t";
			m_OutputKinetics << 
				gSimulation->GetRobot()->getLink(i)->getName() 
				<< "_r20\t";
			m_OutputKinetics << 
				gSimulation->GetRobot()->getLink(i)->getName() 
				<< "_R21\t";
			m_OutputKinetics << 
				gSimulation->GetRobot()->getLink(i)->getName() 
				<< "_R22";
			if (i != myNumLinks - 1) m_OutputKinetics << "\t";
			else m_OutputKinetics << "\n";		
		}
		firstTimeFlag = false;
	}		 
	
	// start by outputting the simulation time
	m_OutputKinetics << gSimulation->GetTime() << "\t";
	
	for (i = 0; i < myNumLinks; i++)
	{
		// output the position
		myDmABForKinStruct = gSimulation->GetRobot()->getForKinStruct(i);
		m_OutputKinetics << myDmABForKinStruct->p_ICS[XC] << "\t"
			<< myDmABForKinStruct->p_ICS[YC] << "\t"
			<< myDmABForKinStruct->p_ICS[ZC] << "\t";
		// then the orientation
		for (j = 0; j < 3; j++)
		{
			for (k = 0; k < 3; k++)
			{
				m_OutputKinetics << myDmABForKinStruct->R_ICS[j][k];
				if (k != 2) m_OutputKinetics << "\t";
			}
			if (j != 2) m_OutputKinetics << "\t";
		}
		if (i != myNumLinks - 1) m_OutputKinetics << "\t";
		else m_OutputKinetics << "\n";		
	}
}

	
