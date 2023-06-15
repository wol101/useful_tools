// Objective.cc - class to hold details of running objectives

#include <stdio.h>
#include <iostream.h>
#include <fstream.h>
#include <strstream.h>
#include <assert.h>
#include <unistd.h>
#include <stdlib.h>

#include <pinet.h>

#include "Objective.h"
#include "Genome.h"
#include "Debug.h"
#include "Genome.h"

#include "../DataFile/DataFile.h"

// the server
extern pt::ipstmserver g_svr;

// constructor
Objective::Objective()
{
  m_RunID = 0;
  m_RunningFlag = false;
  m_Fitness = 0;
  m_Genome = 0;
  m_TimeLimit = 0;
  
  strcpy(m_ProgramName, "objective_socket");
  m_ExtraArguments = 0;
  m_ExtraData = 0;
  m_Client = 0;
}

// destructor
Objective::~Objective()
{
   if (m_ExtraData) delete [] m_ExtraData;
   if (m_ExtraArguments) delete [] m_ExtraArguments;
   if (m_Client) delete m_Client;
}

void Objective::SetProgramName(const char * name)
{
  strcpy(m_ProgramName, name);
}

void Objective::SetExtraData(const char * data)
{
  if (m_ExtraData) delete [] m_ExtraData;
  m_ExtraDataLength = strlen(data);
  m_ExtraData = new char[m_ExtraDataLength + 1];
  strcpy(m_ExtraData, data);
}

void Objective::SetExtraArguments(const char * data)
{
  if (m_ExtraArguments) delete [] m_ExtraArguments;
  m_ExtraArgumentsLength = strlen(data);
  m_ExtraArguments = new char[m_ExtraArgumentsLength + 1];
  strcpy(m_ExtraArguments, data);
}

// set an objective run going
// returns false if 
bool Objective::Run()
{
  m_Client = new pt::ipstream();
  int len;
  char buffer[16];
  
  if (g_svr.poll() == false)
  {
    // no client asking for a task
    delete m_Client;
    m_Client = 0;
    return false;
  }
  
  if (g_svr.serve(*m_Client) == false)
  {
    // some problem communicating
    delete m_Client;
    m_Client = 0;
    return false;
  }
  
  try
  {
    // read the control word
    if (m_Client->waitfor(1) == false) throw(0); // nothing to read
    m_Client->read(buffer, 16);
    if (strcmp(buffer, "ReadyToGo") != 0) throw (0);
    
    // data passed as a 16 byte string containing the integer length
    // followed by the actual string (length bytes long)
    
    if (m_ExtraArguments)
    {
      len = strlen(m_ExtraArguments) + 1;
      sprintf(buffer, "%d", len);
      m_Client->write(buffer, 16);
      m_Client->write(m_ExtraArguments, len);
      m_Client->flush();
    }
    
    if (m_ExtraData)
    {
      len = strlen(m_ExtraData) + 1;
      sprintf(buffer, "%d", len);
      m_Client->write(buffer, 16);
      m_Client->write(m_ExtraData, len);
      m_Client->flush();
    }
    
    strstream data;
    data << *m_Genome;
    
    len = strlen(data.str()) + 1;
    sprintf(buffer, "%d", len);
    m_Client->write(buffer, 16);
    m_Client->write(data.str(), len);
    m_Client->flush();
  }
  
  catch(...)
  {
    delete m_Client;
    m_Client = 0;
    return false;
  }
  
  m_RunningFlag = true;
  m_StartTime = time(0);
  return true;   
}

// check to see whether the command has finished
// and read in the score (we do it here because it makes a bit more sense)

bool Objective::CheckRunning()
{
  char buffer[64];
  
  if (m_RunningFlag == false) return false; // nothing to check
  
  assert(m_Client); // must exist
  
  try
  {
    if (m_TimeLimit) // check to see whether we are over time
    {
       if (time(0) - m_StartTime > m_TimeLimit)
       {
          throw(0);
       }
    }
    
    // is there a result for us?
    if (m_Client->waitfor(1) == false) return true; // still running
    
    m_Client->read(buffer, 64); // result is a 64 byte string
    m_Fitness = strtod(buffer, 0);

    m_RunningFlag = false;
    return false;
  }
  
  catch (...)
  {
    delete m_Client;
    m_Client = 0;
    
    m_RunningFlag = false;
    while (Run() == false) 
    {
      usleep(10000);
    }
    return m_RunningFlag;
  }
}


