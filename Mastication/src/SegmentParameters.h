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

// -------------------------------------------
// Links
// -------------------------------------------

// -------------------------------------------
// Skull
// -------------------------------------------
SEG_PARAM_EXTERN char kSkullPartName[256];
SEG_PARAM_EXTERN char kSkullGraphicFile[256];
SEG_PARAM_EXTERN double kSkullGraphicScale;
//SEG_PARAM_EXTERN double kSkullGraphicScale[3];
//SEG_PARAM_EXTERN double kSkullGraphicOffset[3];
SEG_PARAM_EXTERN double kSkullMass;
SEG_PARAM_EXTERN CartesianTensor kSkullMOI;
SEG_PARAM_EXTERN CartesianVector kSkullCG;
SEG_PARAM_EXTERN double kSkullPosition[7];
SEG_PARAM_EXTERN SpatialVector kSkullVelocity;

// -------------------------------------------
// Mandible
// -------------------------------------------
SEG_PARAM_EXTERN char kMandiblePartName[256];
SEG_PARAM_EXTERN char kMandibleGraphicFile[256];
SEG_PARAM_EXTERN double kMandibleGraphicScale;
//SEG_PARAM_EXTERN double kMandibleGraphicScale[3];
//SEG_PARAM_EXTERN double kMandibleGraphicOffset[3];
SEG_PARAM_EXTERN double kMandibleMass;
SEG_PARAM_EXTERN CartesianTensor kMandibleMOI;
SEG_PARAM_EXTERN CartesianVector kMandibleCG;
SEG_PARAM_EXTERN double kMandiblePosition[7];
SEG_PARAM_EXTERN SpatialVector kMandibleVelocity;

// -------------------------------------------
// Muscles
// -------------------------------------------

// LeftTemporalis
SEG_PARAM_EXTERN char kLeftTemporalisName[256];
SEG_PARAM_EXTERN CartesianVector kLeftTemporalisOrigin;
SEG_PARAM_EXTERN CartesianVector kLeftTemporalisInsertion;
SEG_PARAM_EXTERN double kLeftTemporalisPCA;
SEG_PARAM_EXTERN double kLeftTemporalisLength;

// LeftMasseter
SEG_PARAM_EXTERN char kLeftMasseterName[256];
SEG_PARAM_EXTERN CartesianVector kLeftMasseterOrigin;
SEG_PARAM_EXTERN CartesianVector kLeftMasseterInsertion;
SEG_PARAM_EXTERN double kLeftMasseterPCA;
SEG_PARAM_EXTERN double kLeftMasseterLength;

// LeftMedialPterygoid
SEG_PARAM_EXTERN char kLeftMedialPterygoidName[256];
SEG_PARAM_EXTERN CartesianVector kLeftMedialPterygoidOrigin;
SEG_PARAM_EXTERN CartesianVector kLeftMedialPterygoidInsertion;
SEG_PARAM_EXTERN double kLeftMedialPterygoidPCA;
SEG_PARAM_EXTERN double kLeftMedialPterygoidLength;

// LeftLateralPterygoid
SEG_PARAM_EXTERN char kLeftLateralPterygoidName[256];
SEG_PARAM_EXTERN CartesianVector kLeftLateralPterygoidOrigin;
SEG_PARAM_EXTERN CartesianVector kLeftLateralPterygoidInsertion;
SEG_PARAM_EXTERN double kLeftLateralPterygoidPCA;
SEG_PARAM_EXTERN double kLeftLateralPterygoidLength;

// RightTemporalis
SEG_PARAM_EXTERN char kRightTemporalisName[256];
SEG_PARAM_EXTERN CartesianVector kRightTemporalisOrigin;
SEG_PARAM_EXTERN CartesianVector kRightTemporalisInsertion;
SEG_PARAM_EXTERN double kRightTemporalisPCA;
SEG_PARAM_EXTERN double kRightTemporalisLength;

// RightMasseter
SEG_PARAM_EXTERN char kRightMasseterName[256];
SEG_PARAM_EXTERN CartesianVector kRightMasseterOrigin;
SEG_PARAM_EXTERN CartesianVector kRightMasseterInsertion;
SEG_PARAM_EXTERN double kRightMasseterPCA;
SEG_PARAM_EXTERN double kRightMasseterLength;

// RightMedialPterygoid
SEG_PARAM_EXTERN char kRightMedialPterygoidName[256];
SEG_PARAM_EXTERN CartesianVector kRightMedialPterygoidOrigin;
SEG_PARAM_EXTERN CartesianVector kRightMedialPterygoidInsertion;
SEG_PARAM_EXTERN double kRightMedialPterygoidPCA;
SEG_PARAM_EXTERN double kRightMedialPterygoidLength;

// RightLateralPterygoid
SEG_PARAM_EXTERN char kRightLateralPterygoidName[256];
SEG_PARAM_EXTERN CartesianVector kRightLateralPterygoidOrigin;
SEG_PARAM_EXTERN CartesianVector kRightLateralPterygoidInsertion;
SEG_PARAM_EXTERN double kRightLateralPterygoidPCA;
SEG_PARAM_EXTERN double kRightLateralPterygoidLength;

// -------------------------------------------
// Springs
// -------------------------------------------

// Tooth
SEG_PARAM_EXTERN char kToothName[256];
SEG_PARAM_EXTERN CartesianVector kToothOrigin;
SEG_PARAM_EXTERN CartesianVector kToothInsertion;
SEG_PARAM_EXTERN double kToothLength;
SEG_PARAM_EXTERN double kToothDamping;
SEG_PARAM_EXTERN double kToothSpringConstant;

// LeftTMJ
SEG_PARAM_EXTERN char kLeftTMJName[256];
SEG_PARAM_EXTERN CartesianVector kLeftTMJOrigin;
SEG_PARAM_EXTERN CartesianVector kLeftTMJInsertion;
SEG_PARAM_EXTERN double kLeftTMJLength;
SEG_PARAM_EXTERN double kLeftTMJDamping;
SEG_PARAM_EXTERN double kLeftTMJSpringConstant;

// RightTMJ
SEG_PARAM_EXTERN char kRightTMJName[256];
SEG_PARAM_EXTERN CartesianVector kRightTMJOrigin;
SEG_PARAM_EXTERN CartesianVector kRightTMJInsertion;
SEG_PARAM_EXTERN double kRightTMJLength;
SEG_PARAM_EXTERN double kRightTMJDamping;
SEG_PARAM_EXTERN double kRightTMJSpringConstant;

// -------------------------------------------
// Kinematic Goal Parameters
// -------------------------------------------

SEG_PARAM_EXTERN double kMandiblePositionTarget[7];
SEG_PARAM_EXTERN SpatialVector kMandibleVelocityTarget;

#endif // SEGMENTPARAMETERS_H
