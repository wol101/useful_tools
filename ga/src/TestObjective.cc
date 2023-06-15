#if defined (USE_FS)

// TestObjective.cc - routine to calculate the fitness

// this routine give a score of 100 if all values in the genome are 0.5
// if they aren't then the value is lower...


#include <stdio.h>
#include <iostream>
#include <fstream>
#include <string.h>

int main(int argc, char *argv[])
{
  int i;
  double score = 0;
  double difference;
  int genomeLength;
  std::ifstream in;
  FILE *out;
  int outputIndex;
  double value;

  for (i = 1; i < argc; i++)
  {
    if (strcmp(argv[i], "--genome") == 0)
    {
      i++;
      if (i > argc) return 1;
      in.open(argv[i]);
    }
  
    if (strcmp(argv[i], "--score") == 0)
    {
      i++;
      if (i > argc) return 1;
      outputIndex = i;
    }
  }
  
  in >> genomeLength;
  for (i = 0; i < genomeLength; i++)
  {
    in >> value;
    difference = value - 0.5;
    score -= (difference * difference);
  }

  // make score positive by adding 100
  score += 100;
  out = fopen(argv[outputIndex], "wb");
  fwrite(&score, sizeof(double), 1, out);
  fclose(out);
}

#elif defined (USE_PVM)

// TestObjective.cc - routine to calculate the fitness

// this routine give a score of 100 if all values in the genome are 0.5
// if they aren't then the value is lower...

#include <stdio.h>
#include <iostream>
#include <fstream>
#include <string.h>

#include <pvm3.h>

#include "PVMMessages.h"

int main(int argc, char *argv[])
{
  int i;
  double score = 0;
  double difference;
  int genomeLength;
  double genomeData[256];

   // check PVM
  int mytid = pvm_mytid();
  if (mytid < 0)
  {
     std::cerr << "PVM error: pvm_mytid()\n";
     return -1;
  }
  
  // get parent ID
  int myparent = pvm_parent();
  if (myparent < 0)
  {
     std::cerr << "PVM error: pvm_parent()\n";
     pvm_exit();
     return -1;
  }
  
  // get genome from parent
  int  bufid = pvm_recv( myparent, kMessageDoubleArray );
  int info = pvm_upkint( &genomeLength, 1, 1 );
  info = pvm_upkdouble( genomeData, genomeLength, 1 );
  
   // very simple fitness function (make value close to 0.5)  
  for (i = 0; i < genomeLength; i++)
  {
    difference = genomeData[i] - 0.5;
    score -= (difference * difference);
  }

  // make score positive by adding 100
  score += 100;
  
  // send fitness back to parent
  bufid = pvm_initsend( PvmDataDefault );
  info = pvm_pkdouble( &score, 1, 1 );
  info = pvm_send( myparent, kMessageDouble );
  
  // wait for acknowledge before quitting
  bufid = pvm_recv( myparent, kMessageAcknowledge );
  
  info = pvm_exit();
}


#elif defined (USE_SOCKETS)

// TestObjective.cc - routine to calculate the fitness

// this routine give a score of 100 if all values in the genome are 0.5
// if they aren't then the value is lower...

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <pinet.h>

#include "DataFile.h"

#include "SocketMessages.h"

int main(int argc, char *argv[])
{
  int i;
  double score = 0;
  double difference;
  int genomeLength;
  double genomeData[256];
  
  int testport = 8085;
  char *hostname = "localhost";
  char buffer[kSocketMaxMessageLength];
  char *buf = 0;
  int l;
  char *tokens[256];
  int nTokens;

  pt::ipstream host(hostname, testport);
      
  // endless loop
  while (true)
  {
    try
    {
      host.open();
      strcpy(buffer, kSocketRequestTaskMessage);
      host.write(buffer, kSocketMaxMessageLength);
      host.flush();
      
      // read the genome
      host.read(buffer, kSocketMaxMessageLength);
      if (strcmp(buffer, kSocketSendingGenome) != 0) throw (0);
      host.read(buffer, 16);
      l = (int)strtol(buffer, 0, 10);
      buf = new char[l];
      host.read(buf, l);
      
      nTokens = DataFile::ReturnTokens(buf, tokens, 256);
      if (nTokens < 1) throw (0);
      
      genomeLength = (int)strtol(tokens[0], 0, 10);
      if (nTokens < genomeLength + 1) throw (0);
      if (genomeLength > 256) throw (0);
      
      for (i = 0; i < genomeLength; i++)
      {
        genomeData[i] = strtod(tokens[i + 1], 0);
      }
      
      delete [] buf;
      buf = 0;
      
       // very simple fitness function (make value close to 0.5)  
      score = 0;
      for (i = 0; i < genomeLength; i++)
      {
        difference = genomeData[i] - 0.5;
        score -= (difference * difference);
      }

      // make score positive by adding 100
      score += 100;
      
      strcpy(buffer, kSocketSendingScore);
      host.write(buffer, kSocketMaxMessageLength);
      sprintf(buffer, "%.17f", score);
      host.write(buffer, 64);
      host.flush();
      
      host.close();
      
    }
    
    catch (...)
    {
      if (buf) delete [] buf;
      host.close();
    }
  }
}


#endif

