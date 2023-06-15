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

// Simulation - must be a global instantiated somewhere
Simulation *gSimulation;

// debug output control
extern int gDebug;

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
  m_GenomeFileName = "genome.tmp";
  m_ControlFileName = "ControlFile.dat";
  m_GeneMappingFileName = "GeneMapping.dat";
  m_ConfigFileName = "Biped.cfg";
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
}

void
Fitness::ReadInitialisationData(bool graphicsFlag)
{
  int i;
  ControllerList *myControllerList;
  Controller *myController;
  int startGene;
  int numGenes;
  bool error;
  
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
  gSimulation->Load("Biped.cfg");
  
  // load the gene mapping file
  ParameterFile geneMappingFile;
  geneMappingFile.ReadFile(m_GeneMappingFileName);
      
  // assign the controller values based on the current genome
  
  myControllerList = gSimulation->GetControllerList();
  for (i = 0; i < myControllerList->GetSize(); i++)
  {
    myController= myControllerList->GetEntry(i);
    if (gDebug > 0) cerr << "Fitness\tReadInitialisationData\tmyController\t"
      << myController->getName();
    error = geneMappingFile.FindParameter(myController->getName());
    assert(error == false);
    error = geneMappingFile.ReadNext(&numGenes);
    assert(error == false);
    error = geneMappingFile.ReadNext(&startGene);
    assert(error == false);
    if (gDebug > 0) cerr << "\tnumGenes\t" << numGenes 
      << "\tstartGene\t" << startGene << "\n";
    myController->SetParameters(numGenes, m_Genome + startGene);
  }
  
  // open the output kinetics file
  if (m_KineticsFileName) m_OutputKinetics.open(m_KineticsFileName);
  
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
  //myLink = (dmLink *) dmuFindObject((char *)kLeftFootPartName, gSimulation->GetRobot());
  //assert(myLink);
  //leftFootLinkIndex = gSimulation->GetRobot()->getLinkIndex(myLink);
  //myLink = (dmLink *) dmuFindObject((char *)kRightFootPartName, gSimulation->GetRobot());
  //assert(myLink);
  //rightFootLinkIndex = gSimulation->GetRobot()->getLinkIndex(myLink);

  // and torso
  myLink = (dmLink *) dmuFindObject((char *)kTorsoPartName, gSimulation->GetRobot());
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
  double runLimit = 5;
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
        
  while (gSimulation->GetTime() < runLimit)
  {
    if (gDebug > 0) 
      cerr << "Fitness::CalculateFitness\tgSimulation->GetTime()\t" << gSimulation->GetTime() << "\n";

      gSimulation->UpdateSimulation();
    if (gErrorCode)
      break;
        
    // use torso position as a sanity check
    // model genome is 1.06 to 1.36
    if (theTorsoDmABForKinStruct->p_ICS[ZC] < 1.0 || 
      theTorsoDmABForKinStruct->p_ICS[ZC] > 1.4) 
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
    if (torsoWorldV[3] < -0.5 || torsoWorldV[3] > 3.5)
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
          ((LoggedForce*) 
          (gSimulation->GetForceList()->GetEntry(i)))->getPower();
      if (power >= 0) // positive work
           totalPower += power;
      else      // negative work -  a bit cheaper
           totalPower -= power * 0.5; 
      force =
          ((LoggedForce*) 
          (gSimulation->GetForceList()->GetEntry(i)))->getForce();
      totalForce += force;
    }
    energy += totalPower * gSimulation->GetTimeIncrement();
    impulse += totalForce * gSimulation->GetTimeIncrement();
    if (gDebug > 0) cerr << "Fitness::CalculateFitness\ttotalForce\t" << totalForce << "\n";
    
    runCount++;
    if (m_KineticsFileName) OutputKinetics();
  }
  if (successFlag) score = 3000 * theTorsoDmABForKinStruct->p_ICS[XC] - energy;
  else score = theTorsoDmABForKinStruct->p_ICS[XC];
  if (gDebug > 0) 
    cerr << "Fitness::CalculateFitness\tscore\t" << score << 
        "\tenergy\t" << energy << "\n";
  return score;
}

// function to produce a file of link kinetics

void 
Fitness::OutputKinetics()
{
  int myNumLinks = gSimulation->GetRobot()->getNumLinks();
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
  int i;
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


