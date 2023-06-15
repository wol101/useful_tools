#include <stdio.h>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <assert.h>
#include <unistd.h>
#include <float.h>

#ifdef USE_OPENGL
#include <glut.h>
#include "GLUIRoutines.h"
#endif

#define DEFINING_DM_GLOBALS
#include <dm.h>
#include <dmArticulation.hpp>

#ifdef USE_PVM
#include <pvm3.h>
#include "../../../ga/src/PVMMessages.h"
#endif

#ifdef USE_SOCKETS
#include <pinet.h>
#include "../../../ga/src/SocketMessages.h"
#endif

#include "Simulation.h"
#include "ForceList.h"
#include "Segments.h"
#include "SegmentParameters.h"
#include "ModifiedContactModel.h"
#include "MAMuscle.h"
#include "Util.h"

#include "DataFile.h"

#define DEBUG_MAIN
#include "DebugControl.h"

enum FinishReason
{
  NoReason,
  TimeLimit,
  EnergyLimit,
  PositionSanity,
  VelocitySanity,
  DynamechsError
};

// Simulation global
Simulation *gSimulation;

// simulation parameters
// these are floats because of a GLUI interface limitation
float gTimeLimit = 50; 
float gEnergyLimit = 0; // set to <= 0 to ignore

// window size
int gWindowWidth = 640;
int gWindowHeight = 480;

// control window separate?
bool gUseSeparateWindow = true;
  
// kinetics file
char * gKineticsFilenamePtr = 0;
    
// energy calculations
bool gMechanicalEnergyFlag = false; // metabolic if false

// root directory for bones graphics
char *gGraphicsRoot = 0;

// run control
bool gFinishedFlag = true;

// output model control
double gOutputStateAt = -1.0;

// runtime limit
long gRunTimeLimit = 0;

#if defined(USE_PVM)
static int gMyParent = -1;
#elif defined(USE_SOCKETS)
// the server
static pt::ipstream *gHost;
#endif

// command line arguments
static int testport = 8085;
static char hostname[256] = "localhost";
static char *genomeFilenamePtr = "genome.tmp";
static char *configFilenamePtr = "ModelData.txt";
static char *scoreFilenamePtr = "score.tmp";
static int numStartupPhases = 2;
static int numCyclicPhases = 6;
static int numAllelesPerPhase = 7;
static int outputStateAtPhase = -1;
static bool asciiGenomeFlag = true;
static bool summaryFlag = false;


int ReadModel();
void WriteModel();
void OutputProgramState(ostream &out);
void OutputKinetics();

static void ParseArguments(int argc, char ** argv);

int main(int argc, char ** argv)
{
#if defined(USE_OPENGL)
  // read any arguments relevent to OpenGL and remove them from the list
  // also intialises the windowing system
  glutInit(&argc, argv);
#endif

  // start by parsing the command line arguments
  ParseArguments(argc - 1, &(argv[1]));

  #if defined(USE_OPENGL)
  // now initialise the OpenGL bits
  StartGlut();

  // and enter the never to be returned loop
  glutMainLoop();
  #else
  // another never returned loop (exits are in WriteModel when required)
  long runTime = 0;
  long startTime = time(0);
  while(gRunTimeLimit == 0 || runTime <= gRunTimeLimit)
  {
    runTime = time(0) - startTime;
  
    if (gFinishedFlag)
    {
      if (ReadModel() == 0)
      {
        gFinishedFlag = false;
      }
    }  
    else
    {
      while (gSimulation->GetTime() < gTimeLimit && gSimulation->GetMetabolicEnergy() 
      < gEnergyLimit)
      {
        gSimulation->UpdateSimulation();
        if (gKineticsFilenamePtr) OutputKinetics();
        
        if (gOutputStateAt >= 0)
        {
          if (gSimulation->GetTime() >= gOutputStateAt)
          {
              ofstream out("ModelState.txt");
              OutputProgramState(out);
              out.close();
              gOutputStateAt = -1.0;
          }
        }
        
        if (gSimulation->TestForCatastrophy()) break;
      }
      
      gFinishedFlag = true;
      WriteModel();
    }
  }
  #endif
}

void ParseArguments(int argc, char ** argv)
{
  int i;
  
  // do some simple stuff with command line arguments
  
  for (i = 0; i < argc; i++)
  {
    if (strcmp(argv[i], "--kinetics") == 0 ||
      strcmp(argv[i], "-k") == 0)
    {
      i++;
      if (i >= argc) 
      {
        cerr << "Error parsing kinetics filename\n";
        exit(1);
      }
      gKineticsFilenamePtr = argv[i];
    }
    else
    if (strcmp(argv[i], "--genome") == 0 ||
      strcmp(argv[i], "-g") == 0)
    {
      i++;
      if (i >= argc) 
      {
        cerr << "Error parsing genome filename\n";
        exit(1);
      }
      genomeFilenamePtr = argv[i];
    }
    else
    if (strcmp(argv[i], "--score") == 0 ||
      strcmp(argv[i], "-s") == 0)
    {
      i++;
      if (i >= argc) 
      {
        cerr << "Error parsing score filename\n";
        exit(1);
      }
      scoreFilenamePtr = argv[i];
    }
    else
    if (strcmp(argv[i], "--config") == 0 ||
      strcmp(argv[i], "-c") == 0)
    {
      i++;
      if (i >= argc) 
      {
        cerr << "Error parsing config filename\n";
        exit(1);
      }
      configFilenamePtr = argv[i];
    }
    else
    if (strcmp(argv[i], "--graphicsRoot") == 0)
    {
      i++;
      if (i >= argc) 
      {
        cerr << "Error parsing graphics root directory\n";
        exit(1);
      }
      gGraphicsRoot = argv[i];
    }
    else
    if (strcmp(argv[i], "--debug") == 0 ||
      strcmp(argv[i], "-e") == 0)
    {
      i++;
      if (i >= argc) 
      {
        cerr << "Error parsing debug level\n";
        exit(1);
      }
      if (sscanf(argv[i], "%d", &gDebug) == 0)
      {
        cerr << "Error parsing debug level\n";
        exit(1);
      }
    }
    else
    if (strcmp(argv[i], "--summary") == 0 ||
      strcmp(argv[i], "-u") == 0)
    {
      summaryFlag = true;
    }
    else
    if (strcmp(argv[i], "--timeLimit") == 0 ||
      strcmp(argv[i], "-t") == 0)
    {
      i++;
      if (i >= argc) 
      {
        cerr << "Error parsing time limit\n";
        exit(1);
      }
      if (sscanf(argv[i], "%f", &gTimeLimit) == 0)
      {
        cerr << "Error parsing time limit\n";
        exit(1);
      }
    }
    else
    if (strcmp(argv[i], "--gOutputStateAt") == 0)
    {
      i++;
      if (i >= argc) 
      {
        cerr << "Error parsing gOutputStateAt\n";
        exit(1);
      }
      if (sscanf(argv[i], "%lf", &gOutputStateAt) == 0)
      {
        cerr << "Error parsing gOutputStateAt\n";
        exit(1);
      }
    }
    else
    if (strcmp(argv[i], "--outputStateAtPhase") == 0)
    {
      i++;
      if (i >= argc) 
      {
        cerr << "Error parsing outputStateAtPhase\n";
        exit(1);
      }
      if (sscanf(argv[i], "%d", &outputStateAtPhase) == 0)
      {
        cerr << "Error parsing outputStateAtPhase\n";
        exit(1);
      }
    }
    else
    if (strcmp(argv[i], "--numStartupPhases") == 0)
    {
      i++;
      if (i >= argc) 
      {
        cerr << "Error parsing numStartupPhases\n";
        exit(1);
      }
      if (sscanf(argv[i], "%d", &numStartupPhases) == 0)
      {
        cerr << "Error parsing numStartupPhases\n";
        exit(1);
      }
    }
    else
    if (strcmp(argv[i], "--numCyclicPhases") == 0)
    {
      i++;
      if (i >= argc) 
      {
        cerr << "Error parsing numCyclicPhases\n";
        exit(1);
      }
      if (sscanf(argv[i], "%d", &numCyclicPhases) == 0)
      {
        cerr << "Error parsing numCyclicPhases\n";
        exit(1);
      }
    }
    else
    if (strcmp(argv[i], "--numAllelesPerPhase") == 0)
    {
      i++;
      if (i >= argc) 
      {
        cerr << "Error parsing numAllelesPerPhase\n";
        exit(1);
      }
      if (sscanf(argv[i], "%d", &numAllelesPerPhase) == 0)
      {
        cerr << "Error parsing numAllelesPerPhase\n";
        exit(1);
      }
    }
    else
    if (strcmp(argv[i], "--energyLimit") == 0 ||
      strcmp(argv[i], "-y") == 0)
    {
      i++;
      if (i >= argc) 
      {
        cerr << "Error parsing energy limit\n";
        exit(1);
      }
      if (sscanf(argv[i], "%f", &gEnergyLimit) == 0)
      {
        cerr << "Error parsing energy limit\n";
        exit(1);
      }
    }
    else
    if (strcmp(argv[i], "--mechanicalEnergy") == 0)
    {
      gMechanicalEnergyFlag = true;
    }
    else
    if (strcmp(argv[i], "--windowSize") == 0)
    {
      i++;
      if (i >= argc) 
      {
        cerr << "Error parsing window size x\n";
        exit(1);
      }
      if (sscanf(argv[i], "%d", &gWindowWidth) == 0)
      {
        cerr << "Error parsing window size x\n";
        exit(1);
      }
      i++;
      if (i >= argc) 
      {
        cerr << "Error parsing window size y\n";
        exit(1);
      }
      if (sscanf(argv[i], "%d", &gWindowHeight) == 0)
      {
        cerr << "Error parsing window size y\n";
        exit(1);
      }
    }
    else
    if (strcmp(argv[i], "--port") == 0)
    {
      i++;
      if (i >= argc)
      {
        cerr << "Error parsing --port\n";
        exit(1);
      }
      testport = strtol(argv[i], 0, 10);
    }
    else 
    if (strcmp(argv[i], "--hostname") == 0) 
    {
      i++;
      if (i >= argc)
      {
        cerr << "Error parsing --hostname\n";
        exit(1);
      }
      strcpy(hostname, argv[i]);
    }
    else
    if (strcmp(argv[i], "--runTimeLimit") == 0)
    {
      i++;
      if (i >= argc)
      {
        cerr << "Error parsing --runTimeLimit\n";
        exit(1);
      }
      gRunTimeLimit = strtol(argv[i], 0, 10);
    }
    else 
    if (strcmp(argv[i], "--help") == 0 ||
      strcmp(argv[i], "-h") == 0 ||
      strcmp(argv[i], "-?") == 0)
    {
      cerr << "\nObjective build " << __DATE__ << " " << __TIME__ << "\n";
      cerr << "\nobjective produces a fitness score in score.tmp depending on the input data in\n";
      cerr << "genome.tmp and the model information in ModelData.txt which generally loads s\n";
      cerr << "number of .xan files).\n\n";
      cerr << "There are a number of command line options available:\n\n";
      cerr << "-k filename, --kinetics filename\n";
      cerr << "Creates a kinetics file containing kinetic data for all the\n";
      cerr << "segments in the model\n\n";
      cerr << "-g filename, --genome filename\n";
      cerr << "Reads filename rather than the default genome.tmp as the genome data\n\n";
      cerr << "-c filename, --config filename\n";
      cerr << "Reads filename rather than the default ModelData.tmp as the config data\n\n";
      cerr << "-s filename, --score filename\n";
      cerr << "Writes filename rather than the default score.tmp as the genome data\n\n";
      cerr << "--graphicsRoot path\n";
      cerr << "Prepends path to the graphics filenames\n\n";
      cerr << "-u, --summary\n";
      cerr << "Writes out summary information at end of fitness run.\n\n";
      cerr << "-e n, --debug n\n";
      cerr << "Prints out a lot of extra debugging information on stderr if n is higher than 0.\n";
      cerr << "Suitable values of n are defined in DebugControl.h\n\n";
      cerr << "--windowSize x y\n";
      cerr << "Sets the initial display window size - requires separate x and y values.\n\n";
      cerr << "-t n, --timeLimit n\n";
      cerr << "Sets a time limit on the simulation. The simulation will pause after this limit\n";
      cerr << "in display mode or stop in fitness mode.\n\n";
      cerr << "--gOutputStateAt n\n";
      cerr << "If set to a value greater than or equal to zero, will output model state\n";
      cerr << "information at that time to ModelState.txt\n\n";
      cerr << "--outputStateAtPhase n\n";
      cerr << "If set to a value greater than or equal to zero, will output model state\n";
      cerr << "information at the numbered phase (overules --gOutputStateAt)\n\n";
      cerr << "--numStartupPhases n\n";
      cerr << "Sets the number of startup phases (default 2)\n\n";
      cerr << "--numCyclicPhases n\n";
      cerr << "Sets the number of cyclic phases (default 6)\n\n";
      cerr << "--numAllelesPerPhase n\n";
      cerr << "Sets the number of alleles per phase (default 7)\n\n";
      cerr << "-y n, --energyLimit n\n";
      cerr << "Sets an energy limit on the simulation. The simulation will stop after this\n";
      cerr << "limit in fitness mode. No effect in display mode. If <= 0 then no limit!\n\n";
      cerr << "--mechanicalEnergy\n";
      cerr << "Energy calculations are mechanical rather than metabolic\n\n";
      cerr << "--hostname ip_address\n";
      cerr << "Set the name of the server for the socket version\n\n";
      cerr << "--port n\n";
      cerr << "Sets the port that the socket version will use for connection\n\n";
      cerr << "--runTimeLimit n\n";
      cerr << "Quits the program (approximately) after it has run n seconds\n\n";
      cerr << "-h, -?, --help\n";
      cerr << "Prints this message!\n\n";

      exit(1);
    }
    else
    {
      cerr << "Unrecognised option. Try 'objective --help' for more info\n";
      exit(1);
    }
  }

}

// this routine attemps to read the model specification and initialise the simulation
// it returns zero on success
int ReadModel()
{
  int i;
  int genomeLength;
  double *genomeData;
#if defined(USE_PVM)   
  int mytid;
  int bufid;
  int info;
  int length;
  char *buffer;
#elif defined(USE_SOCKETS)   
  char *tokens[256];
  int nTokens;
  char *buf = 0;
  int l;
  char buffer[kSocketMaxMessageLength];
#endif    
     
  // initialisation phase
  
#if defined(USE_SOCKETS)
  gHost = new pt::ipstream(hostname, testport);
  
  try
  {
    gHost->open();
    strcpy(buffer, kSocketRequestTaskMessage);
    gHost->write(buffer, kSocketMaxMessageLength);
    gHost->flush();
    if (gDebug == SocketDebug) *gDebugStream <<  "main\tStart String\n" << buffer << "\n";

    // read the command line arguments
    gHost->read(buffer, kSocketMaxMessageLength);
    if (strcmp(buffer, kSocketSendingCommandLine) != 0) throw (0);
    gHost->read(buffer, 16);
    l = (int)strtol(buffer, 0, 10);
    buf = new char[l];
    gHost->read(buf, l);
    
    if (gDebug == SocketDebug) *gDebugStream <<  "ReadModel\tCommandLine\n" << buf << "\n";

    nTokens = DataFile::ReturnTokens(buf, tokens, 256);

    ParseArguments(nTokens, tokens);
    delete [] buf;
    buf = 0;
  }
  
  catch (...)
  {
    if (buf) delete [] buf;
    gHost->close();
    delete gHost;
    return 1;
  }
#elif defined(USE_PVM)
  mytid = pvm_mytid();
  if (mytid < 0)
  {
     cerr << "PVM error: pvm_mytid()\n";
     return 1;
  }

  // get parent ID
  gMyParent = pvm_parent();
  if (gMyParent < 0)
  {
     cerr << "PVM error: pvm_parent()\n";
     pvm_exit();
     return 1;
  }
#endif
           
  // create the simulation object
  gSimulation = new Simulation();
#ifdef USE_OPENGL    
  gSimulation->SetGraphics(true);
#endif

  DataFile myFile;
  myFile.SetExitOnError(true);

  // load the config file
#if defined(USE_PVM)   
  // get model config file from parent
  bufid = pvm_recv( gMyParent, kMessageString );
  info = pvm_upkint( &length, 1, 1 );
  buffer = new char[length + 1];
  info = pvm_upkstr( buffer );
  myFile.SetRawData(buffer);
  delete [] buffer;
#elif defined(USE_SOCKETS)
  // get model config file from server
  try
  {
    gHost->read(buffer, kSocketMaxMessageLength);
    if (strcmp(buffer, kSocketSendingExtraData) != 0) throw (0);
    gHost->read(buffer, 16);
    l = (int)strtol(buffer, 0, 10);
    buf = new char[l];
    gHost->read(buf, l);
    
    if (gDebug == SocketDebug) *gDebugStream << "ReadModel\tConfig File\n" <<  buf << "\n";

    myFile.SetRawData(buf);
    delete [] buf;
    buf = 0;
  }
  catch (...)
  {
    if (buf) delete [] buf;
    delete gSimulation;
    delete gHost;
    return 1;
  }
#else  
  myFile.ReadFile(configFilenamePtr);
#endif  
  
  gSimulation->LoadModel(myFile);
  
  // load the genome
  gSimulation->GetCPG()->Initialise(numStartupPhases, numCyclicPhases, 
        numAllelesPerPhase);
  
#if defined(USE_PVM)    
  // get genome from parent
  bufid = pvm_recv( gMyParent, kMessageDoubleArray );
  info = pvm_upkint( &genomeLength, 1, 1 );
  genomeData = new double[genomeLength];
  info = pvm_upkdouble( genomeData, genomeLength, 1 );
  gSimulation->GetCPG()->SetGenome(genomeLength, genomeData);
  delete [] genomeData;
#elif defined(USE_SOCKETS)
  // get genome from server
  try
  {
    // read the genome
    gHost->read(buffer, kSocketMaxMessageLength);
    if (strcmp(buffer, kSocketSendingGenome) != 0) throw (0);
    gHost->read(buffer, 16);
    l = (int)strtol(buffer, 0, 10);
    buf = new char[l];
    gHost->read(buf, l);
    
    if (gDebug == SocketDebug) *gDebugStream << "ObjectiveMain\tGenome\n" <<  buf << "\n";

    nTokens = DataFile::ReturnTokens(buf, tokens, 256);
    if (nTokens < 1) throw (0);

    genomeLength = (int)strtol(tokens[0], 0, 10);
    if (nTokens < genomeLength + 1) throw (0);

    genomeData = new double[genomeLength];
    for (i = 0; i < genomeLength; i++) genomeData[i] = strtod(tokens[i + 1], 0);
    gSimulation->GetCPG()->SetGenome(genomeLength, genomeData);

    delete [] genomeData;
    delete [] buf;
    buf = 0;
  }
  catch (...)
  {
    if (buf) delete [] buf;
    delete gSimulation;
    delete gHost;
    return 1;
  }
  
#else
  if (asciiGenomeFlag)
  {
    ifstream inFile(genomeFilenamePtr);
    assert(inFile.good());
    inFile >> genomeLength;
    genomeData = new double[genomeLength];
    for (i = 0; i < genomeLength; i++) inFile >> genomeData[i];
    inFile.close();
    gSimulation->GetCPG()->SetGenome(genomeLength, genomeData);
    delete [] genomeData;
  }
  else
  {
    DataFile genomeFile;
    genomeFile.ReadFile(genomeFilenamePtr);
    genomeFile.ReadNextBinary(&genomeLength);
    genomeData = new double[genomeLength];
    genomeFile.ReadNextBinary(genomeData, genomeLength);
    gSimulation->GetCPG()->SetGenome(genomeLength, genomeData);
    delete [] genomeData;

  }    
#endif
      
  gSimulation->GetCPG()->ParseGenome();
  
  if (outputStateAtPhase >= 0) 
  {
     gOutputStateAt = 0;
     for (i = 0; i < outputStateAtPhase; i++)
     {
        if (i < numStartupPhases)
           gOutputStateAt += gSimulation->GetCPG()->GetDuration(i);
        else
        {
           gOutputStateAt += 
                 gSimulation->GetCPG()->GetDuration(
                 numStartupPhases + ((i - numStartupPhases) % numCyclicPhases));
        }
     }
  }
  
  return 0;
}

void WriteModel()
{
  double score;
#if defined(USE_PVM)   
  int bufid;
  int info;
#elif defined(USE_SOCKETS)   
  char buffer[kSocketMaxMessageLength];
#endif
  
  score = gSimulation->CalculateInstantaneousFitness();
  
  if (summaryFlag) 
    cerr << "Simulation Time: " << gSimulation->GetTime() << 
      " Score: " << score << 
      " Mechanical Energy: " << gSimulation->GetMechanicalEnergy() << 
      " Metabolic Energy: " << gSimulation->GetMetabolicEnergy() << "\n";
  
#if defined(USE_PVM)  
  // send fitness back to parent
  bufid = pvm_initsend( PvmDataDefault );
  info = pvm_pkdouble( &score, 1, 1 );
  info = pvm_send( gMyParent, kMessageDouble );
  // wait for acknowledge before quitting
  bufid = pvm_recv( gMyParent, kMessageAcknowledge );
  info = pvm_exit();
#elif defined(USE_SOCKETS)
  try
  {
    strcpy(buffer, kSocketSendingScore);
    gHost->write(buffer, kSocketMaxMessageLength);
    sprintf(buffer, "%.17f", score);
    gHost->write(buffer, 64);
    gHost->flush();
    if (gDebug == SocketDebug) *gDebugStream << "ObjectiveMain\tScore\n" <<  buffer << "\n";
  }
  catch (...)
  {
  }
  delete gHost;
#else
  FILE *out;                
  out = fopen(scoreFilenamePtr, "wb");
  fwrite(&score, sizeof(double), 1, out);
  fclose(out);
#endif
    
  if (gDebug == MemoryDebug) 
    *gDebugStream << "main\tAbout to delete gSimulation\n";
  delete gSimulation;  

#if ! defined(USE_SOCKETS) && ! defined (USE_OPENGL)
  cerr << "exiting\n";
  exit(0);
#endif  
}

// function to produce a file of link kinetics

void 
OutputKinetics()
{
  if (gKineticsFilenamePtr == 0) return;
  
  unsigned int myNumLinks = gSimulation->GetRobot()->getNumLinks();
  const dmABForKinStruct *myDmABForKinStruct;
  unsigned int i;
  int j, k;
  static bool firstTimeFlag = true;
  static ofstream outputKineticsFile;
  
  // first time through output the column headings
  if (firstTimeFlag)
  {
    outputKineticsFile.open(gKineticsFilenamePtr);
    
    outputKineticsFile << "time\t";
    for (i = 0; i < myNumLinks; i++)
    {
      outputKineticsFile << 
        gSimulation->GetRobot()->getLink(i)->getName() 
        << "_X\t";
      outputKineticsFile << 
        gSimulation->GetRobot()->getLink(i)->getName() 
        << "_Y\t";
      outputKineticsFile << 
        gSimulation->GetRobot()->getLink(i)->getName() 
        << "_Z\t";
      outputKineticsFile << 
        gSimulation->GetRobot()->getLink(i)->getName() 
        << "_R00\t";
      outputKineticsFile << 
        gSimulation->GetRobot()->getLink(i)->getName() 
        << "_R01\t";
      outputKineticsFile << 
        gSimulation->GetRobot()->getLink(i)->getName() 
        << "_R02\t";
      outputKineticsFile << 
        gSimulation->GetRobot()->getLink(i)->getName() 
        << "_R10\t";
      outputKineticsFile << 
        gSimulation->GetRobot()->getLink(i)->getName() 
        << "_R11\t";
      outputKineticsFile << 
        gSimulation->GetRobot()->getLink(i)->getName() 
        << "_R12\t";
      outputKineticsFile << 
        gSimulation->GetRobot()->getLink(i)->getName() 
        << "_r20\t";
      outputKineticsFile << 
        gSimulation->GetRobot()->getLink(i)->getName() 
        << "_R21\t";
      outputKineticsFile << 
        gSimulation->GetRobot()->getLink(i)->getName() 
        << "_R22";
      if (i != myNumLinks - 1) outputKineticsFile << "\t";
      else outputKineticsFile << "\n";    
    }
    firstTimeFlag = false;
  }     
  
  // start by outputting the simulation time
  outputKineticsFile << gSimulation->GetTime() << "\t";
  
  for (i = 0; i < myNumLinks; i++)
  {
    // output the position
    myDmABForKinStruct = gSimulation->GetRobot()->getForKinStruct(i);
    outputKineticsFile << myDmABForKinStruct->p_ICS[0] << "\t"
      << myDmABForKinStruct->p_ICS[1] << "\t"
      << myDmABForKinStruct->p_ICS[2] << "\t";
    // then the orientation
    for (j = 0; j < 3; j++)
    {
      for (k = 0; k < 3; k++)
      {
        outputKineticsFile << myDmABForKinStruct->R_ICS[j][k];
        if (k != 2) outputKineticsFile << "\t";
      }
      if (j != 2) outputKineticsFile << "\t";
    }
    if (i != myNumLinks - 1) outputKineticsFile << "\t";
    else outputKineticsFile << "\n";    
  }
}

  
// output the simulation state in a format that can be re-read

void
OutputProgramState(ostream &out)
{
   int i;
   double q[7];
   double qd[7];
   
   // set precision (could also use manipulator)
   out.precision(DBL_DIG);

   out << "Time " << gSimulation->GetTime() << "\n\n";
   
   Torso *torso = gSimulation->GetTorso();
   torso->getState(q, qd);
   
   out << "kTorsoPosition";
   for (i = 0; i < 7; i++) out << " " << q[i];
   out << "\n";
   out << "kTorsoVelocity";
   for (i = 0; i < 6; i++) out << " " << qd[i];
   out << "\n\n";
   
   LeftThigh *theLeftThigh = gSimulation->GetLeftThigh();
   theLeftThigh->getState(q, qd);
   out << "kLeftThighMDHTheta " << q[0] << "\n";
   out << "kLeftThighInitialJointVelocity " << qd[0] << "\n\n";
   
   RightThigh *theRightThigh = gSimulation->GetRightThigh();
   theRightThigh->getState(q, qd);
   out << "kRightThighMDHTheta " << q[0] << "\n";
   out << "kRightThighInitialJointVelocity " << qd[0] << "\n\n";
   
   LeftLeg *theLeftLeg = gSimulation->GetLeftLeg();
   theLeftLeg->getState(q, qd);
   out << "kLeftLegMDHTheta " << q[0] << "\n";
   out << "kLeftLegInitialJointVelocity " << qd[0] << "\n\n";
   
   RightLeg *theRightLeg = gSimulation->GetRightLeg();
   theRightLeg->getState(q, qd);
   out << "kRightLegMDHTheta " << q[0] << "\n";
   out << "kRightLegInitialJointVelocity " << qd[0] << "\n\n";
   
   LeftFoot *theLeftFoot = gSimulation->GetLeftFoot();
   theLeftFoot->getState(q, qd);
   out << "kLeftFootMDHTheta " << q[0] << "\n";
   out << "kLeftFootInitialJointVelocity " << qd[0] << "\n\n";
   
   RightFoot *theRightFoot = gSimulation->GetRightFoot();
   theRightFoot->getState(q, qd);
   out << "kRightFootMDHTheta " << q[0] << "\n";
   out << "kRightFootInitialJointVelocity " << qd[0] << "\n\n";
   
}
