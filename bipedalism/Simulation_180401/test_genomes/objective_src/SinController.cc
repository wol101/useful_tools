// SinController

// cyclic controller based on a series of sin functions

#include <math.h>
#include <assert.h>

#include "SinController.h"

SinController::SinController():
Controller()
{
}

SinController::SinController(char *fileName):Controller(fileName)
{
}

SinController::~SinController()
{
}

#define OCTAVES 6

double SinController::Calculate(double time)
{
	double output;
	int i;

// the general equation is:

// output = P0 + P1 sin (P2 time + P3) + P4 sin (P5 time + P6)

// so m_nParamaters must be 1, 4, 7, 10 etc

// however we want to normalise the output to be from -1 to +1 and
// we want the parameters to be from -1 to +1
// and the time parameter needs to be a power term to cover a sensible range

	assert(m_nParamaters);
	if (m_nParamaters % 3 != 1)
		return 0;

	output = m_Parameters[0];

	for (i = 1; i < m_nParamaters; i += 3)
	{
		output +=
			m_Parameters[i] * sin(pow(10, m_Parameters[i + 1] * OCTAVES / 2) *
					      time + m_Parameters[i + 2] * M_PI);
	}

	return output / (1 + (m_nParamaters - 1) / 3);
}
