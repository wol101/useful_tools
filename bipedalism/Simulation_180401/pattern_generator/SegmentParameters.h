// SegmentParameters.h
// This file contains the required segment parameters for the model

#ifndef SEGMENTPARAMETERS_H
#define SEGMENTPARAMETERS_H

// cope with declaring globals extern
#ifndef SEG_PARAM_EXTERN
#define SEG_PARAM_EXTERN extern
#endif

const int SEG_PARAM_NAME_LENGTH = 256;

// some generic values

SEG_PARAM_EXTERN double kDefaultJointLimitSpringConstant;
SEG_PARAM_EXTERN double kDefaultJointLimitDamperConstant;
SEG_PARAM_EXTERN double kDefaultJointFriction;

// -------------------------------------------
// Links
// -------------------------------------------

// -------------------------------------------
// Torso
// -------------------------------------------
SEG_PARAM_EXTERN char kTorsoPartName[SEG_PARAM_NAME_LENGTH];
SEG_PARAM_EXTERN char kTorsoGraphicFile[SEG_PARAM_NAME_LENGTH];
SEG_PARAM_EXTERN double kTorsoMass;
SEG_PARAM_EXTERN CartesianTensor kTorsoMOI;
SEG_PARAM_EXTERN CartesianVector kTorsoCG;
SEG_PARAM_EXTERN double kTorsoPosition[7];
SEG_PARAM_EXTERN SpatialVector kTorsoVelocity;

// -------------------------------------------
// Left Thigh
// -------------------------------------------
SEG_PARAM_EXTERN char kLeftThighPartName[SEG_PARAM_NAME_LENGTH];
SEG_PARAM_EXTERN char kLeftThighGraphicFile[SEG_PARAM_NAME_LENGTH];
SEG_PARAM_EXTERN double kLeftThighMass;
SEG_PARAM_EXTERN CartesianTensor kLeftThighMOI;
SEG_PARAM_EXTERN CartesianVector kLeftThighCG;
SEG_PARAM_EXTERN double kLeftThighMDHA;
SEG_PARAM_EXTERN double kLeftThighMDHAlpha;
SEG_PARAM_EXTERN double kLeftThighMDHD;
SEG_PARAM_EXTERN double kLeftThighMDHTheta;
SEG_PARAM_EXTERN double kLeftThighInitialJointVelocity;
SEG_PARAM_EXTERN double kLeftThighJointMin;
SEG_PARAM_EXTERN double kLeftThighJointMax;
SEG_PARAM_EXTERN double kLeftThighJointLimitSpringConstant;
SEG_PARAM_EXTERN double kLeftThighJointLimitDamperConstant;
SEG_PARAM_EXTERN double kLeftThighJointFriction;

// -------------------------------------------
// Right Thigh
// -------------------------------------------
SEG_PARAM_EXTERN char kRightThighPartName[SEG_PARAM_NAME_LENGTH];
SEG_PARAM_EXTERN char kRightThighGraphicFile[SEG_PARAM_NAME_LENGTH];
SEG_PARAM_EXTERN double kRightThighMass;
SEG_PARAM_EXTERN CartesianTensor kRightThighMOI;
SEG_PARAM_EXTERN CartesianVector kRightThighCG;
SEG_PARAM_EXTERN double kRightThighMDHA;
SEG_PARAM_EXTERN double kRightThighMDHAlpha;
SEG_PARAM_EXTERN double kRightThighMDHD;
SEG_PARAM_EXTERN double kRightThighMDHTheta;
SEG_PARAM_EXTERN double kRightThighInitialJointVelocity;
SEG_PARAM_EXTERN double kRightThighJointMin;
SEG_PARAM_EXTERN double kRightThighJointMax;
SEG_PARAM_EXTERN double kRightThighJointLimitSpringConstant;
SEG_PARAM_EXTERN double kRightThighJointLimitDamperConstant;
SEG_PARAM_EXTERN double kRightThighJointFriction;

// -------------------------------------------
// Left Leg
// -------------------------------------------
SEG_PARAM_EXTERN char kLeftLegPartName[SEG_PARAM_NAME_LENGTH];
SEG_PARAM_EXTERN char kLeftLegGraphicFile[SEG_PARAM_NAME_LENGTH];
SEG_PARAM_EXTERN double kLeftLegMass;
SEG_PARAM_EXTERN CartesianTensor kLeftLegMOI;
SEG_PARAM_EXTERN CartesianVector kLeftLegCG;
SEG_PARAM_EXTERN double kLeftLegMDHA;
SEG_PARAM_EXTERN double kLeftLegMDHAlpha;
SEG_PARAM_EXTERN double kLeftLegMDHD;
SEG_PARAM_EXTERN double kLeftLegMDHTheta;
SEG_PARAM_EXTERN double kLeftLegInitialJointVelocity;
SEG_PARAM_EXTERN double kLeftLegJointMin;
SEG_PARAM_EXTERN double kLeftLegJointMax;
SEG_PARAM_EXTERN double kLeftLegJointLimitSpringConstant;
SEG_PARAM_EXTERN double kLeftLegJointLimitDamperConstant;
SEG_PARAM_EXTERN double kLeftLegJointFriction;

// -------------------------------------------
// Right Leg
// -------------------------------------------
SEG_PARAM_EXTERN char kRightLegPartName[SEG_PARAM_NAME_LENGTH];
SEG_PARAM_EXTERN char kRightLegGraphicFile[SEG_PARAM_NAME_LENGTH];
SEG_PARAM_EXTERN double kRightLegMass;
SEG_PARAM_EXTERN CartesianTensor kRightLegMOI;
SEG_PARAM_EXTERN CartesianVector kRightLegCG;
SEG_PARAM_EXTERN double kRightLegMDHA;
SEG_PARAM_EXTERN double kRightLegMDHAlpha;
SEG_PARAM_EXTERN double kRightLegMDHD;
SEG_PARAM_EXTERN double kRightLegMDHTheta;
SEG_PARAM_EXTERN double kRightLegInitialJointVelocity;
SEG_PARAM_EXTERN double kRightLegJointMin;
SEG_PARAM_EXTERN double kRightLegJointMax;
SEG_PARAM_EXTERN double kRightLegJointLimitSpringConstant;
SEG_PARAM_EXTERN double kRightLegJointLimitDamperConstant;
SEG_PARAM_EXTERN double kRightLegJointFriction;

// -------------------------------------------
// Left Foot
// -------------------------------------------
SEG_PARAM_EXTERN char kLeftFootPartName[SEG_PARAM_NAME_LENGTH];
SEG_PARAM_EXTERN char kLeftFootGraphicFile[SEG_PARAM_NAME_LENGTH];
SEG_PARAM_EXTERN double kLeftFootMass;
SEG_PARAM_EXTERN CartesianTensor kLeftFootMOI;
SEG_PARAM_EXTERN CartesianVector kLeftFootCG;
SEG_PARAM_EXTERN double kLeftFootMDHA;
SEG_PARAM_EXTERN double kLeftFootMDHAlpha;
SEG_PARAM_EXTERN double kLeftFootMDHD;
SEG_PARAM_EXTERN double kLeftFootMDHTheta;
SEG_PARAM_EXTERN double kLeftFootInitialJointVelocity;
SEG_PARAM_EXTERN double kLeftFootJointMin;
SEG_PARAM_EXTERN double kLeftFootJointMax;
SEG_PARAM_EXTERN double kLeftFootJointLimitSpringConstant;
SEG_PARAM_EXTERN double kLeftFootJointLimitDamperConstant;
SEG_PARAM_EXTERN double kLeftFootJointFriction;

// -------------------------------------------
// Right Foot
// -------------------------------------------
SEG_PARAM_EXTERN char kRightFootPartName[SEG_PARAM_NAME_LENGTH];
SEG_PARAM_EXTERN char kRightFootGraphicFile[SEG_PARAM_NAME_LENGTH];
SEG_PARAM_EXTERN double kRightFootMass;
SEG_PARAM_EXTERN CartesianTensor kRightFootMOI;
SEG_PARAM_EXTERN CartesianVector kRightFootCG;
SEG_PARAM_EXTERN double kRightFootMDHA;
SEG_PARAM_EXTERN double kRightFootMDHAlpha;
SEG_PARAM_EXTERN double kRightFootMDHD;
SEG_PARAM_EXTERN double kRightFootMDHTheta;
SEG_PARAM_EXTERN double kRightFootInitialJointVelocity;
SEG_PARAM_EXTERN double kRightFootJointMin;
SEG_PARAM_EXTERN double kRightFootJointMax;
SEG_PARAM_EXTERN double kRightFootJointLimitSpringConstant;
SEG_PARAM_EXTERN double kRightFootJointLimitDamperConstant;
SEG_PARAM_EXTERN double kRightFootJointFriction;

// -------------------------------------------
// Muscles
// -------------------------------------------

// Left Hip Extensor
SEG_PARAM_EXTERN char kLeftHipExtensorName[SEG_PARAM_NAME_LENGTH];
SEG_PARAM_EXTERN CartesianVector kLeftHipExtensorOrigin;
SEG_PARAM_EXTERN CartesianVector kLeftHipExtensorInsertion;
SEG_PARAM_EXTERN double kLeftHipExtensorMinLength;
SEG_PARAM_EXTERN double kLeftHipExtensorDampingFactor;
SEG_PARAM_EXTERN double kLeftHipExtensorControlFactor;
SEG_PARAM_EXTERN double kLeftHipExtensorSpringConstant;

// Left Hip Flexor
SEG_PARAM_EXTERN char kLeftHipFlexorName[SEG_PARAM_NAME_LENGTH];
SEG_PARAM_EXTERN CartesianVector kLeftHipFlexorOrigin;
SEG_PARAM_EXTERN CartesianVector kLeftHipFlexorInsertion;
SEG_PARAM_EXTERN double kLeftHipFlexorMinLength;
SEG_PARAM_EXTERN double kLeftHipFlexorDampingFactor;
SEG_PARAM_EXTERN double kLeftHipFlexorControlFactor;
SEG_PARAM_EXTERN double kLeftHipFlexorSpringConstant;

// Right Hip Extensor
SEG_PARAM_EXTERN char kRightHipExtensorName[SEG_PARAM_NAME_LENGTH];
SEG_PARAM_EXTERN CartesianVector kRightHipExtensorOrigin;
SEG_PARAM_EXTERN CartesianVector kRightHipExtensorInsertion;
SEG_PARAM_EXTERN double kRightHipExtensorMinLength;
SEG_PARAM_EXTERN double kRightHipExtensorDampingFactor;
SEG_PARAM_EXTERN double kRightHipExtensorControlFactor;
SEG_PARAM_EXTERN double kRightHipExtensorSpringConstant;

// Right Hip Flexor
SEG_PARAM_EXTERN char kRightHipFlexorName[SEG_PARAM_NAME_LENGTH];
SEG_PARAM_EXTERN CartesianVector kRightHipFlexorOrigin;
SEG_PARAM_EXTERN CartesianVector kRightHipFlexorInsertion;
SEG_PARAM_EXTERN double kRightHipFlexorMinLength;
SEG_PARAM_EXTERN double kRightHipFlexorDampingFactor;
SEG_PARAM_EXTERN double kRightHipFlexorControlFactor;
SEG_PARAM_EXTERN double kRightHipFlexorSpringConstant;

// Left Knee Extensor
SEG_PARAM_EXTERN char kLeftKneeExtensorName[SEG_PARAM_NAME_LENGTH];
SEG_PARAM_EXTERN CartesianVector kLeftKneeExtensorOrigin;
SEG_PARAM_EXTERN CartesianVector kLeftKneeExtensorInsertion;
SEG_PARAM_EXTERN double kLeftKneeExtensorMinLength;
SEG_PARAM_EXTERN double kLeftKneeExtensorDampingFactor;
SEG_PARAM_EXTERN double kLeftKneeExtensorControlFactor;
SEG_PARAM_EXTERN double kLeftKneeExtensorSpringConstant;

// Left Knee Flexor
SEG_PARAM_EXTERN char kLeftKneeFlexorName[SEG_PARAM_NAME_LENGTH];
SEG_PARAM_EXTERN CartesianVector kLeftKneeFlexorOrigin;
SEG_PARAM_EXTERN CartesianVector kLeftKneeFlexorInsertion;
SEG_PARAM_EXTERN double kLeftKneeFlexorMinLength;
SEG_PARAM_EXTERN double kLeftKneeFlexorDampingFactor;
SEG_PARAM_EXTERN double kLeftKneeFlexorControlFactor;
SEG_PARAM_EXTERN double kLeftKneeFlexorSpringConstant;

// Right Knee Extensor
SEG_PARAM_EXTERN char kRightKneeExtensorName[SEG_PARAM_NAME_LENGTH];
SEG_PARAM_EXTERN CartesianVector kRightKneeExtensorOrigin;
SEG_PARAM_EXTERN CartesianVector kRightKneeExtensorInsertion;
SEG_PARAM_EXTERN double kRightKneeExtensorMinLength;
SEG_PARAM_EXTERN double kRightKneeExtensorDampingFactor;
SEG_PARAM_EXTERN double kRightKneeExtensorControlFactor;
SEG_PARAM_EXTERN double kRightKneeExtensorSpringConstant;

// Right Knee Flexor
SEG_PARAM_EXTERN char kRightKneeFlexorName[SEG_PARAM_NAME_LENGTH];
SEG_PARAM_EXTERN CartesianVector kRightKneeFlexorOrigin;
SEG_PARAM_EXTERN CartesianVector kRightKneeFlexorInsertion;
SEG_PARAM_EXTERN double kRightKneeFlexorMinLength;
SEG_PARAM_EXTERN double kRightKneeFlexorDampingFactor;
SEG_PARAM_EXTERN double kRightKneeFlexorControlFactor;
SEG_PARAM_EXTERN double kRightKneeFlexorSpringConstant;

// Left Ankle Extensor (dorsiflexion)
SEG_PARAM_EXTERN char kLeftAnkleExtensorName[SEG_PARAM_NAME_LENGTH];
SEG_PARAM_EXTERN CartesianVector kLeftAnkleExtensorOrigin;
SEG_PARAM_EXTERN CartesianVector kLeftAnkleExtensorInsertion;
SEG_PARAM_EXTERN double kLeftAnkleExtensorMinLength;
SEG_PARAM_EXTERN double kLeftAnkleExtensorDampingFactor;
SEG_PARAM_EXTERN double kLeftAnkleExtensorControlFactor;
SEG_PARAM_EXTERN double kLeftAnkleExtensorSpringConstant;

// Left Ankle Flexor (plantarflexion)
SEG_PARAM_EXTERN char kLeftAnkleFlexorName[SEG_PARAM_NAME_LENGTH];
SEG_PARAM_EXTERN CartesianVector kLeftAnkleFlexorOrigin;
SEG_PARAM_EXTERN CartesianVector kLeftAnkleFlexorInsertion;
SEG_PARAM_EXTERN double kLeftAnkleFlexorMinLength;
SEG_PARAM_EXTERN double kLeftAnkleFlexorDampingFactor;
SEG_PARAM_EXTERN double kLeftAnkleFlexorControlFactor;
SEG_PARAM_EXTERN double kLeftAnkleFlexorSpringConstant;

// Right Ankle Extensor (dorsiflexion)
SEG_PARAM_EXTERN char kRightAnkleExtensorName[SEG_PARAM_NAME_LENGTH];
SEG_PARAM_EXTERN CartesianVector kRightAnkleExtensorOrigin;
SEG_PARAM_EXTERN CartesianVector kRightAnkleExtensorInsertion;
SEG_PARAM_EXTERN double kRightAnkleExtensorMinLength;
SEG_PARAM_EXTERN double kRightAnkleExtensorDampingFactor;
SEG_PARAM_EXTERN double kRightAnkleExtensorControlFactor;
SEG_PARAM_EXTERN double kRightAnkleExtensorSpringConstant;

// Right Ankle Flexor (plantarflexion)
SEG_PARAM_EXTERN char kRightAnkleFlexorName[SEG_PARAM_NAME_LENGTH];
SEG_PARAM_EXTERN CartesianVector kRightAnkleFlexorOrigin;
SEG_PARAM_EXTERN CartesianVector kRightAnkleFlexorInsertion;
SEG_PARAM_EXTERN double kRightAnkleFlexorMinLength;
SEG_PARAM_EXTERN double kRightAnkleFlexorDampingFactor;
SEG_PARAM_EXTERN double kRightAnkleFlexorControlFactor;
SEG_PARAM_EXTERN double kRightAnkleFlexorSpringConstant;

// -------------------------------------------
// Controller
// -------------------------------------------

SEG_PARAM_EXTERN double kControllerStepTime;
SEG_PARAM_EXTERN char kControllerSuffix[SEG_PARAM_NAME_LENGTH];

// -------------------------------------------
// Contact Models
// -------------------------------------------

// Left Foot

SEG_PARAM_EXTERN char kLeftFootContactName[SEG_PARAM_NAME_LENGTH];
SEG_PARAM_EXTERN int kLeftFootNumContactPoints;
SEG_PARAM_EXTERN CartesianVector *kLeftFootContactPositions;

// Right Foot

SEG_PARAM_EXTERN char kRightFootContactName[SEG_PARAM_NAME_LENGTH];
SEG_PARAM_EXTERN int kRightFootNumContactPoints;
SEG_PARAM_EXTERN CartesianVector *kRightFootContactPositions;

#endif // SEGMENTPARAMETERS_H
