// DebugParsing.cc

// this file is used to extract data from debug files
// produced by the various bipedal simulation programs

#include <stdio.h>
#include <iostream.h>
#include <fstream.h>

#include "ParameterFile.h"

int main(int argc, char ** argv)
{
  char *inputFileName;
  char *outputFileName;
  ParameterFile theFile;
  double t;
  double p;
  double e;
  int count;
  
  if (argc != 3)
  {
    cerr << "Usage DebugParsing infile outfile\n";
    return 1;
  }
  
  inputFileName = argv[1];
  outputFileName = argv[2];
  ofstream outFile(outputFileName);
  
  theFile.ReadFile(inputFileName);
  
  count = 0;
  while (theFile.RetrieveParameter("gSimulation->GetTime()", &t, false) == false)
  {
    theFile.RetrieveParameter("theTorsoDmABForKinStruct->p_ICS[0]", &p, false);
    theFile.RetrieveParameter("energy", &e, false);
    if (count % 100 == 0)
    {
      outFile << t << "\t" << p << "\t" << e << "\n";
    }
    count++;
  }
}

    
