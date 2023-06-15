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

// this function was just put here to test the velocity goal
// #define TEST_VELOCITY
#ifdef TEST_VELOCITY    
void test_velocity();
#endif

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
  double score;
  int runCount;
  dmLink *myLink;
  unsigned int leftFootLinkIndex, rightFootLinkIndex;
  unsigned int torsoIndex;
  const dmABForKinStruct *theLeftFootDmABForKinStruct;
  const dmABForKinStruct *theRightFootDmABForKinStruct;
  const dmABForKinStruct *theTorsoDmABForKinStruct;
  ModifiedContactModel *leftFootContact;
  ModifiedContactModel *rightFootContact;
  double xLeftFootMin, yLeftFootMin, zLeftFootMin;
  double xLeftFootMax, yLeftFootMax, zLeftFootMax;
  double xRightFootMin, yRightFootMin, zRightFootMin;
  double xRightFootMax, yRightFootMax, zRightFootMax;
  SpatialVector torsoWorldV;
  SpatialVector leftFootWorldV;
  SpatialVector rightFootWorldV;
  //SpatialVector leftContactForce;
  //SpatialVector rightContactForce;
  double runLimit;
  int i;
  //double power;
  //double energy;
  //double totalPower;
  double force;
  double totalForce;
  double gaussian;
  double error;
  double leftFootXV; 
  bool leftFootShouldContact;
  double rightFootXV; 
  bool rightFootShouldContact;
  double torsoV;        
  
  // get the indices of the feet
  myLink = (dmLink *) dmuFindObject((char *)kLeftFootPartName, gSimulation->GetRobot());
  assert(myLink);
  leftFootLinkIndex = gSimulation->GetRobot()->getLinkIndex(myLink);
  myLink = (dmLink *) dmuFindObject((char *)kRightFootPartName, gSimulation->GetRobot());
  assert(myLink);
  rightFootLinkIndex = gSimulation->GetRobot()->getLinkIndex(myLink);

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

  // get some pointers for later
  theLeftFootDmABForKinStruct = gSimulation->GetRobot()->getForKinStruct(leftFootLinkIndex);
  theRightFootDmABForKinStruct = gSimulation->GetRobot()->getForKinStruct(rightFootLinkIndex);
  theTorsoDmABForKinStruct = gSimulation->GetRobot()->getForKinStruct(torsoIndex);

  // and run for required amount of time
  gErrorCode = 0;
  score = 0;
  runCount = 0;
  // runLimit = 2.0; // run for 2.0 second max (1 full stride plus startup)
  // runLimit = 0.4; // run for 0.4 second max (startup only)
  runLimit = 1.6; // run for 1.6 second max (1 full stride with no startup)
        
  while (gSimulation->GetTime() < runLimit)
  {
    if (gDebug > 0) 
      cerr << "Fitness::CalculateFitness\tgSimulation->GetTime()\t" << gSimulation->GetTime() << "\n";

      gSimulation->UpdateSimulation();
    if (gErrorCode)
      break;

    // get some world velocities
    ConvertLocalToWorldV(theLeftFootDmABForKinStruct, theLeftFootDmABForKinStruct->v, leftFootWorldV);
    ConvertLocalToWorldV(theRightFootDmABForKinStruct, theRightFootDmABForKinStruct->v, rightFootWorldV);
    ConvertLocalToWorldV(theTorsoDmABForKinStruct, theTorsoDmABForKinStruct->v, torsoWorldV);
    
    // use torso position as a sanity check
    if (theTorsoDmABForKinStruct->p_ICS[ZC] < 0.5 || 
      theTorsoDmABForKinStruct->p_ICS[ZC] > 1.5) 
      break;
    //if (SanityCheck(leftFootWorldV)) 
    //  break;
    //if (SanityCheck(rightFootWorldV)) 
    //  break;
    //if (SanityCheck(torsoWorldV)) 
    //  break;
       
    // if torso is behind the back foot then we will tip over backwards!
    //if (theTorsoDmABForKinStruct->p_ICS[XC] < MIN(xLeftFootMin, xRightFootMin)) 
    //  break;

    // velocity goal scoring
    
    GoalXVelocity(gSimulation->GetTime(), 
      &leftFootXV, &leftFootShouldContact,
      &rightFootXV,&rightFootShouldContact,
      &torsoV);
    
    if (gDebug > 0) cerr << "Fitness::CalculateFitness" <<
      "\tleftFootXV\t" << leftFootXV << "\tleftFootShouldContact\t" << leftFootShouldContact <<
      "\trightFootXV\t" << rightFootXV << "\trightFootShouldContact\t" << rightFootShouldContact << 
      "\ttorsoV\t" << torsoV << "\n";
    if (gDebug > 0) cerr << "Fitness::CalculateFitness" <<
      "\tleftFootWorldV[3]\t" << leftFootWorldV[3] << 
      "\tleftFootContact->AnyContact()\t" << leftFootContact->AnyContact() <<
      "\trightFootWorldV[3]\t" << rightFootWorldV[3] << 
      "\trightFootContact->AnyContact()\t" << rightFootContact->AnyContact() << 
      "\ttorsoWorldV[3]\t" << torsoWorldV[3] << "\n";

    error = ABS(leftFootXV - leftFootWorldV[3]);
    score += 100 * gSimulation->GetTimeIncrement() / (error + 0.1);
    error = ABS(rightFootXV - rightFootWorldV[3]);
    score += 100 * gSimulation->GetTimeIncrement() / (error + 0.1);
    error = ABS(torsoV - torsoWorldV[3]);
    score += 100 * gSimulation->GetTimeIncrement() / (error + 0.1);
/*    if (leftFootShouldContact)
    {
      if (leftFootContact->AnyContact()) 
        score += gSimulation->GetTimeIncrement();
    }
    else
    {
      if (leftFootContact->AnyContact()) 
        score -= gSimulation->GetTimeIncrement();
    }
    if (rightFootShouldContact)
    {
      if (rightFootContact->AnyContact()) 
        score += gSimulation->GetTimeIncrement();
    }
    else
    {
      if (rightFootContact->AnyContact()) 
        score -= gSimulation->GetTimeIncrement();
    }
        // keeping torso more or less between the feet adds to score
    gaussian = UNIT_HEIGHT_GAUSSIAN(theTorsoDmABForKinStruct->p_ICS[XC],
      (theRightFootDmABForKinStruct->p_ICS[XC] + 
      theLeftFootDmABForKinStruct->p_ICS[XC]) / 2, 0.1) * gSimulation->GetTimeIncrement();
    score += gaussian;

    GetContactLimits(leftFootContact, theLeftFootDmABForKinStruct, 
        xLeftFootMin, yLeftFootMin, zLeftFootMin, 
        xLeftFootMax, yLeftFootMax, zLeftFootMax);
    GetContactLimits(rightFootContact, theRightFootDmABForKinStruct, 
        xRightFootMin, yRightFootMin, zRightFootMin, 
        xRightFootMax, yRightFootMax, zRightFootMax);

    // contact forces - NB local rather than global
    //leftFootContact->GetContactForce(leftContactForce);
    //rightFootContact->GetContactForce(rightContactForce);

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
    score += gSimulation->GetTimeIncrement() / (totalForce + 1);
    if (gDebug > 0) cerr << "Fitness::CalculateFitness\ttotalForce\t" << totalForce << "\n";
*/    
    runCount++;
    if (m_KineticsFileName) OutputKinetics();
  }
  if (gDebug > 0) 
    cerr << "Fitness::CalculateFitness\tscore\t" << score << "\n";
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

// calculate the goal velocity for any given time
// hardwired based on data from Vaughen, Davis, O'Connor, "Dynamics of Human Gait"
// (approximately of course :)

static double kFootPeakXV = 3.0; // foot peak velocity (m/s)
static double kTorsoMeanXV = 0.75; // torso mean velocity (m/s)
static double kStepTime = 0.8; // step time (half full stride time) (s)
// static double kStartStepTime = 0.4; // the start step - probably about half a normal step (s)
static double kStartStepTime = 0.0; // no start step (s)

void Fitness::GoalXVelocity(double theTime, 
    double *leftFootXV, bool *leftFootShouldContact,
    double *rightFootXV, bool *rightFootShouldContact,
    double *torsoV)
{
  double theTimeOffset;
  double stepNumber;
  
  if (theTime < kStartStepTime) // startup phase (left foot first)
  {
    if (theTime < (kStartStepTime / 2))
      *leftFootXV = theTime * kFootPeakXV / (kStartStepTime / 2);
    else
      *leftFootXV = (kStartStepTime - theTime) * kFootPeakXV / (kStartStepTime / 2);
    *leftFootShouldContact = false;
    *rightFootXV = 0;
    *rightFootShouldContact = true;
    *torsoV = theTime * kTorsoMeanXV / kStartStepTime;
  }
  else
  {
    stepNumber = floor((theTime - kStartStepTime) / (kStepTime * 2));
    if (stepNumber < 0) stepNumber = 0; // rounding error
    theTimeOffset = (theTime - kStartStepTime) - (stepNumber * (kStepTime * 2));

      if (theTimeOffset < kStepTime) // first full right step
    {
      if (theTimeOffset < (kStepTime / 2))
        *rightFootXV = theTimeOffset * kFootPeakXV / (kStepTime / 2);
      else
        *rightFootXV = (kStepTime - theTimeOffset) * kFootPeakXV / (kStepTime / 2);
      *rightFootShouldContact = false;
      *leftFootXV = 0;
      *leftFootShouldContact = true;
      *torsoV = kTorsoMeanXV;
    }
    else  
    {
      if (theTimeOffset < (kStepTime * 1.5))
        *leftFootXV = (theTimeOffset - kStepTime) * kFootPeakXV / (kStepTime / 2);
      else
        *leftFootXV = ((kStepTime * 2) - theTimeOffset) * kFootPeakXV / (kStepTime / 2);
      *leftFootShouldContact = false;
      *rightFootXV = 0;
      *rightFootShouldContact = true;
      *torsoV = kTorsoMeanXV;
    }
  }
  // some rounding error checking
  if (*leftFootXV < 0) *leftFootXV = 0;
  if (*rightFootXV < 0) *rightFootXV = 0;
}

#ifdef TEST_VELOCITY    
void test_velocity()
{
  ofstream o("test_velocity.txt");
  double theTime; 
  double leftFootXV; 
  bool leftFootShouldContact;
  double rightFootXV; 
  bool rightFootShouldContact;
  double torsoV;        

  o << "theTime\tleftFootXV\tleftFootShouldContact\trightFootXV\trightFootShouldContact\ttorsoV\n";
  for (theTime = 0; theTime < 5; theTime += 0.01)
  {
    Fitness::GoalXVelocity(theTime, 
      &leftFootXV, &leftFootShouldContact,
      &rightFootXV,&rightFootShouldContact,
      &torsoV);
    o << theTime << "\t" << leftFootXV << "\t" << leftFootShouldContact <<
      "\t" << rightFootXV << "\t" << rightFootShouldContact << "\t" <<
      torsoV << "\n";
  }
}
#endif       
