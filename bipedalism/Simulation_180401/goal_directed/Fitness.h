// Fitness.h

// holds the objective function for GA use

#ifndef FITNESS_H
#define FITNESS_H

#include <vector>
#include "Util.h"
#include <dm.h>

class ModifiedContactModel;

struct GeneMapping
{
	char	name[NAME_SIZE];	// name of the controller
	int	start;			// start gene
	int	n;			// number of genes
	int	index;			// controller index
};

class Fitness
{
	public:
			
	Fitness();	// default constructor
	~Fitness();	// destructor
	
	void ReadInitialisationData(bool graphicsFlag);
	double CalculateFitness();
	
	void SetGenomeFileName(char *name) { m_GenomeFileName = name; };
	void SetKineticsFileName(char *name) { m_KineticsFileName = name; };
	void ConvertLocalToWorldP(const dmABForKinStruct *m, const CartesianVector local, CartesianVector world);
	void ConvertLocalToWorldV(const dmABForKinStruct *m, const SpatialVector local, SpatialVector world);
	void GetContactLimits(ModifiedContactModel *contactModel, const dmABForKinStruct *m,
		double &xMin, double &yMin, double &zMin, 
		double &xMax, double &yMax, double &zMax);
	bool SanityCheck(const SpatialVector v);

	protected:

	void OutputKinetics();
			
	double				*m_Genome;
	int				m_GenomeLength;
	double				m_RunLimit;
	
	std::vector<GeneMapping>	m_GeneMapping;
	char				*m_GenomeFileName;
	char				*m_ControlFileName;
	char				*m_GeneMappingFileName;
	char				*m_KineticsFileName;
	char				*m_ConfigFileName;

	ofstream			m_OutputKinetics;
};

#endif // FITNESS_H
