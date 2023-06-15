// Objective.cpp

// this file implements the objective as a separate process
// I'm doing this because of memory leaks (sigh)

#include <stdio.h>
#include <iostream.h>
#include <iomanip.h>
#include <fstream.h>
#include <assert.h>

#include <glut.h>

#include <dmGLTime.h>
#include <dmGLMouse.h>
#include <dmGLPolarCamera_zup.h>
#include <dmEnvironment.h>
#include <dmArticulation.h>
#include <dmMobileBaseLink.h>
#include <dmu.h>

#include "Bar.h"
#include "ForceList.h"
#include "Simulation.h"
#include "Controller.h"
#include "ControllerList.h"
#include "GripModel.h"
#include "LoggedForce.h"
#include "Util.h"

// prototypes
void OutputKinetics(ofstream &outputKineticsFile);

// simulation global
Simulation *gSimulation;

int main(int argc  , char ** argv)
{
	double simData[100];			 // can't cope with more than 100 parameters per controller :->
	int i, j;
	int nGenes, startGene;
	ifstream geneMapingFile("gene_mapping.dat");
	ifstream controlGenomeFile("ControllerGenome.dat");
	char string[NAME_SIZE];
	ControllerList *controllers;
	Controller *theController;
	const dmABForKinStruct *theLeftDmABForKinStruct;
	const dmABForKinStruct *theRightDmABForKinStruct;
	double *data;
	double score;
	int genomeLength;
	double runLimit;			 // the time to run the simulation for (in simulation units)
	ifstream controlFile("ControlFile.dat");
	bool err;
	bool outputKineticsFlag = false;
	ofstream outputKineticsFile;
	char buffer[256];
	double error;
	int runCount;
	double myTime;
	dmLink *theLink;
	unsigned int leftHandLinkIndex, rightHandLinkIndex;
	double lastLeftHandX;
	double lastRightHandX;

	// do some simple stuff with command line arguments
	
	for (i = 1; i < argc; i++)
	{
		if (strcmp(argv[i], "--kinetics") == 0)
			outputKineticsFlag = true;
		
		if (strcmp(argv[i], "--genome") == 0)
		{
			i++;
			if (i >= argc) exit(1);
			controlGenomeFile.close();
			controlGenomeFile.open(argv[i]);
		}
	}
	
	if (outputKineticsFlag) outputKineticsFile.open("Kinetics.dat");
	
	// read in the relevent parts of ControlFile.dat
	err = ReadParameter(controlFile, "genomeLength", genomeLength);
	if (err)
	{
		cerr << "Must Specify genomeLength in ControlFile.dat\n";
		return 1;
	}
	err = ReadParameter(controlFile, "runLimit", runLimit);
	if (err)
		runLimit = 5;

	data = new double[genomeLength];

	gSimulation = new Simulation();

	// turn off graphics

	gSimulation->SetGraphics(false);

	// turn off SetControllerDataFromFile

	gSimulation->SetControllerDataFromFile(false);

	// load the simulation data file
	gSimulation->Load("gibbon.cfg");

	// set the values from the genome

	// load the genome data into the controllers
	for (i = 0; i < genomeLength; i++)
		controlGenomeFile >> data[i];
	controllers = gSimulation->GetControllerList();
	while (GetQuotedString(geneMapingFile, string))
	{
		geneMapingFile >> nGenes >> startGene;
		for (j = 0; j < nGenes; j++)
		{
			simData[j] = data[startGene + j];
		}
		theController = controllers->FindByName(string);
		assert(theController);
		theController->SetParameters(nGenes, simData);
	}
	
	// get the grip forces for scoring
	GripModel *leftHandGrip = (GripModel *) gSimulation->GetForceList()->FindByName("left_hand_grip");
	GripModel *rightHandGrip = (GripModel *) gSimulation->GetForceList()->FindByName("right_hand_grip");

	// get the indices of the hands
	theLink = (dmLink *) dmuFindObject("left_hand", gSimulation->GetRobot());
	leftHandLinkIndex = gSimulation->GetRobot()->getLinkIndex(theLink);
	theLink = (dmLink *) dmuFindObject("right_hand", gSimulation->GetRobot());
	rightHandLinkIndex = gSimulation->GetRobot()->getLinkIndex(theLink);
	
	// load up the trajectory
	int numRuns = (int)(0.5 + runLimit / gSimulation->GetTimeIncrement());
	double *leftHandX = new double[numRuns];
	double *rightHandX = new double[numRuns];
	double *leftHandY = new double[numRuns];
	double *rightHandY = new double[numRuns];
	double *leftGrip = new double[numRuns];
	double *rightGrip = new double[numRuns];
	double dummy;
	ifstream trajectoryFile("TargetKinetics.dat");
	trajectoryFile.getline(buffer, sizeof(buffer) - 1, '\n');
	for (i = 0; i < numRuns; i++)
	{
		trajectoryFile >> myTime >> dummy >> leftHandX[i] >> rightHandX[i]
				>> leftHandY[i] >> rightHandY[i] 
				>> leftGrip[i] >> rightGrip[i];
	}
	double leftHandZ = 20;
	double rightHandZ = 20;
	
	// and run for required amount of time

	gErrorCode = 0;
	score = 1000000;
	runCount = 0;
	lastLeftHandX = 0;
	lastRightHandX = 0;
	while (gSimulation->GetTime() < runLimit)
	{
		gSimulation->UpdateSimulation();
		if (gErrorCode)
			break;

		// get locations and subtract error squared from score
		theLeftDmABForKinStruct = gSimulation->GetRobot()->getForKinStruct(leftHandLinkIndex);
		error = theLeftDmABForKinStruct->p_ICS[XC] - leftHandX[runCount];
		score -= sqrt(error * error);
		error = theLeftDmABForKinStruct->p_ICS[YC] - leftHandY[runCount];
		score -= sqrt(error * error);
		error = theLeftDmABForKinStruct->p_ICS[ZC] - leftHandZ;
		score -= 0.1 * sqrt(error * error); // weaker selection
		
		theRightDmABForKinStruct = gSimulation->GetRobot()->getForKinStruct(rightHandLinkIndex);
		error = theRightDmABForKinStruct->p_ICS[XC] - rightHandX[runCount];
		score -= sqrt(error * error);
		error = theRightDmABForKinStruct->p_ICS[YC] - rightHandY[runCount];
		score -= sqrt(error * error);
		error = theRightDmABForKinStruct->p_ICS[ZC] - rightHandZ;
		score -= 0.1 * sqrt(error * error); // weaker selection

		// and match grip
		/*error = (leftHandGrip->getGrip() - leftGrip[runCount]) / 100;
		score -= sqrt(error * error);
		error = (rightHandGrip->getGrip() - rightGrip[runCount]) / 100;
		score -= sqrt(error * error);*/
				
		runCount++;
		if (outputKineticsFlag) OutputKinetics(outputKineticsFile);
	}

	ofstream scoreFile("Score.dat");
	if (score < 0) score  = 0;
	scoreFile << score << "\n";

	delete [] data;
	delete [] leftHandX;
	delete [] rightHandX;
	delete [] leftHandY;
	delete [] rightHandY;
	delete [] leftGrip;
	delete [] rightGrip;

	return 0;
}

// function to produce a file of link kinetics

void OutputKinetics(ofstream &outputKineticsFile)
{
	int myNumLinks = gSimulation->GetRobot()->getNumLinks();
	const dmABForKinStruct *myDmABForKinStruct;
	unsigned int i;
	int j, k;
	static bool firstTimeFlag = true;
	
	// first time through output the column headings
	if (firstTimeFlag)
	{
		outputKineticsFile << "time\t";
		for (i = 0; i < myNumLinks; i++)
		{
			outputKineticsFile << 
				gSimulation->GetRobot()->getLink(i)->getName() 
				<< "_X\t";
			outputKineticsFile << 
				gSimulation->GetRobot()->getLink(i)->getName() 
				<< "_Y\t";
			outputKineticsFile << 
				gSimulation->GetRobot()->getLink(i)->getName() 
				<< "_Z\t";
			outputKineticsFile << 
				gSimulation->GetRobot()->getLink(i)->getName() 
				<< "_R00\t";
			outputKineticsFile << 
				gSimulation->GetRobot()->getLink(i)->getName() 
				<< "_R01\t";
			outputKineticsFile << 
				gSimulation->GetRobot()->getLink(i)->getName() 
				<< "_R02\t";
			outputKineticsFile << 
				gSimulation->GetRobot()->getLink(i)->getName() 
				<< "_R10\t";
			outputKineticsFile << 
				gSimulation->GetRobot()->getLink(i)->getName() 
				<< "_R11\t";
			outputKineticsFile << 
				gSimulation->GetRobot()->getLink(i)->getName() 
				<< "_R12\t";
			outputKineticsFile << 
				gSimulation->GetRobot()->getLink(i)->getName() 
				<< "_r20\t";
			outputKineticsFile << 
				gSimulation->GetRobot()->getLink(i)->getName() 
				<< "_R21\t";
			outputKineticsFile << 
				gSimulation->GetRobot()->getLink(i)->getName() 
				<< "_R22";
			if (i != myNumLinks - 1) outputKineticsFile << "\t";
			else outputKineticsFile << "\n";		
		}
		firstTimeFlag = false;
	}		 
	
	// start by outputting the simulation time
	outputKineticsFile << gSimulation->GetTime() << "\t";
	
	for (i = 0; i < myNumLinks; i++)
	{
		// output the position
		myDmABForKinStruct = gSimulation->GetRobot()->getForKinStruct(i);
		outputKineticsFile << myDmABForKinStruct->p_ICS[XC] << "\t"
			<< myDmABForKinStruct->p_ICS[YC] << "\t"
			<< myDmABForKinStruct->p_ICS[ZC] << "\t";
		// then the orientation
		for (j = 0; j < 3; j++)
		{
			for (k = 0; k < 3; k++)
			{
				outputKineticsFile << myDmABForKinStruct->R_ICS[j][k];
				if (k != 2) outputKineticsFile << "\t";
			}
			if (j != 2) outputKineticsFile << "\t";
		}
		if (i != myNumLinks - 1) outputKineticsFile << "\t";
		else outputKineticsFile << "\n";		
	}
}

