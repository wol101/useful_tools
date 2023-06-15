// Objective.cc - class to hold details of running objectives

#if defined (USE_FS)

#include <stdio.h>
#include <iostream>
#include <fstream>
#include <assert.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/stat.h>

#include "Objective.h"
#include "Genome.h"
#include "Debug.h"


// constructor
Objective::Objective()
{
  m_RunID = 0;
  m_ValidFlag = false;
  m_Fitness = 0;
  m_SynchronousFlag = false;
  m_ReadGenomeFlag = false;
  
  strcpy(m_ProgramName, "objective");
  strcpy(m_ExtraArguments, "");
  strcpy(m_ScoreName, "");
  strcpy(m_GenomeName, "");
}

// destructor
Objective::~Objective()
{
}

void Objective::SetProgramName(const char * name)
{
  strcpy(m_ProgramName, name);
}

void Objective::SetExtraArguments(const char * name)
{
  strcpy(m_ExtraArguments, name);
}

// set an objective run going
// might benefit from more checking

void Objective::Run(int id, Genome *genome)
{
  char theCommand[256];
  
  assert(m_ValidFlag == false);
  m_ValidFlag = true;
  m_RunID = id;
  m_Genome = genome;
  
  // set up the command
  sprintf(m_GenomeName, "genome_%d.tmp", m_RunID);
  sprintf(m_ScoreName, "score_%d.tmp", m_RunID);
  if (m_SynchronousFlag)
    sprintf(theCommand, "./%s --genome %s --score %s %s", 
        m_ProgramName, m_GenomeName, m_ScoreName, m_ExtraArguments);
  else
    sprintf(theCommand, "./%s --genome %s --score %s %s &", 
        m_ProgramName, m_GenomeName, m_ScoreName, m_ExtraArguments);
  
  // write out the genome
  std::ofstream out(m_GenomeName);
  out << *genome;
  out.close();
  
  // run the objective command
  system(theCommand);
  
  // std::cerr << "Running " << m_RunID << "\n";
}

// check to see whether the command has finished
// and read in the score (we do it here because it makes a bit more sense)

bool Objective::CheckFinished()
{
  struct stat buf;
  FILE *in;
  int c;
  
  if (m_SynchronousFlag == false) // need to check whether finished
  {
    if (m_ValidFlag == false) return false;

    if (stat(m_ScoreName, &buf) != 0) return false;

    if (buf.st_size < sizeof(double)) return false;
  }
  
  in = fopen(m_ScoreName, "rb");
  if (in == 0) return false;
  
  c = fread(&m_Fitness, sizeof(double), 1, in);
  fclose(in);
  
  
  if (c != 1) return false;

  if (gDebug == Debug_Objective) 
        std::cerr << "Objective::Checkfinished\tfinished\tm_RunID\t" << m_RunID << 
        "\tm_Fitness\t" << m_Fitness << "\n";
  
  if (m_ReadGenomeFlag)
  {
    std::ifstream inFile(m_GenomeName);
    inFile >> *m_Genome;
    inFile.close();
  }
  
  return true;
}

// clean up the files and reset the valid flag
void Objective::CleanUp()
{
  assert(m_ValidFlag == true);
  
  unlink(m_GenomeName);
  unlink(m_ScoreName);
  m_ValidFlag = false;
}
  
#elif defined (USE_PVM)

// Objective.cc - class to hold details of running objectives

#include <stdio.h>
#include <iostream>
#include <fstream>
#include <assert.h>
#include <unistd.h>
#include <stdlib.h>

#include <pvm3.h>

#include "Objective.h"
#include "Genome.h"
#include "Debug.h"
#include "PVMMessages.h"
#include "Genome.h"

#include "DataFile.h"

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
        // std::cerr << taskp[j].ti_a_out << "\n";
        if (strcmp(command, taskp[j].ti_a_out) == 0) nObj++;
      }
    }
    else
    {
      nObj = ntask;
    }
    // std::cerr << hostp[i].hi_name << " " << nObj << " " 
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

#elif defined (USE_SOCKETS)

// Objective.cc - class to hold details of running objectives

#include <stdio.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <assert.h>
#include <unistd.h>
#include <stdlib.h>

#include <pinet.h>

#include "Objective.h"
#include "Genome.h"
#include "Debug.h"
#include "Genome.h"

#include "DataFile.h"

#include "SocketMessages.h"

// the server
extern pt::ipstmserver g_svr;

// log file
extern std::ofstream gSocketLogFile;

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
  char buffer[kSocketMaxMessageLength];
  
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
  
  time_t theTime = time(0);
  struct tm *theLocalTime = localtime(&theTime);
  char theTimeString[64];
  sprintf(theTimeString, "%04d-%02d-%02d_%02d.%02d.%02d", 
      theLocalTime->tm_year + 1900, theLocalTime->tm_mon + 1, 
      theLocalTime->tm_mday,
      theLocalTime->tm_hour, theLocalTime->tm_min,
      theLocalTime->tm_sec);
  pt::string ip = pt::iptostring(m_Client->get_ip());
  gSocketLogFile << ip << "\tstart\t" << m_RunID << "\t" << theTimeString << "\n";
  gSocketLogFile.flush();
  
  try
  {
    // read the control word
    if (m_Client->waitfor(100) == false) throw(0); // nothing to read
    m_Client->read(buffer, kSocketMaxMessageLength);
    if (strcmp(buffer, kSocketRequestTaskMessage) != 0) throw (0);
    
    // data passed as a 16 byte string containing the integer length
    // followed by the actual string (length bytes long)
    
    if (m_ExtraArguments)
    {
      strcpy(buffer, kSocketSendingCommandLine);
      m_Client->write(buffer, kSocketMaxMessageLength);
      len = strlen(m_ExtraArguments) + 1;
      sprintf(buffer, "%d", len);
      m_Client->write(buffer, 16);
      m_Client->write(m_ExtraArguments, len);
      m_Client->flush();
    }
    
    if (m_ExtraData)
    {
      strcpy(buffer, kSocketSendingExtraData);
      m_Client->write(buffer, kSocketMaxMessageLength);
      len = strlen(m_ExtraData) + 1;
      sprintf(buffer, "%d", len);
      m_Client->write(buffer, 16);
      m_Client->write(m_ExtraData, len);
      m_Client->flush();
    }
    
    std::ostringstream data;
    data << *m_Genome << "\0";
    
    strcpy(buffer, kSocketSendingGenome);
    m_Client->write(buffer, kSocketMaxMessageLength);
    len = data.str().size();
    sprintf(buffer, "%d", len);
    m_Client->write(buffer, 16);
    m_Client->write(data.str().data(), len);
    m_Client->flush();
  }
  
  catch(...)
  {
    delete m_Client;
    m_Client = 0;
    return false;
  }
  
  m_RunningFlag = true;
  m_StartTime = theTime;
  return true;   
}

// check to see whether the command has finished
// and read in the score (we do it here because it makes a bit more sense)

bool Objective::CheckRunning()
{
  char buffer[kSocketMaxMessageLength];
  time_t theTime;
  time_t runTime;
  char theTimeString[64];
  struct tm *theLocalTime;
  pt::string ip;
    
  if (m_RunningFlag == false) return false; // nothing to check
  
  assert(m_Client); // must exist
  
  theTime = time(0);
  runTime = theTime - m_StartTime;
  
  try
  {
    if (m_TimeLimit) // check to see whether we are over time
    {
       if (runTime > m_TimeLimit)
       {
          throw(0);
       }
    }
    
    // is there a result for us?
    if (m_Client->waitfor(1) == false) return true; // still running
    m_Client->read(buffer, kSocketMaxMessageLength);
    if (strcmp(buffer, kSocketSendingScore) != 0) throw (0);
    
    m_Client->read(buffer, 64); // result is a 64 byte string
    m_Fitness = strtod(buffer, 0);
    
    theLocalTime = localtime(&theTime);
    sprintf(theTimeString, "%04d-%02d-%02d_%02d.%02d.%02d", 
        theLocalTime->tm_year + 1900, theLocalTime->tm_mon + 1, 
        theLocalTime->tm_mday,
        theLocalTime->tm_hour, theLocalTime->tm_min,
        theLocalTime->tm_sec);
    ip = pt::iptostring(m_Client->get_ip());
    gSocketLogFile << ip << "\tfinish\t" << m_RunID << "\t" << theTimeString << 
        "\t" << buffer << "\t" << runTime << "\n";
    gSocketLogFile.flush();
    
    m_RunningFlag = false;
    return false;
  }
  
  catch (...)
  {
    theLocalTime = localtime(&theTime);
    sprintf(theTimeString, "%04d-%02d-%02d_%02d.%02d.%02d", 
        theLocalTime->tm_year + 1900, theLocalTime->tm_mon + 1, 
        theLocalTime->tm_mday,
        theLocalTime->tm_hour, theLocalTime->tm_min,
        theLocalTime->tm_sec);
    ip = pt::iptostring(m_Client->get_ip());
    gSocketLogFile << ip << "\tabort\t" << m_RunID << "\t" << theTimeString << 
        "\t" << runTime << "\n";
        
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



#endif


