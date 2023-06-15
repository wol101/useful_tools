// SegmentParameters.h
// This file contains the required segment parameters for the model

#ifndef SEGMENTPARAMETERS_H
#define SEGMENTPARAMETERS_H

// cope with declaring globals extern
#ifndef SEG_PARAM_EXTERN
#define SEG_PARAM_EXTERN extern
#endif

// some generic values

SEG_PARAM_EXTERN double kDefaultJointLimitSpringConstant;
SEG_PARAM_EXTERN double kDefaultJointLimitDamperConstant;
SEG_PARAM_EXTERN double kDefaultJointFriction;
SEG_PARAM_EXTERN double kDefaultMuscleActivationK;
SEG_PARAM_EXTERN double kDefaultMuscleForcePerUnitArea;
SEG_PARAM_EXTERN double kDefaultMuscleVMaxFactor;

// -------------------------------------------
// Links
// -------------------------------------------

// -------------------------------------------
// Torso
// -------------------------------------------
SEG_PARAM_EXTERN char kTorsoPartName[256];
SEG_PARAM_EXTERN char kTorsoGraphicFile[256];
SEG_PARAM_EXTERN double kTorsoMass;
SEG_PARAM_EXTERN CartesianTensor kTorsoMOI;
SEG_PARAM_EXTERN CartesianVector kTorsoCG;
SEG_PARAM_EXTERN double kTorsoPosition[7];
SEG_PARAM_EXTERN SpatialVector kTorsoVelocity;

// -------------------------------------------
// Left Thigh
// -------------------------------------------
SEG_PARAM_EXTERN char kLeftThighPartName[256];
SEG_PARAM_EXTERN char kLeftThighGraphicFile[256];
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
SEG_PARAM_EXTERN char kRightThighPartName[256];
SEG_PARAM_EXTERN char kRightThighGraphicFile[256];
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
SEG_PARAM_EXTERN char kLeftLegPartName[256];
SEG_PARAM_EXTERN char kLeftLegGraphicFile[256];
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
SEG_PARAM_EXTERN char kRightLegPartName[256];
SEG_PARAM_EXTERN char kRightLegGraphicFile[256];
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
SEG_PARAM_EXTERN char kLeftFootPartName[256];
SEG_PARAM_EXTERN char kLeftFootGraphicFile[256];
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
SEG_PARAM_EXTERN char kRightFootPartName[256];
SEG_PARAM_EXTERN char kRightFootGraphicFile[256];
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
SEG_PARAM_EXTERN char kLeftHipExtensorName[256];
SEG_PARAM_EXTERN CartesianVector kLeftHipExtensorOrigin;
SEG_PARAM_EXTERN CartesianVector kLeftHipExtensorInsertion;
SEG_PARAM_EXTERN double kLeftHipExtensorPCA;
SEG_PARAM_EXTERN double kLeftHipExtensorLength;

// Left Hip Flexor
SEG_PARAM_EXTERN char kLeftHipFlexorName[256];
SEG_PARAM_EXTERN CartesianVector kLeftHipFlexorOrigin;
SEG_PARAM_EXTERN CartesianVector kLeftHipFlexorInsertion;
SEG_PARAM_EXTERN double kLeftHipFlexorPCA;
SEG_PARAM_EXTERN double kLeftHipFlexorLength;

// Right Hip Extensor
SEG_PARAM_EXTERN char kRightHipExtensorName[256];
SEG_PARAM_EXTERN CartesianVector kRightHipExtensorOrigin;
SEG_PARAM_EXTERN CartesianVector kRightHipExtensorInsertion;
SEG_PARAM_EXTERN double kRightHipExtensorPCA;
SEG_PARAM_EXTERN double kRightHipExtensorLength;

// Right Hip Flexor
SEG_PARAM_EXTERN char kRightHipFlexorName[256];
SEG_PARAM_EXTERN CartesianVector kRightHipFlexorOrigin;
SEG_PARAM_EXTERN CartesianVector kRightHipFlexorInsertion;
SEG_PARAM_EXTERN double kRightHipFlexorPCA;
SEG_PARAM_EXTERN double kRightHipFlexorLength;

// Left Knee Extensor
SEG_PARAM_EXTERN char kLeftKneeExtensorName[256];
SEG_PARAM_EXTERN CartesianVector kLeftKneeExtensorOrigin;
SEG_PARAM_EXTERN CartesianVector kLeftKneeExtensorMidPoint;
SEG_PARAM_EXTERN CartesianVector kLeftKneeExtensorInsertion;
SEG_PARAM_EXTERN double kLeftKneeExtensorPCA;
SEG_PARAM_EXTERN double kLeftKneeExtensorLength;

// Left Knee Flexor
SEG_PARAM_EXTERN char kLeftKneeFlexorName[256];
SEG_PARAM_EXTERN CartesianVector kLeftKneeFlexorOrigin;
SEG_PARAM_EXTERN CartesianVector kLeftKneeFlexorMidPoint;
SEG_PARAM_EXTERN CartesianVector kLeftKneeFlexorInsertion;
SEG_PARAM_EXTERN double kLeftKneeFlexorPCA;
SEG_PARAM_EXTERN double kLeftKneeFlexorLength;

// Right Knee Extensor
SEG_PARAM_EXTERN char kRightKneeExtensorName[256];
SEG_PARAM_EXTERN CartesianVector kRightKneeExtensorOrigin;
SEG_PARAM_EXTERN CartesianVector kRightKneeExtensorMidPoint;
SEG_PARAM_EXTERN CartesianVector kRightKneeExtensorInsertion;
SEG_PARAM_EXTERN double kRightKneeExtensorPCA;
SEG_PARAM_EXTERN double kRightKneeExtensorLength;

// Right Knee Flexor
SEG_PARAM_EXTERN char kRightKneeFlexorName[256];
SEG_PARAM_EXTERN CartesianVector kRightKneeFlexorOrigin;
SEG_PARAM_EXTERN CartesianVector kRightKneeFlexorMidPoint;
SEG_PARAM_EXTERN CartesianVector kRightKneeFlexorInsertion;
SEG_PARAM_EXTERN double kRightKneeFlexorPCA;
SEG_PARAM_EXTERN double kRightKneeFlexorLength;

// Left Ankle Extensor (dorsiflexion)
SEG_PARAM_EXTERN char kLeftAnkleExtensorName[256];
SEG_PARAM_EXTERN CartesianVector kLeftAnkleExtensorOrigin;
SEG_PARAM_EXTERN CartesianVector kLeftAnkleExtensorInsertion;
SEG_PARAM_EXTERN double kLeftAnkleExtensorPCA;
SEG_PARAM_EXTERN double kLeftAnkleExtensorLength;

// Left Ankle Flexor (plantarflexion)
SEG_PARAM_EXTERN char kLeftAnkleFlexorName[256];
SEG_PARAM_EXTERN CartesianVector kLeftAnkleFlexorOrigin;
SEG_PARAM_EXTERN CartesianVector kLeftAnkleFlexorInsertion;
SEG_PARAM_EXTERN double kLeftAnkleFlexorPCA;
SEG_PARAM_EXTERN double kLeftAnkleFlexorLength;

// Right Ankle Extensor (dorsiflexion)
SEG_PARAM_EXTERN char kRightAnkleExtensorName[256];
SEG_PARAM_EXTERN CartesianVector kRightAnkleExtensorOrigin;
SEG_PARAM_EXTERN CartesianVector kRightAnkleExtensorInsertion;
SEG_PARAM_EXTERN double kRightAnkleExtensorPCA;
SEG_PARAM_EXTERN double kRightAnkleExtensorLength;

// Right Ankle Flexor (plantarflexion)
SEG_PARAM_EXTERN char kRightAnkleFlexorName[256];
SEG_PARAM_EXTERN CartesianVector kRightAnkleFlexorOrigin;
SEG_PARAM_EXTERN CartesianVector kRightAnkleFlexorInsertion;
SEG_PARAM_EXTERN double kRightAnkleFlexorPCA;
SEG_PARAM_EXTERN double kRightAnkleFlexorLength;

// -------------------------------------------
// Contact Models
// -------------------------------------------

// Left Foot

SEG_PARAM_EXTERN char kLeftFootContactName[256];
SEG_PARAM_EXTERN int kLeftFootNumContactPoints;
SEG_PARAM_EXTERN CartesianVector *kLeftFootContactPositions;

// Right Foot

SEG_PARAM_EXTERN char kRightFootContactName[256];
SEG_PARAM_EXTERN int kRightFootNumContactPoints;
SEG_PARAM_EXTERN CartesianVector *kRightFootContactPositions;

#endif // SEGMENTPARAMETERS_H
