// TestObjective.cc - routine to calculate the fitness

// this routine give a score of 100 if all values in the genome are 0.5
// if they aren't then the value is lower...

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <pinet.h>

#include "../DataFile/DataFile.h"

int main(int argc, char *argv[])
{
  int i;
  double score = 0;
  double difference;
  int genomeLength;
  double genomeData[256];
  
  int testport = 8085;
  char *hostname = "192.168.0.1";
  char buffer[64];
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
      strcpy(buffer, "ReadyToGo");
      host.write(buffer, 16);
      host.flush();
      
      // read the genome
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
      
      sprintf(buffer, "%.17f", score);
      host.write(buffer, 64);
      
      host.close();
      
    }
    
    catch (...)
    {
      if (buf) delete [] buf;
      host.close();
    }
  }
}

