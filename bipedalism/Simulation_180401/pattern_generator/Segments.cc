// Segments.cc - the various segment objects

#include <string.h>
#include <glLoadModels.h>

#include "Segments.h"
#include "Simulation.h"
#include "StepController.h"
#include "ModifiedContactModel.h"
#include "MuscleModel.h"
#include "ParameterFile.h"

// Note having the single load routine and hundreds of globals is not
// a good way of doing things

// the globals are declare here
#define SEG_PARAM_EXTERN 
#include "SegmentParameters.h"

extern Simulation *gSimulation;

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
  
  StepController *controller;
  MuscleModel *muscleModel;
        
  // Left Hip Extensor
  controller = new StepController(kControllerStepTime);
  controller->SetName(kLeftHipExtensorName, kControllerSuffix);
  muscleModel = new MuscleModel(controller, kLeftHipExtensorOrigin, 
      kLeftThighPartName, kLeftHipExtensorInsertion);
  muscleModel->setMinMuscleLength(kLeftHipExtensorMinLength);
  muscleModel->setDampingFactor(kLeftHipExtensorDampingFactor);
  muscleModel->setControlFactor(kLeftHipExtensorControlFactor);
  muscleModel->setSpringConstant(kLeftHipExtensorSpringConstant);
  muscleModel->setName(kLeftHipExtensorName);
  addForce(muscleModel);
  
  // Left Hip Flexor
  controller = new StepController(kControllerStepTime);
  controller->SetName(kLeftHipFlexorName, kControllerSuffix);
  muscleModel = new MuscleModel(controller, kLeftHipFlexorOrigin, 
      kLeftThighPartName, kLeftHipFlexorInsertion);
  muscleModel->setMinMuscleLength(kLeftHipFlexorMinLength);
  muscleModel->setDampingFactor(kLeftHipFlexorDampingFactor);
  muscleModel->setControlFactor(kLeftHipFlexorControlFactor);
  muscleModel->setSpringConstant(kLeftHipFlexorSpringConstant);
  muscleModel->setName(kLeftHipFlexorName);
  addForce(muscleModel);
  
  // Right Hip Extensor
  controller = new StepController(kControllerStepTime);
  controller->SetName(kRightHipExtensorName, kControllerSuffix);
  muscleModel = new MuscleModel(controller, kRightHipExtensorOrigin, 
      kRightThighPartName, kRightHipExtensorInsertion);
  muscleModel->setMinMuscleLength(kRightHipExtensorMinLength);
  muscleModel->setDampingFactor(kRightHipExtensorDampingFactor);
  muscleModel->setControlFactor(kRightHipExtensorControlFactor);
  muscleModel->setSpringConstant(kRightHipExtensorSpringConstant);
  muscleModel->setName(kRightHipExtensorName);
  addForce(muscleModel);
  
  // Right Hip Flexor
  controller = new StepController(kControllerStepTime);
  controller->SetName(kRightHipFlexorName, kControllerSuffix);
  muscleModel = new MuscleModel(controller, kRightHipFlexorOrigin, 
      kRightThighPartName, kRightHipFlexorInsertion);
  muscleModel->setMinMuscleLength(kRightHipFlexorMinLength);
  muscleModel->setDampingFactor(kRightHipFlexorDampingFactor);
  muscleModel->setControlFactor(kRightHipFlexorControlFactor);
  muscleModel->setSpringConstant(kRightHipFlexorSpringConstant);
  muscleModel->setName(kRightHipFlexorName);
  addForce(muscleModel);
  
}

Torso::~Torso()
{
}


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
  
  StepController *controller;
  MuscleModel *muscleModel;

  // Left Hip Extensor
  controller = new StepController(kControllerStepTime);
  controller->SetName(kLeftHipExtensorName, kControllerSuffix);
  muscleModel = new MuscleModel(controller, kLeftHipExtensorInsertion, 
      kTorsoPartName, kLeftHipExtensorOrigin);
  muscleModel->setMinMuscleLength(kLeftHipExtensorMinLength);
  muscleModel->setDampingFactor(kLeftHipExtensorDampingFactor);
  muscleModel->setControlFactor(kLeftHipExtensorControlFactor);
  muscleModel->setSpringConstant(kLeftHipExtensorSpringConstant);
  muscleModel->setName(kLeftHipExtensorName);
  addForce(muscleModel);
  
  // Left Hip Flexor
  controller = new StepController(kControllerStepTime);
  controller->SetName(kLeftHipFlexorName, kControllerSuffix);
  muscleModel = new MuscleModel(controller, kLeftHipFlexorInsertion, 
      kTorsoPartName, kLeftHipFlexorOrigin);
  muscleModel->setMinMuscleLength(kLeftHipFlexorMinLength);
  muscleModel->setDampingFactor(kLeftHipFlexorDampingFactor);
  muscleModel->setControlFactor(kLeftHipFlexorControlFactor);
  muscleModel->setSpringConstant(kLeftHipFlexorSpringConstant);
  muscleModel->setName(kLeftHipFlexorName);
  addForce(muscleModel);

  // Left Knee Extensor
  controller = new StepController(kControllerStepTime);
  controller->SetName(kLeftKneeExtensorName, kControllerSuffix);
  muscleModel = new MuscleModel(controller, kLeftKneeExtensorOrigin, 
      kLeftLegPartName, kLeftKneeExtensorInsertion);
  muscleModel->setMinMuscleLength(kLeftKneeExtensorMinLength);
  muscleModel->setDampingFactor(kLeftKneeExtensorDampingFactor);
  muscleModel->setControlFactor(kLeftKneeExtensorControlFactor);
  muscleModel->setSpringConstant(kLeftKneeExtensorSpringConstant);
  muscleModel->setName(kLeftKneeExtensorName);
  addForce(muscleModel);
  
  // Left Knee Flexor
  controller = new StepController(kControllerStepTime);
  controller->SetName(kLeftKneeFlexorName, kControllerSuffix);
  muscleModel = new MuscleModel(controller, kLeftKneeFlexorOrigin, 
      kLeftLegPartName, kLeftKneeFlexorInsertion);
  muscleModel->setMinMuscleLength(kLeftKneeFlexorMinLength);
  muscleModel->setDampingFactor(kLeftKneeFlexorDampingFactor);
  muscleModel->setControlFactor(kLeftKneeFlexorControlFactor);
  muscleModel->setSpringConstant(kLeftKneeFlexorSpringConstant);
  muscleModel->setName(kLeftKneeFlexorName);
  addForce(muscleModel);
  
}

LeftThigh::~LeftThigh()
{
}

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
  
  StepController *controller;
  MuscleModel *muscleModel;

  // Right Hip Extensor
  controller = new StepController(kControllerStepTime);
  controller->SetName(kRightHipExtensorName, kControllerSuffix);
  muscleModel = new MuscleModel(controller, kRightHipExtensorInsertion, 
      kTorsoPartName, kRightHipExtensorOrigin);
  muscleModel->setMinMuscleLength(kRightHipExtensorMinLength);
  muscleModel->setDampingFactor(kRightHipExtensorDampingFactor);
  muscleModel->setControlFactor(kRightHipExtensorControlFactor);
  muscleModel->setSpringConstant(kRightHipExtensorSpringConstant);
  muscleModel->setName(kRightHipExtensorName);
  addForce(muscleModel);
  
  // Right Hip Flexor
  controller = new StepController(kControllerStepTime);
  controller->SetName(kRightHipFlexorName, kControllerSuffix);
  muscleModel = new MuscleModel(controller, kRightHipFlexorInsertion, 
      kTorsoPartName, kRightHipFlexorOrigin);
  muscleModel->setMinMuscleLength(kRightHipFlexorMinLength);
  muscleModel->setDampingFactor(kRightHipFlexorDampingFactor);
  muscleModel->setControlFactor(kRightHipFlexorControlFactor);
  muscleModel->setSpringConstant(kRightHipFlexorSpringConstant);
  muscleModel->setName(kRightHipFlexorName);
  addForce(muscleModel);

  // Right Knee Extensor
  controller = new StepController(kControllerStepTime);
  controller->SetName(kRightKneeExtensorName, kControllerSuffix);
  muscleModel = new MuscleModel(controller, kRightKneeExtensorOrigin, 
      kRightLegPartName, kRightKneeExtensorInsertion);
  muscleModel->setMinMuscleLength(kRightKneeExtensorMinLength);
  muscleModel->setDampingFactor(kRightKneeExtensorDampingFactor);
  muscleModel->setControlFactor(kRightKneeExtensorControlFactor);
  muscleModel->setSpringConstant(kRightKneeExtensorSpringConstant);
  muscleModel->setName(kRightKneeExtensorName);
  addForce(muscleModel);
  
  // Right Knee Flexor
  controller = new StepController(kControllerStepTime);
  controller->SetName(kRightKneeFlexorName, kControllerSuffix);
  muscleModel = new MuscleModel(controller, kRightKneeFlexorOrigin, 
      kRightLegPartName, kRightKneeFlexorInsertion);
  muscleModel->setMinMuscleLength(kRightKneeFlexorMinLength);
  muscleModel->setDampingFactor(kRightKneeFlexorDampingFactor);
  muscleModel->setControlFactor(kRightKneeFlexorControlFactor);
  muscleModel->setSpringConstant(kRightKneeFlexorSpringConstant);
  muscleModel->setName(kRightKneeFlexorName);
  addForce(muscleModel);
  
}

RightThigh::~RightThigh()
{
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
  
  StepController *controller;
  MuscleModel *muscleModel;

  // Left Knee Extensor
  controller = new StepController(kControllerStepTime);
  controller->SetName(kLeftKneeExtensorName, kControllerSuffix);
  muscleModel = new MuscleModel(controller, kLeftKneeExtensorInsertion, 
      kLeftThighPartName, kLeftKneeExtensorOrigin);
  muscleModel->setMinMuscleLength(kLeftKneeExtensorMinLength);
  muscleModel->setDampingFactor(kLeftKneeExtensorDampingFactor);
  muscleModel->setControlFactor(kLeftKneeExtensorControlFactor);
  muscleModel->setSpringConstant(kLeftKneeExtensorSpringConstant);
  muscleModel->setName(kLeftKneeExtensorName);
  addForce(muscleModel);
  
  // Left Knee Flexor
  controller = new StepController(kControllerStepTime);
  controller->SetName(kLeftKneeFlexorName, kControllerSuffix);
  muscleModel = new MuscleModel(controller, kLeftKneeFlexorInsertion, 
      kLeftThighPartName, kLeftKneeFlexorOrigin);
  muscleModel->setMinMuscleLength(kLeftKneeFlexorMinLength);
  muscleModel->setDampingFactor(kLeftKneeFlexorDampingFactor);
  muscleModel->setControlFactor(kLeftKneeFlexorControlFactor);
  muscleModel->setSpringConstant(kLeftKneeFlexorSpringConstant);
  muscleModel->setName(kLeftKneeFlexorName);
  addForce(muscleModel);

  // Left Ankle Extensor
  controller = new StepController(kControllerStepTime);
  controller->SetName(kLeftAnkleExtensorName, kControllerSuffix);
  muscleModel = new MuscleModel(controller, kLeftAnkleExtensorOrigin, 
      kLeftFootPartName, kLeftAnkleExtensorInsertion);
  muscleModel->setMinMuscleLength(kLeftAnkleExtensorMinLength);
  muscleModel->setDampingFactor(kLeftAnkleExtensorDampingFactor);
  muscleModel->setControlFactor(kLeftAnkleExtensorControlFactor);
  muscleModel->setSpringConstant(kLeftAnkleExtensorSpringConstant);
  muscleModel->setName(kLeftAnkleExtensorName);
  addForce(muscleModel);
  
  // Left Ankle Flexor
  controller = new StepController(kControllerStepTime);
  controller->SetName(kLeftAnkleFlexorName, kControllerSuffix);
  muscleModel = new MuscleModel(controller, kLeftAnkleFlexorOrigin, 
      kLeftFootPartName, kLeftAnkleFlexorInsertion);
  muscleModel->setMinMuscleLength(kLeftAnkleFlexorMinLength);
  muscleModel->setDampingFactor(kLeftAnkleFlexorDampingFactor);
  muscleModel->setControlFactor(kLeftAnkleFlexorControlFactor);
  muscleModel->setSpringConstant(kLeftAnkleFlexorSpringConstant);
  muscleModel->setName(kLeftAnkleFlexorName);
  addForce(muscleModel);
}

LeftLeg::~LeftLeg()
{
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
  
  StepController *controller;
  MuscleModel *muscleModel;

  // Right Knee Extensor
  controller = new StepController(kControllerStepTime);
  controller->SetName(kRightKneeExtensorName, kControllerSuffix);
  muscleModel = new MuscleModel(controller, kRightKneeExtensorInsertion, 
      kRightThighPartName, kRightKneeExtensorOrigin);
  muscleModel->setMinMuscleLength(kRightKneeExtensorMinLength);
  muscleModel->setDampingFactor(kRightKneeExtensorDampingFactor);
  muscleModel->setControlFactor(kRightKneeExtensorControlFactor);
  muscleModel->setSpringConstant(kRightKneeExtensorSpringConstant);
  muscleModel->setName(kRightKneeExtensorName);
  addForce(muscleModel);
  
  // Right Knee Flexor
  controller = new StepController(kControllerStepTime);
  controller->SetName(kRightKneeFlexorName, kControllerSuffix);
  muscleModel = new MuscleModel(controller, kRightKneeFlexorInsertion, 
      kRightThighPartName, kRightKneeFlexorOrigin);
  muscleModel->setMinMuscleLength(kRightKneeFlexorMinLength);
  muscleModel->setDampingFactor(kRightKneeFlexorDampingFactor);
  muscleModel->setControlFactor(kRightKneeFlexorControlFactor);
  muscleModel->setSpringConstant(kRightKneeFlexorSpringConstant);
  muscleModel->setName(kRightKneeFlexorName);
  addForce(muscleModel);

  // Right Ankle Extensor
  controller = new StepController(kControllerStepTime);
  controller->SetName(kRightAnkleExtensorName, kControllerSuffix);
  muscleModel = new MuscleModel(controller, kRightAnkleExtensorOrigin, 
      kRightFootPartName, kRightAnkleExtensorInsertion);
  muscleModel->setMinMuscleLength(kRightAnkleExtensorMinLength);
  muscleModel->setDampingFactor(kRightAnkleExtensorDampingFactor);
  muscleModel->setControlFactor(kRightAnkleExtensorControlFactor);
  muscleModel->setSpringConstant(kRightAnkleExtensorSpringConstant);
  muscleModel->setName(kRightAnkleExtensorName);
  addForce(muscleModel);
  
  // Right Ankle Flexor
  controller = new StepController(kControllerStepTime);
  controller->SetName(kRightAnkleFlexorName, kControllerSuffix);
  muscleModel = new MuscleModel(controller, kRightAnkleFlexorOrigin, 
      kRightFootPartName, kRightAnkleFlexorInsertion);
  muscleModel->setMinMuscleLength(kRightAnkleFlexorMinLength);
  muscleModel->setDampingFactor(kRightAnkleFlexorDampingFactor);
  muscleModel->setControlFactor(kRightAnkleFlexorControlFactor);
  muscleModel->setSpringConstant(kRightAnkleFlexorSpringConstant);
  muscleModel->setName(kRightAnkleFlexorName);
  addForce(muscleModel);
}

RightLeg::~RightLeg()
{
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
  
  StepController *controller;
  MuscleModel *muscleModel;

  // Left Ankle Extensor
  controller = new StepController(kControllerStepTime);
  controller->SetName(kLeftAnkleExtensorName, kControllerSuffix);
  muscleModel = new MuscleModel(controller, kLeftAnkleExtensorInsertion, 
      kLeftLegPartName, kLeftAnkleExtensorOrigin);
  muscleModel->setMinMuscleLength(kLeftAnkleExtensorMinLength);
  muscleModel->setDampingFactor(kLeftAnkleExtensorDampingFactor);
  muscleModel->setControlFactor(kLeftAnkleExtensorControlFactor);
  muscleModel->setSpringConstant(kLeftAnkleExtensorSpringConstant);
  muscleModel->setName(kLeftAnkleExtensorName);
  addForce(muscleModel);
  
  // Left Ankle Flexor
  controller = new StepController(kControllerStepTime);
  controller->SetName(kLeftAnkleFlexorName, kControllerSuffix);
  muscleModel = new MuscleModel(controller, kLeftAnkleFlexorInsertion, 
      kLeftLegPartName, kLeftAnkleFlexorOrigin);
  muscleModel->setMinMuscleLength(kLeftAnkleFlexorMinLength);
  muscleModel->setDampingFactor(kLeftAnkleFlexorDampingFactor);
  muscleModel->setControlFactor(kLeftAnkleFlexorControlFactor);
  muscleModel->setSpringConstant(kLeftAnkleFlexorSpringConstant);
  muscleModel->setName(kLeftAnkleFlexorName);
  addForce(muscleModel);

  // Contact Points
  ModifiedContactModel *contactModel = new ModifiedContactModel();
  contactModel->setContactPoints(kLeftFootNumContactPoints, kLeftFootContactPositions);
  contactModel->setName(kLeftFootContactName);
  addForce(contactModel);
}

LeftFoot::~LeftFoot()
{
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
  
  StepController *controller;
  MuscleModel *muscleModel;

  // Right Ankle Extensor
  controller = new StepController(kControllerStepTime);
  controller->SetName(kRightAnkleExtensorName, kControllerSuffix);
  muscleModel = new MuscleModel(controller, kRightAnkleExtensorInsertion, 
      kRightLegPartName, kRightAnkleExtensorOrigin);
  muscleModel->setMinMuscleLength(kRightAnkleExtensorMinLength);
  muscleModel->setDampingFactor(kRightAnkleExtensorDampingFactor);
  muscleModel->setControlFactor(kRightAnkleExtensorControlFactor);
  muscleModel->setSpringConstant(kRightAnkleExtensorSpringConstant);
  muscleModel->setName(kRightAnkleExtensorName);
  addForce(muscleModel);
  
  // Right Ankle Flexor
  controller = new StepController(kControllerStepTime);
  controller->SetName(kRightAnkleFlexorName, kControllerSuffix);
  muscleModel = new MuscleModel(controller, kRightAnkleFlexorInsertion, 
      kRightLegPartName, kRightAnkleFlexorOrigin);
  muscleModel->setMinMuscleLength(kRightAnkleFlexorMinLength);
  muscleModel->setDampingFactor(kRightAnkleFlexorDampingFactor);
  muscleModel->setControlFactor(kRightAnkleFlexorControlFactor);
  muscleModel->setSpringConstant(kRightAnkleFlexorSpringConstant);
  muscleModel->setName(kRightAnkleFlexorName);
  addForce(muscleModel);

  // Contact Points
  ModifiedContactModel *contactModel = new ModifiedContactModel();
  contactModel->setContactPoints(kRightFootNumContactPoints, kRightFootContactPositions);
  contactModel->setName(kRightFootContactName);
  addForce(contactModel);
    
}

RightFoot::~RightFoot()
{
}

void ReadSegmentGlobals(ParameterFile &file)
{
  // -------------------------------------------
  // Defaults
  // -------------------------------------------
  
  file.RetrieveParameter("kDefaultJointLimitSpringConstant", &kDefaultJointLimitSpringConstant);
  file.RetrieveParameter("kDefaultJointLimitDamperConstant", &kDefaultJointLimitDamperConstant);
  file.RetrieveParameter("kDefaultJointFriction", &kDefaultJointFriction);

  // -------------------------------------------
  // Links
  // -------------------------------------------

  // -------------------------------------------
  // Torso
  file.RetrieveQuotedStringParameter("kTorsoPartName", kTorsoPartName, SEG_PARAM_NAME_LENGTH);
  file.RetrieveQuotedStringParameter("kTorsoGraphicFile", kTorsoGraphicFile, SEG_PARAM_NAME_LENGTH);
  file.RetrieveParameter("kTorsoMass", &kTorsoMass);
  file.RetrieveParameter("kTorsoMOI", 9, (double *)kTorsoMOI);
  file.RetrieveParameter("kTorsoCG", 3, kTorsoCG);
  file.RetrieveParameter("kTorsoPosition", 7, kTorsoPosition);
  file.RetrieveParameter("kTorsoVelocity", 6, kTorsoVelocity);
  
  // -------------------------------------------
  // Left Thigh
  file.RetrieveQuotedStringParameter("kLeftThighPartName", kLeftThighPartName, SEG_PARAM_NAME_LENGTH);
  file.RetrieveQuotedStringParameter("kLeftThighGraphicFile", kLeftThighGraphicFile, SEG_PARAM_NAME_LENGTH);
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
  file.RetrieveQuotedStringParameter("kRightThighPartName", kRightThighPartName, SEG_PARAM_NAME_LENGTH);
  file.RetrieveQuotedStringParameter("kRightThighGraphicFile", kRightThighGraphicFile, SEG_PARAM_NAME_LENGTH);
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
  file.RetrieveQuotedStringParameter("kLeftLegPartName", kLeftLegPartName, SEG_PARAM_NAME_LENGTH);
  file.RetrieveQuotedStringParameter("kLeftLegGraphicFile", kLeftLegGraphicFile, SEG_PARAM_NAME_LENGTH);
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
  file.RetrieveQuotedStringParameter("kRightLegPartName", kRightLegPartName, SEG_PARAM_NAME_LENGTH);
  file.RetrieveQuotedStringParameter("kRightLegGraphicFile", kRightLegGraphicFile, SEG_PARAM_NAME_LENGTH);
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
  file.RetrieveQuotedStringParameter("kLeftFootPartName", kLeftFootPartName, SEG_PARAM_NAME_LENGTH);
  file.RetrieveQuotedStringParameter("kLeftFootGraphicFile", kLeftFootGraphicFile, SEG_PARAM_NAME_LENGTH);
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
  file.RetrieveQuotedStringParameter("kRightFootPartName", kRightFootPartName, SEG_PARAM_NAME_LENGTH);
  file.RetrieveQuotedStringParameter("kRightFootGraphicFile", kRightFootGraphicFile, SEG_PARAM_NAME_LENGTH);
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
  file.RetrieveQuotedStringParameter("kLeftHipExtensorName", kLeftHipExtensorName, SEG_PARAM_NAME_LENGTH);
  file.RetrieveParameter("kLeftHipExtensorOrigin", 3, kLeftHipExtensorOrigin);
  file.RetrieveParameter("kLeftHipExtensorInsertion", 3, kLeftHipExtensorInsertion);
  file.RetrieveParameter("kLeftHipExtensorMinLength", &kLeftHipExtensorMinLength);
  file.RetrieveParameter("kLeftHipExtensorDampingFactor", &kLeftHipExtensorDampingFactor);
  file.RetrieveParameter("kLeftHipExtensorControlFactor", &kLeftHipExtensorControlFactor);
  file.RetrieveParameter("kLeftHipExtensorSpringConstant", &kLeftHipExtensorSpringConstant);

  // Left Hip Flexor
  file.RetrieveQuotedStringParameter("kLeftHipFlexorName", kLeftHipFlexorName, SEG_PARAM_NAME_LENGTH);
  file.RetrieveParameter("kLeftHipFlexorOrigin", 3, kLeftHipFlexorOrigin);
  file.RetrieveParameter("kLeftHipFlexorInsertion", 3, kLeftHipFlexorInsertion);
  file.RetrieveParameter("kLeftHipFlexorMinLength", &kLeftHipFlexorMinLength);
  file.RetrieveParameter("kLeftHipFlexorDampingFactor", &kLeftHipFlexorDampingFactor);
  file.RetrieveParameter("kLeftHipFlexorControlFactor", &kLeftHipFlexorControlFactor);
  file.RetrieveParameter("kLeftHipFlexorSpringConstant", &kLeftHipFlexorSpringConstant);

  // Right Hip Extensor
  file.RetrieveQuotedStringParameter("kRightHipExtensorName", kRightHipExtensorName, SEG_PARAM_NAME_LENGTH);
  file.RetrieveParameter("kRightHipExtensorOrigin", 3, kRightHipExtensorOrigin);
  file.RetrieveParameter("kRightHipExtensorInsertion", 3, kRightHipExtensorInsertion);
  file.RetrieveParameter("kRightHipExtensorMinLength", &kRightHipExtensorMinLength);
  file.RetrieveParameter("kRightHipExtensorDampingFactor", &kRightHipExtensorDampingFactor);
  file.RetrieveParameter("kRightHipExtensorControlFactor", &kRightHipExtensorControlFactor);
  file.RetrieveParameter("kRightHipExtensorSpringConstant", &kRightHipExtensorSpringConstant);

  // Right Hip Flexor
  file.RetrieveQuotedStringParameter("kRightHipFlexorName", kRightHipFlexorName, SEG_PARAM_NAME_LENGTH);
  file.RetrieveParameter("kRightHipFlexorOrigin", 3, kRightHipFlexorOrigin);
  file.RetrieveParameter("kRightHipFlexorInsertion", 3, kRightHipFlexorInsertion);
  file.RetrieveParameter("kRightHipFlexorMinLength", &kRightHipFlexorMinLength);
  file.RetrieveParameter("kRightHipFlexorDampingFactor", &kRightHipFlexorDampingFactor);
  file.RetrieveParameter("kRightHipFlexorControlFactor", &kRightHipFlexorControlFactor);
  file.RetrieveParameter("kRightHipFlexorSpringConstant", &kRightHipFlexorSpringConstant);

  // Left Knee Extensor
  file.RetrieveQuotedStringParameter("kLeftKneeExtensorName", kLeftKneeExtensorName, SEG_PARAM_NAME_LENGTH);
  file.RetrieveParameter("kLeftKneeExtensorOrigin", 3, kLeftKneeExtensorOrigin);
  file.RetrieveParameter("kLeftKneeExtensorInsertion", 3, kLeftKneeExtensorInsertion);
  file.RetrieveParameter("kLeftKneeExtensorMinLength", &kLeftKneeExtensorMinLength);
  file.RetrieveParameter("kLeftKneeExtensorDampingFactor", &kLeftKneeExtensorDampingFactor);
  file.RetrieveParameter("kLeftKneeExtensorControlFactor", &kLeftKneeExtensorControlFactor);
  file.RetrieveParameter("kLeftKneeExtensorSpringConstant", &kLeftKneeExtensorSpringConstant);

  // Left Knee Flexor
  file.RetrieveQuotedStringParameter("kLeftKneeFlexorName", kLeftKneeFlexorName, SEG_PARAM_NAME_LENGTH);
  file.RetrieveParameter("kLeftKneeFlexorOrigin", 3, kLeftKneeFlexorOrigin);
  file.RetrieveParameter("kLeftKneeFlexorInsertion", 3, kLeftKneeFlexorInsertion);
  file.RetrieveParameter("kLeftKneeFlexorMinLength", &kLeftKneeFlexorMinLength);
  file.RetrieveParameter("kLeftKneeFlexorDampingFactor", &kLeftKneeFlexorDampingFactor);
  file.RetrieveParameter("kLeftKneeFlexorControlFactor", &kLeftKneeFlexorControlFactor);
  file.RetrieveParameter("kLeftKneeFlexorSpringConstant", &kLeftKneeFlexorSpringConstant);

  // Right Knee Extensor
  file.RetrieveQuotedStringParameter("kRightKneeExtensorName", kRightKneeExtensorName, SEG_PARAM_NAME_LENGTH);
  file.RetrieveParameter("kRightKneeExtensorOrigin", 3, kRightKneeExtensorOrigin);
  file.RetrieveParameter("kRightKneeExtensorInsertion", 3, kRightKneeExtensorInsertion);
  file.RetrieveParameter("kRightKneeExtensorMinLength", &kRightKneeExtensorMinLength);
  file.RetrieveParameter("kRightKneeExtensorDampingFactor", &kRightKneeExtensorDampingFactor);
  file.RetrieveParameter("kRightKneeExtensorControlFactor", &kRightKneeExtensorControlFactor);
  file.RetrieveParameter("kRightKneeExtensorSpringConstant", &kRightKneeExtensorSpringConstant);

  // Right Knee Flexor
  file.RetrieveQuotedStringParameter("kRightKneeFlexorName", kRightKneeFlexorName, SEG_PARAM_NAME_LENGTH);
  file.RetrieveParameter("kRightKneeFlexorOrigin", 3, kRightKneeFlexorOrigin);
  file.RetrieveParameter("kRightKneeFlexorInsertion", 3, kRightKneeFlexorInsertion);
  file.RetrieveParameter("kRightKneeFlexorMinLength", &kRightKneeFlexorMinLength);
  file.RetrieveParameter("kRightKneeFlexorDampingFactor", &kRightKneeFlexorDampingFactor);
  file.RetrieveParameter("kRightKneeFlexorControlFactor", &kRightKneeFlexorControlFactor);
  file.RetrieveParameter("kRightKneeFlexorSpringConstant", &kRightKneeFlexorSpringConstant);

  // Left Ankle Extensor
  file.RetrieveQuotedStringParameter("kLeftAnkleExtensorName", kLeftAnkleExtensorName, SEG_PARAM_NAME_LENGTH);
  file.RetrieveParameter("kLeftAnkleExtensorOrigin", 3, kLeftAnkleExtensorOrigin);
  file.RetrieveParameter("kLeftAnkleExtensorInsertion", 3, kLeftAnkleExtensorInsertion);
  file.RetrieveParameter("kLeftAnkleExtensorMinLength", &kLeftAnkleExtensorMinLength);
  file.RetrieveParameter("kLeftAnkleExtensorDampingFactor", &kLeftAnkleExtensorDampingFactor);
  file.RetrieveParameter("kLeftAnkleExtensorControlFactor", &kLeftAnkleExtensorControlFactor);
  file.RetrieveParameter("kLeftAnkleExtensorSpringConstant", &kLeftAnkleExtensorSpringConstant);

  // Left Ankle Flexor
  file.RetrieveQuotedStringParameter("kLeftAnkleFlexorName", kLeftAnkleFlexorName, SEG_PARAM_NAME_LENGTH);
  file.RetrieveParameter("kLeftAnkleFlexorOrigin", 3, kLeftAnkleFlexorOrigin);
  file.RetrieveParameter("kLeftAnkleFlexorInsertion", 3, kLeftAnkleFlexorInsertion);
  file.RetrieveParameter("kLeftAnkleFlexorMinLength", &kLeftAnkleFlexorMinLength);
  file.RetrieveParameter("kLeftAnkleFlexorDampingFactor", &kLeftAnkleFlexorDampingFactor);
  file.RetrieveParameter("kLeftAnkleFlexorControlFactor", &kLeftAnkleFlexorControlFactor);
  file.RetrieveParameter("kLeftAnkleFlexorSpringConstant", &kLeftAnkleFlexorSpringConstant);

  // Right Ankle Extensor
  file.RetrieveQuotedStringParameter("kRightAnkleExtensorName", kRightAnkleExtensorName, SEG_PARAM_NAME_LENGTH);
  file.RetrieveParameter("kRightAnkleExtensorOrigin", 3, kRightAnkleExtensorOrigin);
  file.RetrieveParameter("kRightAnkleExtensorInsertion", 3, kRightAnkleExtensorInsertion);
  file.RetrieveParameter("kRightAnkleExtensorMinLength", &kRightAnkleExtensorMinLength);
  file.RetrieveParameter("kRightAnkleExtensorDampingFactor", &kRightAnkleExtensorDampingFactor);
  file.RetrieveParameter("kRightAnkleExtensorControlFactor", &kRightAnkleExtensorControlFactor);
  file.RetrieveParameter("kRightAnkleExtensorSpringConstant", &kRightAnkleExtensorSpringConstant);

  // Right Ankle Flexor
  file.RetrieveQuotedStringParameter("kRightAnkleFlexorName", kRightAnkleFlexorName, SEG_PARAM_NAME_LENGTH);
  file.RetrieveParameter("kRightAnkleFlexorOrigin", 3, kRightAnkleFlexorOrigin);
  file.RetrieveParameter("kRightAnkleFlexorInsertion", 3, kRightAnkleFlexorInsertion);
  file.RetrieveParameter("kRightAnkleFlexorMinLength", &kRightAnkleFlexorMinLength);
  file.RetrieveParameter("kRightAnkleFlexorDampingFactor", &kRightAnkleFlexorDampingFactor);
  file.RetrieveParameter("kRightAnkleFlexorControlFactor", &kRightAnkleFlexorControlFactor);
  file.RetrieveParameter("kRightAnkleFlexorSpringConstant", &kRightAnkleFlexorSpringConstant);

  // -------------------------------------------
  // Controller
  // -------------------------------------------

  file.RetrieveParameter("kControllerStepTime", &kControllerStepTime);
  file.RetrieveQuotedStringParameter("kControllerSuffix", kControllerSuffix, SEG_PARAM_NAME_LENGTH);

  // -------------------------------------------
  // Contact Models
  // -------------------------------------------

  // Left Foot

  file.RetrieveQuotedStringParameter("kLeftFootContactName", kLeftFootContactName, SEG_PARAM_NAME_LENGTH);
  file.RetrieveParameter("kLeftFootNumContactPoints", &kLeftFootNumContactPoints);
  // WARNING - memory leak possible
  kLeftFootContactPositions = new CartesianVector[kLeftFootNumContactPoints];
  file.RetrieveParameter("kLeftFootContactPositions", kLeftFootNumContactPoints * 3, (double *)kLeftFootContactPositions);
  
  // Right Foot

  file.RetrieveQuotedStringParameter("kRightFootContactName", kRightFootContactName, SEG_PARAM_NAME_LENGTH);
  file.RetrieveParameter("kRightFootNumContactPoints", &kRightFootNumContactPoints);
  // WARNING - memory leak possible
  kRightFootContactPositions = new CartesianVector[kRightFootNumContactPoints];
  file.RetrieveParameter("kRightFootContactPositions", kRightFootNumContactPoints * 3, (double *)kRightFootContactPositions);

}
