// this routine does the GA - it runs objective as a separate process
// to overcome memory leak problems in the simulation code

#include <stdio.h>
#include <iostream.h>
#include <iomanip.h>
#include <fstream.h>
#include <assert.h>
#include <exception>

#include <ga/ga.h>
#include <ga/GARealGenome.h>
#include <ga/GARealGenome.C>
#include <ga/GAPopulation.h>

#include "Util.h"

#ifdef INLINE_OBJECTIVE
#include "Fitness.h"
Fitness gFitness;
#endif

float Objective(GAGenome & g);
int GARealGaussianMutatorScaled(GAGenome & g, float pmut);

double gGaussianSD;

int main(int /*argc */ , char ** /*argv */ )
{
	char buffer[NAME_SIZE];
	ofstream *outp;
	int i, j;
	GAPopulation pop;
	GARealGenome *theGenome;
	ifstream controlFile("ControlFile.dat");
	bool err;
	int innerLoop;
	int outerLoop;
	double lowerBound;
	double upperBound;
	int genomeLength;


#ifdef INLINE_OBJECTIVE
	gFitness.SetTrajectoryFileName("TargetTrajectory.dat");
	gFitness.ReadInitialisationData();
#endif
	
	// read in my parameters

	err = ReadParameter(controlFile, "innerLoop", innerLoop);
	if (err)
		innerLoop = 10;
	err = ReadParameter(controlFile, "outerLoop", outerLoop);
	if (err)
		outerLoop = 10;
	err = ReadParameter(controlFile, "lowerBound", lowerBound);
	if (err)
		lowerBound = -1;
	err = ReadParameter(controlFile, "upperBound", upperBound);
	if (err)
		upperBound = 1;
	err = ReadParameter(controlFile, "genomeLength", genomeLength);
	if (err)
	{
		cerr << "Must Specify genomeLength in ControlFile.dat\n";
		return 1;
	}
		
	// set the gaussian width to one twentieth of the range
	
	err = ReadParameter(controlFile, "gaussianSD", gGaussianSD);
	if (err)
		gGaussianSD = (upperBound - lowerBound) / 100;

// This genome uses a bounded set of continous numbers.  The default arguments
// are INCLUSIVE for both the lower and upper bounds, so in this case the 
// allele set is [-1, 1] and any element of the genome may assume a value [-1, 1].

	GARealAlleleSet alleles(lowerBound, upperBound);

// NB need to set the genome here and in the read loop
	GARealGenome genome(genomeLength, alleles, Objective);
// the defaults are UniformInitializer, ElementComparator, GARealGassianMutator, UniformCrossover
// NB any changes also have to be made later in the program

	genome.crossover(GARealBlendCrossover);	
	// genome.crossover(GARealGenome::TwoPointCrossover);	

  	genome.mutator(GARealGaussianMutatorScaled);
	// genome.mutator(GARealGenome::FlipMutator); 


// Now do a genetic algorithm for each one of the genomes that we created.

	GASteadyStateGA ga(genome);
	ga.parameters("Parameters.dat", gaTrue); // load up the parameters

	// load up a starting genome if it exists

	FILE	*file = fopen("StartingPopulation.dat", "r");
	bool	fileExists = false;
	if (file)
	{
		fileExists = true;
		fclose(file);
	}
	
	if (fileExists)
	{
		ifstream inGenome("StartingPopulation.dat");
		cout << "Loading StartingPopulation.dat population\n";

		for (i = 0; i < ga.populationSize(); i++)
		{
			theGenome = new GARealGenome(genomeLength, alleles, Objective);
			theGenome->crossover(GARealBlendCrossover);
  			theGenome->mutator(GARealGaussianMutatorScaled);
			inGenome >> (*theGenome);
			pop.add(theGenome);
		}
		// do the bits in ga.initialize() that are needed
		GARandomSeed(0);
		pop.evaluate(gaTrue);
		ga.population(pop);
		// can't work out a way of resetting the stats - hopefully non necessary
	} 
	else
	{
		cout << "Initialising new population\n";
		ga.initialize();
	}

		// evolutionary loop
	for (i = 0; i < outerLoop; i++)
	{
		for (j = 0; j < innerLoop; j++)
		{
			ga.step();
		}
		ga.flushScores();
		sprintf(buffer, "Pop_%03d", i);
		outp = new ofstream(buffer);
		(*outp) << ga.population();
		delete outp;
		sprintf(buffer, "Best_%03d", i);
		outp = new ofstream(buffer);
		(*outp) << ga.statistics().bestIndividual();
		delete outp;
	}

	return 0;
}

// altered so the Gaussian SD is controlled by a global value

/* ----------------------------------------------------------------------------
   Operator specializations
---------------------------------------------------------------------------- */
// The Gaussian mutator picks a new value based on a Gaussian distribution
// around the current value.  We respect the bounds (if any).
//*** need to figure out a way to make the stdev other than 1.0 
int GARealGaussianMutatorScaled(GAGenome & g, float pmut)
{
	GA1DArrayAlleleGenome < float >&child = DYN_CAST(GA1DArrayAlleleGenome < float >&, g);
	register int n, i;
	if (pmut <= 0.0)
		return (0);

	float nMut = pmut * (float) (child.length());
	int length = child.length() - 1;
	if (nMut < 1.0)
	{					 // we have to do a flip test on each element
		nMut = 0;
		for (i = length; i >= 0; i--)
		{
			float value = child.gene(i);
			if (GAFlipCoin(pmut))
			{
				if (child.alleleset(i).type() == (int) GAAllele::ENUMERATED ||
				    child.alleleset(i).type() == (int) GAAllele::DISCRETIZED)
					value = child.alleleset(i).allele();
				else if (child.alleleset(i).type() == (int) GAAllele::BOUNDED)
				{
					value += GAUnitGaussian() * gGaussianSD;
					value = GAMax(child.alleleset(i).lower(), value);
					value = GAMin(child.alleleset(i).upper(), value);
				}
				child.gene(i, value);
				nMut++;
			}
		}
	} else
	{					 // only mutate the ones we need to
		for (n = 0; n < nMut; n++)
		{
			int idx = GARandomInt(0, length);
			float value = child.gene(idx);
			if (child.alleleset(idx).type() == (int) GAAllele::ENUMERATED ||
			    child.alleleset(idx).type() == (int) GAAllele::DISCRETIZED)
				value = child.alleleset(idx).allele();
			else if (child.alleleset(idx).type() == (int) GAAllele::BOUNDED)
			{
				value += GAUnitGaussian() * gGaussianSD;
				value = GAMax(child.alleleset(idx).lower(), value);
				value = GAMin(child.alleleset(idx).upper(), value);
			}
			child.gene(idx, value);
		}
	}
	return ((int) nMut);
}

