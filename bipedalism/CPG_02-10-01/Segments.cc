// Segments.cc - the various segment objects

#include <string.h>
#include <glLoadModels.h>

#include "Segments.h"
#include "Simulation.h"
#include "ModifiedContactModel.h"
#include "ParameterFile.h"
#include "MAMuscle.h"
#include "StrapForceAnchor.h"
#include "DebugControl.h"
#include "Util.h"

// Note having the single load routine and hundreds of globals is not
// a good way of doing things

// the globals are declare here
#define SEG_PARAM_EXTERN 
#include "SegmentParameters.h"

extern Simulation *gSimulation;
// axes
extern int gAxisFlag;
extern float gAxisSize;

static void DrawAxes();

// Torso defined here

Torso::Torso():
  dmMobileBaseLink()
{
  // set the link name
  setName(kTorsoPartName);
  
  // load up a graphics model if required
  if (gSimulation->UseGraphics())
  {
    GLuint *dlist = new GLuint;
    *dlist = glLoadModel(kTorsoGraphicFile);
    setUserData((void *) dlist);
  }

  setInertiaParameters(kTorsoMass, kTorsoMOI, kTorsoCG);
  setState(kTorsoPosition, kTorsoVelocity);
  
  // add the muscle forces
  
  StrapForceAnchor *strapForceAnchor;
  MAMuscle *strapForce;
        
  // Left Hip Extensor
  
  strapForce = gSimulation->GetCPG()->GetLeftHipExtensor();
  strapForce->SetNumAnchors(2);
  strapForce->setName(kLeftHipExtensorName);
  strapForce->SetVMax(kLeftHipExtensorLength, kDefaultMuscleVMaxFactor);
  strapForce->SetF0(kLeftHipExtensorPCA, kDefaultMuscleForcePerUnitArea);
  strapForce->SetK(kDefaultMuscleActivationK);
  
  strapForceAnchor = new StrapForceAnchor();
  strapForceAnchor->setName(strcat(kLeftHipExtensorName, "Origin"));
  strapForceAnchor->SetStrapForce(this, kLeftHipExtensorOrigin, strapForce, 0);  
  addForce(strapForceAnchor);
  
  // Left Hip Flexor
  strapForce = gSimulation->GetCPG()->GetLeftHipFlexor();
  strapForce->SetNumAnchors(2);
  strapForce->setName(kLeftHipFlexorName);
  strapForce->SetVMax(kLeftHipFlexorLength, kDefaultMuscleVMaxFactor);
  strapForce->SetF0(kLeftHipFlexorPCA, kDefaultMuscleForcePerUnitArea);
  strapForce->SetK(kDefaultMuscleActivationK);
  
  strapForceAnchor = new StrapForceAnchor();
  strapForceAnchor->setName(strcat(kLeftHipFlexorName, "Origin"));
  strapForceAnchor->SetStrapForce(this, kLeftHipFlexorOrigin, strapForce, 0);  
  addForce(strapForceAnchor);
  
  // Right Hip Extensor
  strapForce = gSimulation->GetCPG()->GetRightHipExtensor();
  strapForce->SetNumAnchors(2);
  strapForce->setName(kRightHipExtensorName);
  strapForce->SetVMax(kRightHipExtensorLength, kDefaultMuscleVMaxFactor);
  strapForce->SetF0(kRightHipExtensorPCA, kDefaultMuscleForcePerUnitArea);
  strapForce->SetK(kDefaultMuscleActivationK);
  
  strapForceAnchor = new StrapForceAnchor();
  strapForceAnchor->setName(strcat(kRightHipExtensorName, "Origin"));
  strapForceAnchor->SetStrapForce(this, kRightHipExtensorOrigin, strapForce, 0);  
  addForce(strapForceAnchor);
  
  // Right Hip Flexor
  strapForce = gSimulation->GetCPG()->GetRightHipFlexor();
  strapForce->SetNumAnchors(2);
  strapForce->setName(kRightHipFlexorName);
  strapForce->SetVMax(kRightHipFlexorLength, kDefaultMuscleVMaxFactor);
  strapForce->SetF0(kRightHipFlexorPCA, kDefaultMuscleForcePerUnitArea);
  strapForce->SetK(kDefaultMuscleActivationK);
  
  strapForceAnchor = new StrapForceAnchor();
  strapForceAnchor->setName(strcat(kRightHipFlexorName, "Origin"));
  strapForceAnchor->SetStrapForce(this, kRightHipFlexorOrigin, strapForce, 0);  
  addForce(strapForceAnchor);
  
  if (gDebug == SegmentsDebug)
  {
     cerr << "Torso::Torso()\tgetName()\t" << getName() << "\n";
  }
  
}

Torso::~Torso()
{
}

void
Torso::draw() const
{
   dmMobileBaseLink::draw();
   DrawAxes();
}

// Left Limb Segments

LeftThigh::LeftThigh():
  dmRevoluteLink()
{
  // set the link name
  setName(kLeftThighPartName);
  
  // load up a graphics model if required
  if (gSimulation->UseGraphics())
  {
    GLuint *dlist = new GLuint;
    *dlist = glLoadModel(kLeftThighGraphicFile);
    setUserData((void *) dlist);
  }

  setInertiaParameters(kLeftThighMass, kLeftThighMOI, kLeftThighCG);
  setMDHParameters(kLeftThighMDHA, kLeftThighMDHAlpha, kLeftThighMDHD, kLeftThighMDHTheta);
  double q, qd;
  getState(&q, &qd);
  qd = kLeftThighInitialJointVelocity;
  setState(&q, &qd);
  setJointLimits(kLeftThighJointMin, kLeftThighJointMax, 
      kLeftThighJointLimitSpringConstant, kLeftThighJointLimitDamperConstant);
  setJointFriction(kLeftThighJointFriction);

  // add the muscle forces
  
  StrapForceAnchor *strapForceAnchor;
  MAMuscle *strapForce;
        
  // Left Hip Extensor
  
  strapForce = gSimulation->GetCPG()->GetLeftHipExtensor();
  
  strapForceAnchor = new StrapForceAnchor();
  strapForceAnchor->setName(strcat(kLeftHipExtensorName, "Insertion"));
  strapForceAnchor->SetStrapForce(this, kLeftHipExtensorInsertion, strapForce, 1);  
  addForce(strapForceAnchor);
  
  // Left Hip Flexor
  strapForce = gSimulation->GetCPG()->GetLeftHipFlexor();
  
  strapForceAnchor = new StrapForceAnchor();
  strapForceAnchor->setName(strcat(kLeftHipFlexorName, "Insertion"));
  strapForceAnchor->SetStrapForce(this, kLeftHipFlexorInsertion, strapForce, 1);  
  addForce(strapForceAnchor);
  
  // Left Knee Extensor
  strapForce = gSimulation->GetCPG()->GetLeftKneeExtensor();
  strapForce->SetNumAnchors(3);
  strapForce->setName(kLeftKneeExtensorName);
  strapForce->SetVMax(kLeftKneeExtensorLength, kDefaultMuscleVMaxFactor);
  strapForce->SetF0(kLeftKneeExtensorPCA, kDefaultMuscleForcePerUnitArea);
  strapForce->SetK(kDefaultMuscleActivationK);
  
  strapForceAnchor = new StrapForceAnchor();
  strapForceAnchor->setName(strcat(kLeftKneeExtensorName, "Origin"));
  strapForceAnchor->SetStrapForce(this, kLeftKneeExtensorOrigin, strapForce, 0);  
  addForce(strapForceAnchor);
  
  strapForceAnchor = new StrapForceAnchor();
  strapForceAnchor->setName(strcat(kLeftKneeExtensorName, "MidPoint"));
  strapForceAnchor->SetStrapForce(this, kLeftKneeExtensorMidPoint, strapForce, 1);  
  addForce(strapForceAnchor);
  
  // Left Knee Flexor
  strapForce = gSimulation->GetCPG()->GetLeftKneeFlexor();
  strapForce->SetNumAnchors(3);
  strapForce->setName(kLeftKneeFlexorName);
  strapForce->SetVMax(kLeftKneeFlexorLength, kDefaultMuscleVMaxFactor);
  strapForce->SetF0(kLeftKneeFlexorPCA, kDefaultMuscleForcePerUnitArea);
  strapForce->SetK(kDefaultMuscleActivationK);
  
  strapForceAnchor = new StrapForceAnchor();
  strapForceAnchor->setName(strcat(kLeftKneeFlexorName, "Origin"));
  strapForceAnchor->SetStrapForce(this, kLeftKneeFlexorOrigin, strapForce, 0);  
  addForce(strapForceAnchor);
  
  strapForceAnchor = new StrapForceAnchor();
  strapForceAnchor->setName(strcat(kLeftKneeFlexorName, "MidPoint"));
  strapForceAnchor->SetStrapForce(this, kLeftKneeFlexorMidPoint, strapForce, 1);  
  addForce(strapForceAnchor);
  
  if (gDebug == SegmentsDebug)
  {
     cerr << "LeftThigh::LeftThigh()\tgetName()\t" << getName() << "\n";
  }
  
}

LeftThigh::~LeftThigh()
{
}

void
LeftThigh::UpdateMidpoint()
{
  double a, alpha, d, theta;
  CartesianVector location;
        
  // Left Knee Extensor
  Util::Copy3x1(kLeftKneeExtensorMidPoint, location);
  
  // calculate amount of rotation needed
  // (half the rotation from the start position)
  gSimulation->GetLeftLeg()->getMDHParameters(&a, &alpha, &d, &theta);
  theta = kLeftLegMDHTheta + ((theta - kLeftLegMDHTheta) / 2);
  Util::MDHTransform(a, alpha, d, theta, location);
  gSimulation->GetCPG()->GetLeftKneeExtensor()->SetAnchorLocation(1, location);

  if (gDebug == SegmentsDebug)
  {
   cerr << "LeftThigh::UpdateMidpoint\ta\t" << a <<
      "\talpha\t" << alpha << "\td\t" << d << "\ttheta\t" << theta <<
      "\tExtensor location\t" << location[0] << "\t" << 
      location[1] << "\t" << location[2] << "\t";
  }
  
  // Left Knee Flexor
  Util::Copy3x1(kLeftKneeFlexorMidPoint, location);
  
  // calculate amount of rotation needed
  // (half the rotation from the start position)
  Util::MDHTransform(a, alpha, d, theta, location);
  gSimulation->GetCPG()->GetLeftKneeFlexor()->SetAnchorLocation(1, location);
  
  if (gDebug == SegmentsDebug)
  {
   cerr << "Flexor location\t" << location[0] << "\t" << 
      location[1] << "\t" << location[2] << "\n";
  }
}

void
LeftThigh::draw() const
{
   dmRevoluteLink::draw();
   DrawAxes();
}

LeftLeg::LeftLeg():
  dmRevoluteLink()
{
  // set the link name
  setName(kLeftLegPartName);
  
  // load up a graphics model if required
  if (gSimulation->UseGraphics())
  {
    GLuint *dlist = new GLuint;
    *dlist = glLoadModel(kLeftLegGraphicFile);
    setUserData((void *) dlist);
  }

  setInertiaParameters(kLeftLegMass, kLeftLegMOI, kLeftLegCG);
  setMDHParameters(kLeftLegMDHA, kLeftLegMDHAlpha, kLeftLegMDHD, kLeftLegMDHTheta);
  double q, qd;
  getState(&q, &qd);
  qd = kLeftLegInitialJointVelocity;
  setState(&q, &qd);
  setJointLimits(kLeftLegJointMin, kLeftLegJointMax, 
      kLeftLegJointLimitSpringConstant, kLeftLegJointLimitDamperConstant);
  setJointFriction(kLeftLegJointFriction);
  
  // add the muscle forces
  
  StrapForceAnchor *strapForceAnchor;
  MAMuscle *strapForce;
        
  // Left Knee Extensor
  
  strapForce = gSimulation->GetCPG()->GetLeftKneeExtensor();
  
  strapForceAnchor = new StrapForceAnchor();
  strapForceAnchor->setName(strcat(kLeftKneeExtensorName, "Insertion"));
  strapForceAnchor->SetStrapForce(this, kLeftKneeExtensorInsertion, strapForce, 2);  
  addForce(strapForceAnchor);
  
  // Left Knee Flexor
  strapForce = gSimulation->GetCPG()->GetLeftKneeFlexor();
  
  strapForceAnchor = new StrapForceAnchor();
  strapForceAnchor->setName(strcat(kLeftKneeFlexorName, "Insertion"));
  strapForceAnchor->SetStrapForce(this, kLeftKneeFlexorInsertion, strapForce, 2);  
  addForce(strapForceAnchor);
  
  // Left Ankle Extensor
  strapForce = gSimulation->GetCPG()->GetLeftAnkleExtensor();
  strapForce->SetNumAnchors(2);
  strapForce->setName(kLeftAnkleExtensorName);
  strapForce->SetVMax(kLeftAnkleExtensorLength, kDefaultMuscleVMaxFactor);
  strapForce->SetF0(kLeftAnkleExtensorPCA, kDefaultMuscleForcePerUnitArea);
  strapForce->SetK(kDefaultMuscleActivationK);
  
  strapForceAnchor = new StrapForceAnchor();
  strapForceAnchor->setName(strcat(kLeftAnkleExtensorName, "Origin"));
  strapForceAnchor->SetStrapForce(this, kLeftAnkleExtensorOrigin, strapForce, 0);  
  addForce(strapForceAnchor);
  
  // Left Ankle Flexor
  strapForce = gSimulation->GetCPG()->GetLeftAnkleFlexor();
  strapForce->SetNumAnchors(2);
  strapForce->setName(kLeftAnkleFlexorName);
  strapForce->SetVMax(kLeftAnkleFlexorLength, kDefaultMuscleVMaxFactor);
  strapForce->SetF0(kLeftAnkleFlexorPCA, kDefaultMuscleForcePerUnitArea);
  strapForce->SetK(kDefaultMuscleActivationK);
  
  strapForceAnchor = new StrapForceAnchor();
  strapForceAnchor->setName(strcat(kLeftAnkleFlexorName, "Origin"));
  strapForceAnchor->SetStrapForce(this, kLeftAnkleFlexorOrigin, strapForce, 0);  
  addForce(strapForceAnchor);
  
  if (gDebug == SegmentsDebug)
  {
     cerr << "LeftLeg::LeftLeg()\tgetName()\t" << getName() << "\n";
  }
  
}

LeftLeg::~LeftLeg()
{
}

void
LeftLeg::draw() const
{
   dmRevoluteLink::draw();
   DrawAxes();
}

LeftFoot::LeftFoot():
  dmRevoluteLink()
{
  // set the link name
  setName(kLeftFootPartName);
  
  // load up a graphics model if required
  if (gSimulation->UseGraphics())
  {
    GLuint *dlist = new GLuint;
    *dlist = glLoadModel(kLeftFootGraphicFile);
    setUserData((void *) dlist);
  }

  setInertiaParameters(kLeftFootMass, kLeftFootMOI, kLeftFootCG);
  setMDHParameters(kLeftFootMDHA, kLeftFootMDHAlpha, kLeftFootMDHD, kLeftFootMDHTheta);
  double q, qd;
  getState(&q, &qd);
  qd = kLeftFootInitialJointVelocity;
  setState(&q, &qd);
  setJointLimits(kLeftFootJointMin, kLeftFootJointMax, 
      kLeftFootJointLimitSpringConstant, kLeftFootJointLimitDamperConstant);
  setJointFriction(kLeftFootJointFriction);

  // add the muscle forces
  
  StrapForceAnchor *strapForceAnchor;
  MAMuscle *strapForce;
        
  // Left Ankle Extensor
  
  strapForce = gSimulation->GetCPG()->GetLeftAnkleExtensor();
  
  strapForceAnchor = new StrapForceAnchor();
  strapForceAnchor->setName(strcat(kLeftAnkleExtensorName, "Insertion"));
  strapForceAnchor->SetStrapForce(this, kLeftAnkleExtensorInsertion, strapForce, 1);  
  addForce(strapForceAnchor);
  
  // Left Ankle Flexor
  strapForce = gSimulation->GetCPG()->GetLeftAnkleFlexor();
  
  strapForceAnchor = new StrapForceAnchor();
  strapForceAnchor->setName(strcat(kLeftAnkleFlexorName, "Insertion"));
  strapForceAnchor->SetStrapForce(this, kLeftAnkleFlexorInsertion, strapForce, 1);  
  addForce(strapForceAnchor);
  
  // Contact Points
  ModifiedContactModel *contactModel = new ModifiedContactModel();
  contactModel->setContactPoints(kLeftFootNumContactPoints, kLeftFootContactPositions);
  contactModel->setName(kLeftFootContactName);
  addForce(contactModel);

  if (gDebug == SegmentsDebug)
  {
     cerr << "LeftFoot::LeftFoot()\tgetName()\t" << getName() << "\n";
  }
  
}

LeftFoot::~LeftFoot()
{
}

void
LeftFoot::draw() const
{
   dmRevoluteLink::draw();
   DrawAxes();
}

// Right Limb Segments - these are just the left limb segements
// with a global replace of "Left" to "Right"

RightThigh::RightThigh():
  dmRevoluteLink()
{
  // set the link name
  setName(kRightThighPartName);
  
  // load up a graphics model if required
  if (gSimulation->UseGraphics())
  {
    GLuint *dlist = new GLuint;
    *dlist = glLoadModel(kRightThighGraphicFile);
    setUserData((void *) dlist);
  }

  setInertiaParameters(kRightThighMass, kRightThighMOI, kRightThighCG);
  setMDHParameters(kRightThighMDHA, kRightThighMDHAlpha, kRightThighMDHD, kRightThighMDHTheta);
  double q, qd;
  getState(&q, &qd);
  qd = kRightThighInitialJointVelocity;
  setState(&q, &qd);
  setJointLimits(kRightThighJointMin, kRightThighJointMax, 
      kRightThighJointLimitSpringConstant, kRightThighJointLimitDamperConstant);
  setJointFriction(kRightThighJointFriction);

  // add the muscle forces
  
  StrapForceAnchor *strapForceAnchor;
  MAMuscle *strapForce;
        
  // Right Hip Extensor
  
  strapForce = gSimulation->GetCPG()->GetRightHipExtensor();
  
  strapForceAnchor = new StrapForceAnchor();
  strapForceAnchor->setName(strcat(kRightHipExtensorName, "Insertion"));
  strapForceAnchor->SetStrapForce(this, kRightHipExtensorInsertion, strapForce, 1);  
  addForce(strapForceAnchor);
  
  // Right Hip Flexor
  strapForce = gSimulation->GetCPG()->GetRightHipFlexor();
  
  strapForceAnchor = new StrapForceAnchor();
  strapForceAnchor->setName(strcat(kRightHipFlexorName, "Insertion"));
  strapForceAnchor->SetStrapForce(this, kRightHipFlexorInsertion, strapForce, 1);  
  addForce(strapForceAnchor);
  
  // Right Knee Extensor
  strapForce = gSimulation->GetCPG()->GetRightKneeExtensor();
  strapForce->SetNumAnchors(3);
  strapForce->setName(kRightKneeExtensorName);
  strapForce->SetVMax(kRightKneeExtensorLength, kDefaultMuscleVMaxFactor);
  strapForce->SetF0(kRightKneeExtensorPCA, kDefaultMuscleForcePerUnitArea);
  strapForce->SetK(kDefaultMuscleActivationK);
  
  strapForceAnchor = new StrapForceAnchor();
  strapForceAnchor->setName(strcat(kRightKneeExtensorName, "Origin"));
  strapForceAnchor->SetStrapForce(this, kRightKneeExtensorOrigin, strapForce, 0);  
  addForce(strapForceAnchor);
  
  strapForceAnchor = new StrapForceAnchor();
  strapForceAnchor->setName(strcat(kRightKneeExtensorName, "MidPoint"));
  strapForceAnchor->SetStrapForce(this, kRightKneeExtensorMidPoint, strapForce, 1);  
  addForce(strapForceAnchor);
  
  // Right Knee Flexor
  strapForce = gSimulation->GetCPG()->GetRightKneeFlexor();
  strapForce->SetNumAnchors(3);
  strapForce->setName(kRightKneeFlexorName);
  strapForce->SetVMax(kRightKneeFlexorLength, kDefaultMuscleVMaxFactor);
  strapForce->SetF0(kRightKneeFlexorPCA, kDefaultMuscleForcePerUnitArea);
  strapForce->SetK(kDefaultMuscleActivationK);
  
  strapForceAnchor = new StrapForceAnchor();
  strapForceAnchor->setName(strcat(kRightKneeFlexorName, "Origin"));
  strapForceAnchor->SetStrapForce(this, kRightKneeFlexorOrigin, strapForce, 0);  
  addForce(strapForceAnchor);
  
  strapForceAnchor = new StrapForceAnchor();
  strapForceAnchor->setName(strcat(kRightKneeFlexorName, "MidPoint"));
  strapForceAnchor->SetStrapForce(this, kRightKneeFlexorMidPoint, strapForce, 1);  
  addForce(strapForceAnchor);
  
  if (gDebug == SegmentsDebug)
  {
     cerr << "RightThigh::RightThigh()\tgetName()\t" << getName() << "\n";
  }
  
}

RightThigh::~RightThigh()
{
}

void
RightThigh::UpdateMidpoint()
{
  double a, alpha, d, theta;
  CartesianVector location;
        
  // Right Knee Extensor
  Util::Copy3x1(kRightKneeExtensorMidPoint, location);
  
  // calculate amount of rotation needed
  // (half the rotation from the start position)
  gSimulation->GetRightLeg()->getMDHParameters(&a, &alpha, &d, &theta);
  theta = kRightLegMDHTheta + ((theta - kRightLegMDHTheta) / 2);
  Util::MDHTransform(a, alpha, d, theta, location);
  gSimulation->GetCPG()->GetRightKneeExtensor()->SetAnchorLocation(1, location);

  if (gDebug == SegmentsDebug)
  {
   cerr << "RightThigh::UpdateMidpoint\ta\t" << a <<
      "\talpha\t" << alpha << "\td\t" << d << "\ttheta\t" << theta <<
      "\tExtensor location\t" << location[0] << "\t" << 
      location[1] << "\t" << location[2] << "\t";
  }
  
  // Right Knee Flexor
  Util::Copy3x1(kRightKneeFlexorMidPoint, location);
  
  // calculate amount of rotation needed
  // (half the rotation from the start position)
  Util::MDHTransform(a, alpha, d, theta, location);
  gSimulation->GetCPG()->GetRightKneeFlexor()->SetAnchorLocation(1, location);
  
  if (gDebug == SegmentsDebug)
  {
   cerr << "Flexor location\t" << location[0] << "\t" << 
      location[1] << "\t" << location[2] << "\n";
  }
}

void
RightThigh::draw() const
{
   dmRevoluteLink::draw();
   DrawAxes();
}

RightLeg::RightLeg():
  dmRevoluteLink()
{
  // set the link name
  setName(kRightLegPartName);
  
  // load up a graphics model if required
  if (gSimulation->UseGraphics())
  {
    GLuint *dlist = new GLuint;
    *dlist = glLoadModel(kRightLegGraphicFile);
    setUserData((void *) dlist);
  }

  setInertiaParameters(kRightLegMass, kRightLegMOI, kRightLegCG);
  setMDHParameters(kRightLegMDHA, kRightLegMDHAlpha, kRightLegMDHD, kRightLegMDHTheta);
  double q, qd;
  getState(&q, &qd);
  qd = kRightLegInitialJointVelocity;
  setState(&q, &qd);
  setJointLimits(kRightLegJointMin, kRightLegJointMax, 
      kRightLegJointLimitSpringConstant, kRightLegJointLimitDamperConstant);
  setJointFriction(kRightLegJointFriction);
  
  // add the muscle forces
  
  StrapForceAnchor *strapForceAnchor;
  MAMuscle *strapForce;
        
  // Right Knee Extensor
  
  strapForce = gSimulation->GetCPG()->GetRightKneeExtensor();
  
  strapForceAnchor = new StrapForceAnchor();
  strapForceAnchor->setName(strcat(kRightKneeExtensorName, "Insertion"));
  strapForceAnchor->SetStrapForce(this, kRightKneeExtensorInsertion, strapForce, 2);  
  addForce(strapForceAnchor);
  
  // Right Knee Flexor
  strapForce = gSimulation->GetCPG()->GetRightKneeFlexor();
  
  strapForceAnchor = new StrapForceAnchor();
  strapForceAnchor->setName(strcat(kRightKneeFlexorName, "Insertion"));
  strapForceAnchor->SetStrapForce(this, kRightKneeFlexorInsertion, strapForce, 2);  
  addForce(strapForceAnchor);
  
  // Right Ankle Extensor
  strapForce = gSimulation->GetCPG()->GetRightAnkleExtensor();
  strapForce->SetNumAnchors(2);
  strapForce->setName(kRightAnkleExtensorName);
  strapForce->SetVMax(kRightAnkleExtensorLength, kDefaultMuscleVMaxFactor);
  strapForce->SetF0(kRightAnkleExtensorPCA, kDefaultMuscleForcePerUnitArea);
  strapForce->SetK(kDefaultMuscleActivationK);
  
  strapForceAnchor = new StrapForceAnchor();
  strapForceAnchor->setName(strcat(kRightAnkleExtensorName, "Origin"));
  strapForceAnchor->SetStrapForce(this, kRightAnkleExtensorOrigin, strapForce, 0);  
  addForce(strapForceAnchor);
  
  // Right Ankle Flexor
  strapForce = gSimulation->GetCPG()->GetRightAnkleFlexor();
  strapForce->SetNumAnchors(2);
  strapForce->setName(kRightAnkleFlexorName);
  strapForce->SetVMax(kRightAnkleFlexorLength, kDefaultMuscleVMaxFactor);
  strapForce->SetF0(kRightAnkleFlexorPCA, kDefaultMuscleForcePerUnitArea);
  strapForce->SetK(kDefaultMuscleActivationK);
  
  strapForceAnchor = new StrapForceAnchor();
  strapForceAnchor->setName(strcat(kRightAnkleFlexorName, "Origin"));
  strapForceAnchor->SetStrapForce(this, kRightAnkleFlexorOrigin, strapForce, 0);  
  addForce(strapForceAnchor);
  
  if (gDebug == SegmentsDebug)
  {
     cerr << "RightLeg::RightLeg()\tgetName()\t" << getName() << "\n";
  }
  
}

RightLeg::~RightLeg()
{
}

void
RightLeg::draw() const
{
   dmRevoluteLink::draw();
   DrawAxes();
}

RightFoot::RightFoot():
  dmRevoluteLink()
{
  // set the link name
  setName(kRightFootPartName);
  
  // load up a graphics model if required
  if (gSimulation->UseGraphics())
  {
    GLuint *dlist = new GLuint;
    *dlist = glLoadModel(kRightFootGraphicFile);
    setUserData((void *) dlist);
  }

  setInertiaParameters(kRightFootMass, kRightFootMOI, kRightFootCG);
  setMDHParameters(kRightFootMDHA, kRightFootMDHAlpha, kRightFootMDHD, kRightFootMDHTheta);
  double q, qd;
  getState(&q, &qd);
  qd = kRightFootInitialJointVelocity;
  setState(&q, &qd);
  setJointLimits(kRightFootJointMin, kRightFootJointMax, 
      kRightFootJointLimitSpringConstant, kRightFootJointLimitDamperConstant);
  setJointFriction(kRightFootJointFriction);

  // add the muscle forces
  
  StrapForceAnchor *strapForceAnchor;
  MAMuscle *strapForce;
        
  // Right Ankle Extensor
  
  strapForce = gSimulation->GetCPG()->GetRightAnkleExtensor();
  
  strapForceAnchor = new StrapForceAnchor();
  strapForceAnchor->setName(strcat(kRightAnkleExtensorName, "Insertion"));
  strapForceAnchor->SetStrapForce(this, kRightAnkleExtensorInsertion, strapForce, 1);  
  addForce(strapForceAnchor);
  
  // Right Ankle Flexor
  strapForce = gSimulation->GetCPG()->GetRightAnkleFlexor();
  
  strapForceAnchor = new StrapForceAnchor();
  strapForceAnchor->setName(strcat(kRightAnkleFlexorName, "Insertion"));
  strapForceAnchor->SetStrapForce(this, kRightAnkleFlexorInsertion, strapForce, 1);  
  addForce(strapForceAnchor);
  
  // Contact Points
  ModifiedContactModel *contactModel = new ModifiedContactModel();
  contactModel->setContactPoints(kRightFootNumContactPoints, kRightFootContactPositions);
  contactModel->setName(kRightFootContactName);
  addForce(contactModel);
  
  if (gDebug == SegmentsDebug)
  {
     cerr << "RightFoot::RightFoot()\tgetName()\t" << getName() << "\n";
  }
  
}

RightFoot::~RightFoot()
{
}

void
RightFoot::draw() const
{
   dmRevoluteLink::draw();
   DrawAxes();
}



void ReadSegmentGlobals(ParameterFile &file)
{
  // -------------------------------------------
  // Defaults
  // -------------------------------------------
  
  file.RetrieveParameter("kDefaultJointLimitSpringConstant", &kDefaultJointLimitSpringConstant);
  file.RetrieveParameter("kDefaultJointLimitDamperConstant", &kDefaultJointLimitDamperConstant);
  file.RetrieveParameter("kDefaultJointFriction", &kDefaultJointFriction);
  file.RetrieveParameter("kDefaultMuscleActivationK", &kDefaultMuscleActivationK);
  file.RetrieveParameter("kDefaultMuscleForcePerUnitArea", &kDefaultMuscleForcePerUnitArea);
  file.RetrieveParameter("kDefaultMuscleVMaxFactor", &kDefaultMuscleVMaxFactor);

  // -------------------------------------------
  // Links
  // -------------------------------------------

  // -------------------------------------------
  // Torso
  file.RetrieveQuotedStringParameter("kTorsoPartName", kTorsoPartName, 256);
  file.RetrieveQuotedStringParameter("kTorsoGraphicFile", kTorsoGraphicFile, 256);
  file.RetrieveParameter("kTorsoMass", &kTorsoMass);
  file.RetrieveParameter("kTorsoMOI", 9, (double *)kTorsoMOI);
  file.RetrieveParameter("kTorsoCG", 3, kTorsoCG);
  file.RetrieveParameter("kTorsoPosition", 7, kTorsoPosition);
  file.RetrieveParameter("kTorsoVelocity", 6, kTorsoVelocity);
  
  // -------------------------------------------
  // Left Thigh
  file.RetrieveQuotedStringParameter("kLeftThighPartName", kLeftThighPartName, 256);
  file.RetrieveQuotedStringParameter("kLeftThighGraphicFile", kLeftThighGraphicFile, 256);
  file.RetrieveParameter("kLeftThighMass", &kLeftThighMass);
  file.RetrieveParameter("kLeftThighMOI", 9, (double *)kLeftThighMOI);
  file.RetrieveParameter("kLeftThighCG", 3, kLeftThighCG);
  file.RetrieveParameter("kLeftThighMDHA", &kLeftThighMDHA);
  file.RetrieveParameter("kLeftThighMDHAlpha", &kLeftThighMDHAlpha);
  file.RetrieveParameter("kLeftThighMDHD", &kLeftThighMDHD);
  file.RetrieveParameter("kLeftThighMDHTheta", &kLeftThighMDHTheta);
  file.RetrieveParameter("kLeftThighInitialJointVelocity", &kLeftThighInitialJointVelocity);
  file.RetrieveParameter("kLeftThighJointMin", &kLeftThighJointMin);
  file.RetrieveParameter("kLeftThighJointMax", &kLeftThighJointMax);
  file.RetrieveParameter("kLeftThighJointLimitSpringConstant", &kLeftThighJointLimitSpringConstant);
  if (kLeftThighJointLimitSpringConstant < 0) kLeftThighJointLimitSpringConstant = kDefaultJointLimitSpringConstant;
  file.RetrieveParameter("kLeftThighJointLimitDamperConstant", &kLeftThighJointLimitDamperConstant);
  if (kLeftThighJointLimitDamperConstant < 0) kLeftThighJointLimitDamperConstant = kDefaultJointLimitDamperConstant;
  file.RetrieveParameter("kLeftThighJointFriction", &kLeftThighJointFriction);
  if (kLeftThighJointFriction < 0) kLeftThighJointFriction = kDefaultJointFriction;

  // -------------------------------------------
  // Right Thigh
  file.RetrieveQuotedStringParameter("kRightThighPartName", kRightThighPartName, 256);
  file.RetrieveQuotedStringParameter("kRightThighGraphicFile", kRightThighGraphicFile, 256);
  file.RetrieveParameter("kRightThighMass", &kRightThighMass);
  file.RetrieveParameter("kRightThighMOI", 9, (double *)kRightThighMOI);
  file.RetrieveParameter("kRightThighCG", 3, kRightThighCG);
  file.RetrieveParameter("kRightThighMDHA", &kRightThighMDHA);
  file.RetrieveParameter("kRightThighMDHAlpha", &kRightThighMDHAlpha);
  file.RetrieveParameter("kRightThighMDHD", &kRightThighMDHD);
  file.RetrieveParameter("kRightThighMDHTheta", &kRightThighMDHTheta);
  file.RetrieveParameter("kRightThighInitialJointVelocity", &kRightThighInitialJointVelocity);
  file.RetrieveParameter("kRightThighJointMin", &kRightThighJointMin);
  file.RetrieveParameter("kRightThighJointMax", &kRightThighJointMax);
  file.RetrieveParameter("kRightThighJointLimitSpringConstant", &kRightThighJointLimitSpringConstant);
  if (kRightThighJointLimitSpringConstant < 0) kRightThighJointLimitSpringConstant = kDefaultJointLimitSpringConstant;
  file.RetrieveParameter("kRightThighJointLimitDamperConstant", &kRightThighJointLimitDamperConstant);
  if (kRightThighJointLimitDamperConstant < 0) kRightThighJointLimitDamperConstant = kDefaultJointLimitDamperConstant;
  file.RetrieveParameter("kRightThighJointFriction", &kRightThighJointFriction);
  if (kRightThighJointFriction < 0) kRightThighJointFriction = kDefaultJointFriction;

  // -------------------------------------------
  // Left Leg
  file.RetrieveQuotedStringParameter("kLeftLegPartName", kLeftLegPartName, 256);
  file.RetrieveQuotedStringParameter("kLeftLegGraphicFile", kLeftLegGraphicFile, 256);
  file.RetrieveParameter("kLeftLegMass", &kLeftLegMass);
  file.RetrieveParameter("kLeftLegMOI", 9, (double *)kLeftLegMOI);
  file.RetrieveParameter("kLeftLegCG", 3, kLeftLegCG);
  file.RetrieveParameter("kLeftLegMDHA", &kLeftLegMDHA);
  file.RetrieveParameter("kLeftLegMDHAlpha", &kLeftLegMDHAlpha);
  file.RetrieveParameter("kLeftLegMDHD", &kLeftLegMDHD);
  file.RetrieveParameter("kLeftLegMDHTheta", &kLeftLegMDHTheta);
  file.RetrieveParameter("kLeftLegInitialJointVelocity", &kLeftLegInitialJointVelocity);
  file.RetrieveParameter("kLeftLegJointMin", &kLeftLegJointMin);
  file.RetrieveParameter("kLeftLegJointMax", &kLeftLegJointMax);
  file.RetrieveParameter("kLeftLegJointLimitSpringConstant", &kLeftLegJointLimitSpringConstant);
  if (kLeftLegJointLimitSpringConstant < 0) kLeftLegJointLimitSpringConstant = kDefaultJointLimitSpringConstant;
  file.RetrieveParameter("kLeftLegJointLimitDamperConstant", &kLeftLegJointLimitDamperConstant);
  if (kLeftLegJointLimitDamperConstant < 0) kLeftLegJointLimitDamperConstant = kDefaultJointLimitDamperConstant;
  file.RetrieveParameter("kLeftLegJointFriction", &kLeftLegJointFriction);
  if (kLeftLegJointFriction < 0) kLeftLegJointFriction = kDefaultJointFriction;

  // -------------------------------------------
  // Right Leg
  file.RetrieveQuotedStringParameter("kRightLegPartName", kRightLegPartName, 256);
  file.RetrieveQuotedStringParameter("kRightLegGraphicFile", kRightLegGraphicFile, 256);
  file.RetrieveParameter("kRightLegMass", &kRightLegMass);
  file.RetrieveParameter("kRightLegMOI", 9, (double *)kRightLegMOI);
  file.RetrieveParameter("kRightLegCG", 3, kRightLegCG);
  file.RetrieveParameter("kRightLegMDHA", &kRightLegMDHA);
  file.RetrieveParameter("kRightLegMDHAlpha", &kRightLegMDHAlpha);
  file.RetrieveParameter("kRightLegMDHD", &kRightLegMDHD);
  file.RetrieveParameter("kRightLegMDHTheta", &kRightLegMDHTheta);
  file.RetrieveParameter("kRightLegInitialJointVelocity", &kRightLegInitialJointVelocity);
  file.RetrieveParameter("kRightLegJointMin", &kRightLegJointMin);
  file.RetrieveParameter("kRightLegJointMax", &kRightLegJointMax);
  file.RetrieveParameter("kRightLegJointLimitSpringConstant", &kRightLegJointLimitSpringConstant);
  if (kRightLegJointLimitSpringConstant < 0) kRightLegJointLimitSpringConstant = kDefaultJointLimitSpringConstant;
  file.RetrieveParameter("kRightLegJointLimitDamperConstant", &kRightLegJointLimitDamperConstant);
  if (kRightLegJointLimitDamperConstant < 0) kRightLegJointLimitDamperConstant = kDefaultJointLimitDamperConstant;
  file.RetrieveParameter("kRightLegJointFriction", &kRightLegJointFriction);
  if (kRightLegJointFriction < 0) kRightLegJointFriction = kDefaultJointFriction;

  // -------------------------------------------
  // Left Foot
  file.RetrieveQuotedStringParameter("kLeftFootPartName", kLeftFootPartName, 256);
  file.RetrieveQuotedStringParameter("kLeftFootGraphicFile", kLeftFootGraphicFile, 256);
  file.RetrieveParameter("kLeftFootMass", &kLeftFootMass);
  file.RetrieveParameter("kLeftFootMOI", 9, (double *)kLeftFootMOI);
  file.RetrieveParameter("kLeftFootCG", 3, kLeftFootCG);
  file.RetrieveParameter("kLeftFootMDHA", &kLeftFootMDHA);
  file.RetrieveParameter("kLeftFootMDHAlpha", &kLeftFootMDHAlpha);
  file.RetrieveParameter("kLeftFootMDHD", &kLeftFootMDHD);
  file.RetrieveParameter("kLeftFootMDHTheta", &kLeftFootMDHTheta);
  file.RetrieveParameter("kLeftFootInitialJointVelocity", &kLeftFootInitialJointVelocity);
  file.RetrieveParameter("kLeftFootJointMin", &kLeftFootJointMin);
  file.RetrieveParameter("kLeftFootJointMax", &kLeftFootJointMax);
  file.RetrieveParameter("kLeftFootJointLimitSpringConstant", &kLeftFootJointLimitSpringConstant);
  if (kLeftFootJointLimitSpringConstant < 0) kLeftFootJointLimitSpringConstant = kDefaultJointLimitSpringConstant;
  file.RetrieveParameter("kLeftFootJointLimitDamperConstant", &kLeftFootJointLimitDamperConstant);
  if (kLeftFootJointLimitDamperConstant < 0) kLeftFootJointLimitDamperConstant = kDefaultJointLimitDamperConstant;
  file.RetrieveParameter("kLeftFootJointFriction", &kLeftFootJointFriction);
  if (kLeftFootJointFriction < 0) kLeftFootJointFriction = kDefaultJointFriction;

  // -------------------------------------------
  // Right Foot
  file.RetrieveQuotedStringParameter("kRightFootPartName", kRightFootPartName, 256);
  file.RetrieveQuotedStringParameter("kRightFootGraphicFile", kRightFootGraphicFile, 256);
  file.RetrieveParameter("kRightFootMass", &kRightFootMass);
  file.RetrieveParameter("kRightFootMOI", 9, (double *)kRightFootMOI);
  file.RetrieveParameter("kRightFootCG", 3, kRightFootCG);
  file.RetrieveParameter("kRightFootMDHA", &kRightFootMDHA);
  file.RetrieveParameter("kRightFootMDHAlpha", &kRightFootMDHAlpha);
  file.RetrieveParameter("kRightFootMDHD", &kRightFootMDHD);
  file.RetrieveParameter("kRightFootMDHTheta", &kRightFootMDHTheta);
  file.RetrieveParameter("kRightFootInitialJointVelocity", &kRightFootInitialJointVelocity);
  file.RetrieveParameter("kRightFootJointMin", &kRightFootJointMin);
  file.RetrieveParameter("kRightFootJointMax", &kRightFootJointMax);
  file.RetrieveParameter("kRightFootJointLimitSpringConstant", &kRightFootJointLimitSpringConstant);
  if (kRightFootJointLimitSpringConstant < 0) kRightFootJointLimitSpringConstant = kDefaultJointLimitSpringConstant;
  file.RetrieveParameter("kRightFootJointLimitDamperConstant", &kRightFootJointLimitDamperConstant);
  if (kRightFootJointLimitDamperConstant < 0) kRightFootJointLimitDamperConstant = kDefaultJointLimitDamperConstant;
  file.RetrieveParameter("kRightFootJointFriction", &kRightFootJointFriction);
  if (kRightFootJointFriction < 0) kRightFootJointFriction = kDefaultJointFriction;

  // -------------------------------------------
  // Muscles
  // -------------------------------------------

  // Left Hip Extensor
  file.RetrieveQuotedStringParameter("kLeftHipExtensorName", kLeftHipExtensorName, 256);
  file.RetrieveParameter("kLeftHipExtensorOrigin", 3, kLeftHipExtensorOrigin);
  file.RetrieveParameter("kLeftHipExtensorInsertion", 3, kLeftHipExtensorInsertion);
  file.RetrieveParameter("kLeftHipExtensorPCA", &kLeftHipExtensorPCA);
  file.RetrieveParameter("kLeftHipExtensorLength", &kLeftHipExtensorLength);

  // Left Hip Flexor
  file.RetrieveQuotedStringParameter("kLeftHipFlexorName", kLeftHipFlexorName, 256);
  file.RetrieveParameter("kLeftHipFlexorOrigin", 3, kLeftHipFlexorOrigin);
  file.RetrieveParameter("kLeftHipFlexorInsertion", 3, kLeftHipFlexorInsertion);
  file.RetrieveParameter("kLeftHipFlexorPCA", &kLeftHipFlexorPCA);
  file.RetrieveParameter("kLeftHipFlexorLength", &kLeftHipFlexorLength);

  // Right Hip Extensor
  file.RetrieveQuotedStringParameter("kRightHipExtensorName", kRightHipExtensorName, 256);
  file.RetrieveParameter("kRightHipExtensorOrigin", 3, kRightHipExtensorOrigin);
  file.RetrieveParameter("kRightHipExtensorInsertion", 3, kRightHipExtensorInsertion);
  file.RetrieveParameter("kRightHipExtensorPCA", &kRightHipExtensorPCA);
  file.RetrieveParameter("kRightHipExtensorLength", &kRightHipExtensorLength);

  // Right Hip Flexor
  file.RetrieveQuotedStringParameter("kRightHipFlexorName", kRightHipFlexorName, 256);
  file.RetrieveParameter("kRightHipFlexorOrigin", 3, kRightHipFlexorOrigin);
  file.RetrieveParameter("kRightHipFlexorInsertion", 3, kRightHipFlexorInsertion);
  file.RetrieveParameter("kRightHipFlexorPCA", &kRightHipFlexorPCA);
  file.RetrieveParameter("kRightHipFlexorLength", &kRightHipFlexorLength);

  // Left Knee Extensor
  file.RetrieveQuotedStringParameter("kLeftKneeExtensorName", kLeftKneeExtensorName, 256);
  file.RetrieveParameter("kLeftKneeExtensorOrigin", 3, kLeftKneeExtensorOrigin);
  file.RetrieveParameter("kLeftKneeExtensorMidPoint", 3, kLeftKneeExtensorMidPoint);
  file.RetrieveParameter("kLeftKneeExtensorInsertion", 3, kLeftKneeExtensorInsertion);
  file.RetrieveParameter("kLeftKneeExtensorPCA", &kLeftKneeExtensorPCA);
  file.RetrieveParameter("kLeftKneeExtensorLength", &kLeftKneeExtensorLength);

  // Left Knee Flexor
  file.RetrieveQuotedStringParameter("kLeftKneeFlexorName", kLeftKneeFlexorName, 256);
  file.RetrieveParameter("kLeftKneeFlexorOrigin", 3, kLeftKneeFlexorOrigin);
  file.RetrieveParameter("kLeftKneeFlexorMidPoint", 3, kLeftKneeFlexorMidPoint);
  file.RetrieveParameter("kLeftKneeFlexorInsertion", 3, kLeftKneeFlexorInsertion);
  file.RetrieveParameter("kLeftKneeFlexorPCA", &kLeftKneeFlexorPCA);
  file.RetrieveParameter("kLeftKneeFlexorLength", &kLeftKneeFlexorLength);

  // Right Knee Extensor
  file.RetrieveQuotedStringParameter("kRightKneeExtensorName", kRightKneeExtensorName, 256);
  file.RetrieveParameter("kRightKneeExtensorOrigin", 3, kRightKneeExtensorOrigin);
  file.RetrieveParameter("kRightKneeExtensorMidPoint", 3, kRightKneeExtensorMidPoint);
  file.RetrieveParameter("kRightKneeExtensorInsertion", 3, kRightKneeExtensorInsertion);
  file.RetrieveParameter("kRightKneeExtensorPCA", &kRightKneeExtensorPCA);
  file.RetrieveParameter("kRightKneeExtensorLength", &kRightKneeExtensorLength);

  // Right Knee Flexor
  file.RetrieveQuotedStringParameter("kRightKneeFlexorName", kRightKneeFlexorName, 256);
  file.RetrieveParameter("kRightKneeFlexorOrigin", 3, kRightKneeFlexorOrigin);
  file.RetrieveParameter("kRightKneeFlexorMidPoint", 3, kRightKneeFlexorMidPoint);
  file.RetrieveParameter("kRightKneeFlexorInsertion", 3, kRightKneeFlexorInsertion);
  file.RetrieveParameter("kRightKneeFlexorPCA", &kRightKneeFlexorPCA);
  file.RetrieveParameter("kRightKneeFlexorLength", &kRightKneeFlexorLength);

  // Left Ankle Extensor
  file.RetrieveQuotedStringParameter("kLeftAnkleExtensorName", kLeftAnkleExtensorName, 256);
  file.RetrieveParameter("kLeftAnkleExtensorOrigin", 3, kLeftAnkleExtensorOrigin);
  file.RetrieveParameter("kLeftAnkleExtensorInsertion", 3, kLeftAnkleExtensorInsertion);
  file.RetrieveParameter("kLeftAnkleExtensorPCA", &kLeftAnkleExtensorPCA);
  file.RetrieveParameter("kLeftAnkleExtensorLength", &kLeftAnkleExtensorLength);

  // Left Ankle Flexor
  file.RetrieveQuotedStringParameter("kLeftAnkleFlexorName", kLeftAnkleFlexorName, 256);
  file.RetrieveParameter("kLeftAnkleFlexorOrigin", 3, kLeftAnkleFlexorOrigin);
  file.RetrieveParameter("kLeftAnkleFlexorInsertion", 3, kLeftAnkleFlexorInsertion);
  file.RetrieveParameter("kLeftAnkleFlexorPCA", &kLeftAnkleFlexorPCA);
  file.RetrieveParameter("kLeftAnkleFlexorLength", &kLeftAnkleFlexorLength);

  // Right Ankle Extensor
  file.RetrieveQuotedStringParameter("kRightAnkleExtensorName", kRightAnkleExtensorName, 256);
  file.RetrieveParameter("kRightAnkleExtensorOrigin", 3, kRightAnkleExtensorOrigin);
  file.RetrieveParameter("kRightAnkleExtensorInsertion", 3, kRightAnkleExtensorInsertion);
  file.RetrieveParameter("kRightAnkleExtensorPCA", &kRightAnkleExtensorPCA);
  file.RetrieveParameter("kRightAnkleExtensorLength", &kRightAnkleExtensorLength);

  // Right Ankle Flexor
  file.RetrieveQuotedStringParameter("kRightAnkleFlexorName", kRightAnkleFlexorName, 256);
  file.RetrieveParameter("kRightAnkleFlexorOrigin", 3, kRightAnkleFlexorOrigin);
  file.RetrieveParameter("kRightAnkleFlexorInsertion", 3, kRightAnkleFlexorInsertion);
  file.RetrieveParameter("kRightAnkleFlexorPCA", &kRightAnkleFlexorPCA);
  file.RetrieveParameter("kRightAnkleFlexorLength", &kRightAnkleFlexorLength);

  // -------------------------------------------
  // Contact Models
  // -------------------------------------------

  // Left Foot

  file.RetrieveQuotedStringParameter("kLeftFootContactName", kLeftFootContactName, 256);
  file.RetrieveParameter("kLeftFootNumContactPoints", &kLeftFootNumContactPoints);
  // WARNING - memory leak possible
  kLeftFootContactPositions = new CartesianVector[kLeftFootNumContactPoints];
  file.RetrieveParameter("kLeftFootContactPositions", kLeftFootNumContactPoints * 3, (double *)kLeftFootContactPositions);
  
  // Right Foot

  file.RetrieveQuotedStringParameter("kRightFootContactName", kRightFootContactName, 256);
  file.RetrieveParameter("kRightFootNumContactPoints", &kRightFootNumContactPoints);
  // WARNING - memory leak possible
  kRightFootContactPositions = new CartesianVector[kRightFootNumContactPoints];
  file.RetrieveParameter("kRightFootContactPositions", kRightFootNumContactPoints * 3, (double *)kRightFootContactPositions);

}

void
DrawAxes()
{
   if (gAxisFlag)
   {
      glDisable(GL_LIGHTING);

      glBegin(GL_LINES);
      glColor3f(1.0, 0.0, 0.0);
      glVertex3f(gAxisSize, 0.0, 0.0);
      glVertex3f(0.0, 0.0, 0.0);
      glEnd();

      glBegin(GL_LINES);
      glColor3f(0.0, 1.0, 0.0);
      glVertex3f(0.0, gAxisSize, 0.0);
      glVertex3f(0.0, 0.0, 0.0);
      glEnd();

      glBegin(GL_LINES);
      glColor3f(0.0, 0.0, 1.0);
      glVertex3f(0.0, 0.0, gAxisSize);
      glVertex3f(0.0, 0.0, 0.0);
      glEnd();

      glEnable(GL_LIGHTING);
   }
}

