#include <stdio.h>
#include <iostream.h>
#include <iomanip.h>
#include <fstream.h>
#include <assert.h>
#include <unistd.h>
#include <float.h>

#include <glut.h>

#include <dm.h>
#include <dmu.h>
#include <dmArticulation.hpp>

#include "Simulation.h"
#include "ForceList.h"
#include "Segments.h"
#include "SegmentParameters.h"
#include "ExtendedForce.h"
#include "ModifiedContactModel.h"
#include "MAMuscle.h"
#include "GLUIRoutines.h"

#define DEBUG_EXTERN
#include "DebugControl.h"

// Simulation global
Simulation *gSimulation;

float gTimeLimit = 50; // needs to be a float for GLUI
double gEnergyLimit = 0; // set to <= 0 to ignore

// window size
int gWindowWidth = 640;
int gWindowHeight = 480;
  
// kinetics file
char * gKineticsFilenamePtr = 0;

// summary flag
bool gSummaryFlag = false;
    
// energy calculations
bool gMechanicalEnergyFlag = false; // metabolic if false

// output state
double gOutputStateAt = -1.0;


static double CalculateFitness();
static void OutputKinetics();
static void ConvertLocalToWorldP(const dmABForKinStruct *m, 
   const CartesianVector local, CartesianVector world);
static void ConvertLocalToWorldV(const dmABForKinStruct *m, 
   const SpatialVector local, SpatialVector world);
static double CalculateEnergyCost(MAMuscle *strapForce);
static void OutputProgramState(ostream &out);

int main(int argc, char ** argv)
{
  char *genomeFilenamePtr = "genome.tmp";
  char *configFilenamePtr = "ModelData.txt";
  char *scoreFilenamePtr = "score.tmp";
  bool displayFlag = false;
  double score;
  int i, j;
  double hipExtensorLengthFeedbackP2 = 100; // dummy large value
  int numStartupPhases = 2;
  int numCyclicPhases = 6;
  int numAllelesPerPhase = 7;
  int outputStateAtPhase = -1;
      
  // debugging control - set to zero for no debugging output
  gDebug = NoDebug;

  // do some simple stuff with command line arguments
  
  for (i = 1; i < argc; i++)
  {
    if (strcmp(argv[i], "--kinetics") == 0 ||
      strcmp(argv[i], "-k") == 0)
    {
      i++;
      if (i >= argc) 
      {
        cerr << "Error parsing kinetics filename\n";
        return 1;
      }
      gKineticsFilenamePtr = argv[i];
    }
    else
    if (strcmp(argv[i], "--display") == 0 ||
      strcmp(argv[i], "-d") == 0)
      displayFlag = true;
    else
    if (strcmp(argv[i], "--genome") == 0 ||
      strcmp(argv[i], "-g") == 0)
    {
      i++;
      if (i >= argc) 
      {
        cerr << "Error parsing genome filename\n";
        return 1;
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
        return 1;
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
        return 1;
      }
      configFilenamePtr = argv[i];
    }
    else
    if (strcmp(argv[i], "--debug") == 0 ||
      strcmp(argv[i], "-e") == 0)
    {
      i++;
      if (i >= argc) 
      {
        cerr << "Error parsing debug level\n";
        return 1;
      }
      if (sscanf(argv[i], "%d", &j) == 0)
      {
        cerr << "Error parsing debug level\n";
        return 1;
      }
      gDebug = (DebugControl)j;
    }
    else
    if (strcmp(argv[i], "--summary") == 0 ||
      strcmp(argv[i], "-u") == 0)
    {
      gSummaryFlag = true;
    }
    else
    if (strcmp(argv[i], "--timeLimit") == 0 ||
      strcmp(argv[i], "-t") == 0)
    {
      i++;
      if (i >= argc) 
      {
        cerr << "Error parsing time limit\n";
        return 1;
      }
      if (sscanf(argv[i], "%f", &gTimeLimit) == 0)
      {
        cerr << "Error parsing time limit\n";
        return 1;
      }
    }
    else
    if (strcmp(argv[i], "--outputStateAt") == 0)
    {
      i++;
      if (i >= argc) 
      {
        cerr << "Error parsing outputStateAt\n";
        return 1;
      }
      if (sscanf(argv[i], "%lf", &gOutputStateAt) == 0)
      {
        cerr << "Error parsing outputStateAt\n";
        return 1;
      }
    }
    else
    if (strcmp(argv[i], "--outputStateAtPhase") == 0)
    {
      i++;
      if (i >= argc) 
      {
        cerr << "Error parsing outputStateAtPhase\n";
        return 1;
      }
      if (sscanf(argv[i], "%d", &outputStateAtPhase) == 0)
      {
        cerr << "Error parsing outputStateAtPhase\n";
        return 1;
      }
    }
    else
    if (strcmp(argv[i], "--numStartupPhases") == 0)
    {
      i++;
      if (i >= argc) 
      {
        cerr << "Error parsing numStartupPhases\n";
        return 1;
      }
      if (sscanf(argv[i], "%d", &numStartupPhases) == 0)
      {
        cerr << "Error parsing numStartupPhases\n";
        return 1;
      }
    }
    else
    if (strcmp(argv[i], "--numCyclicPhases") == 0)
    {
      i++;
      if (i >= argc) 
      {
        cerr << "Error parsing numCyclicPhases\n";
        return 1;
      }
      if (sscanf(argv[i], "%d", &numCyclicPhases) == 0)
      {
        cerr << "Error parsing numCyclicPhases\n";
        return 1;
      }
    }
    else
    if (strcmp(argv[i], "--numAllelesPerPhase") == 0)
    {
      i++;
      if (i >= argc) 
      {
        cerr << "Error parsing numAllelesPerPhase\n";
        return 1;
      }
      if (sscanf(argv[i], "%d", &numAllelesPerPhase) == 0)
      {
        cerr << "Error parsing numAllelesPerPhase\n";
        return 1;
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
        return 1;
      }
      if (sscanf(argv[i], "%lf", &gEnergyLimit) == 0)
      {
        cerr << "Error parsing energy limit\n";
        return 1;
      }
    }
    else
    if (strcmp(argv[i], "--hipExtensorLengthFeedbackP2") == 0)
    {
      i++;
      if (i >= argc) 
      {
        cerr << "Error parsing hipExtensorLengthFeedbackP2\n";
        return 1;
      }
      if (sscanf(argv[i], "%lf", &hipExtensorLengthFeedbackP2) == 0)
      {
        cerr << "Error parsing hipExtensorLengthFeedbackP2\n";
        return 1;
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
        return 1;
      }
      if (sscanf(argv[i], "%d", &gWindowWidth) == 0)
      {
        cerr << "Error parsing window size x\n";
        return 1;
      }
      i++;
      if (i >= argc) 
      {
        cerr << "Error parsing window size y\n";
        return 1;
      }
      if (sscanf(argv[i], "%d", &gWindowHeight) == 0)
      {
        cerr << "Error parsing window size y\n";
        return 1;
      }
    }
    else
    if (strcmp(argv[i], "--help") == 0 ||
      strcmp(argv[i], "-h") == 0 ||
      strcmp(argv[i], "-?") == 0)
    {
      cerr <<       
"\nObjective build " << __DATE__ << " " << __TIME__ << "\n" <<
"\nobjective produces a fitness score in score.tmp depending on the input data in\n\
genome.tmp and the model information in ModelData.txt which generally loads s\n\
number of .xan files).\n\n\
There are a number of command line options available:\n\n\
-d, --display\n\
Switches to interactive mode. No score.tmp file is produced and an animation of\n\
the model is produced in a new window. The animation is started and stopped with\n\
the (p) key, stepped with the (s) key, toggled between slow and fast with the\n\
(l) key, and quit with the (esc) key\n\n\
-k filename, --kinetics filename\n\
Creates a kinetics file containing kinetic data for all the\n\
segments in the model\n\n\
-g filename, --genome filename\n\
Reads filename rather than the default genome.tmp as the genome data\n\n\
-c filename, --config filename\n\
Reads filename rather than the default ModelData.tmp as the config data\n\n\
-s filename, --score filename\n\
Writes filename rather than the default score.tmp as the genome data\n\n\
-u, --summary\n\
Writes out summary information at end of fitness run.\n\n\
-e n, --debug n\n\
Prints out a lot of extra debugging information on stderr if n is higher than 0.\n\
Suitable values of n are defined in DebugControl.h\n\n\
--windowSize x y\n\
Sets the initial display window size - requires separate x and y values.\n\n\
-t n, --timeLimit n\n\
Sets a time limit on the simulation. The simulation will pause after this limit\n\
in display mode or stop in fitness mode.\n\n\
--outputStateAt n\n\
If set to a value greater than or equal to zero, will output model state\n\
information at that time to ModelState.txt\n\n\
--outputStateAtPhase n\n\
If set to a value greater than or equal to zero, will output model state\n\
information at the numbered phase (overules --outputStateAt)\n\n\
--numStartupPhases n\n\
Sets the number of startup phases (default 2)\n\n\
-- numCyclicPhases n\n\
Sets the number of cyclic phases (default 6)\n\n\
--numAllelesPerPhase n\n\
Sets the number of alleles per phase (default 7)\n\n\
-y n, --energyLimit n\n\
Sets an energy limit on the simulation. The simulation will stop after this\n\
limit in fitness mode. No effect in display mode. If <= 0 then no limit!\n\n\
--mechanicalEnergy\n\
Energy calculations are mechanical rather than metabolic\n\n\
--hipExtensorLengthFeedbackP2 n\n\
Sets the value of the hipExtensorLengthFeedbackP2 parameters - overrides\n\
any value set in the genome.\n\n\
-h, -?, --help\n\
Prints this message!\n\n";
      return 1;
    }
    else
    {
      cerr << "Unrecognised option. Try 'objective --help' for more info\n";
      return 1;
    }
  }
  
  if (displayFlag) StartGlut();
     
  // create the simulation object
  gSimulation = new Simulation();
  gSimulation->SetGraphics(displayFlag);
  gSimulation->LoadModel(configFilenamePtr);
  
  // load the genome
  gSimulation->GetCPG()->Initialise(numStartupPhases, numCyclicPhases, 
        numAllelesPerPhase);
  gSimulation->GetCPG()->ReadGenome(genomeFilenamePtr);
  
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
  
  // fix HipExtensorLengthFeedbackP2?
  // bit of a kludge relying on friend status
  if (hipExtensorLengthFeedbackP2 >= -1.0 && 
      hipExtensorLengthFeedbackP2 <= 1.0)
  {
    for (i = 0; i < gSimulation->GetCPG()->m_NumPhases; i++)
    {
      gSimulation->GetCPG()->m_LeftHipExtensorLengthFeedbackP2[i] 
          = hipExtensorLengthFeedbackP2;
      gSimulation->GetCPG()->m_RightHipExtensorLengthFeedbackP2[i] 
          = hipExtensorLengthFeedbackP2;
    }
  }
        
  if (displayFlag)
  {
    glutMainLoop();
  }
  else
  {
    FILE *out;                
    score = CalculateFitness();
    out = fopen(scoreFilenamePtr, "wb");
    fwrite(&score, sizeof(double), 1, out);
    fclose(out);
  }
  
  return 0;
}


double
CalculateFitness()
{
  dmLink *myLink;
  unsigned int torsoIndex;
  const dmABForKinStruct *theTorsoDmABForKinStruct;
  SpatialVector torsoWorldV;
  double startingPosition = 0;
  bool firstTime = true;
  
  // get the indices of torso
  myLink = (dmLink *) dmuFindObject(kTorsoPartName, gSimulation->GetRobot());
  assert(myLink);
  torsoIndex = gSimulation->GetRobot()->getLinkIndex(myLink);
    
  // get some pointers for later
  theTorsoDmABForKinStruct = gSimulation->GetRobot()->getForKinStruct(torsoIndex);
  
  // and run for required amount of time
  double score = 0;
  int runCount = 0;
  double energy = 0;
  bool successFlag = true;
  
  // 10 seconds when maximising speed 
  // 30 when maximising economy     
  while (gSimulation->GetTime() < gTimeLimit)
  {
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
          
    gSimulation->UpdateSimulation();
    if (firstTime)
    {
       startingPosition = theTorsoDmABForKinStruct->p_ICS[0];
       firstTime = false;
    }
        
    // use torso position as a sanity check
    if (theTorsoDmABForKinStruct->p_ICS[2] < 0.5 || 
      theTorsoDmABForKinStruct->p_ICS[2] > 2.5) 
    {
      successFlag = false;
      break;
    }
    
    // get torso world velocity and use as sanity check
    ConvertLocalToWorldV(theTorsoDmABForKinStruct, theTorsoDmABForKinStruct->v, 
      torsoWorldV);
    if (torsoWorldV[3] < -1.0 || torsoWorldV[3] > 10.0)
    {
      successFlag = false;
      break;
    }
    
    // calculate energy driving system
    
    energy += CalculateEnergyCost(gSimulation->GetCPG()->GetLeftHipExtensor());
    energy += CalculateEnergyCost(gSimulation->GetCPG()->GetLeftHipFlexor());
    energy += CalculateEnergyCost(gSimulation->GetCPG()->GetRightHipExtensor());
    energy += CalculateEnergyCost(gSimulation->GetCPG()->GetRightHipFlexor());
    energy += CalculateEnergyCost(gSimulation->GetCPG()->GetLeftKneeExtensor());
    energy += CalculateEnergyCost(gSimulation->GetCPG()->GetLeftKneeFlexor());
    energy += CalculateEnergyCost(gSimulation->GetCPG()->GetRightKneeExtensor());
    energy += CalculateEnergyCost(gSimulation->GetCPG()->GetRightKneeFlexor());
    energy += CalculateEnergyCost(gSimulation->GetCPG()->GetLeftAnkleExtensor());
    energy += CalculateEnergyCost(gSimulation->GetCPG()->GetLeftAnkleFlexor());
    energy += CalculateEnergyCost(gSimulation->GetCPG()->GetRightAnkleExtensor());
    energy += CalculateEnergyCost(gSimulation->GetCPG()->GetRightAnkleFlexor());
    
    // adding this criterion makes the distance proportional to efficiency rather
    // than speed
    if (gEnergyLimit > 0)
    {
      if (energy >= gEnergyLimit) break;
    }

    if (gDebug == MainDebug) 
      cerr << "CalculateFitness\tgSimulation->GetTime()\t" << gSimulation->GetTime() << 
        "\ttheTorsoDmABForKinStruct->p_ICS[0]\t" << theTorsoDmABForKinStruct->p_ICS[0] << 
        "\tenergy\t" << energy << "\n";

    runCount++;
    if (gKineticsFilenamePtr) OutputKinetics();
  }
  score = theTorsoDmABForKinStruct->p_ICS[0] - startingPosition;
  if (gDebug == MainDebug) 
    cerr << "CalculateFitness\tscore\t" << score << 
        "\tenergy\t" << energy << "\n";
  
  if (gSummaryFlag) 
    cerr << "Simulation Time: " << gSimulation->GetTime() << 
      " Distance: " << theTorsoDmABForKinStruct->p_ICS[0] - startingPosition << 
      " Energy: " << energy << "\n";
  
  return score;
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

  
// convert a link local coordinate to a world coordinate

void 
ConvertLocalToWorldP(const dmABForKinStruct *m, const CartesianVector local, CartesianVector world)
{
  for (int j = 0; j < 3; j++)
  {

    world[j] = m->p_ICS[j] +
      m->R_ICS[j][0] * local[0] +
      m->R_ICS[j][1] * local[1] +
      m->R_ICS[j][2] * local[2];
  }
}

// convert a link local coordinate to a world coordinate
// don't know if this is actually corect for the rotations (are they
// global or local?)

void 
ConvertLocalToWorldV(const dmABForKinStruct *m, const SpatialVector local, SpatialVector world)
{
  for (int j = 0; j < 3; j++)
  {
    world[j] = 
      m->R_ICS[j][0] * local[0] +
      m->R_ICS[j][1] * local[1] +
      m->R_ICS[j][2] * local[2];

    world[j + 3] = 
      m->R_ICS[j][0] * local[0 + 3] +
      m->R_ICS[j][1] * local[1 + 3] +
      m->R_ICS[j][2] * local[2 + 3];
  }
}

// calulate the energy cost from a StrapForce

double
CalculateEnergyCost(MAMuscle *strapForce)
{
   if (gMechanicalEnergyFlag)
   {
      return (strapForce->GetPower() * gSimulation->GetTimeIncrement());
   }
   else
   {
      return (strapForce->GetMetabolicPower() * gSimulation->GetTimeIncrement());
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
