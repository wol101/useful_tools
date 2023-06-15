// Segments.cc - the various segment objects

#include <string.h>
#include <glLoadModels.h>

#include "Segments.h"
#include "Simulation.h"
#include "SegmentParameters.h"
#include "StepController.h"
#include "ModifiedContactModel.h"
#include "MuscleModel.h"

// Note lots of casting away of const since the dynamechs prototypes are
// rarely const

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
    *dlist = glLoadModel((char *)kTorsoGraphicFile);
    setUserData((void *) dlist);
  }

  setInertiaParameters(kTorsoMass, (double (*)[3])kTorsoMOI, (double *)kTorsoCG);
  setState((double *)kTorsoPosition, (double *)kTorsoVelocity);
  
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
    *dlist = glLoadModel((char *)kLeftThighGraphicFile);
    setUserData((void *) dlist);
  }

  setInertiaParameters(kLeftThighMass, (double (*)[3])kLeftThighMOI, (double *)kLeftThighCG);
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
    *dlist = glLoadModel((char *)kRightThighGraphicFile);
    setUserData((void *) dlist);
  }

  setInertiaParameters(kRightThighMass, (double (*)[3])kRightThighMOI, (double *)kRightThighCG);
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
    *dlist = glLoadModel((char *)kLeftLegGraphicFile);
    setUserData((void *) dlist);
  }

  setInertiaParameters(kLeftLegMass, (double (*)[3])kLeftLegMOI, (double *)kLeftLegCG);
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
    *dlist = glLoadModel((char *)kRightLegGraphicFile);
    setUserData((void *) dlist);
  }

  setInertiaParameters(kRightLegMass, (double (*)[3])kRightLegMOI, (double *)kRightLegCG);
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
    *dlist = glLoadModel((char *)kLeftFootGraphicFile);
    setUserData((void *) dlist);
  }

  setInertiaParameters(kLeftFootMass, (double (*)[3])kLeftFootMOI, (double *)kLeftFootCG);
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
  contactModel->setContactPoints(kLeftFootNumContactPoints, (CartesianVector *)kLeftFootContactPositions);
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
    *dlist = glLoadModel((char *)kRightFootGraphicFile);
    setUserData((void *) dlist);
  }

  setInertiaParameters(kRightFootMass, (double (*)[3])kRightFootMOI, (double *)kRightFootCG);
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
  contactModel->setContactPoints(kRightFootNumContactPoints, (CartesianVector *)kRightFootContactPositions);
  contactModel->setName(kRightFootContactName);
  addForce(contactModel);
    
}

RightFoot::~RightFoot()
{
}


