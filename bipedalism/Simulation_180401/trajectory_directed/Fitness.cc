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
#define SQUARE(x) ((x) * (x))

Fitness::Fitness()
{
  gSimulation = 0;
  m_GenomeFileName = "genome.tmp";
  m_ControlFileName = "ControlFile.dat";
  m_GeneMappingFileName = "GeneMapping.dat";
  m_ConfigFileName = "Biped.cfg";
  m_KineticsFileName = 0;
  m_Genome = 0;
}

Fitness::~Fitness()
{
  if (gSimulation) delete gSimulation;
  if (m_Genome) delete [] m_Genome;
}

void
Fitness::ReadInitialisationData(bool graphicsFlag)
{
  GeneMapping myGeneMapping;
  int i;
  ControllerList *myControllerList;
  Controller *myController;
  
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
  ifstream geneMapingFile(m_GeneMappingFileName);
  assert(geneMapingFile.good());
  while (GetQuotedString(geneMapingFile, myGeneMapping.name))
  {
    geneMapingFile >> myGeneMapping.n >> myGeneMapping.start;
    for (i = 0; i < gSimulation->GetControllerList()->GetSize(); i++)
    {
      if (strcmp(gSimulation->GetControllerList()->GetEntry(i)->getName(),
          myGeneMapping.name) == 0)
      {
        myGeneMapping.index = i;
        break;
      }
    }
    m_GeneMapping.push_back(myGeneMapping);
  }

  // assign the controller values based on the current genome
  
  myControllerList = gSimulation->GetControllerList();
  for (i = 0; i < m_GeneMapping.size(); i++)
  {
    myController = myControllerList->GetEntry(m_GeneMapping[i].index);
    assert(myController);
    myController->SetParameters(m_GeneMapping[i].n, m_Genome + m_GeneMapping[i].start);
  }
  
  // open the output kinetics file
  if (m_KineticsFileName) m_OutputKinetics.open(m_KineticsFileName);
  
}

double
Fitness::CalculateFitness()
{
  double score;
  int runCount;
  dmLink *myLink;
  unsigned int leftFootLinkIndex, rightFootLinkIndex;
  unsigned int leftThighLinkIndex, rightThighLinkIndex;
  unsigned int leftCalfLinkIndex, rightCalfLinkIndex;
  unsigned int torsoIndex;
  const dmABForKinStruct *theLeftFootDmABForKinStruct;
  const dmABForKinStruct *theRightFootDmABForKinStruct;
  const dmABForKinStruct *theLeftThighDmABForKinStruct;
  const dmABForKinStruct *theRightThighDmABForKinStruct;
  const dmABForKinStruct *theLeftCalfDmABForKinStruct;
  const dmABForKinStruct *theRightCalfDmABForKinStruct;
  const dmABForKinStruct *theTorsoDmABForKinStruct;
  ModifiedContactModel *leftFootContact;
  ModifiedContactModel *rightFootContact;
  double xLeftFootMin, yLeftFootMin, zLeftFootMin;
  double xLeftFootMax, yLeftFootMax, zLeftFootMax;
  double xRightFootMin, yRightFootMin, zRightFootMin;
  double xRightFootMax, yRightFootMax, zRightFootMax;

  double runLimit;
  int i;
  //double power;
  //double energy;
  //double totalPower;
  double force;
  double totalForce;
  int index;
  double interp;
  double x;
    
  // get the indices of the feet
  myLink = (dmLink *) dmuFindObject("left_foot", gSimulation->GetRobot());
  assert(myLink);
  leftFootLinkIndex = gSimulation->GetRobot()->getLinkIndex(myLink);
  myLink = (dmLink *) dmuFindObject("right_foot", gSimulation->GetRobot());
  assert(myLink);
  rightFootLinkIndex = gSimulation->GetRobot()->getLinkIndex(myLink);

  // get the indices of the thighs
  myLink = (dmLink *) dmuFindObject("left_thigh", gSimulation->GetRobot());
  assert(myLink);
  leftThighLinkIndex = gSimulation->GetRobot()->getLinkIndex(myLink);
  myLink = (dmLink *) dmuFindObject("right_thigh", gSimulation->GetRobot());
  assert(myLink);
  rightThighLinkIndex = gSimulation->GetRobot()->getLinkIndex(myLink);

  // get the indices of the thighs
  myLink = (dmLink *) dmuFindObject("left_calf", gSimulation->GetRobot());
  assert(myLink);
  leftCalfLinkIndex = gSimulation->GetRobot()->getLinkIndex(myLink);
  myLink = (dmLink *) dmuFindObject("right_calf", gSimulation->GetRobot());
  assert(myLink);
  rightCalfLinkIndex = gSimulation->GetRobot()->getLinkIndex(myLink);

  // and torso
  myLink = (dmLink *) dmuFindObject("torso", gSimulation->GetRobot());
  assert(myLink);
  torsoIndex = gSimulation->GetRobot()->getLinkIndex(myLink);
  
  // get the contact models
  leftFootContact = (ModifiedContactModel *) 
    gSimulation->GetForceList()->FindByName("left_foot_contact");
  assert(leftFootContact);
  rightFootContact = (ModifiedContactModel *) 
    gSimulation->GetForceList()->FindByName("right_foot_contact");
  assert(rightFootContact);

  // get some pointers for later
  theLeftFootDmABForKinStruct = gSimulation->GetRobot()->getForKinStruct(leftFootLinkIndex);
  theRightFootDmABForKinStruct = gSimulation->GetRobot()->getForKinStruct(rightFootLinkIndex);
  theLeftThighDmABForKinStruct = gSimulation->GetRobot()->getForKinStruct(leftThighLinkIndex);
  theRightThighDmABForKinStruct = gSimulation->GetRobot()->getForKinStruct(rightThighLinkIndex);
  theLeftCalfDmABForKinStruct = gSimulation->GetRobot()->getForKinStruct(leftCalfLinkIndex);
  theRightCalfDmABForKinStruct = gSimulation->GetRobot()->getForKinStruct(rightCalfLinkIndex);
  theTorsoDmABForKinStruct = gSimulation->GetRobot()->getForKinStruct(torsoIndex);
  
  // load up the trajectory
  
  ifstream trajectoryFile("Kinematics_1_sec.txt");
  char buffer[256];
  double rightFootXTrajectory[51];
  double rightThighXTrajectory[51];
  double leftFootXTrajectory[51];
  double leftThighXTrajectory[51];
  trajectoryFile.getline(buffer, sizeof(buffer) - 1, '\n');
  for (i = 0; i < 51; i++)
  {
    SkipToken(trajectoryFile, 8);
    trajectoryFile >> rightFootXTrajectory[i];
    SkipToken(trajectoryFile, 9);
    trajectoryFile >> rightThighXTrajectory[i];
    SkipToken(trajectoryFile, 10);
    trajectoryFile >> leftFootXTrajectory[i];
    SkipToken(trajectoryFile, 9);
    trajectoryFile >> leftThighXTrajectory[i];
    SkipToken(trajectoryFile, 9);
  }
  trajectoryFile.close();
        
  // and run for required amount of time
  gErrorCode = 0;
  score = 0;
  runCount = 0;
  runLimit = 1; // run for 2 second max (enough for starup step and one whole step
        
  while (gSimulation->GetTime() < runLimit)
  {
    gSimulation->UpdateSimulation();
    if (gErrorCode)
      break;
    
    // interpolation values
    index = (int)(gSimulation->GetTime() / 0.02);
    interp = (gSimulation->GetTime() / 0.02) - (double)index;
    
    x = leftFootXTrajectory[index] + 
        interp * (leftFootXTrajectory[index + 1] - leftFootXTrajectory[index]);
    score += 1 / (ABS(theLeftFootDmABForKinStruct->p_ICS[XC] - x) + 0.1);
    x = rightFootXTrajectory[index] + 
        interp * (rightFootXTrajectory[index + 1] - rightFootXTrajectory[index]);
    score += 1 / (ABS(theRightFootDmABForKinStruct->p_ICS[XC] - x) + 0.1);
    
    x = leftThighXTrajectory[index] + 
        interp * (leftThighXTrajectory[index + 1] - leftThighXTrajectory[index]);
    score += 1 / (ABS(theLeftThighDmABForKinStruct->p_ICS[XC] - x) + 0.1);
    x = rightThighXTrajectory[index] + 
        interp * (rightThighXTrajectory[index + 1] - rightThighXTrajectory[index]);
    score += 1 / (ABS(theRightThighDmABForKinStruct->p_ICS[XC] - x) + 0.1);
    
    // use torso position as a sanity check
    if (theTorsoDmABForKinStruct->p_ICS[ZC] < 0.7 || 
      theTorsoDmABForKinStruct->p_ICS[ZC] > 1.5) 
      break;
   
    // keeping torso more or less upright adds to score
    score += UNIT_HEIGHT_GAUSSIAN(theTorsoDmABForKinStruct->p_ICS[XC],
      (theRightThighDmABForKinStruct->p_ICS[XC] + 
      theLeftThighDmABForKinStruct->p_ICS[XC]) / 2, 0.1) * gSimulation->GetTimeIncrement();
    
    GetContactLimits(leftFootContact, theLeftFootDmABForKinStruct, 
        xLeftFootMin, yLeftFootMin, zLeftFootMin, 
        xLeftFootMax, yLeftFootMax, zLeftFootMax);
    GetContactLimits(rightFootContact, theRightFootDmABForKinStruct, 
        xRightFootMin, yRightFootMin, zRightFootMin, 
        xRightFootMax, yRightFootMax, zRightFootMax);
    
    // if torso is behind the back foot then we will tip over backwards!
    if (theTorsoDmABForKinStruct->p_ICS[XC] < MIN(xLeftFootMin, xRightFootMin)) 
      break;
    
    // calculate energy driving system
    //totalPower = 0;
    totalForce = 0;
    for (i = 0; i < gSimulation->GetForceList()->GetSize(); i++)
    {
      // all forces in the list are actually derived from LoggedForce
      //power =
      //    ((LoggedForce*) 
      //    (gSimulation->GetForceList()->GetEntry(i)))->getPower();
      //if (power >= 0) // positive work
      //     totalPower += power;
      //else      // negative work -  a bit cheaper
      //     totalPower -= power * 0.5; 
      force =
          ((LoggedForce*) 
          (gSimulation->GetForceList()->GetEntry(i)))->getForce();
      totalForce += force;
    }
    //energy += totalPower * gSimulation->GetTimeIncrement();
    // removed because power calculation is wrong because velocity
    // calculation in muscle model are wrong because they need to be
    // converted to world coordinates
    // score += 0.1 / (totalPower + 1); 
    score += 1 / (totalForce + 1);
    if (gDebug > 0) cerr << "Fitness::CalculateFitness\ttotalForce\t" << totalForce << "\n";

    runCount++;
    if (m_KineticsFileName) OutputKinetics();
  }
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

      
      
