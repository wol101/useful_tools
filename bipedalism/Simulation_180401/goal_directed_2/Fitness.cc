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
  enum WalkingState
  {
    right_support_left_lift,
    right_support_left_down,
    left_support_right_lift,
    left_support_right_down
  };
  WalkingState state;  
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
  double lastTorsoMaxX;
  ModifiedContactModel *leftFootContact;
  ModifiedContactModel *rightFootContact;
  double a;
  double xLeftFootMin, yLeftFootMin, zLeftFootMin;
  double xLeftFootMax, yLeftFootMax, zLeftFootMax;
  double xRightFootMin, yRightFootMin, zRightFootMin;
  double xRightFootMax, yRightFootMax, zRightFootMax;
  //SpatialVector torsoWorldV;
  //SpatialVector leftFootWorldV;
  //SpatialVector rightFootWorldV;
  double lastLeftFootMaxX;
  double lastRightFootMaxX;
  double lastLeftThighMaxX;
  double lastRightThighMaxX;
  double lastLeftCalfMaxX;
  double lastRightCalfMaxX;
  double zStore;
  double maxTorsoDiff;
  double leftLimbLengthStore;
  double rightLimbLengthStore;
  double currentStrideOrigin;
  double currentStrideGoal;
  double minStrideLength;
  //SpatialVector leftContactForce;
  //SpatialVector rightContactForce;
  double runLimit;
  int i;
  //double power;
  //double energy;
  //double totalPower;
  double force;
  double totalForce;
  bool finish;
    
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

  // and run for required amount of time
  gErrorCode = 0;
  score = 0;
  runCount = 0;
  //energy = 0;
  
  // these variables are highly dependent on the start configuration
  state = right_support_left_down; // this seems to be the correct starting point
  lastTorsoMaxX = 0;
  lastLeftFootMaxX = 0;
  lastRightFootMaxX = 0;
  lastLeftThighMaxX = 0;
  lastRightThighMaxX = 0;
  lastLeftCalfMaxX = 0;
  lastRightCalfMaxX = 0;
  zStore = 0.3;
  maxTorsoDiff = -1;
  currentStrideOrigin = 5.0;
  minStrideLength = 0.3;
  currentStrideGoal = currentStrideOrigin + minStrideLength;
  runLimit = 5; // run for 1 second max  
  
  leftLimbLengthStore = Util::Distance3x1(theLeftThighDmABForKinStruct->p_ICS, 
      theLeftFootDmABForKinStruct->p_ICS);
  rightLimbLengthStore = Util::Distance3x1(theRightThighDmABForKinStruct->p_ICS, 
      theRightFootDmABForKinStruct->p_ICS);
      
  while (gSimulation->GetTime() < runLimit)
  {
    gSimulation->UpdateSimulation();
    if (gErrorCode)
      break;

    // get some world velocities
    // ConvertLocalToWorldV(theLeftFootDmABForKinStruct, theLeftFootDmABForKinStruct->v, leftFootWorldV);
    // ConvertLocalToWorldV(theRightFootDmABForKinStruct, theRightFootDmABForKinStruct->v, rightFootWorldV);
    // ConvertLocalToWorldV(theTorsoDmABForKinStruct, theTorsoDmABForKinStruct->v, torsoWorldV);
    
    // use torso position as a sanity check
    if (theTorsoDmABForKinStruct->p_ICS[ZC] < 0.8 || 
      theTorsoDmABForKinStruct->p_ICS[ZC] > 1.5) 
      break;
    //if (SanityCheck(leftFootWorldV)) 
    //  break;
    //if (SanityCheck(rightFootWorldV)) 
    //  break;
    //if (SanityCheck(torsoWorldV)) 
    //  break;
    
    // torso moving forward adds to score
    score += theTorsoDmABForKinStruct->p_ICS[XC] - lastTorsoMaxX;
    if (gDebug > 0) cerr << "Fitness::CalculateFitness\ttheTorsoDmABForKinStruct->p_ICS[XC]\t" <<
      theTorsoDmABForKinStruct->p_ICS[XC] << "\tlastTorsoMaxX\t" <<
      lastTorsoMaxX << "\n";

    // thighs moving forward adds to score
    score += theLeftThighDmABForKinStruct->p_ICS[XC] - lastLeftThighMaxX;
    score += theRightThighDmABForKinStruct->p_ICS[XC] - lastRightThighMaxX;
    if (gDebug > 0) cerr << "Fitness::CalculateFitness\ttheLeftThighDmABForKinStruct->p_ICS[XC]\t" <<
      theLeftThighDmABForKinStruct->p_ICS[XC] << "\tlastLeftThighMaxX\t" <<
      lastLeftThighMaxX << "\ttheRightThighDmABForKinStruct->p_ICS[XC]\t" <<
      theRightThighDmABForKinStruct->p_ICS[XC] << "\tlastRightThighMaxX\t" <<
      lastRightThighMaxX << "\n";

    // calves moving forward adds to score
    score += theLeftCalfDmABForKinStruct->p_ICS[XC] - lastLeftCalfMaxX;
    score += theRightCalfDmABForKinStruct->p_ICS[XC] - lastRightCalfMaxX;
    if (gDebug > 0) cerr << "Fitness::CalculateFitness\ttheLeftCalfDmABForKinStruct->p_ICS[XC]\t" <<
      theLeftCalfDmABForKinStruct->p_ICS[XC] << "\tlastLeftCalfMaxX\t" <<
      lastLeftCalfMaxX << "\ttheRightCalfDmABForKinStruct->p_ICS[XC]\t" <<
      theRightCalfDmABForKinStruct->p_ICS[XC] << "\tlastRightCalfMaxX\t" <<
      lastRightCalfMaxX << "\n";

    if (gDebug > 0) cerr << "Fitness::CalculateFitness\ttheLeftFootDmABForKinStruct->p_ICS[XC]\t" <<
      theLeftFootDmABForKinStruct->p_ICS[XC] << "\tlastLeftFootMaxX\t" <<
      lastLeftFootMaxX << "\ttheRightFootDmABForKinStruct->p_ICS[XC]\t" <<
      theRightFootDmABForKinStruct->p_ICS[XC] << "\tlastRightFootMaxX\t" <<
      lastRightFootMaxX << "\n";
   
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
    score += 1 / (totalForce + 1);
    if (gDebug > 0) cerr << "Fitness::CalculateFitness\ttotalForce\t" << totalForce << "\n";

    // penalty for dual support
    
    // if (leftFootContact->AnyContact() && rightFootContact->AnyContact())
    //  score -= gSimulation->GetTimeIncrement();
    
    // use a set of states for fitness criteria

    if (gDebug > 0) cerr << "Fitness::CalculateFitness\tcurrentStrideOrigin\t" <<
      currentStrideOrigin << "\tcurrentStrideGoal\t" << currentStrideGoal << "\n";
    if (gDebug > 0) cerr << "Fitness::CalculateFitness\tmaxTorsoDiff\t" <<
      maxTorsoDiff << "\tzStore\t" << zStore << "\n";
    if (gDebug > 0) cerr << "Fitness::CalculateFitness\trightLimbLengthStore\t" <<
      rightLimbLengthStore << "\tleftLimbLengthStore\t" << leftLimbLengthStore << "\n";
    
    finish = false;
    switch (state)
    {
      case right_support_left_down:
        
      if (gDebug > 0) cerr << "Fitness::CalculateFitness\tstate\tright_support_left_down\n";
      
      // change state conditions
      if (theLeftFootDmABForKinStruct->p_ICS[XC] > currentStrideGoal && 
        leftFootContact->AnyContact())
      {
        state = left_support_right_lift;
        score += 5;
        score += theTorsoDmABForKinStruct->p_ICS[XC];
        zStore = theLeftFootDmABForKinStruct->p_ICS[ZC];
        maxTorsoDiff = -1;
        rightLimbLengthStore = Util::Distance3x1(theRightThighDmABForKinStruct->p_ICS, 
          theRightFootDmABForKinStruct->p_ICS);
        currentStrideOrigin = theLeftFootDmABForKinStruct->p_ICS[XC];  
        currentStrideGoal = currentStrideOrigin + minStrideLength;  

        break;
      }
      
      // left foot moving forward
      score += theLeftFootDmABForKinStruct->p_ICS[XC] - lastLeftFootMaxX;
      
      // left foot moving faster than torso
      score += theLeftFootDmABForKinStruct->p_ICS[XC] - theTorsoDmABForKinStruct->p_ICS[XC] - maxTorsoDiff;
      maxTorsoDiff = MAX(maxTorsoDiff, theLeftFootDmABForKinStruct->p_ICS[XC] - theTorsoDmABForKinStruct->p_ICS[XC]);

      // left foot moving down
      score += zStore - theLeftFootDmABForKinStruct->p_ICS[ZC];
      zStore = MIN(zStore, theLeftFootDmABForKinStruct->p_ICS[ZC]);
      
      // right foot not moving
      score += UNIT_HEIGHT_GAUSSIAN(theRightFootDmABForKinStruct->p_ICS[XC], currentStrideOrigin, 0.01) 
          * gSimulation->GetTimeIncrement();
      if (rightFootContact->AnyContact()) score += gSimulation->GetTimeIncrement();
      
      // touch too soon!
      if (leftFootContact->AnyContact()) finish = true;
      
      break;

      case left_support_right_lift:
        
      if (gDebug > 0) cerr << "Fitness::CalculateFitness\tstate\tleft_support_right_lift\n";
      
      // change state conditions
      if (theRightFootDmABForKinStruct->p_ICS[XC] > currentStrideOrigin)
      {
        state = left_support_right_down;
        score += 5;
        zStore = theRightFootDmABForKinStruct->p_ICS[ZC];
        break;
      }
      
      // right foot moving forward
      score += theRightFootDmABForKinStruct->p_ICS[XC] - lastRightFootMaxX;
      
      // right foot moving faster than torso
      score += theRightFootDmABForKinStruct->p_ICS[XC] - theTorsoDmABForKinStruct->p_ICS[XC] - maxTorsoDiff;
      maxTorsoDiff = MAX(maxTorsoDiff, theRightFootDmABForKinStruct->p_ICS[XC] - theTorsoDmABForKinStruct->p_ICS[XC]);

      // right foot moving up
      score += theRightFootDmABForKinStruct->p_ICS[ZC] - zStore;
      zStore = MAX(zStore, theRightFootDmABForKinStruct->p_ICS[ZC]);
      
      // get that back foot off the ground
      if (rightFootContact->AnyContact())
        score -= gSimulation->GetTimeIncrement();
      else
        score += gSimulation->GetTimeIncrement();
      
      // bend that knee
      a = Util::Distance3x1(theRightThighDmABForKinStruct->p_ICS, 
          theRightFootDmABForKinStruct->p_ICS);
      score += rightLimbLengthStore - a;
      rightLimbLengthStore = MIN(rightLimbLengthStore, a);
      
      // left foot not moving
      score += UNIT_HEIGHT_GAUSSIAN(theLeftFootDmABForKinStruct->p_ICS[XC], currentStrideOrigin, 0.01) 
          * gSimulation->GetTimeIncrement();
      if (leftFootContact->AnyContact()) score += gSimulation->GetTimeIncrement();
      
      break;

      case left_support_right_down:
        
      if (gDebug > 0) cerr << "Fitness::CalculateFitness\tstate\tleft_support_right_down\n";
      
      // change state conditions
      if (theRightFootDmABForKinStruct->p_ICS[XC] > currentStrideGoal && 
        rightFootContact->AnyContact())
      {
        state = right_support_left_lift;
        score += 5;
        score += theTorsoDmABForKinStruct->p_ICS[XC];
        zStore = theRightFootDmABForKinStruct->p_ICS[ZC];
        maxTorsoDiff = -1;
        leftLimbLengthStore = Util::Distance3x1(theLeftThighDmABForKinStruct->p_ICS, 
          theLeftFootDmABForKinStruct->p_ICS);
        currentStrideOrigin = theRightFootDmABForKinStruct->p_ICS[XC];  
        currentStrideGoal = currentStrideOrigin + minStrideLength;  
        break;
      }
      
      // right foot moving forward
      score += theRightFootDmABForKinStruct->p_ICS[XC] - lastRightFootMaxX;
      
      // right foot moving faster than torso
      score += theRightFootDmABForKinStruct->p_ICS[XC] - theTorsoDmABForKinStruct->p_ICS[XC] - maxTorsoDiff;
      maxTorsoDiff = MAX(maxTorsoDiff, theRightFootDmABForKinStruct->p_ICS[XC] - theTorsoDmABForKinStruct->p_ICS[XC]);

      // right foot moving down
      score += zStore - theRightFootDmABForKinStruct->p_ICS[ZC];
      zStore = MIN(zStore, theRightFootDmABForKinStruct->p_ICS[ZC]);
      
      // left foot not moving
      score += UNIT_HEIGHT_GAUSSIAN(theLeftFootDmABForKinStruct->p_ICS[XC], currentStrideOrigin, 0.01) 
          * gSimulation->GetTimeIncrement();
      if (leftFootContact->AnyContact()) score += gSimulation->GetTimeIncrement();
      
      // touch too soon!
      if (rightFootContact->AnyContact()) finish = true;
      
      break;

      case right_support_left_lift:
        
      if (gDebug > 0) cerr << "Fitness::CalculateFitness\tstate\tright_support_left_lift\n";
      
      // change state conditions
      if (theLeftFootDmABForKinStruct->p_ICS[XC] > currentStrideOrigin)
      {
        state = right_support_left_down;
        score += 5;
        zStore = theLeftFootDmABForKinStruct->p_ICS[ZC];
        break;
      }
      
      // left foot moving forward
      score += theLeftFootDmABForKinStruct->p_ICS[XC] - lastLeftFootMaxX;
      
      // left foot moving faster than torso
      score += theLeftFootDmABForKinStruct->p_ICS[XC] - theTorsoDmABForKinStruct->p_ICS[XC] - maxTorsoDiff;
      maxTorsoDiff = MAX(maxTorsoDiff, theLeftFootDmABForKinStruct->p_ICS[XC] - theTorsoDmABForKinStruct->p_ICS[XC]);

      // left foot moving up
      score += theLeftFootDmABForKinStruct->p_ICS[ZC] - zStore;
      zStore = MAX(zStore, theLeftFootDmABForKinStruct->p_ICS[ZC]);
      
      // get that back foot off the ground
      if (leftFootContact->AnyContact())
        score -= gSimulation->GetTimeIncrement();
      else
        score += gSimulation->GetTimeIncrement();
      
      // bend that knee
      a = Util::Distance3x1(theLeftThighDmABForKinStruct->p_ICS, 
          theLeftFootDmABForKinStruct->p_ICS);
      score += leftLimbLengthStore - a;
      leftLimbLengthStore = MIN(leftLimbLengthStore, a);
      
      // right foot not moving
      score += UNIT_HEIGHT_GAUSSIAN(theRightFootDmABForKinStruct->p_ICS[XC], currentStrideOrigin, 0.01) 
          * gSimulation->GetTimeIncrement();
      if (rightFootContact->AnyContact()) score += gSimulation->GetTimeIncrement();
      
      break;

    }
    if (finish) break;
        
    lastTorsoMaxX = MAX(lastTorsoMaxX, theTorsoDmABForKinStruct->p_ICS[XC]);
    lastLeftFootMaxX = MAX(lastLeftFootMaxX, theLeftFootDmABForKinStruct->p_ICS[XC]);
    lastRightFootMaxX = MAX(lastRightFootMaxX, theRightFootDmABForKinStruct->p_ICS[XC]);
    lastLeftThighMaxX = MAX(lastLeftThighMaxX, theLeftThighDmABForKinStruct->p_ICS[XC]);
    lastRightThighMaxX = MAX(lastRightThighMaxX, theRightThighDmABForKinStruct->p_ICS[XC]);
    lastLeftCalfMaxX = MAX(lastLeftCalfMaxX, theLeftCalfDmABForKinStruct->p_ICS[XC]);
    lastRightCalfMaxX = MAX(lastRightCalfMaxX, theRightCalfDmABForKinStruct->p_ICS[XC]);

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

      
      
