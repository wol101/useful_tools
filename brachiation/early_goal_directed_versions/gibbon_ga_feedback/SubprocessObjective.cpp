#include <stdio.h>
#include <iostream.h>
#include <iomanip.h>
#include <fstream.h>
#include <assert.h>
#include <exception>

#include <ga/ga.h>
#include <ga/GARealGenome.h>
#include <ga/GAPopulation.h>

#include "GAUtils.h"

float Objective(GAGenome & g);

// the simulation objective

float Objective(GAGenome & g)
{
	ofstream outFile("ControllerGenome.dat");
	ifstream inFile("Score.dat");
	GARealGenome & genome = (GARealGenome &) g;
	float score;
	int i;

	for (i = 0; i < genome.size(); i++)
		outFile << genome.gene(i) << "\n";
	outFile.close();
	if (system("./objective") == 0)
	{
		inFile >> score;
		cout << score << "\n";
		return score;
	} else
	{
		return 0;
	}
}
