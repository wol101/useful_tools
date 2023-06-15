// Controller Parent Class

// provides basic functionality

// can read itself from a file

// can calculate itself for any given time

#include <iostream.h>
#include <iomanip.h>
#include <fstream.h>
#include <string.h>

#include "Controller.h"
#include "ControllerList.h"
#include "Simulation.h"
#include "DebugControl.h"

extern Simulation *gSimulation;

Controller::Controller()
{
	m_nParamaters = 0;
	m_Parameters = 0;

	gSimulation->GetControllerList()->AddController(this);

}

Controller::Controller(char *fileName)
{
	int i;
	ifstream inFile(fileName);

	m_nParamaters = 0;
	m_Parameters = 0;

	try
	{

		// read the number of parameters
		inFile >> m_nParamaters;

		m_Parameters = new double[m_nParamaters];

		for (i = 0; i < m_nParamaters; i++)
			inFile >> m_Parameters[i];

		inFile.close();
	}
	catch(...)
	{
		if (m_Parameters)
			delete[]m_Parameters;
		m_nParamaters = 0;
		m_Parameters = 0;
	}
	gSimulation->GetControllerList()->AddController(this);
}

Controller::~Controller()
{
	if (m_Parameters)
		delete[]m_Parameters;
}

void Controller::SetName(const char * const prefix, const char * const suffix)
{
  char *buffer;
  
  buffer = new char[strlen(prefix) + strlen(suffix) + 1];
  strcpy(buffer, prefix);
  strcat(buffer, suffix);
  setName(buffer);
  delete [] buffer;
}

void Controller::SetParameters(int n, double *p)
{
	int i;

	if (m_Parameters)
		delete[]m_Parameters;

	m_nParamaters = n;
	m_Parameters = new double[m_nParamaters];

	for (i = 0; i < m_nParamaters; i++)
		m_Parameters[i] = p[i];
	
	if (gDebug == ControllerDebug)
	{
		cerr << "Controller::SetParameters" << "\t" << m_name << "\t";
		for (i = 0; i < m_nParamaters; i++)
			cerr << m_Parameters[i] << "\t";
		cerr << "\n";
	}	
}
