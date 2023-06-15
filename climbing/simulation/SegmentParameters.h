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
// Left Arm
// -------------------------------------------
SEG_PARAM_EXTERN char kLeftArmPartName[256];
SEG_PARAM_EXTERN char kLeftArmGraphicFile[256];
SEG_PARAM_EXTERN double kLeftArmMass;
SEG_PARAM_EXTERN CartesianTensor kLeftArmMOI;
SEG_PARAM_EXTERN CartesianVector kLeftArmCG;
SEG_PARAM_EXTERN double kLeftArmMDHA;
SEG_PARAM_EXTERN double kLeftArmMDHAlpha;
SEG_PARAM_EXTERN double kLeftArmMDHD;
SEG_PARAM_EXTERN double kLeftArmMDHTheta;
SEG_PARAM_EXTERN double kLeftArmInitialJointVelocity;
SEG_PARAM_EXTERN double kLeftArmJointMin;
SEG_PARAM_EXTERN double kLeftArmJointMax;
SEG_PARAM_EXTERN double kLeftArmJointLimitSpringConstant;
SEG_PARAM_EXTERN double kLeftArmJointLimitDamperConstant;
SEG_PARAM_EXTERN double kLeftArmJointFriction;

// -------------------------------------------
// Right Arm
// -------------------------------------------
SEG_PARAM_EXTERN char kRightArmPartName[256];
SEG_PARAM_EXTERN char kRightArmGraphicFile[256];
SEG_PARAM_EXTERN double kRightArmMass;
SEG_PARAM_EXTERN CartesianTensor kRightArmMOI;
SEG_PARAM_EXTERN CartesianVector kRightArmCG;
SEG_PARAM_EXTERN double kRightArmMDHA;
SEG_PARAM_EXTERN double kRightArmMDHAlpha;
SEG_PARAM_EXTERN double kRightArmMDHD;
SEG_PARAM_EXTERN double kRightArmMDHTheta;
SEG_PARAM_EXTERN double kRightArmInitialJointVelocity;
SEG_PARAM_EXTERN double kRightArmJointMin;
SEG_PARAM_EXTERN double kRightArmJointMax;
SEG_PARAM_EXTERN double kRightArmJointLimitSpringConstant;
SEG_PARAM_EXTERN double kRightArmJointLimitDamperConstant;
SEG_PARAM_EXTERN double kRightArmJointFriction;

// -------------------------------------------
// Left Forearm
// -------------------------------------------
SEG_PARAM_EXTERN char kLeftForearmPartName[256];
SEG_PARAM_EXTERN char kLeftForearmGraphicFile[256];
SEG_PARAM_EXTERN double kLeftForearmMass;
SEG_PARAM_EXTERN CartesianTensor kLeftForearmMOI;
SEG_PARAM_EXTERN CartesianVector kLeftForearmCG;
SEG_PARAM_EXTERN double kLeftForearmMDHA;
SEG_PARAM_EXTERN double kLeftForearmMDHAlpha;
SEG_PARAM_EXTERN double kLeftForearmMDHD;
SEG_PARAM_EXTERN double kLeftForearmMDHTheta;
SEG_PARAM_EXTERN double kLeftForearmInitialJointVelocity;
SEG_PARAM_EXTERN double kLeftForearmJointMin;
SEG_PARAM_EXTERN double kLeftForearmJointMax;
SEG_PARAM_EXTERN double kLeftForearmJointLimitSpringConstant;
SEG_PARAM_EXTERN double kLeftForearmJointLimitDamperConstant;
SEG_PARAM_EXTERN double kLeftForearmJointFriction;

// -------------------------------------------
// Right Forearm
// -------------------------------------------
SEG_PARAM_EXTERN char kRightForearmPartName[256];
SEG_PARAM_EXTERN char kRightForearmGraphicFile[256];
SEG_PARAM_EXTERN double kRightForearmMass;
SEG_PARAM_EXTERN CartesianTensor kRightForearmMOI;
SEG_PARAM_EXTERN CartesianVector kRightForearmCG;
SEG_PARAM_EXTERN double kRightForearmMDHA;
SEG_PARAM_EXTERN double kRightForearmMDHAlpha;
SEG_PARAM_EXTERN double kRightForearmMDHD;
SEG_PARAM_EXTERN double kRightForearmMDHTheta;
SEG_PARAM_EXTERN double kRightForearmInitialJointVelocity;
SEG_PARAM_EXTERN double kRightForearmJointMin;
SEG_PARAM_EXTERN double kRightForearmJointMax;
SEG_PARAM_EXTERN double kRightForearmJointLimitSpringConstant;
SEG_PARAM_EXTERN double kRightForearmJointLimitDamperConstant;
SEG_PARAM_EXTERN double kRightForearmJointFriction;

// -------------------------------------------
// Left Hand
// -------------------------------------------
SEG_PARAM_EXTERN char kLeftHandPartName[256];
SEG_PARAM_EXTERN char kLeftHandGraphicFile[256];
SEG_PARAM_EXTERN double kLeftHandMass;
SEG_PARAM_EXTERN CartesianTensor kLeftHandMOI;
SEG_PARAM_EXTERN CartesianVector kLeftHandCG;
SEG_PARAM_EXTERN double kLeftHandMDHA;
SEG_PARAM_EXTERN double kLeftHandMDHAlpha;
SEG_PARAM_EXTERN double kLeftHandMDHD;
SEG_PARAM_EXTERN double kLeftHandMDHTheta;
SEG_PARAM_EXTERN double kLeftHandInitialJointVelocity;
SEG_PARAM_EXTERN double kLeftHandJointMin;
SEG_PARAM_EXTERN double kLeftHandJointMax;
SEG_PARAM_EXTERN double kLeftHandJointLimitSpringConstant;
SEG_PARAM_EXTERN double kLeftHandJointLimitDamperConstant;
SEG_PARAM_EXTERN double kLeftHandJointFriction;

// -------------------------------------------
// Right Hand
// -------------------------------------------
SEG_PARAM_EXTERN char kRightHandPartName[256];
SEG_PARAM_EXTERN char kRightHandGraphicFile[256];
SEG_PARAM_EXTERN double kRightHandMass;
SEG_PARAM_EXTERN CartesianTensor kRightHandMOI;
SEG_PARAM_EXTERN CartesianVector kRightHandCG;
SEG_PARAM_EXTERN double kRightHandMDHA;
SEG_PARAM_EXTERN double kRightHandMDHAlpha;
SEG_PARAM_EXTERN double kRightHandMDHD;
SEG_PARAM_EXTERN double kRightHandMDHTheta;
SEG_PARAM_EXTERN double kRightHandInitialJointVelocity;
SEG_PARAM_EXTERN double kRightHandJointMin;
SEG_PARAM_EXTERN double kRightHandJointMax;
SEG_PARAM_EXTERN double kRightHandJointLimitSpringConstant;
SEG_PARAM_EXTERN double kRightHandJointLimitDamperConstant;
SEG_PARAM_EXTERN double kRightHandJointFriction;

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

// Left Shoulder Abductor
SEG_PARAM_EXTERN char kLeftShoulderAbductorName[256];
SEG_PARAM_EXTERN CartesianVector kLeftShoulderAbductorOrigin;
SEG_PARAM_EXTERN CartesianVector kLeftShoulderAbductorMidPoint;
SEG_PARAM_EXTERN CartesianVector kLeftShoulderAbductorInsertion;
SEG_PARAM_EXTERN double kLeftShoulderAbductorPCA;
SEG_PARAM_EXTERN double kLeftShoulderAbductorLength;

// Left Shoulder Extensor
SEG_PARAM_EXTERN char kLeftShoulderExtensorName[256];
SEG_PARAM_EXTERN CartesianVector kLeftShoulderExtensorOrigin;
SEG_PARAM_EXTERN CartesianVector kLeftShoulderExtensorInsertion;
SEG_PARAM_EXTERN double kLeftShoulderExtensorPCA;
SEG_PARAM_EXTERN double kLeftShoulderExtensorLength;

// Left Shoulder Flexor
SEG_PARAM_EXTERN char kLeftShoulderFlexorName[256];
SEG_PARAM_EXTERN CartesianVector kLeftShoulderFlexorOrigin;
SEG_PARAM_EXTERN CartesianVector kLeftShoulderFlexorInsertion;
SEG_PARAM_EXTERN double kLeftShoulderFlexorPCA;
SEG_PARAM_EXTERN double kLeftShoulderFlexorLength;

// Right Shoulder Abductor
SEG_PARAM_EXTERN char kRightShoulderAbductorName[256];
SEG_PARAM_EXTERN CartesianVector kRightShoulderAbductorOrigin;
SEG_PARAM_EXTERN CartesianVector kRightShoulderAbductorMidPoint;
SEG_PARAM_EXTERN CartesianVector kRightShoulderAbductorInsertion;
SEG_PARAM_EXTERN double kRightShoulderAbductorPCA;
SEG_PARAM_EXTERN double kRightShoulderAbductorLength;

// Right Shoulder Extensor
SEG_PARAM_EXTERN char kRightShoulderExtensorName[256];
SEG_PARAM_EXTERN CartesianVector kRightShoulderExtensorOrigin;
SEG_PARAM_EXTERN CartesianVector kRightShoulderExtensorInsertion;
SEG_PARAM_EXTERN double kRightShoulderExtensorPCA;
SEG_PARAM_EXTERN double kRightShoulderExtensorLength;

// Right Shoulder Flexor
SEG_PARAM_EXTERN char kRightShoulderFlexorName[256];
SEG_PARAM_EXTERN CartesianVector kRightShoulderFlexorOrigin;
SEG_PARAM_EXTERN CartesianVector kRightShoulderFlexorInsertion;
SEG_PARAM_EXTERN double kRightShoulderFlexorPCA;
SEG_PARAM_EXTERN double kRightShoulderFlexorLength;

// Left Elbow Extensor
SEG_PARAM_EXTERN char kLeftElbowExtensorName[256];
SEG_PARAM_EXTERN CartesianVector kLeftElbowExtensorOrigin;
SEG_PARAM_EXTERN CartesianVector kLeftElbowExtensorMidPoint;
SEG_PARAM_EXTERN CartesianVector kLeftElbowExtensorInsertion;
SEG_PARAM_EXTERN double kLeftElbowExtensorPCA;
SEG_PARAM_EXTERN double kLeftElbowExtensorLength;

// Left Elbow Flexor
SEG_PARAM_EXTERN char kLeftElbowFlexorName[256];
SEG_PARAM_EXTERN CartesianVector kLeftElbowFlexorOrigin;
SEG_PARAM_EXTERN CartesianVector kLeftElbowFlexorInsertion;
SEG_PARAM_EXTERN double kLeftElbowFlexorPCA;
SEG_PARAM_EXTERN double kLeftElbowFlexorLength;

// Right Elbow Extensor
SEG_PARAM_EXTERN char kRightElbowExtensorName[256];
SEG_PARAM_EXTERN CartesianVector kRightElbowExtensorOrigin;
SEG_PARAM_EXTERN CartesianVector kRightElbowExtensorMidPoint;
SEG_PARAM_EXTERN CartesianVector kRightElbowExtensorInsertion;
SEG_PARAM_EXTERN double kRightElbowExtensorPCA;
SEG_PARAM_EXTERN double kRightElbowExtensorLength;

// Right Elbow Flexor
SEG_PARAM_EXTERN char kRightElbowFlexorName[256];
SEG_PARAM_EXTERN CartesianVector kRightElbowFlexorOrigin;
SEG_PARAM_EXTERN CartesianVector kRightElbowFlexorInsertion;
SEG_PARAM_EXTERN double kRightElbowFlexorPCA;
SEG_PARAM_EXTERN double kRightElbowFlexorLength;

// Left Wrist Extensor (dorsiflexion)
SEG_PARAM_EXTERN char kLeftWristExtensorName[256];
SEG_PARAM_EXTERN CartesianVector kLeftWristExtensorOrigin;
SEG_PARAM_EXTERN CartesianVector kLeftWristExtensorMidPoint;
SEG_PARAM_EXTERN CartesianVector kLeftWristExtensorInsertion;
SEG_PARAM_EXTERN double kLeftWristExtensorPCA;
SEG_PARAM_EXTERN double kLeftWristExtensorLength;

// Left Wrist Flexor (plantarflexion)
SEG_PARAM_EXTERN char kLeftWristFlexorName[256];
SEG_PARAM_EXTERN CartesianVector kLeftWristFlexorOrigin;
SEG_PARAM_EXTERN CartesianVector kLeftWristFlexorMidPoint;
SEG_PARAM_EXTERN CartesianVector kLeftWristFlexorInsertion;
SEG_PARAM_EXTERN double kLeftWristFlexorPCA;
SEG_PARAM_EXTERN double kLeftWristFlexorLength;

// Right Wrist Extensor (dorsiflexion)
SEG_PARAM_EXTERN char kRightWristExtensorName[256];
SEG_PARAM_EXTERN CartesianVector kRightWristExtensorOrigin;
SEG_PARAM_EXTERN CartesianVector kRightWristExtensorMidPoint;
SEG_PARAM_EXTERN CartesianVector kRightWristExtensorInsertion;
SEG_PARAM_EXTERN double kRightWristExtensorPCA;
SEG_PARAM_EXTERN double kRightWristExtensorLength;

// Right Wrist Flexor (plantarflexion)
SEG_PARAM_EXTERN char kRightWristFlexorName[256];
SEG_PARAM_EXTERN CartesianVector kRightWristFlexorOrigin;
SEG_PARAM_EXTERN CartesianVector kRightWristFlexorMidPoint;
SEG_PARAM_EXTERN CartesianVector kRightWristFlexorInsertion;
SEG_PARAM_EXTERN double kRightWristFlexorPCA;
SEG_PARAM_EXTERN double kRightWristFlexorLength;

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

// Left Hand

SEG_PARAM_EXTERN char kLeftHandContactName[256];
SEG_PARAM_EXTERN int kLeftHandNumContactPoints;
SEG_PARAM_EXTERN CartesianVector *kLeftHandContactPositions;

// Right Hand

SEG_PARAM_EXTERN char kRightHandContactName[256];
SEG_PARAM_EXTERN int kRightHandNumContactPoints;
SEG_PARAM_EXTERN CartesianVector *kRightHandContactPositions;

#endif // SEGMENTPARAMETERS_H
