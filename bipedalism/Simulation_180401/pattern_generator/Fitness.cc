// Fitness.cc

// holds the objective function for GA use

#include <stdio.h>
#include <iostream.h>
#include <iomanip.h>
#include <fstream.h>
#include <assert.h>
#include <vector>
#include <math.h>
#include <float.h>

#include <glut.h>

#include <dmGLTime.h>
#include <dmGLMouse.h>
#include <dmGLPolarCamera_zup.h>
#include <dmEnvironment.h>
#include <dmArticulation.h>
#include <dmMobileBaseLink.h>
#include <dmu.h>

#include "ForceList.h"
#include "Simulation.h"
#include "Controller.h"
#include "ControllerList.h"
#include "LoggedForce.h"
#include "Util.h"
#include "Fitness.h"
#include "ModifiedContactModel.h"
#include "SegmentParameters.h"
#include "ParameterFile.h"
#include "MuscleModel.h"
#include "ControllerData.h"
#include "DebugControl.h"

// Simulation - must be a global instantiated somewhere
Simulation *gSimulation;

// fitness parameters
const double kRunLimit = 30;
const double kStepTime = 0.005;
const int kSteps = (int)((kRunLimit / kStepTime) + 0.5); 

// 2.506628275 is sqrt(2 * M_PI)
#define GAUSSIAN(x, mu, sigma) ((1/((sigma)*2.506628275)) * exp(-((x)-(mu))*((x)-(mu)) / (2*(sigma)*(sigma))))
#define UNIT_HEIGHT_GAUSSIAN(x, mu, sigma) (exp(-((x)-(mu))*((x)-(mu)) / (2*(sigma)*(sigma))))
#define MAX(a,b) ((a) > (b) ? (a) : (b))
#define MIN(a,b) ((a) < (b) ? (a) : (b))
#define ABS(a) ((a) >= 0 ? (a) : -(a))
#define ODD(n) ((n) & 1)


Fitness::Fitness()
{
  gSimulation = 0;
  m_GenomeFileName = 0;
  m_ConfigFileName = 0;
  m_KineticsFileName = 0;
  m_Genome = 0;    

#ifdef TEST_VELOCITY    
  test_velocity();
#endif
}

Fitness::~Fitness()
{
  if (gSimulation) delete gSimulation;
  if (m_Genome) delete [] m_Genome;
  if (m_GenomeFileName) delete [] m_GenomeFileName;
  if (m_ConfigFileName) delete [] m_ConfigFileName;
  if (m_KineticsFileName) delete [] m_KineticsFileName ;
}

void
Fitness::ReadInitialisationData(bool graphicsFlag)
{
  int i, j, k;
  
  // read the gene data
  
  ifstream inFile(m_GenomeFileName);
  assert(inFile.good());
  inFile >> m_GenomeLength;
  m_Genome = new double[m_GenomeLength];
  for (i = 0; i < m_GenomeLength; i++)
    inFile >> m_Genome[i];
  inFile.close();

  gSimulation = new Simulation();

  // turn off/on graphics

  gSimulation->SetGraphics(graphicsFlag);

  // turn off SetControllerDataFromFile

  gSimulation->SetControllerDataFromFile(false);

  // load the simulation data file
  gSimulation->Load(m_ConfigFileName);
  
  // assign the controller values based on the current genome
  
  ControllerData controllerData;
  controllerData.SetSize(kSteps);
  
  int kBlockSize = 0;
  int kNumStartupBlocks = 0;
  int kNumCycleBlocks = 0;
  int kTotalBlocks = 0;
  if (m_GenomeLength == 208)
  {
     kBlockSize = 13;
     kNumStartupBlocks = 10;
     kNumCycleBlocks = 6;
     kTotalBlocks = kNumStartupBlocks + kNumCycleBlocks;
     assert(kTotalBlocks * kBlockSize == m_GenomeLength);
  }
  if (m_GenomeLength == 65)
  {
     kBlockSize = 13;
     kNumStartupBlocks = 2;
     kNumCycleBlocks = 3;
     kTotalBlocks = kNumStartupBlocks + kNumCycleBlocks;
  }
  assert(kTotalBlocks * kBlockSize == m_GenomeLength);
  int *keyIndices = new int[kTotalBlocks];
  
  // just check that the step time read in from the file matches
  assert(kStepTime == kControllerStepTime);
  
  // calculate the key index values
  for (i = 0;  i < kTotalBlocks; i++)
  {
    keyIndices[i] =  
        (int)(0.5 + m_Genome[i * kBlockSize] / kStepTime);
  }
            
  // startup section
  int genomeOffset = 0;
  i = 0;
  for (j = 0; j < kNumStartupBlocks; j++)
  {
    for (k = 0; k < keyIndices[j]; k++)
    {
      controllerData.m_RightHipExtensorController[i] = m_Genome[genomeOffset + 1];
      controllerData.m_RightHipFlexorController[i] = m_Genome[genomeOffset + 2];
      controllerData.m_RightKneeExtensorController[i] = m_Genome[genomeOffset + 3];
      controllerData.m_RightKneeFlexorController[i] = m_Genome[genomeOffset + 4];
      controllerData.m_RightAnkleExtensorController[i] = m_Genome[genomeOffset + 5];
      controllerData.m_RightAnkleFlexorController[i] = m_Genome[genomeOffset + 6];
      controllerData.m_LeftHipExtensorController[i] = m_Genome[genomeOffset + 7];
      controllerData.m_LeftHipFlexorController[i] = m_Genome[genomeOffset + 8];
      controllerData.m_LeftKneeExtensorController[i] = m_Genome[genomeOffset + 9];
      controllerData.m_LeftKneeFlexorController[i] = m_Genome[genomeOffset + 10];
      controllerData.m_LeftAnkleExtensorController[i] = m_Genome[genomeOffset + 11];
      controllerData.m_LeftAnkleFlexorController[i] = m_Genome[genomeOffset + 12];
      i++;
    }
    genomeOffset += kBlockSize;
  }
  
  // cyclic values
  int cycleCount = 1;
  while (i < kSteps)
  {
    genomeOffset = kNumStartupBlocks * kBlockSize;
    if (ODD(cycleCount))
    {
      for (j = kNumStartupBlocks; j < kTotalBlocks; j++)
      {
        for (k = 0; k < keyIndices[j]; k++)
        {
          controllerData.m_RightHipExtensorController[i] = m_Genome[genomeOffset + 7];
          controllerData.m_RightHipFlexorController[i] = m_Genome[genomeOffset + 8];
          controllerData.m_RightKneeExtensorController[i] = m_Genome[genomeOffset + 9];
          controllerData.m_RightKneeFlexorController[i] = m_Genome[genomeOffset + 10];
          controllerData.m_RightAnkleExtensorController[i] = m_Genome[genomeOffset + 11];
          controllerData.m_RightAnkleFlexorController[i] = m_Genome[genomeOffset + 12];
          controllerData.m_LeftHipExtensorController[i] = m_Genome[genomeOffset + 1];
          controllerData.m_LeftHipFlexorController[i] = m_Genome[genomeOffset + 2];
          controllerData.m_LeftKneeExtensorController[i] = m_Genome[genomeOffset + 3];
          controllerData.m_LeftKneeFlexorController[i] = m_Genome[genomeOffset + 4];
          controllerData.m_LeftAnkleExtensorController[i] = m_Genome[genomeOffset + 5];
          controllerData.m_LeftAnkleFlexorController[i] = m_Genome[genomeOffset + 6];
          i++;
          if (i >= kSteps) break;
        }
        genomeOffset += kBlockSize;
        if (i >= kSteps) break;
      }
    }
    else
    {
      for (j = kNumStartupBlocks; j < kTotalBlocks; j++)
      {
        for (k = 0; k < keyIndices[j]; k++)
        {
          controllerData.m_RightHipExtensorController[i] = m_Genome[genomeOffset + 1];
          controllerData.m_RightHipFlexorController[i] = m_Genome[genomeOffset + 2];
          controllerData.m_RightKneeExtensorController[i] = m_Genome[genomeOffset + 3];
          controllerData.m_RightKneeFlexorController[i] = m_Genome[genomeOffset + 4];
          controllerData.m_RightAnkleExtensorController[i] = m_Genome[genomeOffset + 5];
          controllerData.m_RightAnkleFlexorController[i] = m_Genome[genomeOffset + 6];
          controllerData.m_LeftHipExtensorController[i] = m_Genome[genomeOffset + 7];
          controllerData.m_LeftHipFlexorController[i] = m_Genome[genomeOffset + 8];
          controllerData.m_LeftKneeExtensorController[i] = m_Genome[genomeOffset + 9];
          controllerData.m_LeftKneeFlexorController[i] = m_Genome[genomeOffset + 10];
          controllerData.m_LeftAnkleExtensorController[i] = m_Genome[genomeOffset + 11];
          controllerData.m_LeftAnkleFlexorController[i] = m_Genome[genomeOffset + 12];
          i++;
          if (i >= kSteps) break;
        }
        genomeOffset += kBlockSize;
        if (i >= kSteps) break;
      }
    }
    cycleCount++;
  }
  
  
  // assign the values to the controller  
  Controller *myController;
  ControllerList *myControllerList = gSimulation->GetControllerList();
  for (i = 0; i < myControllerList->GetSize(); i++)
  {
    myController= myControllerList->GetEntry(i);
    if (strcmp(myController->getName(), "RightHipExtensorController") ==0)
      myController->SetParameters(kSteps, controllerData.m_RightHipExtensorController);
    if (strcmp(myController->getName(), "RightHipFlexorController") ==0)
      myController->SetParameters(kSteps, controllerData.m_RightHipFlexorController);
    if (strcmp(myController->getName(), "RightKneeExtensorController") ==0)
      myController->SetParameters(kSteps, controllerData.m_RightKneeExtensorController);
    if (strcmp(myController->getName(), "RightKneeFlexorController") ==0)
      myController->SetParameters(kSteps, controllerData.m_RightKneeFlexorController);
    if (strcmp(myController->getName(), "RightAnkleExtensorController") ==0)
      myController->SetParameters(kSteps, controllerData.m_RightAnkleExtensorController);
    if (strcmp(myController->getName(), "RightAnkleFlexorController") ==0)
      myController->SetParameters(kSteps, controllerData.m_RightAnkleFlexorController);
    if (strcmp(myController->getName(), "LeftHipExtensorController") ==0)
      myController->SetParameters(kSteps, controllerData.m_LeftHipExtensorController);
    if (strcmp(myController->getName(), "LeftHipFlexorController") ==0)
      myController->SetParameters(kSteps, controllerData.m_LeftHipFlexorController);
    if (strcmp(myController->getName(), "LeftKneeExtensorController") ==0)
      myController->SetParameters(kSteps, controllerData.m_LeftKneeExtensorController);
    if (strcmp(myController->getName(), "LeftKneeFlexorController") ==0)
      myController->SetParameters(kSteps, controllerData.m_LeftKneeFlexorController);
    if (strcmp(myController->getName(), "LeftAnkleExtensorController") ==0)
      myController->SetParameters(kSteps, controllerData.m_LeftAnkleExtensorController);
    if (strcmp(myController->getName(), "LeftAnkleFlexorController") ==0)
      myController->SetParameters(kSteps, controllerData.m_LeftAnkleFlexorController);
  }
  
  // open the output kinetics file
  if (m_KineticsFileName) m_OutputKinetics.open(m_KineticsFileName);
  
  delete [] keyIndices;
}

double
Fitness::CalculateFitness()
{
  dmLink *myLink;
  //unsigned int leftFootLinkIndex, rightFootLinkIndex;
  unsigned int torsoIndex;
  //const dmABForKinStruct *theLeftFootDmABForKinStruct;
  //const dmABForKinStruct *theRightFootDmABForKinStruct;
  const dmABForKinStruct *theTorsoDmABForKinStruct;
  ModifiedContactModel *leftFootContact;
  ModifiedContactModel *rightFootContact;
  //double xLeftFootMin, yLeftFootMin, zLeftFootMin;
  //double xLeftFootMax, yLeftFootMax, zLeftFootMax;
  //double xRightFootMin, yRightFootMin, zRightFootMin;
  //double xRightFootMax, yRightFootMax, zRightFootMax;
  SpatialVector torsoWorldV;
  //SpatialVector leftFootWorldV;
  //SpatialVector rightFootWorldV;
  //SpatialVector leftContactForce;
  //SpatialVector rightContactForce;
  int i;
  
  // get the indices of the feet
  //myLink = (dmLink *) dmuFindObject(kLeftFootPartName, gSimulation->GetRobot());
  //assert(myLink);
  //leftFootLinkIndex = gSimulation->GetRobot()->getLinkIndex(myLink);
  //myLink = (dmLink *) dmuFindObject(kRightFootPartName, gSimulation->GetRobot());
  //assert(myLink);
  //rightFootLinkIndex = gSimulation->GetRobot()->getLinkIndex(myLink);

  // and torso
  myLink = (dmLink *) dmuFindObject(kTorsoPartName, gSimulation->GetRobot());
  assert(myLink);
  torsoIndex = gSimulation->GetRobot()->getLinkIndex(myLink);
  
  // get the contact models
  leftFootContact = (ModifiedContactModel *) 
    gSimulation->GetForceList()->FindByName(kLeftFootContactName);
  assert(leftFootContact);
  rightFootContact = (ModifiedContactModel *) 
    gSimulation->GetForceList()->FindByName(kRightFootContactName);
  assert(rightFootContact);

  // log the left knee force
  // MuscleModel *leftKneeExtensor = (MuscleModel *) 
  //  gSimulation->GetForceList()->FindByName(kLeftKneeExtensorName);
  // assert(leftKneeExtensor);
  // leftKneeExtensor->startLog();
  
  // get some pointers for later
  //theLeftFootDmABForKinStruct = gSimulation->GetRobot()->getForKinStruct(leftFootLinkIndex);
  //theRightFootDmABForKinStruct = gSimulation->GetRobot()->getForKinStruct(rightFootLinkIndex);
  theTorsoDmABForKinStruct = gSimulation->GetRobot()->getForKinStruct(torsoIndex);

  // and run for required amount of time
  int gErrorCode = 0;
  double score = 0;
  int runCount = 0;
  bool successFlag = true;
  double energy = 0;
  double impulse = 0;
  double power;
  double totalPower;
  double force;
  double totalForce;
  double torsoMaxZ = -DBL_MAX;
  double torsoMinZ = DBL_MAX;
  double torsoMaxXV = -DBL_MAX;
  double torsoMinXV = DBL_MAX;
  
  // 10 seconds when maximising speed 
  // kRunLimit when maximising economy     
  while (gSimulation->GetTime() < kRunLimit)
  {
    // should always be less than hardwired run limit
    assert(gSimulation->GetTime() < kRunLimit);

      gSimulation->UpdateSimulation();
    if (gErrorCode)
      break;
        
    if (gDebug == FitnessDebug) 
      cerr << "Fitness::CalculateFitness\tgSimulation->GetTime()\t" << gSimulation->GetTime() << 
        "\ttheTorsoDmABForKinStruct->p_ICS[XC]\t" << theTorsoDmABForKinStruct->p_ICS[XC] << "\n";

    // use torso position as a sanity check
    // model genome is 1.06 to 1.36
    if (theTorsoDmABForKinStruct->p_ICS[ZC] < 0.8 || 
      theTorsoDmABForKinStruct->p_ICS[ZC] > 1.6) 
    {
      successFlag = false;
      break;
    }
    torsoMaxZ = MAX(torsoMaxZ, theTorsoDmABForKinStruct->p_ICS[ZC]);
    torsoMinZ = MIN(torsoMinZ, theTorsoDmABForKinStruct->p_ICS[ZC]);    
    
    // get torso world velocity and use as sanity check
    // model genome is -0.08 to 3.02
    ConvertLocalToWorldV(theTorsoDmABForKinStruct, theTorsoDmABForKinStruct->v, 
      torsoWorldV);
    if (torsoWorldV[3] < -1.0 || torsoWorldV[3] > 10.0)
    {
      successFlag = false;
      break;
    }
    torsoMaxXV = MAX(torsoMaxXV, torsoWorldV[3]);
    torsoMinXV = MIN(torsoMinXV, torsoWorldV[3]);    
    
    // calculate energy driving system
    totalPower = 0;
    totalForce = 0;
    for (i = 0; i < gSimulation->GetForceList()->GetSize(); i++)
    {
      // all forces in the list are actually derived from LoggedForce
      power =
          gSimulation->GetForceList()->GetEntry(i)->getPower();
      if (power >= 0) // positive work
           totalPower += power;
      else      // negative work -  a bit cheaper
           totalPower -= power * 0.5; 
      force =
          gSimulation->GetForceList()->GetEntry(i)->getForce();
      totalForce += force;
    }
    energy += totalPower * gSimulation->GetTimeIncrement();
    // adding this criterion makes the distance proportional to efficiency rather
    // than speed
    if (energy >= 20000) break;
    impulse += totalForce * gSimulation->GetTimeIncrement();

    if (gDebug == FitnessDebug) cerr << "Fitness::CalculateFitness\ttotalForce\t" << totalForce << 
    "\timpulse\t" << impulse << "\ttotalPower\t" << totalPower <<
    "\tenergy\t" << energy << "\n";
    
    runCount++;
    if (m_KineticsFileName) OutputKinetics();
  }
  score = theTorsoDmABForKinStruct->p_ICS[XC] - 5;
  if (gDebug == FitnessDebug) 
    cerr << "Fitness::CalculateFitness\tscore\t" << score << 
        "\tenergy\t" << energy << "\n";
  return score;
}

// function to produce a file of link kinetics

void 
Fitness::OutputKinetics()
{
  unsigned int myNumLinks = gSimulation->GetRobot()->getNumLinks();
  const dmABForKinStruct *myDmABForKinStruct;
  unsigned int i;
  int j, k;
  static bool firstTimeFlag = true;
  
  // first time through output the column headings
  if (firstTimeFlag)
  {
    m_OutputKinetics << "time\t";
    for (i = 0; i < myNumLinks; i++)
    {
      m_OutputKinetics << 
        gSimulation->GetRobot()->getLink(i)->getName() 
        << "_X\t";
      m_OutputKinetics << 
        gSimulation->GetRobot()->getLink(i)->getName() 
        << "_Y\t";
      m_OutputKinetics << 
        gSimulation->GetRobot()->getLink(i)->getName() 
        << "_Z\t";
      m_OutputKinetics << 
        gSimulation->GetRobot()->getLink(i)->getName() 
        << "_R00\t";
      m_OutputKinetics << 
        gSimulation->GetRobot()->getLink(i)->getName() 
        << "_R01\t";
      m_OutputKinetics << 
        gSimulation->GetRobot()->getLink(i)->getName() 
        << "_R02\t";
      m_OutputKinetics << 
        gSimulation->GetRobot()->getLink(i)->getName() 
        << "_R10\t";
      m_OutputKinetics << 
        gSimulation->GetRobot()->getLink(i)->getName() 
        << "_R11\t";
      m_OutputKinetics << 
        gSimulation->GetRobot()->getLink(i)->getName() 
        << "_R12\t";
      m_OutputKinetics << 
        gSimulation->GetRobot()->getLink(i)->getName() 
        << "_r20\t";
      m_OutputKinetics << 
        gSimulation->GetRobot()->getLink(i)->getName() 
        << "_R21\t";
      m_OutputKinetics << 
        gSimulation->GetRobot()->getLink(i)->getName() 
        << "_R22";
      if (i != myNumLinks - 1) m_OutputKinetics << "\t";
      else m_OutputKinetics << "\n";    
    }
    firstTimeFlag = false;
  }     
  
  // start by outputting the simulation time
  m_OutputKinetics << gSimulation->GetTime() << "\t";
  
  for (i = 0; i < myNumLinks; i++)
  {
    // output the position
    myDmABForKinStruct = gSimulation->GetRobot()->getForKinStruct(i);
    m_OutputKinetics << myDmABForKinStruct->p_ICS[XC] << "\t"
      << myDmABForKinStruct->p_ICS[YC] << "\t"
      << myDmABForKinStruct->p_ICS[ZC] << "\t";
    // then the orientation
    for (j = 0; j < 3; j++)
    {
      for (k = 0; k < 3; k++)
      {
        m_OutputKinetics << myDmABForKinStruct->R_ICS[j][k];
        if (k != 2) m_OutputKinetics << "\t";
      }
      if (j != 2) m_OutputKinetics << "\t";
    }
    if (i != myNumLinks - 1) m_OutputKinetics << "\t";
    else m_OutputKinetics << "\n";    
  }
}

  
// convert a link local coordinate to a world coordinate

void 
Fitness::ConvertLocalToWorldP(const dmABForKinStruct *m, const CartesianVector local, CartesianVector world)
{
  for (int j = 0; j < 3; j++)
  {

    world[j] = m->p_ICS[j] +
      m->R_ICS[j][XC] * local[XC] +
      m->R_ICS[j][YC] * local[YC] +
      m->R_ICS[j][ZC] * local[ZC];
  }
}

// convert a link local coordinate to a world coordinate
// don't know if this is actually corect for the rotations (are they
// global or local?)

void 
Fitness::ConvertLocalToWorldV(const dmABForKinStruct *m, const SpatialVector local, SpatialVector world)
{
  for (int j = 0; j < 3; j++)
  {
    world[j] = 
      m->R_ICS[j][XC] * local[XC] +
      m->R_ICS[j][YC] * local[YC] +
      m->R_ICS[j][ZC] * local[ZC];

    world[j + 3] = 
      m->R_ICS[j][XC] * local[XC + 3] +
      m->R_ICS[j][YC] * local[YC + 3] +
      m->R_ICS[j][ZC] * local[ZC + 3];
  }
}

// find the frontmost and rearmost contacts

void
Fitness::GetContactLimits(ModifiedContactModel *contactModel, const dmABForKinStruct *m,
    double &xMin, double &yMin, double &zMin, 
    double &xMax, double &yMax, double &zMax)
{
  unsigned int i;
  CartesianVector local;
  CartesianVector world;
  
  xMin = yMin = zMin = DBL_MAX;
  xMax = yMax = zMax = -DBL_MAX;
  
  for (i = 0; i < contactModel->getNumContactPoints(); i++)
  {
     contactModel->getContactPoint(i, local);
     ConvertLocalToWorldP(m, local, world);
     xMin = MIN(xMin, world[XC]); 
     yMin = MIN(yMin, world[YC]); 
     zMin = MIN(zMin, world[ZC]); 
     xMax = MAX(xMax, world[XC]); 
     yMax = MAX(yMax, world[YC]); 
     zMax = MAX(zMax, world[ZC]); 
   }
 }
 
// sanity check a velocity vector - returns true if insane!
 
bool
Fitness::SanityCheck(const SpatialVector v)
{  
  // rotational
  if (ABS(v[0]) > 100) return true;
  if (ABS(v[1]) > 100) return true;
  if (ABS(v[2]) > 100) return true;
  // translational
  if (ABS(v[3]) > 10) return true;
  if (ABS(v[4]) > 10) return true;
  if (ABS(v[5]) > 10) return true;
  
  // if we get here we're fine
  return false;
}

// set the file names
void Fitness::SetGenomeFileName(const char *name)
{
  if (m_GenomeFileName) delete [] m_GenomeFileName;
  m_GenomeFileName = new char[strlen(name) + 1];
  strcpy(m_GenomeFileName, name);
}
  
void Fitness::SetKineticsFileName(const char *name)
{
  if (m_KineticsFileName) delete [] m_KineticsFileName;
  m_KineticsFileName = new char[strlen(name) + 1];
  strcpy(m_KineticsFileName, name);
}

void Fitness::SetConfigFileName(const char *name)
{
  if (m_ConfigFileName) delete [] m_ConfigFileName;
  m_ConfigFileName = new char[strlen(name) + 1];
  strcpy(m_ConfigFileName, name);
}


