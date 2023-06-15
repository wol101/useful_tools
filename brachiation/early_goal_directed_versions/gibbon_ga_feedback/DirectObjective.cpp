
#include <stdio.h>
#include <iostream.h>
#include <iomanip.h>
#include <fstream.h>
#include <assert.h>

#include <ga/ga.h>
#include <ga/GARealGenome.h>

#include <dmArticulation.h>
#include <dmLink.h>
#include <dmu.h>

#include "Simulation.h"
#include "Controller.h"
#include "ControllerList.h"
#include "Bar.h"
#include <dmMobileBaseLink.h>

#include <Memory.h>

#include "GAUtils.h"

// parameters

extern double gRunLimit;			 // the time to run the simulation for (in simulation units)

// simulation global
Simulation *gSimulation;

// prototype
float Objective(GAGenome &);

// the simulation objective

float Objective(GAGenome & g)
{
	double xPosOnBar, maxXPosOnBar;
	double maxTorsoX, torsoX;
	double simData[100];			 // can't cope with more than 100 parameters per controller :->
	int j;
	int nGenes, startGene;
	ifstream geneMapingFile("gene_mapping.dat");
	char string[NAME_SIZE];
	ControllerList *controllers;
	Controller *theController;
	dmMobileBaseLink *theTorso;
	int linkIndex;
	const dmABForKinStruct *theDmABForKinStruct;

	cerr << "Start FreeMem\t" << FreeMem() << "\t";

	GARealGenome & genome = (GARealGenome &) g;

	gSimulation = new Simulation();

	// turn off graphics

	gSimulation->SetGraphics(false);

	// turn off SetControllerDataFromFile

	gSimulation->SetControllerDataFromFile(false);

	// load the simulation data file
	gSimulation->Load("gibbon.cfg");

	// set the values from the genome

	controllers = gSimulation->GetControllerList();

	while (GetQuotedString(geneMapingFile, string))
	{
		geneMapingFile >> nGenes >> startGene;
		for (j = 0; j < nGenes; j++)
		{
			simData[j] = genome.gene(startGene + j);
		}
		theController = controllers->FindByName(string);
		assert(theController);
		theController->SetParameters(nGenes, simData);
	}

	// and run for required amount of time

	maxXPosOnBar = 0;
	gErrorCode = 0;
	while (gSimulation->GetTime() < gRunLimit)
	{
		gSimulation->UpdateSimulation();
		if (gErrorCode)
			break;

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
	}

	delete gSimulation;

	cerr << "End FreeMem\t" << FreeMem() << "\r";

	return float (maxXPosOnBar + maxTorsoX / 10);
}
