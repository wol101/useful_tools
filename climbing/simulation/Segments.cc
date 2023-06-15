// Segments.cc - the various segment objects

#include <string.h>
#include <gl.h>

#include "LoadObj.h"
#include "Segments.h"
#include "Simulation.h"
#include "LadderContactModel.h"
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
extern bool gAxisFlag;
extern double gAxisSize;

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
    *dlist = LoadObj(kTorsoGraphicFile);
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
  
  // Left Shoulder Abductor
  
  strapForce = gSimulation->GetCPG()->GetLeftShoulderAbductor();
  strapForce->SetNumAnchors(3);
  strapForce->setName(kLeftShoulderAbductorName);
  strapForce->SetVMax(kLeftShoulderAbductorLength, kDefaultMuscleVMaxFactor);
  strapForce->SetF0(kLeftShoulderAbductorPCA, kDefaultMuscleForcePerUnitArea);
  strapForce->SetK(kDefaultMuscleActivationK);
  
  strapForceAnchor = new StrapForceAnchor();
  strapForceAnchor->setName(strcat(kLeftShoulderAbductorName, "Origin"));
  strapForceAnchor->SetStrapForce(this, kLeftShoulderAbductorOrigin, strapForce, 0);  
  addForce(strapForceAnchor);
  
  strapForceAnchor = new StrapForceAnchor();
  strapForceAnchor->setName(strcat(kLeftShoulderAbductorName, "MidPoint"));
  strapForceAnchor->SetStrapForce(this, kLeftShoulderAbductorMidPoint, strapForce, 1);  
  addForce(strapForceAnchor);
  
  // Left Shoulder Extensor
  
  strapForce = gSimulation->GetCPG()->GetLeftShoulderExtensor();
  strapForce->SetNumAnchors(2);
  strapForce->setName(kLeftShoulderExtensorName);
  strapForce->SetVMax(kLeftShoulderExtensorLength, kDefaultMuscleVMaxFactor);
  strapForce->SetF0(kLeftShoulderExtensorPCA, kDefaultMuscleForcePerUnitArea);
  strapForce->SetK(kDefaultMuscleActivationK);
  
  strapForceAnchor = new StrapForceAnchor();
  strapForceAnchor->setName(strcat(kLeftShoulderExtensorName, "Origin"));
  strapForceAnchor->SetStrapForce(this, kLeftShoulderExtensorOrigin, strapForce, 0);  
  addForce(strapForceAnchor);
  
  // Left Shoulder Flexor
  strapForce = gSimulation->GetCPG()->GetLeftShoulderFlexor();
  strapForce->SetNumAnchors(2);
  strapForce->setName(kLeftShoulderFlexorName);
  strapForce->SetVMax(kLeftShoulderFlexorLength, kDefaultMuscleVMaxFactor);
  strapForce->SetF0(kLeftShoulderFlexorPCA, kDefaultMuscleForcePerUnitArea);
  strapForce->SetK(kDefaultMuscleActivationK);
  
  strapForceAnchor = new StrapForceAnchor();
  strapForceAnchor->setName(strcat(kLeftShoulderFlexorName, "Origin"));
  strapForceAnchor->SetStrapForce(this, kLeftShoulderFlexorOrigin, strapForce, 0);  
  addForce(strapForceAnchor);
  
  // Right Shoulder Abductor
  
  strapForce = gSimulation->GetCPG()->GetRightShoulderAbductor();
  strapForce->SetNumAnchors(3);
  strapForce->setName(kRightShoulderAbductorName);
  strapForce->SetVMax(kRightShoulderAbductorLength, kDefaultMuscleVMaxFactor);
  strapForce->SetF0(kRightShoulderAbductorPCA, kDefaultMuscleForcePerUnitArea);
  strapForce->SetK(kDefaultMuscleActivationK);
  
  strapForceAnchor = new StrapForceAnchor();
  strapForceAnchor->setName(strcat(kRightShoulderAbductorName, "Origin"));
  strapForceAnchor->SetStrapForce(this, kRightShoulderAbductorOrigin, strapForce, 0);  
  addForce(strapForceAnchor);
  
  strapForceAnchor = new StrapForceAnchor();
  strapForceAnchor->setName(strcat(kRightShoulderAbductorName, "MidPoint"));
  strapForceAnchor->SetStrapForce(this, kRightShoulderAbductorMidPoint, strapForce, 1);  
  addForce(strapForceAnchor);
  
  // Right Shoulder Extensor
  strapForce = gSimulation->GetCPG()->GetRightShoulderExtensor();
  strapForce->SetNumAnchors(2);
  strapForce->setName(kRightShoulderExtensorName);
  strapForce->SetVMax(kRightShoulderExtensorLength, kDefaultMuscleVMaxFactor);
  strapForce->SetF0(kRightShoulderExtensorPCA, kDefaultMuscleForcePerUnitArea);
  strapForce->SetK(kDefaultMuscleActivationK);
  
  strapForceAnchor = new StrapForceAnchor();
  strapForceAnchor->setName(strcat(kRightShoulderExtensorName, "Origin"));
  strapForceAnchor->SetStrapForce(this, kRightShoulderExtensorOrigin, strapForce, 0);  
  addForce(strapForceAnchor);
  
  // Right Shoulder Flexor
  strapForce = gSimulation->GetCPG()->GetRightShoulderFlexor();
  strapForce->SetNumAnchors(2);
  strapForce->setName(kRightShoulderFlexorName);
  strapForce->SetVMax(kRightShoulderFlexorLength, kDefaultMuscleVMaxFactor);
  strapForce->SetF0(kRightShoulderFlexorPCA, kDefaultMuscleForcePerUnitArea);
  strapForce->SetK(kDefaultMuscleActivationK);
  
  strapForceAnchor = new StrapForceAnchor();
  strapForceAnchor->setName(strcat(kRightShoulderFlexorName, "Origin"));
  strapForceAnchor->SetStrapForce(this, kRightShoulderFlexorOrigin, strapForce, 0);  
  addForce(strapForceAnchor);
  
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

// Left Hind Limb Segments

LeftThigh::LeftThigh():
  dmRevoluteLink()
{
  // set the link name
  setName(kLeftThighPartName);
  
  // load up a graphics model if required
  if (gSimulation->UseGraphics())
  {
    GLuint *dlist = new GLuint;
    *dlist = LoadObj(kLeftThighGraphicFile);
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
    *dlist = LoadObj(kLeftLegGraphicFile);
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
    *dlist = LoadObj(kLeftFootGraphicFile);
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
  LadderContactModel *contactModel = new LadderContactModel();
  contactModel->setContactPoints(kLeftFootNumContactPoints, kLeftFootContactPositions);
  contactModel->setName(kLeftFootContactName);
  addForce(contactModel);
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

// Right Hind Limb Segments - these are just the left hind limb segements
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
    *dlist = LoadObj(kRightThighGraphicFile);
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
    *dlist = LoadObj(kRightLegGraphicFile);
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
    *dlist = LoadObj(kRightFootGraphicFile);
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
  LadderContactModel *contactModel = new LadderContactModel();
  contactModel->setContactPoints(kRightFootNumContactPoints, kRightFootContactPositions);
  contactModel->setName(kRightFootContactName);
  addForce(contactModel);
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

// Left Fore Limb Segments

LeftArm::LeftArm():
  dmRevoluteLink()
{
  // set the link name
  setName(kLeftArmPartName);
  
  // load up a graphics model if required
  if (gSimulation->UseGraphics())
  {
    GLuint *dlist = new GLuint;
    *dlist = LoadObj(kLeftArmGraphicFile);
    setUserData((void *) dlist);
  }

  setInertiaParameters(kLeftArmMass, kLeftArmMOI, kLeftArmCG);
  setMDHParameters(kLeftArmMDHA, kLeftArmMDHAlpha, kLeftArmMDHD, kLeftArmMDHTheta);
  double q, qd;
  getState(&q, &qd);
  qd = kLeftArmInitialJointVelocity;
  setState(&q, &qd);
  setJointLimits(kLeftArmJointMin, kLeftArmJointMax, 
      kLeftArmJointLimitSpringConstant, kLeftArmJointLimitDamperConstant);
  setJointFriction(kLeftArmJointFriction);

  // add the muscle forces
  
  StrapForceAnchor *strapForceAnchor;
  MAMuscle *strapForce;
        
  // Left Shoulder Abductor
  
  strapForce = gSimulation->GetCPG()->GetLeftShoulderAbductor();
  
  strapForceAnchor = new StrapForceAnchor();
  strapForceAnchor->setName(strcat(kLeftShoulderAbductorName, "Insertion"));
  strapForceAnchor->SetStrapForce(this, kLeftShoulderAbductorInsertion, strapForce, 2);  
  addForce(strapForceAnchor);
  
  // Left Shoulder Extensor
  
  strapForce = gSimulation->GetCPG()->GetLeftShoulderExtensor();
  
  strapForceAnchor = new StrapForceAnchor();
  strapForceAnchor->setName(strcat(kLeftShoulderExtensorName, "Insertion"));
  strapForceAnchor->SetStrapForce(this, kLeftShoulderExtensorInsertion, strapForce, 1);  
  addForce(strapForceAnchor);
  
  // Left Shoulder Flexor
  strapForce = gSimulation->GetCPG()->GetLeftShoulderFlexor();
  
  strapForceAnchor = new StrapForceAnchor();
  strapForceAnchor->setName(strcat(kLeftShoulderFlexorName, "Insertion"));
  strapForceAnchor->SetStrapForce(this, kLeftShoulderFlexorInsertion, strapForce, 1);  
  addForce(strapForceAnchor);
  
  // Left Elbow Extensor
  strapForce = gSimulation->GetCPG()->GetLeftElbowExtensor();
  strapForce->SetNumAnchors(3);
  strapForce->setName(kLeftElbowExtensorName);
  strapForce->SetVMax(kLeftElbowExtensorLength, kDefaultMuscleVMaxFactor);
  strapForce->SetF0(kLeftElbowExtensorPCA, kDefaultMuscleForcePerUnitArea);
  strapForce->SetK(kDefaultMuscleActivationK);
  
  strapForceAnchor = new StrapForceAnchor();
  strapForceAnchor->setName(strcat(kLeftElbowExtensorName, "Origin"));
  strapForceAnchor->SetStrapForce(this, kLeftElbowExtensorOrigin, strapForce, 0);  
  addForce(strapForceAnchor);
  
  strapForceAnchor = new StrapForceAnchor();
  strapForceAnchor->setName(strcat(kLeftElbowExtensorName, "MidPoint"));
  strapForceAnchor->SetStrapForce(this, kLeftElbowExtensorMidPoint, strapForce, 1);  
  addForce(strapForceAnchor);
  
  // Left Elbow Flexor
  strapForce = gSimulation->GetCPG()->GetLeftElbowFlexor();
  strapForce->SetNumAnchors(2);
  strapForce->setName(kLeftElbowFlexorName);
  strapForce->SetVMax(kLeftElbowFlexorLength, kDefaultMuscleVMaxFactor);
  strapForce->SetF0(kLeftElbowFlexorPCA, kDefaultMuscleForcePerUnitArea);
  strapForce->SetK(kDefaultMuscleActivationK);
  
  strapForceAnchor = new StrapForceAnchor();
  strapForceAnchor->setName(strcat(kLeftElbowFlexorName, "Origin"));
  strapForceAnchor->SetStrapForce(this, kLeftElbowFlexorOrigin, strapForce, 0);  
  addForce(strapForceAnchor);
    
}

LeftArm::~LeftArm()
{
}

void
LeftArm::draw() const
{
   dmRevoluteLink::draw();
   DrawAxes();
}

LeftForearm::LeftForearm():
  dmRevoluteLink()
{
  // set the link name
  setName(kLeftForearmPartName);
  
  // load up a graphics model if required
  if (gSimulation->UseGraphics())
  {
    GLuint *dlist = new GLuint;
    *dlist = LoadObj(kLeftForearmGraphicFile);
    setUserData((void *) dlist);
  }

  setInertiaParameters(kLeftForearmMass, kLeftForearmMOI, kLeftForearmCG);
  setMDHParameters(kLeftForearmMDHA, kLeftForearmMDHAlpha, kLeftForearmMDHD, kLeftForearmMDHTheta);
  double q, qd;
  getState(&q, &qd);
  qd = kLeftForearmInitialJointVelocity;
  setState(&q, &qd);
  setJointLimits(kLeftForearmJointMin, kLeftForearmJointMax, 
      kLeftForearmJointLimitSpringConstant, kLeftForearmJointLimitDamperConstant);
  setJointFriction(kLeftForearmJointFriction);
  
  // add the muscle forces
  
  StrapForceAnchor *strapForceAnchor;
  MAMuscle *strapForce;
        
  // Left Elbow Extensor
  
  strapForce = gSimulation->GetCPG()->GetLeftElbowExtensor();
  
  strapForceAnchor = new StrapForceAnchor();
  strapForceAnchor->setName(strcat(kLeftElbowExtensorName, "Insertion"));
  strapForceAnchor->SetStrapForce(this, kLeftElbowExtensorInsertion, strapForce, 2);  
  addForce(strapForceAnchor);
  
  // Left Elbow Flexor
  strapForce = gSimulation->GetCPG()->GetLeftElbowFlexor();
  
  strapForceAnchor = new StrapForceAnchor();
  strapForceAnchor->setName(strcat(kLeftElbowFlexorName, "Insertion"));
  strapForceAnchor->SetStrapForce(this, kLeftElbowFlexorInsertion, strapForce, 1);  
  addForce(strapForceAnchor);
  
  // Left Wrist Extensor
  strapForce = gSimulation->GetCPG()->GetLeftWristExtensor();
  strapForce->SetNumAnchors(3);
  strapForce->setName(kLeftWristExtensorName);
  strapForce->SetVMax(kLeftWristExtensorLength, kDefaultMuscleVMaxFactor);
  strapForce->SetF0(kLeftWristExtensorPCA, kDefaultMuscleForcePerUnitArea);
  strapForce->SetK(kDefaultMuscleActivationK);
  
  strapForceAnchor = new StrapForceAnchor();
  strapForceAnchor->setName(strcat(kLeftWristExtensorName, "Origin"));
  strapForceAnchor->SetStrapForce(this, kLeftWristExtensorOrigin, strapForce, 0);  
  addForce(strapForceAnchor);
  
  strapForceAnchor = new StrapForceAnchor();
  strapForceAnchor->setName(strcat(kLeftWristExtensorName, "MidPoint"));
  strapForceAnchor->SetStrapForce(this, kLeftWristExtensorMidPoint, strapForce, 1);  
  addForce(strapForceAnchor);
  
  // Left Wrist Flexor
  strapForce = gSimulation->GetCPG()->GetLeftWristFlexor();
  strapForce->SetNumAnchors(3);
  strapForce->setName(kLeftWristFlexorName);
  strapForce->SetVMax(kLeftWristFlexorLength, kDefaultMuscleVMaxFactor);
  strapForce->SetF0(kLeftWristFlexorPCA, kDefaultMuscleForcePerUnitArea);
  strapForce->SetK(kDefaultMuscleActivationK);
  
  strapForceAnchor = new StrapForceAnchor();
  strapForceAnchor->setName(strcat(kLeftWristFlexorName, "Origin"));
  strapForceAnchor->SetStrapForce(this, kLeftWristFlexorOrigin, strapForce, 0);  
  addForce(strapForceAnchor);
  
  strapForceAnchor = new StrapForceAnchor();
  strapForceAnchor->setName(strcat(kLeftWristFlexorName, "MidPoint"));
  strapForceAnchor->SetStrapForce(this, kLeftWristFlexorMidPoint, strapForce, 1);  
  addForce(strapForceAnchor);
}

LeftForearm::~LeftForearm()
{
}

void
LeftForearm::draw() const
{
   dmRevoluteLink::draw();
   DrawAxes();
}

LeftHand::LeftHand():
  dmRevoluteLink()
{
  // set the link name
  setName(kLeftHandPartName);
  
  // load up a graphics model if required
  if (gSimulation->UseGraphics())
  {
    GLuint *dlist = new GLuint;
    *dlist = LoadObj(kLeftHandGraphicFile);
    setUserData((void *) dlist);
  }

  setInertiaParameters(kLeftHandMass, kLeftHandMOI, kLeftHandCG);
  setMDHParameters(kLeftHandMDHA, kLeftHandMDHAlpha, kLeftHandMDHD, kLeftHandMDHTheta);
  double q, qd;
  getState(&q, &qd);
  qd = kLeftHandInitialJointVelocity;
  setState(&q, &qd);
  setJointLimits(kLeftHandJointMin, kLeftHandJointMax, 
      kLeftHandJointLimitSpringConstant, kLeftHandJointLimitDamperConstant);
  setJointFriction(kLeftHandJointFriction);

  // add the muscle forces
  
  StrapForceAnchor *strapForceAnchor;
  MAMuscle *strapForce;
        
  // Left Wrist Extensor
  
  strapForce = gSimulation->GetCPG()->GetLeftWristExtensor();
  
  strapForceAnchor = new StrapForceAnchor();
  strapForceAnchor->setName(strcat(kLeftWristExtensorName, "Insertion"));
  strapForceAnchor->SetStrapForce(this, kLeftWristExtensorInsertion, strapForce, 2);  
  addForce(strapForceAnchor);
  
  // Left Wrist Flexor
  strapForce = gSimulation->GetCPG()->GetLeftWristFlexor();
  
  strapForceAnchor = new StrapForceAnchor();
  strapForceAnchor->setName(strcat(kLeftWristFlexorName, "Insertion"));
  strapForceAnchor->SetStrapForce(this, kLeftWristFlexorInsertion, strapForce, 2);  
  addForce(strapForceAnchor);
  
  // Contact Points
  LadderContactModel *contactModel = new LadderContactModel();
  contactModel->setContactPoints(kLeftHandNumContactPoints, kLeftHandContactPositions);
  contactModel->setName(kLeftHandContactName);
  addForce(contactModel);
}

LeftHand::~LeftHand()
{
}

void
LeftHand::draw() const
{
   dmRevoluteLink::draw();
   DrawAxes();
}

// Right Fore Limb Segments 

RightArm::RightArm():
  dmRevoluteLink()
{
  // set the link name
  setName(kRightArmPartName);
  
  // load up a graphics model if required
  if (gSimulation->UseGraphics())
  {
    GLuint *dlist = new GLuint;
    *dlist = LoadObj(kRightArmGraphicFile);
    setUserData((void *) dlist);
  }

  setInertiaParameters(kRightArmMass, kRightArmMOI, kRightArmCG);
  setMDHParameters(kRightArmMDHA, kRightArmMDHAlpha, kRightArmMDHD, kRightArmMDHTheta);
  double q, qd;
  getState(&q, &qd);
  qd = kRightArmInitialJointVelocity;
  setState(&q, &qd);
  setJointLimits(kRightArmJointMin, kRightArmJointMax, 
      kRightArmJointLimitSpringConstant, kRightArmJointLimitDamperConstant);
  setJointFriction(kRightArmJointFriction);

  // add the muscle forces
  
  StrapForceAnchor *strapForceAnchor;
  MAMuscle *strapForce;
        
  // Right Shoulder Abductor
  
  strapForce = gSimulation->GetCPG()->GetRightShoulderAbductor();
  
  strapForceAnchor = new StrapForceAnchor();
  strapForceAnchor->setName(strcat(kRightShoulderAbductorName, "Insertion"));
  strapForceAnchor->SetStrapForce(this, kRightShoulderAbductorInsertion, strapForce, 2);  
  addForce(strapForceAnchor);
  
  // Right Shoulder Extensor
  
  strapForce = gSimulation->GetCPG()->GetRightShoulderExtensor();
  
  strapForceAnchor = new StrapForceAnchor();
  strapForceAnchor->setName(strcat(kRightShoulderExtensorName, "Insertion"));
  strapForceAnchor->SetStrapForce(this, kRightShoulderExtensorInsertion, strapForce, 1);  
  addForce(strapForceAnchor);
  
  // Right Shoulder Flexor
  strapForce = gSimulation->GetCPG()->GetRightShoulderFlexor();
  
  strapForceAnchor = new StrapForceAnchor();
  strapForceAnchor->setName(strcat(kRightShoulderFlexorName, "Insertion"));
  strapForceAnchor->SetStrapForce(this, kRightShoulderFlexorInsertion, strapForce, 1);  
  addForce(strapForceAnchor);
  
  // Right Elbow Extensor
  strapForce = gSimulation->GetCPG()->GetRightElbowExtensor();
  strapForce->SetNumAnchors(3);
  strapForce->setName(kRightElbowExtensorName);
  strapForce->SetVMax(kRightElbowExtensorLength, kDefaultMuscleVMaxFactor);
  strapForce->SetF0(kRightElbowExtensorPCA, kDefaultMuscleForcePerUnitArea);
  strapForce->SetK(kDefaultMuscleActivationK);
  
  strapForceAnchor = new StrapForceAnchor();
  strapForceAnchor->setName(strcat(kRightElbowExtensorName, "Origin"));
  strapForceAnchor->SetStrapForce(this, kRightElbowExtensorOrigin, strapForce, 0);  
  addForce(strapForceAnchor);
  
  strapForceAnchor = new StrapForceAnchor();
  strapForceAnchor->setName(strcat(kRightElbowExtensorName, "MidPoint"));
  strapForceAnchor->SetStrapForce(this, kRightElbowExtensorMidPoint, strapForce, 1);  
  addForce(strapForceAnchor);
  
  // Right Elbow Flexor
  strapForce = gSimulation->GetCPG()->GetRightElbowFlexor();
  strapForce->SetNumAnchors(2);
  strapForce->setName(kRightElbowFlexorName);
  strapForce->SetVMax(kRightElbowFlexorLength, kDefaultMuscleVMaxFactor);
  strapForce->SetF0(kRightElbowFlexorPCA, kDefaultMuscleForcePerUnitArea);
  strapForce->SetK(kDefaultMuscleActivationK);
  
  strapForceAnchor = new StrapForceAnchor();
  strapForceAnchor->setName(strcat(kRightElbowFlexorName, "Origin"));
  strapForceAnchor->SetStrapForce(this, kRightElbowFlexorOrigin, strapForce, 0);  
  addForce(strapForceAnchor);
  
}

RightArm::~RightArm()
{
}

void
RightArm::draw() const
{
   dmRevoluteLink::draw();
   DrawAxes();
}

RightForearm::RightForearm():
  dmRevoluteLink()
{
  // set the link name
  setName(kRightForearmPartName);
  
  // load up a graphics model if required
  if (gSimulation->UseGraphics())
  {
    GLuint *dlist = new GLuint;
    *dlist = LoadObj(kRightForearmGraphicFile);
    setUserData((void *) dlist);
  }

  setInertiaParameters(kRightForearmMass, kRightForearmMOI, kRightForearmCG);
  setMDHParameters(kRightForearmMDHA, kRightForearmMDHAlpha, kRightForearmMDHD, kRightForearmMDHTheta);
  double q, qd;
  getState(&q, &qd);
  qd = kRightForearmInitialJointVelocity;
  setState(&q, &qd);
  setJointLimits(kRightForearmJointMin, kRightForearmJointMax, 
      kRightForearmJointLimitSpringConstant, kRightForearmJointLimitDamperConstant);
  setJointFriction(kRightForearmJointFriction);
  
  // add the muscle forces
  
  StrapForceAnchor *strapForceAnchor;
  MAMuscle *strapForce;
        
  // Right Elbow Extensor
  
  strapForce = gSimulation->GetCPG()->GetRightElbowExtensor();
  
  strapForceAnchor = new StrapForceAnchor();
  strapForceAnchor->setName(strcat(kRightElbowExtensorName, "Insertion"));
  strapForceAnchor->SetStrapForce(this, kRightElbowExtensorInsertion, strapForce, 2);  
  addForce(strapForceAnchor);
  
  // Right Elbow Flexor
  strapForce = gSimulation->GetCPG()->GetRightElbowFlexor();
  
  strapForceAnchor = new StrapForceAnchor();
  strapForceAnchor->setName(strcat(kRightElbowFlexorName, "Insertion"));
  strapForceAnchor->SetStrapForce(this, kRightElbowFlexorInsertion, strapForce, 1);  
  addForce(strapForceAnchor);
  
  // Right Wrist Extensor
  strapForce = gSimulation->GetCPG()->GetRightWristExtensor();
  strapForce->SetNumAnchors(3);
  strapForce->setName(kRightWristExtensorName);
  strapForce->SetVMax(kRightWristExtensorLength, kDefaultMuscleVMaxFactor);
  strapForce->SetF0(kRightWristExtensorPCA, kDefaultMuscleForcePerUnitArea);
  strapForce->SetK(kDefaultMuscleActivationK);
  
  strapForceAnchor = new StrapForceAnchor();
  strapForceAnchor->setName(strcat(kRightWristExtensorName, "Origin"));
  strapForceAnchor->SetStrapForce(this, kRightWristExtensorOrigin, strapForce, 0);  
  addForce(strapForceAnchor);
  
  strapForceAnchor = new StrapForceAnchor();
  strapForceAnchor->setName(strcat(kRightWristExtensorName, "MidPoint"));
  strapForceAnchor->SetStrapForce(this, kRightWristExtensorMidPoint, strapForce, 1);  
  addForce(strapForceAnchor);
  
  // Right Wrist Flexor
  strapForce = gSimulation->GetCPG()->GetRightWristFlexor();
  strapForce->SetNumAnchors(3);
  strapForce->setName(kRightWristFlexorName);
  strapForce->SetVMax(kRightWristFlexorLength, kDefaultMuscleVMaxFactor);
  strapForce->SetF0(kRightWristFlexorPCA, kDefaultMuscleForcePerUnitArea);
  strapForce->SetK(kDefaultMuscleActivationK);
  
  strapForceAnchor = new StrapForceAnchor();
  strapForceAnchor->setName(strcat(kRightWristFlexorName, "Origin"));
  strapForceAnchor->SetStrapForce(this, kRightWristFlexorOrigin, strapForce, 0);  
  addForce(strapForceAnchor);
  
  strapForceAnchor = new StrapForceAnchor();
  strapForceAnchor->setName(strcat(kRightWristFlexorName, "MidPoint"));
  strapForceAnchor->SetStrapForce(this, kRightWristFlexorMidPoint, strapForce, 1);  
  addForce(strapForceAnchor);
}

RightForearm::~RightForearm()
{
}

void
RightForearm::draw() const
{
   dmRevoluteLink::draw();
   DrawAxes();
}

RightHand::RightHand():
  dmRevoluteLink()
{
  // set the link name
  setName(kRightHandPartName);
  
  // load up a graphics model if required
  if (gSimulation->UseGraphics())
  {
    GLuint *dlist = new GLuint;
    *dlist = LoadObj(kRightHandGraphicFile);
    setUserData((void *) dlist);
  }

  setInertiaParameters(kRightHandMass, kRightHandMOI, kRightHandCG);
  setMDHParameters(kRightHandMDHA, kRightHandMDHAlpha, kRightHandMDHD, kRightHandMDHTheta);
  double q, qd;
  getState(&q, &qd);
  qd = kRightHandInitialJointVelocity;
  setState(&q, &qd);
  setJointLimits(kRightHandJointMin, kRightHandJointMax, 
      kRightHandJointLimitSpringConstant, kRightHandJointLimitDamperConstant);
  setJointFriction(kRightHandJointFriction);

  // add the muscle forces
  
  StrapForceAnchor *strapForceAnchor;
  MAMuscle *strapForce;
        
  // Right Wrist Extensor
  
  strapForce = gSimulation->GetCPG()->GetRightWristExtensor();
  
  strapForceAnchor = new StrapForceAnchor();
  strapForceAnchor->setName(strcat(kRightWristExtensorName, "Insertion"));
  strapForceAnchor->SetStrapForce(this, kRightWristExtensorInsertion, strapForce, 2);  
  addForce(strapForceAnchor);
  
  // Right Wrist Flexor
  strapForce = gSimulation->GetCPG()->GetRightWristFlexor();
  
  strapForceAnchor = new StrapForceAnchor();
  strapForceAnchor->setName(strcat(kRightWristFlexorName, "Insertion"));
  strapForceAnchor->SetStrapForce(this, kRightWristFlexorInsertion, strapForce, 2);  
  addForce(strapForceAnchor);
  
  // Contact Points
  LadderContactModel *contactModel = new LadderContactModel();
  contactModel->setContactPoints(kRightHandNumContactPoints, kRightHandContactPositions);
  contactModel->setName(kRightHandContactName);
  addForce(contactModel);
}

RightHand::~RightHand()
{
}

void
RightHand::draw() const
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
  // Left Arm
  file.RetrieveQuotedStringParameter("kLeftArmPartName", kLeftArmPartName, 256);
  file.RetrieveQuotedStringParameter("kLeftArmGraphicFile", kLeftArmGraphicFile, 256);
  file.RetrieveParameter("kLeftArmMass", &kLeftArmMass);
  file.RetrieveParameter("kLeftArmMOI", 9, (double *)kLeftArmMOI);
  file.RetrieveParameter("kLeftArmCG", 3, kLeftArmCG);
  file.RetrieveParameter("kLeftArmMDHA", &kLeftArmMDHA);
  file.RetrieveParameter("kLeftArmMDHAlpha", &kLeftArmMDHAlpha);
  file.RetrieveParameter("kLeftArmMDHD", &kLeftArmMDHD);
  file.RetrieveParameter("kLeftArmMDHTheta", &kLeftArmMDHTheta);
  file.RetrieveParameter("kLeftArmInitialJointVelocity", &kLeftArmInitialJointVelocity);
  file.RetrieveParameter("kLeftArmJointMin", &kLeftArmJointMin);
  file.RetrieveParameter("kLeftArmJointMax", &kLeftArmJointMax);
  file.RetrieveParameter("kLeftArmJointLimitSpringConstant", &kLeftArmJointLimitSpringConstant);
  if (kLeftArmJointLimitSpringConstant < 0) kLeftArmJointLimitSpringConstant = kDefaultJointLimitSpringConstant;
  file.RetrieveParameter("kLeftArmJointLimitDamperConstant", &kLeftArmJointLimitDamperConstant);
  if (kLeftArmJointLimitDamperConstant < 0) kLeftArmJointLimitDamperConstant = kDefaultJointLimitDamperConstant;
  file.RetrieveParameter("kLeftArmJointFriction", &kLeftArmJointFriction);
  if (kLeftArmJointFriction < 0) kLeftArmJointFriction = kDefaultJointFriction;

  // -------------------------------------------
  // Right Arm
  file.RetrieveQuotedStringParameter("kRightArmPartName", kRightArmPartName, 256);
  file.RetrieveQuotedStringParameter("kRightArmGraphicFile", kRightArmGraphicFile, 256);
  file.RetrieveParameter("kRightArmMass", &kRightArmMass);
  file.RetrieveParameter("kRightArmMOI", 9, (double *)kRightArmMOI);
  file.RetrieveParameter("kRightArmCG", 3, kRightArmCG);
  file.RetrieveParameter("kRightArmMDHA", &kRightArmMDHA);
  file.RetrieveParameter("kRightArmMDHAlpha", &kRightArmMDHAlpha);
  file.RetrieveParameter("kRightArmMDHD", &kRightArmMDHD);
  file.RetrieveParameter("kRightArmMDHTheta", &kRightArmMDHTheta);
  file.RetrieveParameter("kRightArmInitialJointVelocity", &kRightArmInitialJointVelocity);
  file.RetrieveParameter("kRightArmJointMin", &kRightArmJointMin);
  file.RetrieveParameter("kRightArmJointMax", &kRightArmJointMax);
  file.RetrieveParameter("kRightArmJointLimitSpringConstant", &kRightArmJointLimitSpringConstant);
  if (kRightArmJointLimitSpringConstant < 0) kRightArmJointLimitSpringConstant = kDefaultJointLimitSpringConstant;
  file.RetrieveParameter("kRightArmJointLimitDamperConstant", &kRightArmJointLimitDamperConstant);
  if (kRightArmJointLimitDamperConstant < 0) kRightArmJointLimitDamperConstant = kDefaultJointLimitDamperConstant;
  file.RetrieveParameter("kRightArmJointFriction", &kRightArmJointFriction);
  if (kRightArmJointFriction < 0) kRightArmJointFriction = kDefaultJointFriction;

  // -------------------------------------------
  // Left Forearm
  file.RetrieveQuotedStringParameter("kLeftForearmPartName", kLeftForearmPartName, 256);
  file.RetrieveQuotedStringParameter("kLeftForearmGraphicFile", kLeftForearmGraphicFile, 256);
  file.RetrieveParameter("kLeftForearmMass", &kLeftForearmMass);
  file.RetrieveParameter("kLeftForearmMOI", 9, (double *)kLeftForearmMOI);
  file.RetrieveParameter("kLeftForearmCG", 3, kLeftForearmCG);
  file.RetrieveParameter("kLeftForearmMDHA", &kLeftForearmMDHA);
  file.RetrieveParameter("kLeftForearmMDHAlpha", &kLeftForearmMDHAlpha);
  file.RetrieveParameter("kLeftForearmMDHD", &kLeftForearmMDHD);
  file.RetrieveParameter("kLeftForearmMDHTheta", &kLeftForearmMDHTheta);
  file.RetrieveParameter("kLeftForearmInitialJointVelocity", &kLeftForearmInitialJointVelocity);
  file.RetrieveParameter("kLeftForearmJointMin", &kLeftForearmJointMin);
  file.RetrieveParameter("kLeftForearmJointMax", &kLeftForearmJointMax);
  file.RetrieveParameter("kLeftForearmJointLimitSpringConstant", &kLeftForearmJointLimitSpringConstant);
  if (kLeftForearmJointLimitSpringConstant < 0) kLeftForearmJointLimitSpringConstant = kDefaultJointLimitSpringConstant;
  file.RetrieveParameter("kLeftForearmJointLimitDamperConstant", &kLeftForearmJointLimitDamperConstant);
  if (kLeftForearmJointLimitDamperConstant < 0) kLeftForearmJointLimitDamperConstant = kDefaultJointLimitDamperConstant;
  file.RetrieveParameter("kLeftForearmJointFriction", &kLeftForearmJointFriction);
  if (kLeftForearmJointFriction < 0) kLeftForearmJointFriction = kDefaultJointFriction;

  // -------------------------------------------
  // Right Forearm
  file.RetrieveQuotedStringParameter("kRightForearmPartName", kRightForearmPartName, 256);
  file.RetrieveQuotedStringParameter("kRightForearmGraphicFile", kRightForearmGraphicFile, 256);
  file.RetrieveParameter("kRightForearmMass", &kRightForearmMass);
  file.RetrieveParameter("kRightForearmMOI", 9, (double *)kRightForearmMOI);
  file.RetrieveParameter("kRightForearmCG", 3, kRightForearmCG);
  file.RetrieveParameter("kRightForearmMDHA", &kRightForearmMDHA);
  file.RetrieveParameter("kRightForearmMDHAlpha", &kRightForearmMDHAlpha);
  file.RetrieveParameter("kRightForearmMDHD", &kRightForearmMDHD);
  file.RetrieveParameter("kRightForearmMDHTheta", &kRightForearmMDHTheta);
  file.RetrieveParameter("kRightForearmInitialJointVelocity", &kRightForearmInitialJointVelocity);
  file.RetrieveParameter("kRightForearmJointMin", &kRightForearmJointMin);
  file.RetrieveParameter("kRightForearmJointMax", &kRightForearmJointMax);
  file.RetrieveParameter("kRightForearmJointLimitSpringConstant", &kRightForearmJointLimitSpringConstant);
  if (kRightForearmJointLimitSpringConstant < 0) kRightForearmJointLimitSpringConstant = kDefaultJointLimitSpringConstant;
  file.RetrieveParameter("kRightForearmJointLimitDamperConstant", &kRightForearmJointLimitDamperConstant);
  if (kRightForearmJointLimitDamperConstant < 0) kRightForearmJointLimitDamperConstant = kDefaultJointLimitDamperConstant;
  file.RetrieveParameter("kRightForearmJointFriction", &kRightForearmJointFriction);
  if (kRightForearmJointFriction < 0) kRightForearmJointFriction = kDefaultJointFriction;

  // -------------------------------------------
  // Left Hand
  file.RetrieveQuotedStringParameter("kLeftHandPartName", kLeftHandPartName, 256);
  file.RetrieveQuotedStringParameter("kLeftHandGraphicFile", kLeftHandGraphicFile, 256);
  file.RetrieveParameter("kLeftHandMass", &kLeftHandMass);
  file.RetrieveParameter("kLeftHandMOI", 9, (double *)kLeftHandMOI);
  file.RetrieveParameter("kLeftHandCG", 3, kLeftHandCG);
  file.RetrieveParameter("kLeftHandMDHA", &kLeftHandMDHA);
  file.RetrieveParameter("kLeftHandMDHAlpha", &kLeftHandMDHAlpha);
  file.RetrieveParameter("kLeftHandMDHD", &kLeftHandMDHD);
  file.RetrieveParameter("kLeftHandMDHTheta", &kLeftHandMDHTheta);
  file.RetrieveParameter("kLeftHandInitialJointVelocity", &kLeftHandInitialJointVelocity);
  file.RetrieveParameter("kLeftHandJointMin", &kLeftHandJointMin);
  file.RetrieveParameter("kLeftHandJointMax", &kLeftHandJointMax);
  file.RetrieveParameter("kLeftHandJointLimitSpringConstant", &kLeftHandJointLimitSpringConstant);
  if (kLeftHandJointLimitSpringConstant < 0) kLeftHandJointLimitSpringConstant = kDefaultJointLimitSpringConstant;
  file.RetrieveParameter("kLeftHandJointLimitDamperConstant", &kLeftHandJointLimitDamperConstant);
  if (kLeftHandJointLimitDamperConstant < 0) kLeftHandJointLimitDamperConstant = kDefaultJointLimitDamperConstant;
  file.RetrieveParameter("kLeftHandJointFriction", &kLeftHandJointFriction);
  if (kLeftHandJointFriction < 0) kLeftHandJointFriction = kDefaultJointFriction;

  // -------------------------------------------
  // Right Hand
  file.RetrieveQuotedStringParameter("kRightHandPartName", kRightHandPartName, 256);
  file.RetrieveQuotedStringParameter("kRightHandGraphicFile", kRightHandGraphicFile, 256);
  file.RetrieveParameter("kRightHandMass", &kRightHandMass);
  file.RetrieveParameter("kRightHandMOI", 9, (double *)kRightHandMOI);
  file.RetrieveParameter("kRightHandCG", 3, kRightHandCG);
  file.RetrieveParameter("kRightHandMDHA", &kRightHandMDHA);
  file.RetrieveParameter("kRightHandMDHAlpha", &kRightHandMDHAlpha);
  file.RetrieveParameter("kRightHandMDHD", &kRightHandMDHD);
  file.RetrieveParameter("kRightHandMDHTheta", &kRightHandMDHTheta);
  file.RetrieveParameter("kRightHandInitialJointVelocity", &kRightHandInitialJointVelocity);
  file.RetrieveParameter("kRightHandJointMin", &kRightHandJointMin);
  file.RetrieveParameter("kRightHandJointMax", &kRightHandJointMax);
  file.RetrieveParameter("kRightHandJointLimitSpringConstant", &kRightHandJointLimitSpringConstant);
  if (kRightHandJointLimitSpringConstant < 0) kRightHandJointLimitSpringConstant = kDefaultJointLimitSpringConstant;
  file.RetrieveParameter("kRightHandJointLimitDamperConstant", &kRightHandJointLimitDamperConstant);
  if (kRightHandJointLimitDamperConstant < 0) kRightHandJointLimitDamperConstant = kDefaultJointLimitDamperConstant;
  file.RetrieveParameter("kRightHandJointFriction", &kRightHandJointFriction);
  if (kRightHandJointFriction < 0) kRightHandJointFriction = kDefaultJointFriction;

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

  // Left Shoulder Abductor
  file.RetrieveQuotedStringParameter("kLeftShoulderAbductorName", kLeftShoulderAbductorName, 256);
  file.RetrieveParameter("kLeftShoulderAbductorOrigin", 3, kLeftShoulderAbductorOrigin);
  file.RetrieveParameter("kLeftShoulderAbductorMidPoint", 3, kLeftShoulderAbductorMidPoint);
  file.RetrieveParameter("kLeftShoulderAbductorInsertion", 3, kLeftShoulderAbductorInsertion);
  file.RetrieveParameter("kLeftShoulderAbductorPCA", &kLeftShoulderAbductorPCA);
  file.RetrieveParameter("kLeftShoulderAbductorLength", &kLeftShoulderAbductorLength);

  // Left Shoulder Extensor
  file.RetrieveQuotedStringParameter("kLeftShoulderExtensorName", kLeftShoulderExtensorName, 256);
  file.RetrieveParameter("kLeftShoulderExtensorOrigin", 3, kLeftShoulderExtensorOrigin);
  file.RetrieveParameter("kLeftShoulderExtensorInsertion", 3, kLeftShoulderExtensorInsertion);
  file.RetrieveParameter("kLeftShoulderExtensorPCA", &kLeftShoulderExtensorPCA);
  file.RetrieveParameter("kLeftShoulderExtensorLength", &kLeftShoulderExtensorLength);

  // Left Shoulder Flexor
  file.RetrieveQuotedStringParameter("kLeftShoulderFlexorName", kLeftShoulderFlexorName, 256);
  file.RetrieveParameter("kLeftShoulderFlexorOrigin", 3, kLeftShoulderFlexorOrigin);
  file.RetrieveParameter("kLeftShoulderFlexorInsertion", 3, kLeftShoulderFlexorInsertion);
  file.RetrieveParameter("kLeftShoulderFlexorPCA", &kLeftShoulderFlexorPCA);
  file.RetrieveParameter("kLeftShoulderFlexorLength", &kLeftShoulderFlexorLength);

  // Right Shoulder Abductor
  file.RetrieveQuotedStringParameter("kRightShoulderAbductorName", kRightShoulderAbductorName, 256);
  file.RetrieveParameter("kRightShoulderAbductorOrigin", 3, kRightShoulderAbductorOrigin);
  file.RetrieveParameter("kRightShoulderAbductorMidPoint", 3, kRightShoulderAbductorMidPoint);
  file.RetrieveParameter("kRightShoulderAbductorInsertion", 3, kRightShoulderAbductorInsertion);
  file.RetrieveParameter("kRightShoulderAbductorPCA", &kRightShoulderAbductorPCA);
  file.RetrieveParameter("kRightShoulderAbductorLength", &kRightShoulderAbductorLength);

  // Right Shoulder Extensor
  file.RetrieveQuotedStringParameter("kRightShoulderExtensorName", kRightShoulderExtensorName, 256);
  file.RetrieveParameter("kRightShoulderExtensorOrigin", 3, kRightShoulderExtensorOrigin);
  file.RetrieveParameter("kRightShoulderExtensorInsertion", 3, kRightShoulderExtensorInsertion);
  file.RetrieveParameter("kRightShoulderExtensorPCA", &kRightShoulderExtensorPCA);
  file.RetrieveParameter("kRightShoulderExtensorLength", &kRightShoulderExtensorLength);

  // Right Shoulder Flexor
  file.RetrieveQuotedStringParameter("kRightShoulderFlexorName", kRightShoulderFlexorName, 256);
  file.RetrieveParameter("kRightShoulderFlexorOrigin", 3, kRightShoulderFlexorOrigin);
  file.RetrieveParameter("kRightShoulderFlexorInsertion", 3, kRightShoulderFlexorInsertion);
  file.RetrieveParameter("kRightShoulderFlexorPCA", &kRightShoulderFlexorPCA);
  file.RetrieveParameter("kRightShoulderFlexorLength", &kRightShoulderFlexorLength);

  // Left Elbow Extensor
  file.RetrieveQuotedStringParameter("kLeftElbowExtensorName", kLeftElbowExtensorName, 256);
  file.RetrieveParameter("kLeftElbowExtensorOrigin", 3, kLeftElbowExtensorOrigin);
  file.RetrieveParameter("kLeftElbowExtensorMidPoint", 3, kLeftElbowExtensorMidPoint);
  file.RetrieveParameter("kLeftElbowExtensorInsertion", 3, kLeftElbowExtensorInsertion);
  file.RetrieveParameter("kLeftElbowExtensorPCA", &kLeftElbowExtensorPCA);
  file.RetrieveParameter("kLeftElbowExtensorLength", &kLeftElbowExtensorLength);

  // Left Elbow Flexor
  file.RetrieveQuotedStringParameter("kLeftElbowFlexorName", kLeftElbowFlexorName, 256);
  file.RetrieveParameter("kLeftElbowFlexorOrigin", 3, kLeftElbowFlexorOrigin);
  file.RetrieveParameter("kLeftElbowFlexorInsertion", 3, kLeftElbowFlexorInsertion);
  file.RetrieveParameter("kLeftElbowFlexorPCA", &kLeftElbowFlexorPCA);
  file.RetrieveParameter("kLeftElbowFlexorLength", &kLeftElbowFlexorLength);

  // Right Elbow Extensor
  file.RetrieveQuotedStringParameter("kRightElbowExtensorName", kRightElbowExtensorName, 256);
  file.RetrieveParameter("kRightElbowExtensorOrigin", 3, kRightElbowExtensorOrigin);
  file.RetrieveParameter("kRightElbowExtensorMidPoint", 3, kRightElbowExtensorMidPoint);
  file.RetrieveParameter("kRightElbowExtensorInsertion", 3, kRightElbowExtensorInsertion);
  file.RetrieveParameter("kRightElbowExtensorPCA", &kRightElbowExtensorPCA);
  file.RetrieveParameter("kRightElbowExtensorLength", &kRightElbowExtensorLength);

  // Right Elbow Flexor
  file.RetrieveQuotedStringParameter("kRightElbowFlexorName", kRightElbowFlexorName, 256);
  file.RetrieveParameter("kRightElbowFlexorOrigin", 3, kRightElbowFlexorOrigin);
  file.RetrieveParameter("kRightElbowFlexorInsertion", 3, kRightElbowFlexorInsertion);
  file.RetrieveParameter("kRightElbowFlexorPCA", &kRightElbowFlexorPCA);
  file.RetrieveParameter("kRightElbowFlexorLength", &kRightElbowFlexorLength);

  // Left Wrist Extensor
  file.RetrieveQuotedStringParameter("kLeftWristExtensorName", kLeftWristExtensorName, 256);
  file.RetrieveParameter("kLeftWristExtensorOrigin", 3, kLeftWristExtensorOrigin);
  file.RetrieveParameter("kLeftWristExtensorMidPoint", 3, kLeftWristExtensorMidPoint);
  file.RetrieveParameter("kLeftWristExtensorInsertion", 3, kLeftWristExtensorInsertion);
  file.RetrieveParameter("kLeftWristExtensorPCA", &kLeftWristExtensorPCA);
  file.RetrieveParameter("kLeftWristExtensorLength", &kLeftWristExtensorLength);

  // Left Wrist Flexor
  file.RetrieveQuotedStringParameter("kLeftWristFlexorName", kLeftWristFlexorName, 256);
  file.RetrieveParameter("kLeftWristFlexorOrigin", 3, kLeftWristFlexorOrigin);
  file.RetrieveParameter("kLeftWristFlexorMidPoint", 3, kLeftWristFlexorMidPoint);
  file.RetrieveParameter("kLeftWristFlexorInsertion", 3, kLeftWristFlexorInsertion);
  file.RetrieveParameter("kLeftWristFlexorPCA", &kLeftWristFlexorPCA);
  file.RetrieveParameter("kLeftWristFlexorLength", &kLeftWristFlexorLength);

  // Right Wrist Extensor
  file.RetrieveQuotedStringParameter("kRightWristExtensorName", kRightWristExtensorName, 256);
  file.RetrieveParameter("kRightWristExtensorOrigin", 3, kRightWristExtensorOrigin);
  file.RetrieveParameter("kRightWristExtensorMidPoint", 3, kRightWristExtensorMidPoint);
  file.RetrieveParameter("kRightWristExtensorInsertion", 3, kRightWristExtensorInsertion);
  file.RetrieveParameter("kRightWristExtensorPCA", &kRightWristExtensorPCA);
  file.RetrieveParameter("kRightWristExtensorLength", &kRightWristExtensorLength);

  // Right Wrist Flexor
  file.RetrieveQuotedStringParameter("kRightWristFlexorName", kRightWristFlexorName, 256);
  file.RetrieveParameter("kRightWristFlexorOrigin", 3, kRightWristFlexorOrigin);
  file.RetrieveParameter("kRightWristFlexorMidPoint", 3, kRightWristFlexorMidPoint);
  file.RetrieveParameter("kRightWristFlexorInsertion", 3, kRightWristFlexorInsertion);
  file.RetrieveParameter("kRightWristFlexorPCA", &kRightWristFlexorPCA);
  file.RetrieveParameter("kRightWristFlexorLength", &kRightWristFlexorLength);

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

  // Left Hand

  file.RetrieveQuotedStringParameter("kLeftHandContactName", kLeftHandContactName, 256);
  file.RetrieveParameter("kLeftHandNumContactPoints", &kLeftHandNumContactPoints);
  // WARNING - memory leak possible
  kLeftHandContactPositions = new CartesianVector[kLeftHandNumContactPoints];
  file.RetrieveParameter("kLeftHandContactPositions", kLeftHandNumContactPoints * 3, (double *)kLeftHandContactPositions);
  
  // Right Hand

  file.RetrieveQuotedStringParameter("kRightHandContactName", kRightHandContactName, 256);
  file.RetrieveParameter("kRightHandNumContactPoints", &kRightHandNumContactPoints);
  // WARNING - memory leak possible
  kRightHandContactPositions = new CartesianVector[kRightHandNumContactPoints];
  file.RetrieveParameter("kRightHandContactPositions", kRightHandNumContactPoints * 3, (double *)kRightHandContactPositions);
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

