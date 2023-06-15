// Controller Parent Class

// provides basic functionality

// can read itself from a file

// can calculate itself for any given time

#include <iostream.h>
#include <iomanip.h>
#include <fstream.h>

#include "Controller.h"
#include "ControllerList.h"
#include "Simulation.h"

extern Simulation *gSimulation;

Controller::Controller()
{
	m_nParameters = 0;
	m_Parameters = 0;

	gSimulation->GetControllerList()->AddController(this);

}

Controller::Controller(char *fileName)
{
	int i;
	ifstream inFile(fileName);

	m_nParameters = 0;
	m_Parameters = 0;

	try
	{

		// read the number of parameters
		inFile >> m_nParameters;

		m_Parameters = new double[m_nParameters];

		for (i = 0; i < m_nParameters; i++)
			inFile >> m_Parameters[i];

		inFile.close();
	}
	catch(...)
	{
		if (m_Parameters)
			delete[]m_Parameters;
		m_nParameters = 0;
		m_Parameters = 0;
	}
	gSimulation->GetControllerList()->AddController(this);
}

Controller::~Controller()
{
	if (m_Parameters)
		delete[]m_Parameters;
}

void Controller::SetParameters(int n, double *p)
{
	int i;

	if (m_Parameters)
		delete[]m_Parameters;

	m_nParameters = n;
	m_Parameters = new double[m_nParameters];

	for (i = 0; i < m_nParameters; i++)
		m_Parameters[i] = p[i];
}

// change parameters from start to end (i = start; i < end; i++)
void Controller::ChangeParameters(int start, int end, double *p)
{
	int i;

	if (m_nParameters < end) end = m_nParameters;
	if (start < 0) start = 0;

	for (i = start; i < end; i++)
		m_Parameters[i] = p[i];
}
