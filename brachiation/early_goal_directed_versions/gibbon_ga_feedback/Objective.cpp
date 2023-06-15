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
#include "FBGripModel.h"
#include "LoggedForce.h"
#include "GAUtils.h"

// simulation global
Simulation *gSimulation;

int main(int /*argc */ , char ** /*argv */ )
{
	double xPosOnBar, maxXPosOnBar;
	double maxTorsoX, torsoX;
	double simData[1000];			 // can't cope with more than 1000 parameters per controller :->
	int i, j;
	int nGenes, startGene;
	ifstream geneMapingFile("gene_mapping.dat");
	ifstream controlGenomeFile("ControllerGenome.dat");
	char string[NAME_SIZE];
	ControllerList *controllers;
	Controller *theController;
	dmMobileBaseLink *theTorso;
	int linkIndex;
	const dmABForKinStruct *theDmABForKinStruct;
	double *data;
	FBGripModel *leftHandGrip;
	FBGripModel *rightHandGrip;
	double leftContactScore;
	double rightContactScore;
	double lastTime;
	double timeInc;
	double score;
	double leftContactDuration;
	double rightContactDuration;
	double energy;
	double power;
	int genomeLength;
	double runLimit;			 // the time to run the simulation for (in simulation units)
	ifstream controlFile("ControlFile.dat");
	bool err;
	double bothContactDuration;
	double positionOnBarWeight;
	double torsoXPositionWeight;
	double contactScoreWeight;
	double energyWeight;
	double bothContactTimeWeight;

	err = ReadParameter(controlFile, "genomeLength", genomeLength);
	if (err)
	{
		cerr << "Must Specify genomeLength in ControlFile.dat\n";
		return 1;
	}
	err = ReadParameter(controlFile, "runLimit", runLimit);
	if (err)
		runLimit = 5;
	err = ReadParameter(controlFile, "positionOnBarWeight", positionOnBarWeight);
	assert(err == 0);
	err = ReadParameter(controlFile, "torsoXPositionWeight", torsoXPositionWeight);
	assert(err == 0);
	err = ReadParameter(controlFile, "contactScoreWeight", contactScoreWeight);
	assert(err == 0);
	err = ReadParameter(controlFile, "energyWeight", energyWeight);
	assert(err == 0);
	err = ReadParameter(controlFile, "bothContactTimeWeight", bothContactTimeWeight);
	assert(err == 0);


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
	leftHandGrip = (FBGripModel *) gSimulation->GetForceList()->FindByName("left_hand_grip");
	rightHandGrip = (FBGripModel *) gSimulation->GetForceList()->FindByName("right_hand_grip");
	// and run for required amount of time

	maxXPosOnBar = 0;
	gErrorCode = 0;
	leftContactScore = rightContactScore = 0;
	lastTime = 0;
	score = 0;
	leftContactDuration = rightContactDuration = bothContactDuration = 0;
	maxTorsoX = 0;
	energy = 0;
	while (gSimulation->GetTime() < runLimit)
	{
		gSimulation->UpdateSimulation();
		timeInc = gSimulation->GetTime() - lastTime;
		if (gErrorCode)
			break;

		// score contact time
		if (leftHandGrip->anyContact() && rightHandGrip->anyContact())	// both in contact
		{
			leftContactDuration = 0;
			rightContactDuration = 0;
			bothContactDuration += timeInc;
		} else
		{
			if (leftHandGrip->anyContact())
			{
				leftContactDuration += timeInc;
				leftContactScore += timeInc / leftContactDuration;
			} else
			{
				if (rightHandGrip->anyContact())
					leftContactDuration = 0;
			}

			if (rightHandGrip->anyContact())
			{
				rightContactDuration += timeInc;
				rightContactScore += timeInc / rightContactDuration;
			} else
			{
				if (leftHandGrip->anyContact())
					rightContactDuration = 0;
			}
		}

		// and calculate the value from the amount of X progressionon the bar

		xPosOnBar = gSimulation->GetBar()->GetXContact();
		if (xPosOnBar > maxXPosOnBar)
			maxXPosOnBar = xPosOnBar;

		// and the position of the torso
		theTorso = (dmMobileBaseLink *) dmuFindObject("torso", gSimulation->GetRobot());
		linkIndex = gSimulation->GetRobot()->getLinkIndex((dmLink *) theTorso);
		theDmABForKinStruct = gSimulation->GetRobot()->getForKinStruct(linkIndex);
		// sanity check
		if (OutRange(theDmABForKinStruct->p_ICS[XC], 0, 200) ||
		    OutRange(theDmABForKinStruct->p_ICS[YC], 0, 30) ||
		    OutRange(theDmABForKinStruct->p_ICS[ZC], 0, 30))
			break;			 // fallen off or gone wild
		torsoX = theDmABForKinStruct->p_ICS[XC];
		if (torsoX > maxTorsoX)
			maxTorsoX = torsoX;

		// calculate energy driving system
		for (i = 0; i < gSimulation->GetForceList()->GetSize(); i++)
		{
			// all forces in the list are actually derived from LoggedForce
			power =
				((LoggedForce
				  *) (gSimulation->GetForceList()->GetEntry(i)))->getPower();
			if (power >= 0)
				energy += power * timeInc;	// positive work
			else
				energy -= power * timeInc * 0.5;	// negative work -  a bit cheaper
		}

		lastTime = gSimulation->GetTime();
	}

	ofstream scoreFile("Score.dat");
	score =
		positionOnBarWeight * maxXPosOnBar +
		torsoXPositionWeight * maxTorsoX +
		contactScoreWeight * leftContactScore +
		contactScoreWeight * rightContactScore +
		energyWeight * energy + bothContactTimeWeight * bothContactDuration;

	if (score < 0)				 // we need to know what's happened
	{
		cout << "ERROR: score less than zero\n";
		cout << "maxXPosOnBar " << maxXPosOnBar << "\n";
		cout << "maxTorsoX " << maxTorsoX << "\n";
		cout << "leftContactScore " << leftContactScore << "\n";
		cout << "rightContactScore " << rightContactScore << "\n";
		cout << "energy " << energy << "\n";
		cout << "score " << score << "\n";
		score = 0;
	}
	scoreFile << score << "\n";

#ifdef DEBUG
	cout << "maxXPosOnBar " << maxXPosOnBar << "\n";
	cout << "maxTorsoX " << maxTorsoX << "\n";
	cout << "leftContactScore " << leftContactScore << "\n";
	cout << "rightContactScore " << rightContactScore << "\n";
	cout << "energy " << energy << "\n";
	cout << "score " << score << "\n";
#endif

	delete[]data;

	return 0;
}
