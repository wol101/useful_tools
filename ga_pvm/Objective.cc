// Objective.cc - class to hold details of running objectives

#include <stdio.h>
#include <iostream.h>
#include <fstream.h>
#include <assert.h>
#include <unistd.h>
#include <stdlib.h>

#include <pvm3.h>

#include "Objective.h"
#include "Genome.h"
#include "Debug.h"
#include "PVMMessages.h"
#include "Genome.h"

#include "../DataFile/DataFile.h"

extern FILE *gPVMLogFile;

// constructor
Objective::Objective()
{
  m_RunID = 0;
  m_RunningFlag = false;
  m_Fitness = 0;
  m_ExtraArgumentsCount = 0;
  m_Genome = 0;
  m_TimeLimit = 0;
  m_RestartOnError = false;
  
  strcpy(m_ProgramName, "objective_pvm");
  strcpy(m_ExtraArguments, "");
  m_ExtraArgumentsList[0] = 0;
  m_ExtraData = 0;
}

// destructor
Objective::~Objective()
{
   if (m_ExtraData) delete [] m_ExtraData;
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

void Objective::SetExtraArguments(const char * args)
{
  strcpy(m_ExtraArguments, args);
  m_ExtraArgumentsCount = DataFile::ReturnTokens(m_ExtraArguments, m_ExtraArgumentsList, 256);
  m_ExtraArgumentsList[m_ExtraArgumentsCount] = 0;
}

// test to see whether load conditions will allow another process to start
// and if so returns the host name
// uses a very simple algorithm. Each host is allowed 1 process per 1000 speed points
char * Objective::GetPreferredHost(char *command)
{
  int info;
  struct pvmhostinfo *hostp;
  int nhost, narch;
  struct pvmtaskinfo *taskp;
  int ntask;
  int i, j;
  int nObj;
  
  // get number of hosts
  info = pvm_config( &nhost, &narch, &hostp );
  
  // loop through hosts
  for (i = 0; i < nhost; i++)
  {
    // get task info for this host
    info = pvm_tasks( hostp[i].hi_tid, &ntask, &taskp );
    // if there is a command only count matches
    if (command)
    {
      nObj = 0;
      for (j = 0; j < ntask; j++)
      {
        // cerr << taskp[j].ti_a_out << "\n";
        if (strcmp(command, taskp[j].ti_a_out) == 0) nObj++;
      }
    }
    else
    {
      nObj = ntask;
    }
    // cerr << hostp[i].hi_name << " " << nObj << " " 
    //    << ((nObj * 1000) / hostp[i].hi_speed) << "\n";
    if ( ((nObj * 1000) / hostp[i].hi_speed) < 1 )
    {
      return hostp[i].hi_name;
    }
  }
  
  return 0;
}

// count the tasks matching 'command' (or all tasks if command = 0)  
int Objective::CountTasks(char *command)
{
  int info;
  struct pvmhostinfo *hostp;
  int nhost, narch;
  struct pvmtaskinfo *taskp;
  int ntask;
  int i, j;
  int nObj = 0;
  
  // get number of hosts
  info = pvm_config( &nhost, &narch, &hostp );
  
  // loop through hosts
  for (i = 0; i < nhost; i++)
  {
    // get task info for this host
    info = pvm_tasks( hostp[i].hi_tid, &ntask, &taskp );
    // if there is a command only count matches
    if (command)
    {
      for (j = 0; j < ntask; j++)
        if (strcmp(command, taskp[j].ti_a_out) == 0) nObj++;
    }
    else
    {
      nObj += ntask;
    }
  }
  
  return nObj;
}
  
    
// set an objective run going
void Objective::Run(char *preferredHost)
{
  int numt = 0;
  int info;
  int bufid;
  
  try
  {   
    // run the objective command
    if (preferredHost)
      numt = pvm_spawn(m_ProgramName, m_ExtraArgumentsList, PvmTaskHost,
         preferredHost, 1, &m_Child);
    else
      numt = pvm_spawn(m_ProgramName, m_ExtraArgumentsList, PvmTaskDefault,
         0, 1, &m_Child);

    if (numt != 1) // trouble starting up task
    {
      // try on any host (again?)
      numt = pvm_spawn(m_ProgramName, m_ExtraArgumentsList, PvmTaskDefault,
         0, 1, &m_Child);
      if (numt != 1) // still a problem
      {
        fprintf(gPVMLogFile, "Objective::Run() pvm_spawn error\n");
        throw(0);
      }
    }

    // set up notification
    /* info = pvm_notify( PvmHostDelete, kMessageHostFailure, 
      1, &m_Child ); */

    // send the extra data first (it might control how the genome is used)
    if (m_ExtraData)
    {
      bufid = pvm_initsend( PvmDataDefault );
      if (bufid < 0)
      {
        fprintf(gPVMLogFile, "Objective::Run() pvm_initsend error\n");
        throw(0);
      }
      info = pvm_pkint( &m_ExtraDataLength, 1, 1 );
      if (info < 0)
      {
        fprintf(gPVMLogFile, "Objective::Run() pvm_pkint error\n");
        throw(0);
      }
      info = pvm_pkstr( m_ExtraData );
      if (info < 0)
      {
        fprintf(gPVMLogFile, "Objective::Run() pvm_pkstr error\n");
        throw(0);
      }
      info = pvm_send( m_Child, kMessageString );
      if (info < 0)
      {
        fprintf(gPVMLogFile, "Objective::Run() pvm_send error\n");
        throw(0);
      }
    }

    // send the genome
    bufid = pvm_initsend( PvmDataDefault );
    if (bufid < 0)
    {
      fprintf(gPVMLogFile, "Objective::Run() pvm_initsend error\n");
      throw(0);
    }
    info = pvm_pkint( &m_Genome->mGenomeLength, 1, 1 );
    if (info < 0)
    {
      fprintf(gPVMLogFile, "Objective::Run() pvm_pkint error\n");
      throw(0);
    }
    info = pvm_pkdouble( m_Genome->mGenes, m_Genome->mGenomeLength, 1 );
    if (info < 0)
    {
      fprintf(gPVMLogFile, "Objective::Run() pvm_pkdouble error\n");
      throw(0);
    }
    info = pvm_send( m_Child, kMessageDoubleArray );
    if (info < 0)
    {
      fprintf(gPVMLogFile, "Objective::Run() pvm_send error\n");
      throw(0);
    }

    m_StartTime = time(0);
    m_RunningFlag = true;
  }
  
  catch (...)
  {
      m_RunningFlag = false;
      if (numt == 1) info = pvm_kill( m_Child );
      fprintf(gPVMLogFile, "Objective::Run() error detected\n");
      fflush(gPVMLogFile);
  }    
}

// check to see whether the command has finished
// and read in the score (we do it here because it makes a bit more sense)

bool Objective::CheckRunning()
{
  int info;
  int bufid;
   
  if (m_RunningFlag == false) return false; // nothing to check
  
  try
  {
    // first off check to see if we are actually running
    if (pvm_pstat( m_Child ) != PvmOk)
    {
      fprintf(gPVMLogFile, "Objective::CheckFinished() Process failure error\n");
      throw(0);
    }
    
    if (m_TimeLimit) // check to see whether we are over time
    {
       if (time(0) - m_StartTime > m_TimeLimit)
       {
          info = pvm_kill( m_Child );
          fprintf(gPVMLogFile, "Objective::CheckFinished() Process timeout error\n");
          throw(0);
       }
    }

    // check for host failure
    /*bufid = pvm_probe( m_Child, kMessageHostFailure);
    if (bufid > 0)
    {
      fprintf(gPVMLogFile, "Objective::CheckFinished() host failure\n");
      throw(0);
    }*/

    // check to see if message available
    bufid = pvm_probe( m_Child, kMessageDouble );
    if (bufid == 0) return true;
    if (bufid < 0) // error condition
    {
       fprintf(gPVMLogFile, "Objective::CheckFinished() pvm_probe error\n");
       throw(0);
    }
    
    // get fitness from child
    bufid = pvm_recv( m_Child, kMessageDouble );
    if (bufid < 0) // error condition
    {
       fprintf(gPVMLogFile, "Objective::CheckFinished() pvm_recv error\n");
       throw(0);
    }
        
    info = pvm_upkdouble( &m_Fitness, 1, 1 );
    if (info < 0) // error condition
    {
       fprintf(gPVMLogFile, "Objective::CheckFinished() pvm_upkdouble error\n");
       throw(0);
    }
    
    // tell the child that it can quit
    bufid = pvm_initsend( PvmDataDefault );
    if (bufid < 0)
    {
      fprintf(gPVMLogFile, "Objective::CheckFinished() pvm_initsend error\n");
      throw(0);
    }
    
    info = pvm_send( m_Child, kMessageAcknowledge );
    if (info < 0)
    {
      fprintf(gPVMLogFile, "Objective::CheckFinished() pvm_send error\n");
      throw(0);
    }

    m_RunningFlag = false;
    return false;
  }
  
  catch (...)
  {
    if (m_RestartOnError > 0) // restart?
    {
       fprintf(gPVMLogFile, "Objective::CheckFinished() error detected: Restarting\n");
       fflush(gPVMLogFile);
       m_RunningFlag = false;
       m_RestartOnError--; // reduce the error counter
       Run(0); // don't care which processor it runs on
       return m_RunningFlag;
    }
    else
    {
       fprintf(gPVMLogFile, "Objective::CheckFinished() error detected: Aborting\n");
       fflush(gPVMLogFile);
       m_RunningFlag = false;
       return false;
    }
  }
}

  

