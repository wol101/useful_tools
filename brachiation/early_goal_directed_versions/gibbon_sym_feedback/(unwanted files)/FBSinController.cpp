// FBSinController

// cyclic controller based on a series of sin functions but with extra aparameters to allow
// easier feedback implimentation

#include <math.h>
#include <assert.h>

#include "FBSinController.h"

FBSinController::FBSinController():
Controller()
{
	m_nFeedbackValues = 0;
	m_FeedbackValues = 0;
}

FBSinController::FBSinController(char *fileName):Controller(fileName)
{
	m_nFeedbackValues = m_nParameters;
	m_FeedbackValues = new double[m_nFeedbackValues];
	for (int i = 0; i < m_nFeedbackValues; i++) m_FeedbackValues[i] = 1;
}

FBSinController::~FBSinController()
{
	if (m_FeedbackValues) delete []m_FeedbackValues;
}

void FBSinController::SetParameters(int n, double *p)
{
	Controller::SetParameters(n, p);

	// reset feedback values to 1.0
	if (m_FeedbackValues) delete []m_FeedbackValues;
	m_nFeedbackValues = m_nParameters;
	m_FeedbackValues = new double[m_nFeedbackValues];
	for (int i = 0; i < m_nFeedbackValues; i++) m_FeedbackValues[i] = 1;
}

void FBSinController::SetFeedbackValues(int n, double *p)
{
	assert(n == m_nParameters);
	if (m_FeedbackValues) delete []m_FeedbackValues;
	m_nFeedbackValues = n;
	m_FeedbackValues = new double[m_nFeedbackValues];
	for (int i = 0; i < m_nFeedbackValues; i++) m_FeedbackValues[i] = p[i];
}

// change parameters from start to end (i = start; i < end; i++)
void FBSinController::ChangeFeedbackValues(int start, int end, double *p)
{
	int i;

	if (m_nFeedbackValues < end) end = m_nFeedbackValues;
	if (start < 0) start = 0;

	for (i = start; i < end; i++)
		m_FeedbackValues[i] = p[i];
}

#define OCTAVES 4

double FBSinController::Calculate(double time)
{
	double output;
	int i;

// the general equation is:

// output = P0 + P1 sin (P2 time + P3) + P4 sin (P5 time + P6)

// so m_nParameters must be 1, 4, 7, 10 etc

// however we want to normalise the output to be from -1 to +1 and
// we want the parameters to be from -1 to +1

	if (m_nParameters % 3 != 1)
		return 0;

	output = m_Parameters[0] * m_FeedbackValues[0];

	for (i = 1; i < m_nParameters; i += 3)
	{
		output +=
			m_Parameters[i] * m_FeedbackValues[i] * 
			sin(pow(10, m_Parameters[i + 1] * m_FeedbackValues[i + 1] * OCTAVES / 2) *
			time + m_Parameters[i + 2] * m_FeedbackValues[i + 2] * M_PI);
	}

	return output / (1 + (m_nParameters - 1) / 3);
}
