#include <stdio.h>
#include <iostream.h>
#include <iomanip.h>
#include <fstream.h>
#include <assert.h>

#include <ga/ga.h>

#include "Util.h"

float Objective(GAGenome & g);

// the simulation objective

float Objective(GAGenome & g)
{
	ofstream outFile("genome.tmp");
	float score;

	outFile << g;
	outFile.close();
	if (system("./objective") == 0)
	{
		ifstream inFile("score.tmp");
		inFile >> score;
		// cerr << score << "\n";
		return score;
	} else
	{
		return 0;
	}
}
