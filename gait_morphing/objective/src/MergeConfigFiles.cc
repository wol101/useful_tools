#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string.h>
#include <assert.h>

// Use the std namespace. To do this we must first guarantee that it exists.
#if defined(__sgi) || defined(__WIN32_) || defined(WIN32) || (defined(__GNUC__) && (__GNUC__>=3))
namespace std {}
using namespace std;
#endif

#include "DataFile.h"

void Merge(double a, double b, double fraction, double &c);
void Merge(double *a, double *b, int n, double fraction, double *c);
void Merge(char * a, char * b, char *c);
void Merge(int a, int b, double fraction, int &c);

typedef double SpatialVector[6];
typedef double CartesianTensor[3][3];
typedef double CartesianVector[3];

// some generic values

static double gFile1IntegrationStepsize;
static CartesianVector gFile1GravityVector;
static double gFile1GroundPlanarSpringConstant;
static double gFile1GroundNormalSpringConstant;
static double gFile1GroundPlanarDamperConstant;
static double gFile1GroundNormalDamperConstant;
static double gFile1GroundStaticFrictionCoeff;
static double gFile1GroundKineticFrictionCoeff;
static double gFile1TorsoPositionRange[6];
static double gFile1TorsoVelocityRange[6];
   
static double gFile1DefaultJointLimitSpringConstant;
static double gFile1DefaultJointLimitDamperConstant;
static double gFile1DefaultJointFriction;
static double gFile1DefaultMuscleActivationK;
static double gFile1DefaultMuscleForcePerUnitArea;
static double gFile1DefaultMuscleVMaxFactor;

// -------------------------------------------
// Links
// -------------------------------------------

// -------------------------------------------
// Torso
// -------------------------------------------
static char gFile1TorsoPartName[256];
static char gFile1TorsoGraphicFile[256];
static double gFile1TorsoGraphicScale;
//static double gFile1TorsoGraphicScale[3];
//static double gFile1TorsoGraphicOffset[3];
static double gFile1TorsoMass;
static CartesianTensor gFile1TorsoMOI;
static CartesianVector gFile1TorsoCG;
static double gFile1TorsoPosition[7];
static SpatialVector gFile1TorsoVelocity;

// -------------------------------------------
// Left Thigh
// -------------------------------------------
static char gFile1LeftThighPartName[256];
static char gFile1LeftThighGraphicFile[256];
static double gFile1LeftThighGraphicScale;
//static double gFile1LeftThighGraphicScale[3];
//static double gFile1LeftThighGraphicOffset[3];
static double gFile1LeftThighMass;
static CartesianTensor gFile1LeftThighMOI;
static CartesianVector gFile1LeftThighCG;
static double gFile1LeftThighMDHA;
static double gFile1LeftThighMDHAlpha;
static double gFile1LeftThighMDHD;
static double gFile1LeftThighMDHTheta;
static double gFile1LeftThighInitialJointVelocity;
static double gFile1LeftThighJointMin;
static double gFile1LeftThighJointMax;
static double gFile1LeftThighJointLimitSpringConstant;
static double gFile1LeftThighJointLimitDamperConstant;
static double gFile1LeftThighJointFriction;

// -------------------------------------------
// Right Thigh
// -------------------------------------------
static char gFile1RightThighPartName[256];
static char gFile1RightThighGraphicFile[256];
static double gFile1RightThighGraphicScale;
//static double gFile1RightThighGraphicScale[3];
//static double gFile1RightThighGraphicOffset[3];
static double gFile1RightThighMass;
static CartesianTensor gFile1RightThighMOI;
static CartesianVector gFile1RightThighCG;
static double gFile1RightThighMDHA;
static double gFile1RightThighMDHAlpha;
static double gFile1RightThighMDHD;
static double gFile1RightThighMDHTheta;
static double gFile1RightThighInitialJointVelocity;
static double gFile1RightThighJointMin;
static double gFile1RightThighJointMax;
static double gFile1RightThighJointLimitSpringConstant;
static double gFile1RightThighJointLimitDamperConstant;
static double gFile1RightThighJointFriction;

// -------------------------------------------
// Left Leg
// -------------------------------------------
static char gFile1LeftLegPartName[256];
static char gFile1LeftLegGraphicFile[256];
static double gFile1LeftLegGraphicScale;
//static double gFile1LeftLegGraphicScale[3];
//static double gFile1LeftLegGraphicOffset[3];
static double gFile1LeftLegMass;
static CartesianTensor gFile1LeftLegMOI;
static CartesianVector gFile1LeftLegCG;
static double gFile1LeftLegMDHA;
static double gFile1LeftLegMDHAlpha;
static double gFile1LeftLegMDHD;
static double gFile1LeftLegMDHTheta;
static double gFile1LeftLegInitialJointVelocity;
static double gFile1LeftLegJointMin;
static double gFile1LeftLegJointMax;
static double gFile1LeftLegJointLimitSpringConstant;
static double gFile1LeftLegJointLimitDamperConstant;
static double gFile1LeftLegJointFriction;

// -------------------------------------------
// Right Leg
// -------------------------------------------
static char gFile1RightLegPartName[256];
static char gFile1RightLegGraphicFile[256];
static double gFile1RightLegGraphicScale;
//static double gFile1RightLegGraphicScale[3];
//static double gFile1RightLegGraphicOffset[3];
static double gFile1RightLegMass;
static CartesianTensor gFile1RightLegMOI;
static CartesianVector gFile1RightLegCG;
static double gFile1RightLegMDHA;
static double gFile1RightLegMDHAlpha;
static double gFile1RightLegMDHD;
static double gFile1RightLegMDHTheta;
static double gFile1RightLegInitialJointVelocity;
static double gFile1RightLegJointMin;
static double gFile1RightLegJointMax;
static double gFile1RightLegJointLimitSpringConstant;
static double gFile1RightLegJointLimitDamperConstant;
static double gFile1RightLegJointFriction;

// -------------------------------------------
// Left Foot
// -------------------------------------------
static char gFile1LeftFootPartName[256];
static char gFile1LeftFootGraphicFile[256];
static double gFile1LeftFootGraphicScale;
//static double gFile1LeftFootGraphicScale[3];
//static double gFile1LeftFootGraphicOffset[3];
static double gFile1LeftFootMass;
static CartesianTensor gFile1LeftFootMOI;
static CartesianVector gFile1LeftFootCG;
static double gFile1LeftFootMDHA;
static double gFile1LeftFootMDHAlpha;
static double gFile1LeftFootMDHD;
static double gFile1LeftFootMDHTheta;
static double gFile1LeftFootInitialJointVelocity;
static double gFile1LeftFootJointMin;
static double gFile1LeftFootJointMax;
static double gFile1LeftFootJointLimitSpringConstant;
static double gFile1LeftFootJointLimitDamperConstant;
static double gFile1LeftFootJointFriction;

// -------------------------------------------
// Right Foot
// -------------------------------------------
static char gFile1RightFootPartName[256];
static char gFile1RightFootGraphicFile[256];
static double gFile1RightFootGraphicScale;
//static double gFile1RightFootGraphicScale[3];
//static double gFile1RightFootGraphicOffset[3];
static double gFile1RightFootMass;
static CartesianTensor gFile1RightFootMOI;
static CartesianVector gFile1RightFootCG;
static double gFile1RightFootMDHA;
static double gFile1RightFootMDHAlpha;
static double gFile1RightFootMDHD;
static double gFile1RightFootMDHTheta;
static double gFile1RightFootInitialJointVelocity;
static double gFile1RightFootJointMin;
static double gFile1RightFootJointMax;
static double gFile1RightFootJointLimitSpringConstant;
static double gFile1RightFootJointLimitDamperConstant;
static double gFile1RightFootJointFriction;

// -------------------------------------------
// Muscles
// -------------------------------------------

// Left Hip Extensor
static char gFile1LeftHipExtensorName[256];
static CartesianVector gFile1LeftHipExtensorOrigin;
static CartesianVector gFile1LeftHipExtensorInsertion;
static double gFile1LeftHipExtensorPCA;
static double gFile1LeftHipExtensorLength;

// Left Hip Flexor
static char gFile1LeftHipFlexorName[256];
static CartesianVector gFile1LeftHipFlexorOrigin;
static CartesianVector gFile1LeftHipFlexorInsertion;
static double gFile1LeftHipFlexorPCA;
static double gFile1LeftHipFlexorLength;

// Right Hip Extensor
static char gFile1RightHipExtensorName[256];
static CartesianVector gFile1RightHipExtensorOrigin;
static CartesianVector gFile1RightHipExtensorInsertion;
static double gFile1RightHipExtensorPCA;
static double gFile1RightHipExtensorLength;

// Right Hip Flexor
static char gFile1RightHipFlexorName[256];
static CartesianVector gFile1RightHipFlexorOrigin;
static CartesianVector gFile1RightHipFlexorInsertion;
static double gFile1RightHipFlexorPCA;
static double gFile1RightHipFlexorLength;

// Left Knee Extensor
static char gFile1LeftKneeExtensorName[256];
static CartesianVector gFile1LeftKneeExtensorOrigin;
static CartesianVector gFile1LeftKneeExtensorMidPoint;
static CartesianVector gFile1LeftKneeExtensorInsertion;
static double gFile1LeftKneeExtensorPCA;
static double gFile1LeftKneeExtensorLength;

// Left Knee Flexor
static char gFile1LeftKneeFlexorName[256];
static CartesianVector gFile1LeftKneeFlexorOrigin;
static CartesianVector gFile1LeftKneeFlexorMidPoint;
static CartesianVector gFile1LeftKneeFlexorInsertion;
static double gFile1LeftKneeFlexorPCA;
static double gFile1LeftKneeFlexorLength;

// Right Knee Extensor
static char gFile1RightKneeExtensorName[256];
static CartesianVector gFile1RightKneeExtensorOrigin;
static CartesianVector gFile1RightKneeExtensorMidPoint;
static CartesianVector gFile1RightKneeExtensorInsertion;
static double gFile1RightKneeExtensorPCA;
static double gFile1RightKneeExtensorLength;

// Right Knee Flexor
static char gFile1RightKneeFlexorName[256];
static CartesianVector gFile1RightKneeFlexorOrigin;
static CartesianVector gFile1RightKneeFlexorMidPoint;
static CartesianVector gFile1RightKneeFlexorInsertion;
static double gFile1RightKneeFlexorPCA;
static double gFile1RightKneeFlexorLength;

// Left Ankle Extensor (dorsiflexion)
static char gFile1LeftAnkleExtensorName[256];
static CartesianVector gFile1LeftAnkleExtensorOrigin;
static CartesianVector gFile1LeftAnkleExtensorInsertion;
static double gFile1LeftAnkleExtensorPCA;
static double gFile1LeftAnkleExtensorLength;

// Left Ankle Flexor (plantarflexion)
static char gFile1LeftAnkleFlexorName[256];
static CartesianVector gFile1LeftAnkleFlexorOrigin;
static CartesianVector gFile1LeftAnkleFlexorInsertion;
static double gFile1LeftAnkleFlexorPCA;
static double gFile1LeftAnkleFlexorLength;

// Right Ankle Extensor (dorsiflexion)
static char gFile1RightAnkleExtensorName[256];
static CartesianVector gFile1RightAnkleExtensorOrigin;
static CartesianVector gFile1RightAnkleExtensorInsertion;
static double gFile1RightAnkleExtensorPCA;
static double gFile1RightAnkleExtensorLength;

// Right Ankle Flexor (plantarflexion)
static char gFile1RightAnkleFlexorName[256];
static CartesianVector gFile1RightAnkleFlexorOrigin;
static CartesianVector gFile1RightAnkleFlexorInsertion;
static double gFile1RightAnkleFlexorPCA;
static double gFile1RightAnkleFlexorLength;

// -------------------------------------------
// Contact Models
// -------------------------------------------

// Left Foot

static char gFile1LeftFootContactName[256];
static int gFile1LeftFootNumContactPoints;
static CartesianVector gFile1LeftFootContactPositions[256];

// Right Foot

static char gFile1RightFootContactName[256];
static int gFile1RightFootNumContactPoints;
static CartesianVector gFile1RightFootContactPositions[256];

// some generic values

static double gFile2IntegrationStepsize;
static CartesianVector gFile2GravityVector;
static double gFile2GroundPlanarSpringConstant;
static double gFile2GroundNormalSpringConstant;
static double gFile2GroundPlanarDamperConstant;
static double gFile2GroundNormalDamperConstant;
static double gFile2GroundStaticFrictionCoeff;
static double gFile2GroundKineticFrictionCoeff;
static double gFile2TorsoPositionRange[6];
static double gFile2TorsoVelocityRange[6];

static double gFile2DefaultJointLimitSpringConstant;
static double gFile2DefaultJointLimitDamperConstant;
static double gFile2DefaultJointFriction;
static double gFile2DefaultMuscleActivationK;
static double gFile2DefaultMuscleForcePerUnitArea;
static double gFile2DefaultMuscleVMaxFactor;

// -------------------------------------------
// Links
// -------------------------------------------

// -------------------------------------------
// Torso
// -------------------------------------------
static char gFile2TorsoPartName[256];
static char gFile2TorsoGraphicFile[256];
static double gFile2TorsoGraphicScale;
//static double gFile2TorsoGraphicScale[3];
//static double gFile2TorsoGraphicOffset[3];
static double gFile2TorsoMass;
static CartesianTensor gFile2TorsoMOI;
static CartesianVector gFile2TorsoCG;
static double gFile2TorsoPosition[7];
static SpatialVector gFile2TorsoVelocity;

// -------------------------------------------
// Left Thigh
// -------------------------------------------
static char gFile2LeftThighPartName[256];
static char gFile2LeftThighGraphicFile[256];
static double gFile2LeftThighGraphicScale;
//static double gFile2LeftThighGraphicScale[3];
//static double gFile2LeftThighGraphicOffset[3];
static double gFile2LeftThighMass;
static CartesianTensor gFile2LeftThighMOI;
static CartesianVector gFile2LeftThighCG;
static double gFile2LeftThighMDHA;
static double gFile2LeftThighMDHAlpha;
static double gFile2LeftThighMDHD;
static double gFile2LeftThighMDHTheta;
static double gFile2LeftThighInitialJointVelocity;
static double gFile2LeftThighJointMin;
static double gFile2LeftThighJointMax;
static double gFile2LeftThighJointLimitSpringConstant;
static double gFile2LeftThighJointLimitDamperConstant;
static double gFile2LeftThighJointFriction;

// -------------------------------------------
// Right Thigh
// -------------------------------------------
static char gFile2RightThighPartName[256];
static char gFile2RightThighGraphicFile[256];
static double gFile2RightThighGraphicScale;
//static double gFile2RightThighGraphicScale[3];
//static double gFile2RightThighGraphicOffset[3];
static double gFile2RightThighMass;
static CartesianTensor gFile2RightThighMOI;
static CartesianVector gFile2RightThighCG;
static double gFile2RightThighMDHA;
static double gFile2RightThighMDHAlpha;
static double gFile2RightThighMDHD;
static double gFile2RightThighMDHTheta;
static double gFile2RightThighInitialJointVelocity;
static double gFile2RightThighJointMin;
static double gFile2RightThighJointMax;
static double gFile2RightThighJointLimitSpringConstant;
static double gFile2RightThighJointLimitDamperConstant;
static double gFile2RightThighJointFriction;

// -------------------------------------------
// Left Leg
// -------------------------------------------
static char gFile2LeftLegPartName[256];
static char gFile2LeftLegGraphicFile[256];
static double gFile2LeftLegGraphicScale;
//static double gFile2LeftLegGraphicScale[3];
//static double gFile2LeftLegGraphicOffset[3];
static double gFile2LeftLegMass;
static CartesianTensor gFile2LeftLegMOI;
static CartesianVector gFile2LeftLegCG;
static double gFile2LeftLegMDHA;
static double gFile2LeftLegMDHAlpha;
static double gFile2LeftLegMDHD;
static double gFile2LeftLegMDHTheta;
static double gFile2LeftLegInitialJointVelocity;
static double gFile2LeftLegJointMin;
static double gFile2LeftLegJointMax;
static double gFile2LeftLegJointLimitSpringConstant;
static double gFile2LeftLegJointLimitDamperConstant;
static double gFile2LeftLegJointFriction;

// -------------------------------------------
// Right Leg
// -------------------------------------------
static char gFile2RightLegPartName[256];
static char gFile2RightLegGraphicFile[256];
static double gFile2RightLegGraphicScale;
//static double gFile2RightLegGraphicScale[3];
//static double gFile2RightLegGraphicOffset[3];
static double gFile2RightLegMass;
static CartesianTensor gFile2RightLegMOI;
static CartesianVector gFile2RightLegCG;
static double gFile2RightLegMDHA;
static double gFile2RightLegMDHAlpha;
static double gFile2RightLegMDHD;
static double gFile2RightLegMDHTheta;
static double gFile2RightLegInitialJointVelocity;
static double gFile2RightLegJointMin;
static double gFile2RightLegJointMax;
static double gFile2RightLegJointLimitSpringConstant;
static double gFile2RightLegJointLimitDamperConstant;
static double gFile2RightLegJointFriction;

// -------------------------------------------
// Left Foot
// -------------------------------------------
static char gFile2LeftFootPartName[256];
static char gFile2LeftFootGraphicFile[256];
static double gFile2LeftFootGraphicScale;
//static double gFile2LeftFootGraphicScale[3];
//static double gFile2LeftFootGraphicOffset[3];
static double gFile2LeftFootMass;
static CartesianTensor gFile2LeftFootMOI;
static CartesianVector gFile2LeftFootCG;
static double gFile2LeftFootMDHA;
static double gFile2LeftFootMDHAlpha;
static double gFile2LeftFootMDHD;
static double gFile2LeftFootMDHTheta;
static double gFile2LeftFootInitialJointVelocity;
static double gFile2LeftFootJointMin;
static double gFile2LeftFootJointMax;
static double gFile2LeftFootJointLimitSpringConstant;
static double gFile2LeftFootJointLimitDamperConstant;
static double gFile2LeftFootJointFriction;

// -------------------------------------------
// Right Foot
// -------------------------------------------
static char gFile2RightFootPartName[256];
static char gFile2RightFootGraphicFile[256];
static double gFile2RightFootGraphicScale;
//static double gFile2RightFootGraphicScale[3];
//static double gFile2RightFootGraphicOffset[3];
static double gFile2RightFootMass;
static CartesianTensor gFile2RightFootMOI;
static CartesianVector gFile2RightFootCG;
static double gFile2RightFootMDHA;
static double gFile2RightFootMDHAlpha;
static double gFile2RightFootMDHD;
static double gFile2RightFootMDHTheta;
static double gFile2RightFootInitialJointVelocity;
static double gFile2RightFootJointMin;
static double gFile2RightFootJointMax;
static double gFile2RightFootJointLimitSpringConstant;
static double gFile2RightFootJointLimitDamperConstant;
static double gFile2RightFootJointFriction;

// -------------------------------------------
// Muscles
// -------------------------------------------

// Left Hip Extensor
static char gFile2LeftHipExtensorName[256];
static CartesianVector gFile2LeftHipExtensorOrigin;
static CartesianVector gFile2LeftHipExtensorInsertion;
static double gFile2LeftHipExtensorPCA;
static double gFile2LeftHipExtensorLength;

// Left Hip Flexor
static char gFile2LeftHipFlexorName[256];
static CartesianVector gFile2LeftHipFlexorOrigin;
static CartesianVector gFile2LeftHipFlexorInsertion;
static double gFile2LeftHipFlexorPCA;
static double gFile2LeftHipFlexorLength;

// Right Hip Extensor
static char gFile2RightHipExtensorName[256];
static CartesianVector gFile2RightHipExtensorOrigin;
static CartesianVector gFile2RightHipExtensorInsertion;
static double gFile2RightHipExtensorPCA;
static double gFile2RightHipExtensorLength;

// Right Hip Flexor
static char gFile2RightHipFlexorName[256];
static CartesianVector gFile2RightHipFlexorOrigin;
static CartesianVector gFile2RightHipFlexorInsertion;
static double gFile2RightHipFlexorPCA;
static double gFile2RightHipFlexorLength;

// Left Knee Extensor
static char gFile2LeftKneeExtensorName[256];
static CartesianVector gFile2LeftKneeExtensorOrigin;
static CartesianVector gFile2LeftKneeExtensorMidPoint;
static CartesianVector gFile2LeftKneeExtensorInsertion;
static double gFile2LeftKneeExtensorPCA;
static double gFile2LeftKneeExtensorLength;

// Left Knee Flexor
static char gFile2LeftKneeFlexorName[256];
static CartesianVector gFile2LeftKneeFlexorOrigin;
static CartesianVector gFile2LeftKneeFlexorMidPoint;
static CartesianVector gFile2LeftKneeFlexorInsertion;
static double gFile2LeftKneeFlexorPCA;
static double gFile2LeftKneeFlexorLength;

// Right Knee Extensor
static char gFile2RightKneeExtensorName[256];
static CartesianVector gFile2RightKneeExtensorOrigin;
static CartesianVector gFile2RightKneeExtensorMidPoint;
static CartesianVector gFile2RightKneeExtensorInsertion;
static double gFile2RightKneeExtensorPCA;
static double gFile2RightKneeExtensorLength;

// Right Knee Flexor
static char gFile2RightKneeFlexorName[256];
static CartesianVector gFile2RightKneeFlexorOrigin;
static CartesianVector gFile2RightKneeFlexorMidPoint;
static CartesianVector gFile2RightKneeFlexorInsertion;
static double gFile2RightKneeFlexorPCA;
static double gFile2RightKneeFlexorLength;

// Left Ankle Extensor (dorsiflexion)
static char gFile2LeftAnkleExtensorName[256];
static CartesianVector gFile2LeftAnkleExtensorOrigin;
static CartesianVector gFile2LeftAnkleExtensorInsertion;
static double gFile2LeftAnkleExtensorPCA;
static double gFile2LeftAnkleExtensorLength;

// Left Ankle Flexor (plantarflexion)
static char gFile2LeftAnkleFlexorName[256];
static CartesianVector gFile2LeftAnkleFlexorOrigin;
static CartesianVector gFile2LeftAnkleFlexorInsertion;
static double gFile2LeftAnkleFlexorPCA;
static double gFile2LeftAnkleFlexorLength;

// Right Ankle Extensor (dorsiflexion)
static char gFile2RightAnkleExtensorName[256];
static CartesianVector gFile2RightAnkleExtensorOrigin;
static CartesianVector gFile2RightAnkleExtensorInsertion;
static double gFile2RightAnkleExtensorPCA;
static double gFile2RightAnkleExtensorLength;

// Right Ankle Flexor (plantarflexion)
static char gFile2RightAnkleFlexorName[256];
static CartesianVector gFile2RightAnkleFlexorOrigin;
static CartesianVector gFile2RightAnkleFlexorInsertion;
static double gFile2RightAnkleFlexorPCA;
static double gFile2RightAnkleFlexorLength;

// -------------------------------------------
// Contact Models
// -------------------------------------------

// Left Foot

static char gFile2LeftFootContactName[256];
static int gFile2LeftFootNumContactPoints;
static CartesianVector gFile2LeftFootContactPositions[256];

// Right Foot

static char gFile2RightFootContactName[256];
static int gFile2RightFootNumContactPoints;
static CartesianVector gFile2RightFootContactPositions[256];


// some generic values

static double gFile3IntegrationStepsize;
static CartesianVector gFile3GravityVector;
static double gFile3GroundPlanarSpringConstant;
static double gFile3GroundNormalSpringConstant;
static double gFile3GroundPlanarDamperConstant;
static double gFile3GroundNormalDamperConstant;
static double gFile3GroundStaticFrictionCoeff;
static double gFile3GroundKineticFrictionCoeff;
static double gFile3TorsoPositionRange[6];
static double gFile3TorsoVelocityRange[6];

static double gFile3DefaultJointLimitSpringConstant;
static double gFile3DefaultJointLimitDamperConstant;
static double gFile3DefaultJointFriction;
static double gFile3DefaultMuscleActivationK;
static double gFile3DefaultMuscleForcePerUnitArea;
static double gFile3DefaultMuscleVMaxFactor;

// -------------------------------------------
// Links
// -------------------------------------------

// -------------------------------------------
// Torso
// -------------------------------------------
static char gFile3TorsoPartName[256];
static char gFile3TorsoGraphicFile[256];
static double gFile3TorsoGraphicScale;
//static double gFile3TorsoGraphicScale[3];
//static double gFile3TorsoGraphicOffset[3];
static double gFile3TorsoMass;
static CartesianTensor gFile3TorsoMOI;
static CartesianVector gFile3TorsoCG;
static double gFile3TorsoPosition[7];
static SpatialVector gFile3TorsoVelocity;

// -------------------------------------------
// Left Thigh
// -------------------------------------------
static char gFile3LeftThighPartName[256];
static char gFile3LeftThighGraphicFile[256];
static double gFile3LeftThighGraphicScale;
//static double gFile3LeftThighGraphicScale[3];
//static double gFile3LeftThighGraphicOffset[3];
static double gFile3LeftThighMass;
static CartesianTensor gFile3LeftThighMOI;
static CartesianVector gFile3LeftThighCG;
static double gFile3LeftThighMDHA;
static double gFile3LeftThighMDHAlpha;
static double gFile3LeftThighMDHD;
static double gFile3LeftThighMDHTheta;
static double gFile3LeftThighInitialJointVelocity;
static double gFile3LeftThighJointMin;
static double gFile3LeftThighJointMax;
static double gFile3LeftThighJointLimitSpringConstant;
static double gFile3LeftThighJointLimitDamperConstant;
static double gFile3LeftThighJointFriction;

// -------------------------------------------
// Right Thigh
// -------------------------------------------
static char gFile3RightThighPartName[256];
static char gFile3RightThighGraphicFile[256];
static double gFile3RightThighGraphicScale;
//static double gFile3RightThighGraphicScale[3];
//static double gFile3RightThighGraphicOffset[3];
static double gFile3RightThighMass;
static CartesianTensor gFile3RightThighMOI;
static CartesianVector gFile3RightThighCG;
static double gFile3RightThighMDHA;
static double gFile3RightThighMDHAlpha;
static double gFile3RightThighMDHD;
static double gFile3RightThighMDHTheta;
static double gFile3RightThighInitialJointVelocity;
static double gFile3RightThighJointMin;
static double gFile3RightThighJointMax;
static double gFile3RightThighJointLimitSpringConstant;
static double gFile3RightThighJointLimitDamperConstant;
static double gFile3RightThighJointFriction;

// -------------------------------------------
// Left Leg
// -------------------------------------------
static char gFile3LeftLegPartName[256];
static char gFile3LeftLegGraphicFile[256];
static double gFile3LeftLegGraphicScale;
//static double gFile3LeftLegGraphicScale[3];
//static double gFile3LeftLegGraphicOffset[3];
static double gFile3LeftLegMass;
static CartesianTensor gFile3LeftLegMOI;
static CartesianVector gFile3LeftLegCG;
static double gFile3LeftLegMDHA;
static double gFile3LeftLegMDHAlpha;
static double gFile3LeftLegMDHD;
static double gFile3LeftLegMDHTheta;
static double gFile3LeftLegInitialJointVelocity;
static double gFile3LeftLegJointMin;
static double gFile3LeftLegJointMax;
static double gFile3LeftLegJointLimitSpringConstant;
static double gFile3LeftLegJointLimitDamperConstant;
static double gFile3LeftLegJointFriction;

// -------------------------------------------
// Right Leg
// -------------------------------------------
static char gFile3RightLegPartName[256];
static char gFile3RightLegGraphicFile[256];
static double gFile3RightLegGraphicScale;
//static double gFile3RightLegGraphicScale[3];
//static double gFile3RightLegGraphicOffset[3];
static double gFile3RightLegMass;
static CartesianTensor gFile3RightLegMOI;
static CartesianVector gFile3RightLegCG;
static double gFile3RightLegMDHA;
static double gFile3RightLegMDHAlpha;
static double gFile3RightLegMDHD;
static double gFile3RightLegMDHTheta;
static double gFile3RightLegInitialJointVelocity;
static double gFile3RightLegJointMin;
static double gFile3RightLegJointMax;
static double gFile3RightLegJointLimitSpringConstant;
static double gFile3RightLegJointLimitDamperConstant;
static double gFile3RightLegJointFriction;

// -------------------------------------------
// Left Foot
// -------------------------------------------
static char gFile3LeftFootPartName[256];
static char gFile3LeftFootGraphicFile[256];
static double gFile3LeftFootGraphicScale;
//static double gFile3LeftFootGraphicScale[3];
//static double gFile3LeftFootGraphicOffset[3];
static double gFile3LeftFootMass;
static CartesianTensor gFile3LeftFootMOI;
static CartesianVector gFile3LeftFootCG;
static double gFile3LeftFootMDHA;
static double gFile3LeftFootMDHAlpha;
static double gFile3LeftFootMDHD;
static double gFile3LeftFootMDHTheta;
static double gFile3LeftFootInitialJointVelocity;
static double gFile3LeftFootJointMin;
static double gFile3LeftFootJointMax;
static double gFile3LeftFootJointLimitSpringConstant;
static double gFile3LeftFootJointLimitDamperConstant;
static double gFile3LeftFootJointFriction;

// -------------------------------------------
// Right Foot
// -------------------------------------------
static char gFile3RightFootPartName[256];
static char gFile3RightFootGraphicFile[256];
static double gFile3RightFootGraphicScale;
//static double gFile3RightFootGraphicScale[3];
//static double gFile3RightFootGraphicOffset[3];
static double gFile3RightFootMass;
static CartesianTensor gFile3RightFootMOI;
static CartesianVector gFile3RightFootCG;
static double gFile3RightFootMDHA;
static double gFile3RightFootMDHAlpha;
static double gFile3RightFootMDHD;
static double gFile3RightFootMDHTheta;
static double gFile3RightFootInitialJointVelocity;
static double gFile3RightFootJointMin;
static double gFile3RightFootJointMax;
static double gFile3RightFootJointLimitSpringConstant;
static double gFile3RightFootJointLimitDamperConstant;
static double gFile3RightFootJointFriction;

// -------------------------------------------
// Muscles
// -------------------------------------------

// Left Hip Extensor
static char gFile3LeftHipExtensorName[256];
static CartesianVector gFile3LeftHipExtensorOrigin;
static CartesianVector gFile3LeftHipExtensorInsertion;
static double gFile3LeftHipExtensorPCA;
static double gFile3LeftHipExtensorLength;

// Left Hip Flexor
static char gFile3LeftHipFlexorName[256];
static CartesianVector gFile3LeftHipFlexorOrigin;
static CartesianVector gFile3LeftHipFlexorInsertion;
static double gFile3LeftHipFlexorPCA;
static double gFile3LeftHipFlexorLength;

// Right Hip Extensor
static char gFile3RightHipExtensorName[256];
static CartesianVector gFile3RightHipExtensorOrigin;
static CartesianVector gFile3RightHipExtensorInsertion;
static double gFile3RightHipExtensorPCA;
static double gFile3RightHipExtensorLength;

// Right Hip Flexor
static char gFile3RightHipFlexorName[256];
static CartesianVector gFile3RightHipFlexorOrigin;
static CartesianVector gFile3RightHipFlexorInsertion;
static double gFile3RightHipFlexorPCA;
static double gFile3RightHipFlexorLength;

// Left Knee Extensor
static char gFile3LeftKneeExtensorName[256];
static CartesianVector gFile3LeftKneeExtensorOrigin;
static CartesianVector gFile3LeftKneeExtensorMidPoint;
static CartesianVector gFile3LeftKneeExtensorInsertion;
static double gFile3LeftKneeExtensorPCA;
static double gFile3LeftKneeExtensorLength;

// Left Knee Flexor
static char gFile3LeftKneeFlexorName[256];
static CartesianVector gFile3LeftKneeFlexorOrigin;
static CartesianVector gFile3LeftKneeFlexorMidPoint;
static CartesianVector gFile3LeftKneeFlexorInsertion;
static double gFile3LeftKneeFlexorPCA;
static double gFile3LeftKneeFlexorLength;

// Right Knee Extensor
static char gFile3RightKneeExtensorName[256];
static CartesianVector gFile3RightKneeExtensorOrigin;
static CartesianVector gFile3RightKneeExtensorMidPoint;
static CartesianVector gFile3RightKneeExtensorInsertion;
static double gFile3RightKneeExtensorPCA;
static double gFile3RightKneeExtensorLength;

// Right Knee Flexor
static char gFile3RightKneeFlexorName[256];
static CartesianVector gFile3RightKneeFlexorOrigin;
static CartesianVector gFile3RightKneeFlexorMidPoint;
static CartesianVector gFile3RightKneeFlexorInsertion;
static double gFile3RightKneeFlexorPCA;
static double gFile3RightKneeFlexorLength;

// Left Ankle Extensor (dorsiflexion)
static char gFile3LeftAnkleExtensorName[256];
static CartesianVector gFile3LeftAnkleExtensorOrigin;
static CartesianVector gFile3LeftAnkleExtensorInsertion;
static double gFile3LeftAnkleExtensorPCA;
static double gFile3LeftAnkleExtensorLength;

// Left Ankle Flexor (plantarflexion)
static char gFile3LeftAnkleFlexorName[256];
static CartesianVector gFile3LeftAnkleFlexorOrigin;
static CartesianVector gFile3LeftAnkleFlexorInsertion;
static double gFile3LeftAnkleFlexorPCA;
static double gFile3LeftAnkleFlexorLength;

// Right Ankle Extensor (dorsiflexion)
static char gFile3RightAnkleExtensorName[256];
static CartesianVector gFile3RightAnkleExtensorOrigin;
static CartesianVector gFile3RightAnkleExtensorInsertion;
static double gFile3RightAnkleExtensorPCA;
static double gFile3RightAnkleExtensorLength;

// Right Ankle Flexor (plantarflexion)
static char gFile3RightAnkleFlexorName[256];
static CartesianVector gFile3RightAnkleFlexorOrigin;
static CartesianVector gFile3RightAnkleFlexorInsertion;
static double gFile3RightAnkleFlexorPCA;
static double gFile3RightAnkleFlexorLength;

// -------------------------------------------
// Contact Models
// -------------------------------------------

// Left Foot

static char gFile3LeftFootContactName[256];
static int gFile3LeftFootNumContactPoints;
static CartesianVector gFile3LeftFootContactPositions[256];

// Right Foot

static char gFile3RightFootContactName[256];
static int gFile3RightFootNumContactPoints;
static CartesianVector gFile3RightFootContactPositions[256];


int main(int argc, char ** argv)
{
  char *fileName1;
  char *fileName2;
  char *fileName3;
  double proportionFile2;
  DataFile file;
  
  if (argc != 5)
  {
    cerr << "Usage: " << argv[0] << " fileName1 fileName2 proportionFile2 fileName3\n";
    exit(1);
  }
  
  fileName1 = argv[1];
  fileName2 = argv[2];
  proportionFile2 = strtod(argv[3], 0);
  fileName3 = argv[4];
  
  file.SetExitOnError(true);
      
  // FILE 1
  file.ReadFile(fileName1);
  
  // -------------------------------------------
  // Defaults
  // -------------------------------------------
  file.RetrieveParameter("kIntegrationStepsize", &gFile1IntegrationStepsize);
  file.RetrieveParameter("kGravityVector", 3, gFile1GravityVector);
  file.RetrieveParameter("kGroundPlanarSpringConstant", &gFile1GroundPlanarSpringConstant);
  file.RetrieveParameter("kGroundNormalSpringConstant", &gFile1GroundNormalSpringConstant);
  file.RetrieveParameter("kGroundPlanarDamperConstant", &gFile1GroundPlanarDamperConstant);
  file.RetrieveParameter("kGroundNormalDamperConstant", &gFile1GroundNormalDamperConstant);
  file.RetrieveParameter("kGroundStaticFrictionCoeff", &gFile1GroundStaticFrictionCoeff);
  file.RetrieveParameter("kGroundKineticFrictionCoeff", &gFile1GroundKineticFrictionCoeff);
  file.RetrieveParameter("kTorsoPositionRange", 6, gFile1TorsoPositionRange);
  file.RetrieveParameter("kTorsoVelocityRange", 6, gFile1TorsoVelocityRange);
  
  file.RetrieveParameter("kDefaultJointLimitSpringConstant", &gFile1DefaultJointLimitSpringConstant);
  file.RetrieveParameter("kDefaultJointLimitDamperConstant", &gFile1DefaultJointLimitDamperConstant);
  file.RetrieveParameter("kDefaultJointFriction", &gFile1DefaultJointFriction);
  file.RetrieveParameter("kDefaultMuscleActivationK", &gFile1DefaultMuscleActivationK);
  file.RetrieveParameter("kDefaultMuscleForcePerUnitArea", &gFile1DefaultMuscleForcePerUnitArea);
  file.RetrieveParameter("kDefaultMuscleVMaxFactor", &gFile1DefaultMuscleVMaxFactor);

  // -------------------------------------------
  // Links
  // -------------------------------------------

  // -------------------------------------------
  // Torso
  file.RetrieveQuotedStringParameter("kTorsoPartName", gFile1TorsoPartName, 256);
  file.RetrieveQuotedStringParameter("kTorsoGraphicFile", gFile1TorsoGraphicFile, 256);
  file.ReadNext(&gFile1TorsoGraphicScale);
  file.RetrieveParameter("kTorsoMass", &gFile1TorsoMass);
  file.RetrieveParameter("kTorsoMOI", 9, (double *)gFile1TorsoMOI);
  file.RetrieveParameter("kTorsoCG", 3, gFile1TorsoCG);
  file.RetrieveParameter("kTorsoPosition", 7, gFile1TorsoPosition);
  file.RetrieveParameter("kTorsoVelocity", 6, gFile1TorsoVelocity);
  
  // -------------------------------------------
  // Left Thigh
  file.RetrieveQuotedStringParameter("kLeftThighPartName", gFile1LeftThighPartName, 256);
  file.RetrieveQuotedStringParameter("kLeftThighGraphicFile", gFile1LeftThighGraphicFile, 256);
  file.ReadNext(&gFile1LeftThighGraphicScale);
  file.RetrieveParameter("kLeftThighMass", &gFile1LeftThighMass);
  file.RetrieveParameter("kLeftThighMOI", 9, (double *)gFile1LeftThighMOI);
  file.RetrieveParameter("kLeftThighCG", 3, gFile1LeftThighCG);
  file.RetrieveParameter("kLeftThighMDHA", &gFile1LeftThighMDHA);
  file.RetrieveParameter("kLeftThighMDHAlpha", &gFile1LeftThighMDHAlpha);
  file.RetrieveParameter("kLeftThighMDHD", &gFile1LeftThighMDHD);
  file.RetrieveParameter("kLeftThighMDHTheta", &gFile1LeftThighMDHTheta);
  file.RetrieveParameter("kLeftThighInitialJointVelocity", &gFile1LeftThighInitialJointVelocity);
  file.RetrieveParameter("kLeftThighJointMin", &gFile1LeftThighJointMin);
  file.RetrieveParameter("kLeftThighJointMax", &gFile1LeftThighJointMax);
  file.RetrieveParameter("kLeftThighJointLimitSpringConstant", &gFile1LeftThighJointLimitSpringConstant);
  if (gFile1LeftThighJointLimitSpringConstant < 0) gFile1LeftThighJointLimitSpringConstant = gFile1DefaultJointLimitSpringConstant;
  file.RetrieveParameter("kLeftThighJointLimitDamperConstant", &gFile1LeftThighJointLimitDamperConstant);
  if (gFile1LeftThighJointLimitDamperConstant < 0) gFile1LeftThighJointLimitDamperConstant = gFile1DefaultJointLimitDamperConstant;
  file.RetrieveParameter("kLeftThighJointFriction", &gFile1LeftThighJointFriction);
  if (gFile1LeftThighJointFriction < 0) gFile1LeftThighJointFriction = gFile1DefaultJointFriction;

  // -------------------------------------------
  // Right Thigh
  file.RetrieveQuotedStringParameter("kRightThighPartName", gFile1RightThighPartName, 256);
  file.RetrieveQuotedStringParameter("kRightThighGraphicFile", gFile1RightThighGraphicFile, 256);
  file.ReadNext(&gFile1RightThighGraphicScale);
  file.RetrieveParameter("kRightThighMass", &gFile1RightThighMass);
  file.RetrieveParameter("kRightThighMOI", 9, (double *)gFile1RightThighMOI);
  file.RetrieveParameter("kRightThighCG", 3, gFile1RightThighCG);
  file.RetrieveParameter("kRightThighMDHA", &gFile1RightThighMDHA);
  file.RetrieveParameter("kRightThighMDHAlpha", &gFile1RightThighMDHAlpha);
  file.RetrieveParameter("kRightThighMDHD", &gFile1RightThighMDHD);
  file.RetrieveParameter("kRightThighMDHTheta", &gFile1RightThighMDHTheta);
  file.RetrieveParameter("kRightThighInitialJointVelocity", &gFile1RightThighInitialJointVelocity);
  file.RetrieveParameter("kRightThighJointMin", &gFile1RightThighJointMin);
  file.RetrieveParameter("kRightThighJointMax", &gFile1RightThighJointMax);
  file.RetrieveParameter("kRightThighJointLimitSpringConstant", &gFile1RightThighJointLimitSpringConstant);
  if (gFile1RightThighJointLimitSpringConstant < 0) gFile1RightThighJointLimitSpringConstant = gFile1DefaultJointLimitSpringConstant;
  file.RetrieveParameter("kRightThighJointLimitDamperConstant", &gFile1RightThighJointLimitDamperConstant);
  if (gFile1RightThighJointLimitDamperConstant < 0) gFile1RightThighJointLimitDamperConstant = gFile1DefaultJointLimitDamperConstant;
  file.RetrieveParameter("kRightThighJointFriction", &gFile1RightThighJointFriction);
  if (gFile1RightThighJointFriction < 0) gFile1RightThighJointFriction = gFile1DefaultJointFriction;

  // -------------------------------------------
  // Left Leg
  file.RetrieveQuotedStringParameter("kLeftLegPartName", gFile1LeftLegPartName, 256);
  file.RetrieveQuotedStringParameter("kLeftLegGraphicFile", gFile1LeftLegGraphicFile, 256);
  file.ReadNext(&gFile1LeftLegGraphicScale);
  file.RetrieveParameter("kLeftLegMass", &gFile1LeftLegMass);
  file.RetrieveParameter("kLeftLegMOI", 9, (double *)gFile1LeftLegMOI);
  file.RetrieveParameter("kLeftLegCG", 3, gFile1LeftLegCG);
  file.RetrieveParameter("kLeftLegMDHA", &gFile1LeftLegMDHA);
  file.RetrieveParameter("kLeftLegMDHAlpha", &gFile1LeftLegMDHAlpha);
  file.RetrieveParameter("kLeftLegMDHD", &gFile1LeftLegMDHD);
  file.RetrieveParameter("kLeftLegMDHTheta", &gFile1LeftLegMDHTheta);
  file.RetrieveParameter("kLeftLegInitialJointVelocity", &gFile1LeftLegInitialJointVelocity);
  file.RetrieveParameter("kLeftLegJointMin", &gFile1LeftLegJointMin);
  file.RetrieveParameter("kLeftLegJointMax", &gFile1LeftLegJointMax);
  file.RetrieveParameter("kLeftLegJointLimitSpringConstant", &gFile1LeftLegJointLimitSpringConstant);
  if (gFile1LeftLegJointLimitSpringConstant < 0) gFile1LeftLegJointLimitSpringConstant = gFile1DefaultJointLimitSpringConstant;
  file.RetrieveParameter("kLeftLegJointLimitDamperConstant", &gFile1LeftLegJointLimitDamperConstant);
  if (gFile1LeftLegJointLimitDamperConstant < 0) gFile1LeftLegJointLimitDamperConstant = gFile1DefaultJointLimitDamperConstant;
  file.RetrieveParameter("kLeftLegJointFriction", &gFile1LeftLegJointFriction);
  if (gFile1LeftLegJointFriction < 0) gFile1LeftLegJointFriction = gFile1DefaultJointFriction;

  // -------------------------------------------
  // Right Leg
  file.RetrieveQuotedStringParameter("kRightLegPartName", gFile1RightLegPartName, 256);
  file.RetrieveQuotedStringParameter("kRightLegGraphicFile", gFile1RightLegGraphicFile, 256);
  file.ReadNext(&gFile1RightLegGraphicScale);
  file.RetrieveParameter("kRightLegMass", &gFile1RightLegMass);
  file.RetrieveParameter("kRightLegMOI", 9, (double *)gFile1RightLegMOI);
  file.RetrieveParameter("kRightLegCG", 3, gFile1RightLegCG);
  file.RetrieveParameter("kRightLegMDHA", &gFile1RightLegMDHA);
  file.RetrieveParameter("kRightLegMDHAlpha", &gFile1RightLegMDHAlpha);
  file.RetrieveParameter("kRightLegMDHD", &gFile1RightLegMDHD);
  file.RetrieveParameter("kRightLegMDHTheta", &gFile1RightLegMDHTheta);
  file.RetrieveParameter("kRightLegInitialJointVelocity", &gFile1RightLegInitialJointVelocity);
  file.RetrieveParameter("kRightLegJointMin", &gFile1RightLegJointMin);
  file.RetrieveParameter("kRightLegJointMax", &gFile1RightLegJointMax);
  file.RetrieveParameter("kRightLegJointLimitSpringConstant", &gFile1RightLegJointLimitSpringConstant);
  if (gFile1RightLegJointLimitSpringConstant < 0) gFile1RightLegJointLimitSpringConstant = gFile1DefaultJointLimitSpringConstant;
  file.RetrieveParameter("kRightLegJointLimitDamperConstant", &gFile1RightLegJointLimitDamperConstant);
  if (gFile1RightLegJointLimitDamperConstant < 0) gFile1RightLegJointLimitDamperConstant = gFile1DefaultJointLimitDamperConstant;
  file.RetrieveParameter("kRightLegJointFriction", &gFile1RightLegJointFriction);
  if (gFile1RightLegJointFriction < 0) gFile1RightLegJointFriction = gFile1DefaultJointFriction;

  // -------------------------------------------
  // Left Foot
  file.RetrieveQuotedStringParameter("kLeftFootPartName", gFile1LeftFootPartName, 256);
  file.RetrieveQuotedStringParameter("kLeftFootGraphicFile", gFile1LeftFootGraphicFile, 256);
  file.ReadNext(&gFile1LeftFootGraphicScale);
  file.RetrieveParameter("kLeftFootMass", &gFile1LeftFootMass);
  file.RetrieveParameter("kLeftFootMOI", 9, (double *)gFile1LeftFootMOI);
  file.RetrieveParameter("kLeftFootCG", 3, gFile1LeftFootCG);
  file.RetrieveParameter("kLeftFootMDHA", &gFile1LeftFootMDHA);
  file.RetrieveParameter("kLeftFootMDHAlpha", &gFile1LeftFootMDHAlpha);
  file.RetrieveParameter("kLeftFootMDHD", &gFile1LeftFootMDHD);
  file.RetrieveParameter("kLeftFootMDHTheta", &gFile1LeftFootMDHTheta);
  file.RetrieveParameter("kLeftFootInitialJointVelocity", &gFile1LeftFootInitialJointVelocity);
  file.RetrieveParameter("kLeftFootJointMin", &gFile1LeftFootJointMin);
  file.RetrieveParameter("kLeftFootJointMax", &gFile1LeftFootJointMax);
  file.RetrieveParameter("kLeftFootJointLimitSpringConstant", &gFile1LeftFootJointLimitSpringConstant);
  if (gFile1LeftFootJointLimitSpringConstant < 0) gFile1LeftFootJointLimitSpringConstant = gFile1DefaultJointLimitSpringConstant;
  file.RetrieveParameter("kLeftFootJointLimitDamperConstant", &gFile1LeftFootJointLimitDamperConstant);
  if (gFile1LeftFootJointLimitDamperConstant < 0) gFile1LeftFootJointLimitDamperConstant = gFile1DefaultJointLimitDamperConstant;
  file.RetrieveParameter("kLeftFootJointFriction", &gFile1LeftFootJointFriction);
  if (gFile1LeftFootJointFriction < 0) gFile1LeftFootJointFriction = gFile1DefaultJointFriction;

  // -------------------------------------------
  // Right Foot
  file.RetrieveQuotedStringParameter("kRightFootPartName", gFile1RightFootPartName, 256);
  file.RetrieveQuotedStringParameter("kRightFootGraphicFile", gFile1RightFootGraphicFile, 256);
  file.ReadNext(&gFile1RightFootGraphicScale);
  file.RetrieveParameter("kRightFootMass", &gFile1RightFootMass);
  file.RetrieveParameter("kRightFootMOI", 9, (double *)gFile1RightFootMOI);
  file.RetrieveParameter("kRightFootCG", 3, gFile1RightFootCG);
  file.RetrieveParameter("kRightFootMDHA", &gFile1RightFootMDHA);
  file.RetrieveParameter("kRightFootMDHAlpha", &gFile1RightFootMDHAlpha);
  file.RetrieveParameter("kRightFootMDHD", &gFile1RightFootMDHD);
  file.RetrieveParameter("kRightFootMDHTheta", &gFile1RightFootMDHTheta);
  file.RetrieveParameter("kRightFootInitialJointVelocity", &gFile1RightFootInitialJointVelocity);
  file.RetrieveParameter("kRightFootJointMin", &gFile1RightFootJointMin);
  file.RetrieveParameter("kRightFootJointMax", &gFile1RightFootJointMax);
  file.RetrieveParameter("kRightFootJointLimitSpringConstant", &gFile1RightFootJointLimitSpringConstant);
  if (gFile1RightFootJointLimitSpringConstant < 0) gFile1RightFootJointLimitSpringConstant = gFile1DefaultJointLimitSpringConstant;
  file.RetrieveParameter("kRightFootJointLimitDamperConstant", &gFile1RightFootJointLimitDamperConstant);
  if (gFile1RightFootJointLimitDamperConstant < 0) gFile1RightFootJointLimitDamperConstant = gFile1DefaultJointLimitDamperConstant;
  file.RetrieveParameter("kRightFootJointFriction", &gFile1RightFootJointFriction);
  if (gFile1RightFootJointFriction < 0) gFile1RightFootJointFriction = gFile1DefaultJointFriction;

  // -------------------------------------------
  // Muscles
  // -------------------------------------------

  // Left Hip Extensor
  file.RetrieveQuotedStringParameter("kLeftHipExtensorName", gFile1LeftHipExtensorName, 256);
  file.RetrieveParameter("kLeftHipExtensorOrigin", 3, gFile1LeftHipExtensorOrigin);
  file.RetrieveParameter("kLeftHipExtensorInsertion", 3, gFile1LeftHipExtensorInsertion);
  file.RetrieveParameter("kLeftHipExtensorPCA", &gFile1LeftHipExtensorPCA);
  file.RetrieveParameter("kLeftHipExtensorLength", &gFile1LeftHipExtensorLength);

  // Left Hip Flexor
  file.RetrieveQuotedStringParameter("kLeftHipFlexorName", gFile1LeftHipFlexorName, 256);
  file.RetrieveParameter("kLeftHipFlexorOrigin", 3, gFile1LeftHipFlexorOrigin);
  file.RetrieveParameter("kLeftHipFlexorInsertion", 3, gFile1LeftHipFlexorInsertion);
  file.RetrieveParameter("kLeftHipFlexorPCA", &gFile1LeftHipFlexorPCA);
  file.RetrieveParameter("kLeftHipFlexorLength", &gFile1LeftHipFlexorLength);

  // Right Hip Extensor
  file.RetrieveQuotedStringParameter("kRightHipExtensorName", gFile1RightHipExtensorName, 256);
  file.RetrieveParameter("kRightHipExtensorOrigin", 3, gFile1RightHipExtensorOrigin);
  file.RetrieveParameter("kRightHipExtensorInsertion", 3, gFile1RightHipExtensorInsertion);
  file.RetrieveParameter("kRightHipExtensorPCA", &gFile1RightHipExtensorPCA);
  file.RetrieveParameter("kRightHipExtensorLength", &gFile1RightHipExtensorLength);

  // Right Hip Flexor
  file.RetrieveQuotedStringParameter("kRightHipFlexorName", gFile1RightHipFlexorName, 256);
  file.RetrieveParameter("kRightHipFlexorOrigin", 3, gFile1RightHipFlexorOrigin);
  file.RetrieveParameter("kRightHipFlexorInsertion", 3, gFile1RightHipFlexorInsertion);
  file.RetrieveParameter("kRightHipFlexorPCA", &gFile1RightHipFlexorPCA);
  file.RetrieveParameter("kRightHipFlexorLength", &gFile1RightHipFlexorLength);

  // Left Knee Extensor
  file.RetrieveQuotedStringParameter("kLeftKneeExtensorName", gFile1LeftKneeExtensorName, 256);
  file.RetrieveParameter("kLeftKneeExtensorOrigin", 3, gFile1LeftKneeExtensorOrigin);
  file.RetrieveParameter("kLeftKneeExtensorMidPoint", 3, gFile1LeftKneeExtensorMidPoint);
  file.RetrieveParameter("kLeftKneeExtensorInsertion", 3, gFile1LeftKneeExtensorInsertion);
  file.RetrieveParameter("kLeftKneeExtensorPCA", &gFile1LeftKneeExtensorPCA);
  file.RetrieveParameter("kLeftKneeExtensorLength", &gFile1LeftKneeExtensorLength);

  // Left Knee Flexor
  file.RetrieveQuotedStringParameter("kLeftKneeFlexorName", gFile1LeftKneeFlexorName, 256);
  file.RetrieveParameter("kLeftKneeFlexorOrigin", 3, gFile1LeftKneeFlexorOrigin);
  file.RetrieveParameter("kLeftKneeFlexorMidPoint", 3, gFile1LeftKneeFlexorMidPoint);
  file.RetrieveParameter("kLeftKneeFlexorInsertion", 3, gFile1LeftKneeFlexorInsertion);
  file.RetrieveParameter("kLeftKneeFlexorPCA", &gFile1LeftKneeFlexorPCA);
  file.RetrieveParameter("kLeftKneeFlexorLength", &gFile1LeftKneeFlexorLength);

  // Right Knee Extensor
  file.RetrieveQuotedStringParameter("kRightKneeExtensorName", gFile1RightKneeExtensorName, 256);
  file.RetrieveParameter("kRightKneeExtensorOrigin", 3, gFile1RightKneeExtensorOrigin);
  file.RetrieveParameter("kRightKneeExtensorMidPoint", 3, gFile1RightKneeExtensorMidPoint);
  file.RetrieveParameter("kRightKneeExtensorInsertion", 3, gFile1RightKneeExtensorInsertion);
  file.RetrieveParameter("kRightKneeExtensorPCA", &gFile1RightKneeExtensorPCA);
  file.RetrieveParameter("kRightKneeExtensorLength", &gFile1RightKneeExtensorLength);

  // Right Knee Flexor
  file.RetrieveQuotedStringParameter("kRightKneeFlexorName", gFile1RightKneeFlexorName, 256);
  file.RetrieveParameter("kRightKneeFlexorOrigin", 3, gFile1RightKneeFlexorOrigin);
  file.RetrieveParameter("kRightKneeFlexorMidPoint", 3, gFile1RightKneeFlexorMidPoint);
  file.RetrieveParameter("kRightKneeFlexorInsertion", 3, gFile1RightKneeFlexorInsertion);
  file.RetrieveParameter("kRightKneeFlexorPCA", &gFile1RightKneeFlexorPCA);
  file.RetrieveParameter("kRightKneeFlexorLength", &gFile1RightKneeFlexorLength);

  // Left Ankle Extensor
  file.RetrieveQuotedStringParameter("kLeftAnkleExtensorName", gFile1LeftAnkleExtensorName, 256);
  file.RetrieveParameter("kLeftAnkleExtensorOrigin", 3, gFile1LeftAnkleExtensorOrigin);
  file.RetrieveParameter("kLeftAnkleExtensorInsertion", 3, gFile1LeftAnkleExtensorInsertion);
  file.RetrieveParameter("kLeftAnkleExtensorPCA", &gFile1LeftAnkleExtensorPCA);
  file.RetrieveParameter("kLeftAnkleExtensorLength", &gFile1LeftAnkleExtensorLength);

  // Left Ankle Flexor
  file.RetrieveQuotedStringParameter("kLeftAnkleFlexorName", gFile1LeftAnkleFlexorName, 256);
  file.RetrieveParameter("kLeftAnkleFlexorOrigin", 3, gFile1LeftAnkleFlexorOrigin);
  file.RetrieveParameter("kLeftAnkleFlexorInsertion", 3, gFile1LeftAnkleFlexorInsertion);
  file.RetrieveParameter("kLeftAnkleFlexorPCA", &gFile1LeftAnkleFlexorPCA);
  file.RetrieveParameter("kLeftAnkleFlexorLength", &gFile1LeftAnkleFlexorLength);

  // Right Ankle Extensor
  file.RetrieveQuotedStringParameter("kRightAnkleExtensorName", gFile1RightAnkleExtensorName, 256);
  file.RetrieveParameter("kRightAnkleExtensorOrigin", 3, gFile1RightAnkleExtensorOrigin);
  file.RetrieveParameter("kRightAnkleExtensorInsertion", 3, gFile1RightAnkleExtensorInsertion);
  file.RetrieveParameter("kRightAnkleExtensorPCA", &gFile1RightAnkleExtensorPCA);
  file.RetrieveParameter("kRightAnkleExtensorLength", &gFile1RightAnkleExtensorLength);

  // Right Ankle Flexor
  file.RetrieveQuotedStringParameter("kRightAnkleFlexorName", gFile1RightAnkleFlexorName, 256);
  file.RetrieveParameter("kRightAnkleFlexorOrigin", 3, gFile1RightAnkleFlexorOrigin);
  file.RetrieveParameter("kRightAnkleFlexorInsertion", 3, gFile1RightAnkleFlexorInsertion);
  file.RetrieveParameter("kRightAnkleFlexorPCA", &gFile1RightAnkleFlexorPCA);
  file.RetrieveParameter("kRightAnkleFlexorLength", &gFile1RightAnkleFlexorLength);

  // -------------------------------------------
  // Contact Models
  // -------------------------------------------

  // Left Foot

  file.RetrieveQuotedStringParameter("kLeftFootContactName", gFile1LeftFootContactName, 256);
  file.RetrieveParameter("kLeftFootNumContactPoints", &gFile1LeftFootNumContactPoints);
  assert(gFile1LeftFootNumContactPoints < 256); // hardwired size limit
  file.RetrieveParameter("kLeftFootContactPositions", gFile1LeftFootNumContactPoints * 3, (double *)gFile1LeftFootContactPositions);
  
  // Right Foot

  file.RetrieveQuotedStringParameter("kRightFootContactName", gFile1RightFootContactName, 256);
  file.RetrieveParameter("kRightFootNumContactPoints", &gFile1RightFootNumContactPoints);
  assert(gFile1RightFootNumContactPoints < 256); // hardwired size limit
  file.RetrieveParameter("kRightFootContactPositions", gFile1RightFootNumContactPoints * 3, (double *)gFile1RightFootContactPositions);

  // FILE 2
  file.ReadFile(fileName2);
  
  // -------------------------------------------
  // Defaults
  // -------------------------------------------
  
  file.RetrieveParameter("kIntegrationStepsize", &gFile2IntegrationStepsize);
  file.RetrieveParameter("kGravityVector", 3, gFile2GravityVector);
  file.RetrieveParameter("kGroundPlanarSpringConstant", &gFile2GroundPlanarSpringConstant);
  file.RetrieveParameter("kGroundNormalSpringConstant", &gFile2GroundNormalSpringConstant);
  file.RetrieveParameter("kGroundPlanarDamperConstant", &gFile2GroundPlanarDamperConstant);
  file.RetrieveParameter("kGroundNormalDamperConstant", &gFile2GroundNormalDamperConstant);
  file.RetrieveParameter("kGroundStaticFrictionCoeff", &gFile2GroundStaticFrictionCoeff);
  file.RetrieveParameter("kGroundKineticFrictionCoeff", &gFile2GroundKineticFrictionCoeff);
  file.RetrieveParameter("kTorsoPositionRange", 6, gFile2TorsoPositionRange);
  file.RetrieveParameter("kTorsoVelocityRange", 6, gFile2TorsoVelocityRange);
  
  file.RetrieveParameter("kDefaultJointLimitSpringConstant", &gFile2DefaultJointLimitSpringConstant);
  file.RetrieveParameter("kDefaultJointLimitDamperConstant", &gFile2DefaultJointLimitDamperConstant);
  file.RetrieveParameter("kDefaultJointFriction", &gFile2DefaultJointFriction);
  file.RetrieveParameter("kDefaultMuscleActivationK", &gFile2DefaultMuscleActivationK);
  file.RetrieveParameter("kDefaultMuscleForcePerUnitArea", &gFile2DefaultMuscleForcePerUnitArea);
  file.RetrieveParameter("kDefaultMuscleVMaxFactor", &gFile2DefaultMuscleVMaxFactor);

  // -------------------------------------------
  // Links
  // -------------------------------------------

  // -------------------------------------------
  // Torso
  file.RetrieveQuotedStringParameter("kTorsoPartName", gFile2TorsoPartName, 256);
  file.RetrieveQuotedStringParameter("kTorsoGraphicFile", gFile2TorsoGraphicFile, 256);
  file.ReadNext(&gFile2TorsoGraphicScale);
  file.RetrieveParameter("kTorsoMass", &gFile2TorsoMass);
  file.RetrieveParameter("kTorsoMOI", 9, (double *)gFile2TorsoMOI);
  file.RetrieveParameter("kTorsoCG", 3, gFile2TorsoCG);
  file.RetrieveParameter("kTorsoPosition", 7, gFile2TorsoPosition);
  file.RetrieveParameter("kTorsoVelocity", 6, gFile2TorsoVelocity);
  
  // -------------------------------------------
  // Left Thigh
  file.RetrieveQuotedStringParameter("kLeftThighPartName", gFile2LeftThighPartName, 256);
  file.RetrieveQuotedStringParameter("kLeftThighGraphicFile", gFile2LeftThighGraphicFile, 256);
  file.ReadNext(&gFile2LeftThighGraphicScale);
  file.RetrieveParameter("kLeftThighMass", &gFile2LeftThighMass);
  file.RetrieveParameter("kLeftThighMOI", 9, (double *)gFile2LeftThighMOI);
  file.RetrieveParameter("kLeftThighCG", 3, gFile2LeftThighCG);
  file.RetrieveParameter("kLeftThighMDHA", &gFile2LeftThighMDHA);
  file.RetrieveParameter("kLeftThighMDHAlpha", &gFile2LeftThighMDHAlpha);
  file.RetrieveParameter("kLeftThighMDHD", &gFile2LeftThighMDHD);
  file.RetrieveParameter("kLeftThighMDHTheta", &gFile2LeftThighMDHTheta);
  file.RetrieveParameter("kLeftThighInitialJointVelocity", &gFile2LeftThighInitialJointVelocity);
  file.RetrieveParameter("kLeftThighJointMin", &gFile2LeftThighJointMin);
  file.RetrieveParameter("kLeftThighJointMax", &gFile2LeftThighJointMax);
  file.RetrieveParameter("kLeftThighJointLimitSpringConstant", &gFile2LeftThighJointLimitSpringConstant);
  if (gFile2LeftThighJointLimitSpringConstant < 0) gFile2LeftThighJointLimitSpringConstant = gFile2DefaultJointLimitSpringConstant;
  file.RetrieveParameter("kLeftThighJointLimitDamperConstant", &gFile2LeftThighJointLimitDamperConstant);
  if (gFile2LeftThighJointLimitDamperConstant < 0) gFile2LeftThighJointLimitDamperConstant = gFile2DefaultJointLimitDamperConstant;
  file.RetrieveParameter("kLeftThighJointFriction", &gFile2LeftThighJointFriction);
  if (gFile2LeftThighJointFriction < 0) gFile2LeftThighJointFriction = gFile2DefaultJointFriction;

  // -------------------------------------------
  // Right Thigh
  file.RetrieveQuotedStringParameter("kRightThighPartName", gFile2RightThighPartName, 256);
  file.RetrieveQuotedStringParameter("kRightThighGraphicFile", gFile2RightThighGraphicFile, 256);
  file.ReadNext(&gFile2RightThighGraphicScale);
  file.RetrieveParameter("kRightThighMass", &gFile2RightThighMass);
  file.RetrieveParameter("kRightThighMOI", 9, (double *)gFile2RightThighMOI);
  file.RetrieveParameter("kRightThighCG", 3, gFile2RightThighCG);
  file.RetrieveParameter("kRightThighMDHA", &gFile2RightThighMDHA);
  file.RetrieveParameter("kRightThighMDHAlpha", &gFile2RightThighMDHAlpha);
  file.RetrieveParameter("kRightThighMDHD", &gFile2RightThighMDHD);
  file.RetrieveParameter("kRightThighMDHTheta", &gFile2RightThighMDHTheta);
  file.RetrieveParameter("kRightThighInitialJointVelocity", &gFile2RightThighInitialJointVelocity);
  file.RetrieveParameter("kRightThighJointMin", &gFile2RightThighJointMin);
  file.RetrieveParameter("kRightThighJointMax", &gFile2RightThighJointMax);
  file.RetrieveParameter("kRightThighJointLimitSpringConstant", &gFile2RightThighJointLimitSpringConstant);
  if (gFile2RightThighJointLimitSpringConstant < 0) gFile2RightThighJointLimitSpringConstant = gFile2DefaultJointLimitSpringConstant;
  file.RetrieveParameter("kRightThighJointLimitDamperConstant", &gFile2RightThighJointLimitDamperConstant);
  if (gFile2RightThighJointLimitDamperConstant < 0) gFile2RightThighJointLimitDamperConstant = gFile2DefaultJointLimitDamperConstant;
  file.RetrieveParameter("kRightThighJointFriction", &gFile2RightThighJointFriction);
  if (gFile2RightThighJointFriction < 0) gFile2RightThighJointFriction = gFile2DefaultJointFriction;

  // -------------------------------------------
  // Left Leg
  file.RetrieveQuotedStringParameter("kLeftLegPartName", gFile2LeftLegPartName, 256);
  file.RetrieveQuotedStringParameter("kLeftLegGraphicFile", gFile2LeftLegGraphicFile, 256);
  file.ReadNext(&gFile2LeftLegGraphicScale);
  file.RetrieveParameter("kLeftLegMass", &gFile2LeftLegMass);
  file.RetrieveParameter("kLeftLegMOI", 9, (double *)gFile2LeftLegMOI);
  file.RetrieveParameter("kLeftLegCG", 3, gFile2LeftLegCG);
  file.RetrieveParameter("kLeftLegMDHA", &gFile2LeftLegMDHA);
  file.RetrieveParameter("kLeftLegMDHAlpha", &gFile2LeftLegMDHAlpha);
  file.RetrieveParameter("kLeftLegMDHD", &gFile2LeftLegMDHD);
  file.RetrieveParameter("kLeftLegMDHTheta", &gFile2LeftLegMDHTheta);
  file.RetrieveParameter("kLeftLegInitialJointVelocity", &gFile2LeftLegInitialJointVelocity);
  file.RetrieveParameter("kLeftLegJointMin", &gFile2LeftLegJointMin);
  file.RetrieveParameter("kLeftLegJointMax", &gFile2LeftLegJointMax);
  file.RetrieveParameter("kLeftLegJointLimitSpringConstant", &gFile2LeftLegJointLimitSpringConstant);
  if (gFile2LeftLegJointLimitSpringConstant < 0) gFile2LeftLegJointLimitSpringConstant = gFile2DefaultJointLimitSpringConstant;
  file.RetrieveParameter("kLeftLegJointLimitDamperConstant", &gFile2LeftLegJointLimitDamperConstant);
  if (gFile2LeftLegJointLimitDamperConstant < 0) gFile2LeftLegJointLimitDamperConstant = gFile2DefaultJointLimitDamperConstant;
  file.RetrieveParameter("kLeftLegJointFriction", &gFile2LeftLegJointFriction);
  if (gFile2LeftLegJointFriction < 0) gFile2LeftLegJointFriction = gFile2DefaultJointFriction;

  // -------------------------------------------
  // Right Leg
  file.RetrieveQuotedStringParameter("kRightLegPartName", gFile2RightLegPartName, 256);
  file.RetrieveQuotedStringParameter("kRightLegGraphicFile", gFile2RightLegGraphicFile, 256);
  file.ReadNext(&gFile2RightLegGraphicScale);
  file.RetrieveParameter("kRightLegMass", &gFile2RightLegMass);
  file.RetrieveParameter("kRightLegMOI", 9, (double *)gFile2RightLegMOI);
  file.RetrieveParameter("kRightLegCG", 3, gFile2RightLegCG);
  file.RetrieveParameter("kRightLegMDHA", &gFile2RightLegMDHA);
  file.RetrieveParameter("kRightLegMDHAlpha", &gFile2RightLegMDHAlpha);
  file.RetrieveParameter("kRightLegMDHD", &gFile2RightLegMDHD);
  file.RetrieveParameter("kRightLegMDHTheta", &gFile2RightLegMDHTheta);
  file.RetrieveParameter("kRightLegInitialJointVelocity", &gFile2RightLegInitialJointVelocity);
  file.RetrieveParameter("kRightLegJointMin", &gFile2RightLegJointMin);
  file.RetrieveParameter("kRightLegJointMax", &gFile2RightLegJointMax);
  file.RetrieveParameter("kRightLegJointLimitSpringConstant", &gFile2RightLegJointLimitSpringConstant);
  if (gFile2RightLegJointLimitSpringConstant < 0) gFile2RightLegJointLimitSpringConstant = gFile2DefaultJointLimitSpringConstant;
  file.RetrieveParameter("kRightLegJointLimitDamperConstant", &gFile2RightLegJointLimitDamperConstant);
  if (gFile2RightLegJointLimitDamperConstant < 0) gFile2RightLegJointLimitDamperConstant = gFile2DefaultJointLimitDamperConstant;
  file.RetrieveParameter("kRightLegJointFriction", &gFile2RightLegJointFriction);
  if (gFile2RightLegJointFriction < 0) gFile2RightLegJointFriction = gFile2DefaultJointFriction;

  // -------------------------------------------
  // Left Foot
  file.RetrieveQuotedStringParameter("kLeftFootPartName", gFile2LeftFootPartName, 256);
  file.RetrieveQuotedStringParameter("kLeftFootGraphicFile", gFile2LeftFootGraphicFile, 256);
  file.ReadNext(&gFile2LeftFootGraphicScale);
  file.RetrieveParameter("kLeftFootMass", &gFile2LeftFootMass);
  file.RetrieveParameter("kLeftFootMOI", 9, (double *)gFile2LeftFootMOI);
  file.RetrieveParameter("kLeftFootCG", 3, gFile2LeftFootCG);
  file.RetrieveParameter("kLeftFootMDHA", &gFile2LeftFootMDHA);
  file.RetrieveParameter("kLeftFootMDHAlpha", &gFile2LeftFootMDHAlpha);
  file.RetrieveParameter("kLeftFootMDHD", &gFile2LeftFootMDHD);
  file.RetrieveParameter("kLeftFootMDHTheta", &gFile2LeftFootMDHTheta);
  file.RetrieveParameter("kLeftFootInitialJointVelocity", &gFile2LeftFootInitialJointVelocity);
  file.RetrieveParameter("kLeftFootJointMin", &gFile2LeftFootJointMin);
  file.RetrieveParameter("kLeftFootJointMax", &gFile2LeftFootJointMax);
  file.RetrieveParameter("kLeftFootJointLimitSpringConstant", &gFile2LeftFootJointLimitSpringConstant);
  if (gFile2LeftFootJointLimitSpringConstant < 0) gFile2LeftFootJointLimitSpringConstant = gFile2DefaultJointLimitSpringConstant;
  file.RetrieveParameter("kLeftFootJointLimitDamperConstant", &gFile2LeftFootJointLimitDamperConstant);
  if (gFile2LeftFootJointLimitDamperConstant < 0) gFile2LeftFootJointLimitDamperConstant = gFile2DefaultJointLimitDamperConstant;
  file.RetrieveParameter("kLeftFootJointFriction", &gFile2LeftFootJointFriction);
  if (gFile2LeftFootJointFriction < 0) gFile2LeftFootJointFriction = gFile2DefaultJointFriction;

  // -------------------------------------------
  // Right Foot
  file.RetrieveQuotedStringParameter("kRightFootPartName", gFile2RightFootPartName, 256);
  file.RetrieveQuotedStringParameter("kRightFootGraphicFile", gFile2RightFootGraphicFile, 256);
  file.ReadNext(&gFile2RightFootGraphicScale);
  file.RetrieveParameter("kRightFootMass", &gFile2RightFootMass);
  file.RetrieveParameter("kRightFootMOI", 9, (double *)gFile2RightFootMOI);
  file.RetrieveParameter("kRightFootCG", 3, gFile2RightFootCG);
  file.RetrieveParameter("kRightFootMDHA", &gFile2RightFootMDHA);
  file.RetrieveParameter("kRightFootMDHAlpha", &gFile2RightFootMDHAlpha);
  file.RetrieveParameter("kRightFootMDHD", &gFile2RightFootMDHD);
  file.RetrieveParameter("kRightFootMDHTheta", &gFile2RightFootMDHTheta);
  file.RetrieveParameter("kRightFootInitialJointVelocity", &gFile2RightFootInitialJointVelocity);
  file.RetrieveParameter("kRightFootJointMin", &gFile2RightFootJointMin);
  file.RetrieveParameter("kRightFootJointMax", &gFile2RightFootJointMax);
  file.RetrieveParameter("kRightFootJointLimitSpringConstant", &gFile2RightFootJointLimitSpringConstant);
  if (gFile2RightFootJointLimitSpringConstant < 0) gFile2RightFootJointLimitSpringConstant = gFile2DefaultJointLimitSpringConstant;
  file.RetrieveParameter("kRightFootJointLimitDamperConstant", &gFile2RightFootJointLimitDamperConstant);
  if (gFile2RightFootJointLimitDamperConstant < 0) gFile2RightFootJointLimitDamperConstant = gFile2DefaultJointLimitDamperConstant;
  file.RetrieveParameter("kRightFootJointFriction", &gFile2RightFootJointFriction);
  if (gFile2RightFootJointFriction < 0) gFile2RightFootJointFriction = gFile2DefaultJointFriction;

  // -------------------------------------------
  // Muscles
  // -------------------------------------------

  // Left Hip Extensor
  file.RetrieveQuotedStringParameter("kLeftHipExtensorName", gFile2LeftHipExtensorName, 256);
  file.RetrieveParameter("kLeftHipExtensorOrigin", 3, gFile2LeftHipExtensorOrigin);
  file.RetrieveParameter("kLeftHipExtensorInsertion", 3, gFile2LeftHipExtensorInsertion);
  file.RetrieveParameter("kLeftHipExtensorPCA", &gFile2LeftHipExtensorPCA);
  file.RetrieveParameter("kLeftHipExtensorLength", &gFile2LeftHipExtensorLength);

  // Left Hip Flexor
  file.RetrieveQuotedStringParameter("kLeftHipFlexorName", gFile2LeftHipFlexorName, 256);
  file.RetrieveParameter("kLeftHipFlexorOrigin", 3, gFile2LeftHipFlexorOrigin);
  file.RetrieveParameter("kLeftHipFlexorInsertion", 3, gFile2LeftHipFlexorInsertion);
  file.RetrieveParameter("kLeftHipFlexorPCA", &gFile2LeftHipFlexorPCA);
  file.RetrieveParameter("kLeftHipFlexorLength", &gFile2LeftHipFlexorLength);

  // Right Hip Extensor
  file.RetrieveQuotedStringParameter("kRightHipExtensorName", gFile2RightHipExtensorName, 256);
  file.RetrieveParameter("kRightHipExtensorOrigin", 3, gFile2RightHipExtensorOrigin);
  file.RetrieveParameter("kRightHipExtensorInsertion", 3, gFile2RightHipExtensorInsertion);
  file.RetrieveParameter("kRightHipExtensorPCA", &gFile2RightHipExtensorPCA);
  file.RetrieveParameter("kRightHipExtensorLength", &gFile2RightHipExtensorLength);

  // Right Hip Flexor
  file.RetrieveQuotedStringParameter("kRightHipFlexorName", gFile2RightHipFlexorName, 256);
  file.RetrieveParameter("kRightHipFlexorOrigin", 3, gFile2RightHipFlexorOrigin);
  file.RetrieveParameter("kRightHipFlexorInsertion", 3, gFile2RightHipFlexorInsertion);
  file.RetrieveParameter("kRightHipFlexorPCA", &gFile2RightHipFlexorPCA);
  file.RetrieveParameter("kRightHipFlexorLength", &gFile2RightHipFlexorLength);

  // Left Knee Extensor
  file.RetrieveQuotedStringParameter("kLeftKneeExtensorName", gFile2LeftKneeExtensorName, 256);
  file.RetrieveParameter("kLeftKneeExtensorOrigin", 3, gFile2LeftKneeExtensorOrigin);
  file.RetrieveParameter("kLeftKneeExtensorMidPoint", 3, gFile2LeftKneeExtensorMidPoint);
  file.RetrieveParameter("kLeftKneeExtensorInsertion", 3, gFile2LeftKneeExtensorInsertion);
  file.RetrieveParameter("kLeftKneeExtensorPCA", &gFile2LeftKneeExtensorPCA);
  file.RetrieveParameter("kLeftKneeExtensorLength", &gFile2LeftKneeExtensorLength);

  // Left Knee Flexor
  file.RetrieveQuotedStringParameter("kLeftKneeFlexorName", gFile2LeftKneeFlexorName, 256);
  file.RetrieveParameter("kLeftKneeFlexorOrigin", 3, gFile2LeftKneeFlexorOrigin);
  file.RetrieveParameter("kLeftKneeFlexorMidPoint", 3, gFile2LeftKneeFlexorMidPoint);
  file.RetrieveParameter("kLeftKneeFlexorInsertion", 3, gFile2LeftKneeFlexorInsertion);
  file.RetrieveParameter("kLeftKneeFlexorPCA", &gFile2LeftKneeFlexorPCA);
  file.RetrieveParameter("kLeftKneeFlexorLength", &gFile2LeftKneeFlexorLength);

  // Right Knee Extensor
  file.RetrieveQuotedStringParameter("kRightKneeExtensorName", gFile2RightKneeExtensorName, 256);
  file.RetrieveParameter("kRightKneeExtensorOrigin", 3, gFile2RightKneeExtensorOrigin);
  file.RetrieveParameter("kRightKneeExtensorMidPoint", 3, gFile2RightKneeExtensorMidPoint);
  file.RetrieveParameter("kRightKneeExtensorInsertion", 3, gFile2RightKneeExtensorInsertion);
  file.RetrieveParameter("kRightKneeExtensorPCA", &gFile2RightKneeExtensorPCA);
  file.RetrieveParameter("kRightKneeExtensorLength", &gFile2RightKneeExtensorLength);

  // Right Knee Flexor
  file.RetrieveQuotedStringParameter("kRightKneeFlexorName", gFile2RightKneeFlexorName, 256);
  file.RetrieveParameter("kRightKneeFlexorOrigin", 3, gFile2RightKneeFlexorOrigin);
  file.RetrieveParameter("kRightKneeFlexorMidPoint", 3, gFile2RightKneeFlexorMidPoint);
  file.RetrieveParameter("kRightKneeFlexorInsertion", 3, gFile2RightKneeFlexorInsertion);
  file.RetrieveParameter("kRightKneeFlexorPCA", &gFile2RightKneeFlexorPCA);
  file.RetrieveParameter("kRightKneeFlexorLength", &gFile2RightKneeFlexorLength);

  // Left Ankle Extensor
  file.RetrieveQuotedStringParameter("kLeftAnkleExtensorName", gFile2LeftAnkleExtensorName, 256);
  file.RetrieveParameter("kLeftAnkleExtensorOrigin", 3, gFile2LeftAnkleExtensorOrigin);
  file.RetrieveParameter("kLeftAnkleExtensorInsertion", 3, gFile2LeftAnkleExtensorInsertion);
  file.RetrieveParameter("kLeftAnkleExtensorPCA", &gFile2LeftAnkleExtensorPCA);
  file.RetrieveParameter("kLeftAnkleExtensorLength", &gFile2LeftAnkleExtensorLength);

  // Left Ankle Flexor
  file.RetrieveQuotedStringParameter("kLeftAnkleFlexorName", gFile2LeftAnkleFlexorName, 256);
  file.RetrieveParameter("kLeftAnkleFlexorOrigin", 3, gFile2LeftAnkleFlexorOrigin);
  file.RetrieveParameter("kLeftAnkleFlexorInsertion", 3, gFile2LeftAnkleFlexorInsertion);
  file.RetrieveParameter("kLeftAnkleFlexorPCA", &gFile2LeftAnkleFlexorPCA);
  file.RetrieveParameter("kLeftAnkleFlexorLength", &gFile2LeftAnkleFlexorLength);

  // Right Ankle Extensor
  file.RetrieveQuotedStringParameter("kRightAnkleExtensorName", gFile2RightAnkleExtensorName, 256);
  file.RetrieveParameter("kRightAnkleExtensorOrigin", 3, gFile2RightAnkleExtensorOrigin);
  file.RetrieveParameter("kRightAnkleExtensorInsertion", 3, gFile2RightAnkleExtensorInsertion);
  file.RetrieveParameter("kRightAnkleExtensorPCA", &gFile2RightAnkleExtensorPCA);
  file.RetrieveParameter("kRightAnkleExtensorLength", &gFile2RightAnkleExtensorLength);

  // Right Ankle Flexor
  file.RetrieveQuotedStringParameter("kRightAnkleFlexorName", gFile2RightAnkleFlexorName, 256);
  file.RetrieveParameter("kRightAnkleFlexorOrigin", 3, gFile2RightAnkleFlexorOrigin);
  file.RetrieveParameter("kRightAnkleFlexorInsertion", 3, gFile2RightAnkleFlexorInsertion);
  file.RetrieveParameter("kRightAnkleFlexorPCA", &gFile2RightAnkleFlexorPCA);
  file.RetrieveParameter("kRightAnkleFlexorLength", &gFile2RightAnkleFlexorLength);

  // -------------------------------------------
  // Contact Models
  // -------------------------------------------

  // Left Foot

  file.RetrieveQuotedStringParameter("kLeftFootContactName", gFile2LeftFootContactName, 256);
  file.RetrieveParameter("kLeftFootNumContactPoints", &gFile2LeftFootNumContactPoints);
  assert(gFile2LeftFootNumContactPoints < 256); // hardwired size limit
  file.RetrieveParameter("kLeftFootContactPositions", gFile2LeftFootNumContactPoints * 3, (double *)gFile2LeftFootContactPositions);
  
  // Right Foot

  file.RetrieveQuotedStringParameter("kRightFootContactName", gFile2RightFootContactName, 256);
  file.RetrieveParameter("kRightFootNumContactPoints", &gFile2RightFootNumContactPoints);
  assert(gFile2RightFootNumContactPoints < 256); // hardwired size limit
  file.RetrieveParameter("kRightFootContactPositions", gFile2RightFootNumContactPoints * 3, (double *)gFile2RightFootContactPositions);

  // merge the data
  
  // some generic values

  Merge(gFile1IntegrationStepsize, gFile2IntegrationStepsize, proportionFile2, gFile3IntegrationStepsize);
  Merge(gFile1GravityVector, gFile2GravityVector, 3, proportionFile2, gFile3GravityVector);
  Merge(gFile1GroundPlanarSpringConstant, gFile2GroundPlanarSpringConstant, proportionFile2, gFile3GroundPlanarSpringConstant);
  Merge(gFile1GroundNormalSpringConstant, gFile2GroundNormalSpringConstant, proportionFile2, gFile3GroundNormalSpringConstant);
  Merge(gFile1GroundPlanarDamperConstant, gFile2GroundPlanarDamperConstant, proportionFile2, gFile3GroundPlanarDamperConstant);
  Merge(gFile1GroundNormalDamperConstant, gFile2GroundNormalDamperConstant, proportionFile2, gFile3GroundNormalDamperConstant);
  Merge(gFile1GroundStaticFrictionCoeff, gFile2GroundStaticFrictionCoeff, proportionFile2, gFile3GroundStaticFrictionCoeff);
  Merge(gFile1GroundKineticFrictionCoeff, gFile2GroundKineticFrictionCoeff, proportionFile2, gFile3GroundKineticFrictionCoeff);
  Merge(gFile1TorsoPositionRange, gFile2TorsoPositionRange, 6, proportionFile2, gFile3TorsoPositionRange);
  Merge(gFile1TorsoVelocityRange, gFile2TorsoVelocityRange, 6, proportionFile2, gFile3TorsoVelocityRange);
  
  Merge(gFile1DefaultJointLimitSpringConstant, gFile2DefaultJointLimitSpringConstant, proportionFile2, gFile3DefaultJointLimitSpringConstant);
  Merge(gFile1DefaultJointLimitDamperConstant, gFile2DefaultJointLimitDamperConstant, proportionFile2, gFile3DefaultJointLimitDamperConstant);
  Merge(gFile1DefaultJointFriction, gFile2DefaultJointFriction, proportionFile2, gFile3DefaultJointFriction);
  Merge(gFile1DefaultMuscleActivationK, gFile2DefaultMuscleActivationK, proportionFile2, gFile3DefaultMuscleActivationK);
  Merge(gFile1DefaultMuscleForcePerUnitArea, gFile2DefaultMuscleForcePerUnitArea, proportionFile2, gFile3DefaultMuscleForcePerUnitArea);
  Merge(gFile1DefaultMuscleVMaxFactor, gFile2DefaultMuscleVMaxFactor, proportionFile2, gFile3DefaultMuscleVMaxFactor);

  // -------------------------------------------
  // Links
  // -------------------------------------------

  // -------------------------------------------
  // Torso
  // -------------------------------------------
  Merge(gFile1TorsoPartName, gFile2TorsoPartName, gFile3TorsoPartName);
  Merge(gFile1TorsoGraphicFile, gFile2TorsoGraphicFile, gFile3TorsoGraphicFile);
  Merge(gFile1TorsoGraphicScale, gFile2TorsoGraphicScale, proportionFile2, gFile3TorsoGraphicScale);
  //Merge(gFile1TorsoGraphicScale, gFile2TorsoGraphicScale, 3, proportionFile2, gFile3TorsoGraphicScale);
  //Merge(gFile1TorsoGraphicOffset, gFile2TorsoGraphicOffset, 3, proportionFile2, gFile3TorsoGraphicOffset);
  Merge(gFile1TorsoMass, gFile2TorsoMass, proportionFile2, gFile3TorsoMass);
  Merge((double *)gFile1TorsoMOI, (double *)gFile2TorsoMOI, 9, proportionFile2, (double *)gFile3TorsoMOI);
  Merge(gFile1TorsoCG, gFile2TorsoCG, 3, proportionFile2, gFile3TorsoCG);
  Merge(gFile1TorsoPosition, gFile2TorsoPosition, 7, proportionFile2, gFile3TorsoPosition);
  Merge(gFile1TorsoVelocity, gFile2TorsoVelocity, 6, proportionFile2, gFile3TorsoVelocity);

  // -------------------------------------------
  // Left Thigh
  // -------------------------------------------
  Merge(gFile1LeftThighPartName, gFile2LeftThighPartName, gFile3LeftThighPartName);
  Merge(gFile1LeftThighGraphicFile, gFile2LeftThighGraphicFile, gFile3LeftThighGraphicFile);
  Merge(gFile1LeftThighGraphicScale, gFile2LeftThighGraphicScale, proportionFile2, gFile3LeftThighGraphicScale);
  //Merge(gFile1LeftThighGraphicScale, gFile2LeftThighGraphicScale, 3, proportionFile2, gFile3LeftThighGraphicScale);
  //Merge(gFile1LeftThighGraphicOffset, gFile2LeftThighGraphicOffset, 3, proportionFile2, gFile3LeftThighGraphicOffset);
  Merge(gFile1LeftThighMass, gFile2LeftThighMass, proportionFile2, gFile3LeftThighMass);
  Merge((double *)gFile1LeftThighMOI, (double *)gFile2LeftThighMOI, 9, proportionFile2, (double *)gFile3LeftThighMOI);
  Merge(gFile1LeftThighCG, gFile2LeftThighCG, 3, proportionFile2, gFile3LeftThighCG);
  Merge(gFile1LeftThighMDHA, gFile2LeftThighMDHA, proportionFile2, gFile3LeftThighMDHA);
  Merge(gFile1LeftThighMDHAlpha, gFile2LeftThighMDHAlpha, proportionFile2, gFile3LeftThighMDHAlpha);
  Merge(gFile1LeftThighMDHD, gFile2LeftThighMDHD, proportionFile2, gFile3LeftThighMDHD);
  Merge(gFile1LeftThighMDHTheta, gFile2LeftThighMDHTheta, proportionFile2, gFile3LeftThighMDHTheta);
  Merge(gFile1LeftThighInitialJointVelocity, gFile2LeftThighInitialJointVelocity, proportionFile2, gFile3LeftThighInitialJointVelocity);
  Merge(gFile1LeftThighJointMin, gFile2LeftThighJointMin, proportionFile2, gFile3LeftThighJointMin);
  Merge(gFile1LeftThighJointMax, gFile2LeftThighJointMax, proportionFile2, gFile3LeftThighJointMax);
  Merge(gFile1LeftThighJointLimitSpringConstant, gFile2LeftThighJointLimitSpringConstant, proportionFile2, gFile3LeftThighJointLimitSpringConstant);
  Merge(gFile1LeftThighJointLimitDamperConstant, gFile2LeftThighJointLimitDamperConstant, proportionFile2, gFile3LeftThighJointLimitDamperConstant);
  Merge(gFile1LeftThighJointFriction, gFile2LeftThighJointFriction, proportionFile2, gFile3LeftThighJointFriction);

  // -------------------------------------------
  // Right Thigh
  // -------------------------------------------
  Merge(gFile1RightThighPartName, gFile2RightThighPartName, gFile3RightThighPartName);
  Merge(gFile1RightThighGraphicFile, gFile2RightThighGraphicFile, gFile3RightThighGraphicFile);
  Merge(gFile1RightThighGraphicScale, gFile2RightThighGraphicScale, proportionFile2, gFile3RightThighGraphicScale);
  //Merge(gFile1RightThighGraphicScale, gFile2RightThighGraphicScale, 3, proportionFile2, gFile3RightThighGraphicScale);
  //Merge(gFile1RightThighGraphicOffset, gFile2RightThighGraphicOffset, 3, proportionFile2, gFile3RightThighGraphicOffset);
  Merge(gFile1RightThighMass, gFile2RightThighMass, proportionFile2, gFile3RightThighMass);
  Merge((double *)gFile1RightThighMOI, (double *)gFile2RightThighMOI, 9, proportionFile2, (double *)gFile3RightThighMOI);
  Merge(gFile1RightThighCG, gFile2RightThighCG, 3, proportionFile2, gFile3RightThighCG);
  Merge(gFile1RightThighMDHA, gFile2RightThighMDHA, proportionFile2, gFile3RightThighMDHA);
  Merge(gFile1RightThighMDHAlpha, gFile2RightThighMDHAlpha, proportionFile2, gFile3RightThighMDHAlpha);
  Merge(gFile1RightThighMDHD, gFile2RightThighMDHD, proportionFile2, gFile3RightThighMDHD);
  Merge(gFile1RightThighMDHTheta, gFile2RightThighMDHTheta, proportionFile2, gFile3RightThighMDHTheta);
  Merge(gFile1RightThighInitialJointVelocity, gFile2RightThighInitialJointVelocity, proportionFile2, gFile3RightThighInitialJointVelocity);
  Merge(gFile1RightThighJointMin, gFile2RightThighJointMin, proportionFile2, gFile3RightThighJointMin);
  Merge(gFile1RightThighJointMax, gFile2RightThighJointMax, proportionFile2, gFile3RightThighJointMax);
  Merge(gFile1RightThighJointLimitSpringConstant, gFile2RightThighJointLimitSpringConstant, proportionFile2, gFile3RightThighJointLimitSpringConstant);
  Merge(gFile1RightThighJointLimitDamperConstant, gFile2RightThighJointLimitDamperConstant, proportionFile2, gFile3RightThighJointLimitDamperConstant);
  Merge(gFile1RightThighJointFriction, gFile2RightThighJointFriction, proportionFile2, gFile3RightThighJointFriction);

  // -------------------------------------------
  // Left Leg
  // -------------------------------------------
  Merge(gFile1LeftLegPartName, gFile2LeftLegPartName, gFile3LeftLegPartName);
  Merge(gFile1LeftLegGraphicFile, gFile2LeftLegGraphicFile, gFile3LeftLegGraphicFile);
  Merge(gFile1LeftLegGraphicScale, gFile2LeftLegGraphicScale, proportionFile2, gFile3LeftLegGraphicScale);
  //Merge(gFile1LeftLegGraphicScale, gFile2LeftLegGraphicScale, 3, proportionFile2, gFile3LeftLegGraphicScale);
  //Merge(gFile1LeftLegGraphicOffset, gFile2LeftLegGraphicOffset, 3, proportionFile2, gFile3LeftLegGraphicOffset);
  Merge(gFile1LeftLegMass, gFile2LeftLegMass, proportionFile2, gFile3LeftLegMass);
  Merge((double *)gFile1LeftLegMOI, (double *)gFile2LeftLegMOI, 9, proportionFile2, (double *)gFile3LeftLegMOI);
  Merge(gFile1LeftLegCG, gFile2LeftLegCG, 3, proportionFile2, gFile3LeftLegCG);
  Merge(gFile1LeftLegMDHA, gFile2LeftLegMDHA, proportionFile2, gFile3LeftLegMDHA);
  Merge(gFile1LeftLegMDHAlpha, gFile2LeftLegMDHAlpha, proportionFile2, gFile3LeftLegMDHAlpha);
  Merge(gFile1LeftLegMDHD, gFile2LeftLegMDHD, proportionFile2, gFile3LeftLegMDHD);
  Merge(gFile1LeftLegMDHTheta, gFile2LeftLegMDHTheta, proportionFile2, gFile3LeftLegMDHTheta);
  Merge(gFile1LeftLegInitialJointVelocity, gFile2LeftLegInitialJointVelocity, proportionFile2, gFile3LeftLegInitialJointVelocity);
  Merge(gFile1LeftLegJointMin, gFile2LeftLegJointMin, proportionFile2, gFile3LeftLegJointMin);
  Merge(gFile1LeftLegJointMax, gFile2LeftLegJointMax, proportionFile2, gFile3LeftLegJointMax);
  Merge(gFile1LeftLegJointLimitSpringConstant, gFile2LeftLegJointLimitSpringConstant, proportionFile2, gFile3LeftLegJointLimitSpringConstant);
  Merge(gFile1LeftLegJointLimitDamperConstant, gFile2LeftLegJointLimitDamperConstant, proportionFile2, gFile3LeftLegJointLimitDamperConstant);
  Merge(gFile1LeftLegJointFriction, gFile2LeftLegJointFriction, proportionFile2, gFile3LeftLegJointFriction);

  // -------------------------------------------
  // Right Leg
  // -------------------------------------------
  Merge(gFile1RightLegPartName, gFile2RightLegPartName, gFile3RightLegPartName);
  Merge(gFile1RightLegGraphicFile, gFile2RightLegGraphicFile, gFile3RightLegGraphicFile);
  Merge(gFile1RightLegGraphicScale, gFile2RightLegGraphicScale, proportionFile2, gFile3RightLegGraphicScale);
  //Merge(gFile1RightLegGraphicScale, gFile2RightLegGraphicScale, 3, proportionFile2, gFile3RightLegGraphicScale);
  //Merge(gFile1RightLegGraphicOffset, gFile2RightLegGraphicOffset, 3, proportionFile2, gFile3RightLegGraphicOffset);
  Merge(gFile1RightLegMass, gFile2RightLegMass, proportionFile2, gFile3RightLegMass);
  Merge((double *)gFile1RightLegMOI, (double *)gFile2RightLegMOI, 9, proportionFile2, (double *)gFile3RightLegMOI);
  Merge(gFile1RightLegCG, gFile2RightLegCG, 3, proportionFile2, gFile3RightLegCG);
  Merge(gFile1RightLegMDHA, gFile2RightLegMDHA, proportionFile2, gFile3RightLegMDHA);
  Merge(gFile1RightLegMDHAlpha, gFile2RightLegMDHAlpha, proportionFile2, gFile3RightLegMDHAlpha);
  Merge(gFile1RightLegMDHD, gFile2RightLegMDHD, proportionFile2, gFile3RightLegMDHD);
  Merge(gFile1RightLegMDHTheta, gFile2RightLegMDHTheta, proportionFile2, gFile3RightLegMDHTheta);
  Merge(gFile1RightLegInitialJointVelocity, gFile2RightLegInitialJointVelocity, proportionFile2, gFile3RightLegInitialJointVelocity);
  Merge(gFile1RightLegJointMin, gFile2RightLegJointMin, proportionFile2, gFile3RightLegJointMin);
  Merge(gFile1RightLegJointMax, gFile2RightLegJointMax, proportionFile2, gFile3RightLegJointMax);
  Merge(gFile1RightLegJointLimitSpringConstant, gFile2RightLegJointLimitSpringConstant, proportionFile2, gFile3RightLegJointLimitSpringConstant);
  Merge(gFile1RightLegJointLimitDamperConstant, gFile2RightLegJointLimitDamperConstant, proportionFile2, gFile3RightLegJointLimitDamperConstant);
  Merge(gFile1RightLegJointFriction, gFile2RightLegJointFriction, proportionFile2, gFile3RightLegJointFriction);

  // -------------------------------------------
  // Left Foot
  // -------------------------------------------
  Merge(gFile1LeftFootPartName, gFile2LeftFootPartName, gFile3LeftFootPartName);
  Merge(gFile1LeftFootGraphicFile, gFile2LeftFootGraphicFile, gFile3LeftFootGraphicFile);
  Merge(gFile1LeftFootGraphicScale, gFile2LeftFootGraphicScale, proportionFile2, gFile3LeftFootGraphicScale);
  //Merge(gFile1LeftFootGraphicScale, gFile2LeftFootGraphicScale, 3, proportionFile2, gFile3LeftFootGraphicScale);
  //Merge(gFile1LeftFootGraphicOffset, gFile2LeftFootGraphicOffset, 3, proportionFile2, gFile3LeftFootGraphicOffset);
  Merge(gFile1LeftFootMass, gFile2LeftFootMass, proportionFile2, gFile3LeftFootMass);
  Merge((double *)gFile1LeftFootMOI, (double *)gFile2LeftFootMOI, 9, proportionFile2, (double *)gFile3LeftFootMOI);
  Merge(gFile1LeftFootCG, gFile2LeftFootCG, 3, proportionFile2, gFile3LeftFootCG);
  Merge(gFile1LeftFootMDHA, gFile2LeftFootMDHA, proportionFile2, gFile3LeftFootMDHA);
  Merge(gFile1LeftFootMDHAlpha, gFile2LeftFootMDHAlpha, proportionFile2, gFile3LeftFootMDHAlpha);
  Merge(gFile1LeftFootMDHD, gFile2LeftFootMDHD, proportionFile2, gFile3LeftFootMDHD);
  Merge(gFile1LeftFootMDHTheta, gFile2LeftFootMDHTheta, proportionFile2, gFile3LeftFootMDHTheta);
  Merge(gFile1LeftFootInitialJointVelocity, gFile2LeftFootInitialJointVelocity, proportionFile2, gFile3LeftFootInitialJointVelocity);
  Merge(gFile1LeftFootJointMin, gFile2LeftFootJointMin, proportionFile2, gFile3LeftFootJointMin);
  Merge(gFile1LeftFootJointMax, gFile2LeftFootJointMax, proportionFile2, gFile3LeftFootJointMax);
  Merge(gFile1LeftFootJointLimitSpringConstant, gFile2LeftFootJointLimitSpringConstant, proportionFile2, gFile3LeftFootJointLimitSpringConstant);
  Merge(gFile1LeftFootJointLimitDamperConstant, gFile2LeftFootJointLimitDamperConstant, proportionFile2, gFile3LeftFootJointLimitDamperConstant);
  Merge(gFile1LeftFootJointFriction, gFile2LeftFootJointFriction, proportionFile2, gFile3LeftFootJointFriction);

  // -------------------------------------------
  // Right Foot
  // -------------------------------------------
  Merge(gFile1RightFootPartName, gFile2RightFootPartName, gFile3RightFootPartName);
  Merge(gFile1RightFootGraphicFile, gFile2RightFootGraphicFile, gFile3RightFootGraphicFile);
  Merge(gFile1RightFootGraphicScale, gFile2RightFootGraphicScale, proportionFile2, gFile3RightFootGraphicScale);
  //Merge(gFile1RightFootGraphicScale, gFile2RightFootGraphicScale, 3, proportionFile2, gFile3RightFootGraphicScale);
  //Merge(gFile1RightFootGraphicOffset, gFile2RightFootGraphicOffset, 3, proportionFile2, gFile3RightFootGraphicOffset);
  Merge(gFile1RightFootMass, gFile2RightFootMass, proportionFile2, gFile3RightFootMass);
  Merge((double *)gFile1RightFootMOI, (double *)gFile2RightFootMOI, 9, proportionFile2, (double *)gFile3RightFootMOI);
  Merge(gFile1RightFootCG, gFile2RightFootCG, 3, proportionFile2, gFile3RightFootCG);
  Merge(gFile1RightFootMDHA, gFile2RightFootMDHA, proportionFile2, gFile3RightFootMDHA);
  Merge(gFile1RightFootMDHAlpha, gFile2RightFootMDHAlpha, proportionFile2, gFile3RightFootMDHAlpha);
  Merge(gFile1RightFootMDHD, gFile2RightFootMDHD, proportionFile2, gFile3RightFootMDHD);
  Merge(gFile1RightFootMDHTheta, gFile2RightFootMDHTheta, proportionFile2, gFile3RightFootMDHTheta);
  Merge(gFile1RightFootInitialJointVelocity, gFile2RightFootInitialJointVelocity, proportionFile2, gFile3RightFootInitialJointVelocity);
  Merge(gFile1RightFootJointMin, gFile2RightFootJointMin, proportionFile2, gFile3RightFootJointMin);
  Merge(gFile1RightFootJointMax, gFile2RightFootJointMax, proportionFile2, gFile3RightFootJointMax);
  Merge(gFile1RightFootJointLimitSpringConstant, gFile2RightFootJointLimitSpringConstant, proportionFile2, gFile3RightFootJointLimitSpringConstant);
  Merge(gFile1RightFootJointLimitDamperConstant, gFile2RightFootJointLimitDamperConstant, proportionFile2, gFile3RightFootJointLimitDamperConstant);
  Merge(gFile1RightFootJointFriction, gFile2RightFootJointFriction, proportionFile2, gFile3RightFootJointFriction);

  // -------------------------------------------
  // Muscles
  // -------------------------------------------

  // Left Hip Extensor
  Merge(gFile1LeftHipExtensorName, gFile2LeftHipExtensorName, gFile3LeftHipExtensorName);
  Merge(gFile1LeftHipExtensorOrigin, gFile2LeftHipExtensorOrigin, 3, proportionFile2, gFile3LeftHipExtensorOrigin);
  Merge(gFile1LeftHipExtensorInsertion, gFile2LeftHipExtensorInsertion, 3, proportionFile2, gFile3LeftHipExtensorInsertion);
  Merge(gFile1LeftHipExtensorPCA, gFile2LeftHipExtensorPCA, proportionFile2, gFile3LeftHipExtensorPCA);
  Merge(gFile1LeftHipExtensorLength, gFile2LeftHipExtensorLength, proportionFile2, gFile3LeftHipExtensorLength);

  // Left Hip Flexor
  Merge(gFile1LeftHipFlexorName, gFile2LeftHipFlexorName, gFile3LeftHipFlexorName);
  Merge(gFile1LeftHipFlexorOrigin, gFile2LeftHipFlexorOrigin, 3, proportionFile2, gFile3LeftHipFlexorOrigin);
  Merge(gFile1LeftHipFlexorInsertion, gFile2LeftHipFlexorInsertion, 3, proportionFile2, gFile3LeftHipFlexorInsertion);
  Merge(gFile1LeftHipFlexorPCA, gFile2LeftHipFlexorPCA, proportionFile2, gFile3LeftHipFlexorPCA);
  Merge(gFile1LeftHipFlexorLength, gFile2LeftHipFlexorLength, proportionFile2, gFile3LeftHipFlexorLength);

  // Right Hip Extensor
  Merge(gFile1RightHipExtensorName, gFile2RightHipExtensorName, gFile3RightHipExtensorName);
  Merge(gFile1RightHipExtensorOrigin, gFile2RightHipExtensorOrigin, 3, proportionFile2, gFile3RightHipExtensorOrigin);
  Merge(gFile1RightHipExtensorInsertion, gFile2RightHipExtensorInsertion, 3, proportionFile2, gFile3RightHipExtensorInsertion);
  Merge(gFile1RightHipExtensorPCA, gFile2RightHipExtensorPCA, proportionFile2, gFile3RightHipExtensorPCA);
  Merge(gFile1RightHipExtensorLength, gFile2RightHipExtensorLength, proportionFile2, gFile3RightHipExtensorLength);

  // Right Hip Flexor
  Merge(gFile1RightHipFlexorName, gFile2RightHipFlexorName, gFile3RightHipFlexorName);
  Merge(gFile1RightHipFlexorOrigin, gFile2RightHipFlexorOrigin, 3, proportionFile2, gFile3RightHipFlexorOrigin);
  Merge(gFile1RightHipFlexorInsertion, gFile2RightHipFlexorInsertion, 3, proportionFile2, gFile3RightHipFlexorInsertion);
  Merge(gFile1RightHipFlexorPCA, gFile2RightHipFlexorPCA, proportionFile2, gFile3RightHipFlexorPCA);
  Merge(gFile1RightHipFlexorLength, gFile2RightHipFlexorLength, proportionFile2, gFile3RightHipFlexorLength);

  // Left Knee Extensor
  Merge(gFile1LeftKneeExtensorName, gFile2LeftKneeExtensorName, gFile3LeftKneeExtensorName);
  Merge(gFile1LeftKneeExtensorOrigin, gFile2LeftKneeExtensorOrigin, 3, proportionFile2, gFile3LeftKneeExtensorOrigin);
  Merge(gFile1LeftKneeExtensorMidPoint, gFile2LeftKneeExtensorMidPoint, 3, proportionFile2, gFile3LeftKneeExtensorMidPoint);
  Merge(gFile1LeftKneeExtensorInsertion, gFile2LeftKneeExtensorInsertion, 3, proportionFile2, gFile3LeftKneeExtensorInsertion);
  Merge(gFile1LeftKneeExtensorPCA, gFile2LeftKneeExtensorPCA, proportionFile2, gFile3LeftKneeExtensorPCA);
  Merge(gFile1LeftKneeExtensorLength, gFile2LeftKneeExtensorLength, proportionFile2, gFile3LeftKneeExtensorLength);

  // Left Knee Flexor
  Merge(gFile1LeftKneeFlexorName, gFile2LeftKneeFlexorName, gFile3LeftKneeFlexorName);
  Merge(gFile1LeftKneeFlexorOrigin, gFile2LeftKneeFlexorOrigin, 3, proportionFile2, gFile3LeftKneeFlexorOrigin);
  Merge(gFile1LeftKneeFlexorMidPoint, gFile2LeftKneeFlexorMidPoint, 3, proportionFile2, gFile3LeftKneeFlexorMidPoint);
  Merge(gFile1LeftKneeFlexorInsertion, gFile2LeftKneeFlexorInsertion, 3, proportionFile2, gFile3LeftKneeFlexorInsertion);
  Merge(gFile1LeftKneeFlexorPCA, gFile2LeftKneeFlexorPCA, proportionFile2, gFile3LeftKneeFlexorPCA);
  Merge(gFile1LeftKneeFlexorLength, gFile2LeftKneeFlexorLength, proportionFile2, gFile3LeftKneeFlexorLength);

  // Right Knee Extensor
  Merge(gFile1RightKneeExtensorName, gFile2RightKneeExtensorName, gFile3RightKneeExtensorName);
  Merge(gFile1RightKneeExtensorOrigin, gFile2RightKneeExtensorOrigin, 3, proportionFile2, gFile3RightKneeExtensorOrigin);
  Merge(gFile1RightKneeExtensorMidPoint, gFile2RightKneeExtensorMidPoint, 3, proportionFile2, gFile3RightKneeExtensorMidPoint);
  Merge(gFile1RightKneeExtensorInsertion, gFile2RightKneeExtensorInsertion, 3, proportionFile2, gFile3RightKneeExtensorInsertion);
  Merge(gFile1RightKneeExtensorPCA, gFile2RightKneeExtensorPCA, proportionFile2, gFile3RightKneeExtensorPCA);
  Merge(gFile1RightKneeExtensorLength, gFile2RightKneeExtensorLength, proportionFile2, gFile3RightKneeExtensorLength);

  // Right Knee Flexor
  Merge(gFile1RightKneeFlexorName, gFile2RightKneeFlexorName, gFile3RightKneeFlexorName);
  Merge(gFile1RightKneeFlexorOrigin, gFile2RightKneeFlexorOrigin, 3, proportionFile2, gFile3RightKneeFlexorOrigin);
  Merge(gFile1RightKneeFlexorMidPoint, gFile2RightKneeFlexorMidPoint, 3, proportionFile2, gFile3RightKneeFlexorMidPoint);
  Merge(gFile1RightKneeFlexorInsertion, gFile2RightKneeFlexorInsertion, 3, proportionFile2, gFile3RightKneeFlexorInsertion);
  Merge(gFile1RightKneeFlexorPCA, gFile2RightKneeFlexorPCA, proportionFile2, gFile3RightKneeFlexorPCA);
  Merge(gFile1RightKneeFlexorLength, gFile2RightKneeFlexorLength, proportionFile2, gFile3RightKneeFlexorLength);

  // Left Ankle Extensor (dorsiflexion)
  Merge(gFile1LeftAnkleExtensorName, gFile2LeftAnkleExtensorName, gFile3LeftAnkleExtensorName);
  Merge(gFile1LeftAnkleExtensorOrigin, gFile2LeftAnkleExtensorOrigin, 3, proportionFile2, gFile3LeftAnkleExtensorOrigin);
  Merge(gFile1LeftAnkleExtensorInsertion, gFile2LeftAnkleExtensorInsertion, 3, proportionFile2, gFile3LeftAnkleExtensorInsertion);
  Merge(gFile1LeftAnkleExtensorPCA, gFile2LeftAnkleExtensorPCA, proportionFile2, gFile3LeftAnkleExtensorPCA);
  Merge(gFile1LeftAnkleExtensorLength, gFile2LeftAnkleExtensorLength, proportionFile2, gFile3LeftAnkleExtensorLength);

  // Left Ankle Flexor (plantarflexion)
  Merge(gFile1LeftAnkleFlexorName, gFile2LeftAnkleFlexorName, gFile3LeftAnkleFlexorName);
  Merge(gFile1LeftAnkleFlexorOrigin, gFile2LeftAnkleFlexorOrigin, 3, proportionFile2, gFile3LeftAnkleFlexorOrigin);
  Merge(gFile1LeftAnkleFlexorInsertion, gFile2LeftAnkleFlexorInsertion, 3, proportionFile2, gFile3LeftAnkleFlexorInsertion);
  Merge(gFile1LeftAnkleFlexorPCA, gFile2LeftAnkleFlexorPCA, proportionFile2, gFile3LeftAnkleFlexorPCA);
  Merge(gFile1LeftAnkleFlexorLength, gFile2LeftAnkleFlexorLength, proportionFile2, gFile3LeftAnkleFlexorLength);

  // Right Ankle Extensor (dorsiflexion)
  Merge(gFile1RightAnkleExtensorName, gFile2RightAnkleExtensorName, gFile3RightAnkleExtensorName);
  Merge(gFile1RightAnkleExtensorOrigin, gFile2RightAnkleExtensorOrigin, 3, proportionFile2, gFile3RightAnkleExtensorOrigin);
  Merge(gFile1RightAnkleExtensorInsertion, gFile2RightAnkleExtensorInsertion, 3, proportionFile2, gFile3RightAnkleExtensorInsertion);
  Merge(gFile1RightAnkleExtensorPCA, gFile2RightAnkleExtensorPCA, proportionFile2, gFile3RightAnkleExtensorPCA);
  Merge(gFile1RightAnkleExtensorLength, gFile2RightAnkleExtensorLength, proportionFile2, gFile3RightAnkleExtensorLength);

  // Right Ankle Flexor (plantarflexion)
  Merge(gFile1RightAnkleFlexorName, gFile2RightAnkleFlexorName, gFile3RightAnkleFlexorName);
  Merge(gFile1RightAnkleFlexorOrigin, gFile2RightAnkleFlexorOrigin, 3, proportionFile2, gFile3RightAnkleFlexorOrigin);
  Merge(gFile1RightAnkleFlexorInsertion, gFile2RightAnkleFlexorInsertion, 3, proportionFile2, gFile3RightAnkleFlexorInsertion);
  Merge(gFile1RightAnkleFlexorPCA, gFile2RightAnkleFlexorPCA, proportionFile2, gFile3RightAnkleFlexorPCA);
  Merge(gFile1RightAnkleFlexorLength, gFile2RightAnkleFlexorLength, proportionFile2, gFile3RightAnkleFlexorLength);

  // -------------------------------------------
  // Contact Models
  // -------------------------------------------

  // Left Foot

  Merge(gFile1LeftFootContactName, gFile2LeftFootContactName, gFile3LeftFootContactName);
  Merge(gFile1LeftFootNumContactPoints, gFile2LeftFootNumContactPoints, proportionFile2, gFile3LeftFootNumContactPoints);
  Merge((double *)gFile1LeftFootContactPositions, (double *)gFile2LeftFootContactPositions, gFile3LeftFootNumContactPoints * 3, proportionFile2, (double *)gFile3LeftFootContactPositions);

  // Right Foot

  Merge(gFile1RightFootContactName, gFile2RightFootContactName, gFile3RightFootContactName);
  Merge(gFile1RightFootNumContactPoints, gFile2RightFootNumContactPoints, proportionFile2, gFile3RightFootNumContactPoints);
  Merge((double *)gFile1RightFootContactPositions, (double *)gFile2RightFootContactPositions, gFile3RightFootNumContactPoints * 3, proportionFile2, (double *)gFile3RightFootContactPositions);
  
  // write the combined file
  
  file.ClearData();
  
    // -------------------------------------------
  // Defaults
  // -------------------------------------------
  
  file.WriteParameter("kIntegrationStepsize", gFile3IntegrationStepsize);
  file.WriteParameter("kGravityVector", 3, gFile3GravityVector);
  file.WriteParameter("kGroundPlanarSpringConstant", gFile3GroundPlanarSpringConstant);
  file.WriteParameter("kGroundNormalSpringConstant", gFile3GroundNormalSpringConstant);
  file.WriteParameter("kGroundPlanarDamperConstant", gFile3GroundPlanarDamperConstant);
  file.WriteParameter("kGroundNormalDamperConstant", gFile3GroundNormalDamperConstant);
  file.WriteParameter("kGroundStaticFrictionCoeff", gFile3GroundStaticFrictionCoeff);
  file.WriteParameter("kGroundKineticFrictionCoeff", gFile3GroundKineticFrictionCoeff);
  file.WriteParameter("kTorsoPositionRange", 6, gFile3TorsoPositionRange);
  file.WriteParameter("kTorsoVelocityRange", 6, gFile3TorsoVelocityRange);
  
  file.WriteParameter("kDefaultJointLimitSpringConstant", gFile3DefaultJointLimitSpringConstant);
  file.WriteParameter("kDefaultJointLimitDamperConstant", gFile3DefaultJointLimitDamperConstant);
  file.WriteParameter("kDefaultJointFriction", gFile3DefaultJointFriction);
  file.WriteParameter("kDefaultMuscleActivationK", gFile3DefaultMuscleActivationK);
  file.WriteParameter("kDefaultMuscleForcePerUnitArea", gFile3DefaultMuscleForcePerUnitArea);
  file.WriteParameter("kDefaultMuscleVMaxFactor", gFile3DefaultMuscleVMaxFactor);

  // -------------------------------------------
  // Links
  // -------------------------------------------

  // -------------------------------------------
  // Torso
  file.WriteQuotedStringParameter("kTorsoPartName", gFile3TorsoPartName);
  file.WriteQuotedStringParameter("kTorsoGraphicFile", gFile3TorsoGraphicFile);
  file.WriteNext(gFile3TorsoGraphicScale, '\n');
  file.WriteParameter("kTorsoMass", gFile3TorsoMass);
  file.WriteParameter("kTorsoMOI", 9, (double *)gFile3TorsoMOI);
  file.WriteParameter("kTorsoCG", 3, gFile3TorsoCG);
  file.WriteParameter("kTorsoPosition", 7, gFile3TorsoPosition);
  file.WriteParameter("kTorsoVelocity", 6, gFile3TorsoVelocity);
  
  // -------------------------------------------
  // Left Thigh
  file.WriteQuotedStringParameter("kLeftThighPartName", gFile3LeftThighPartName);
  file.WriteQuotedStringParameter("kLeftThighGraphicFile", gFile3LeftThighGraphicFile);
  file.WriteNext(gFile3LeftThighGraphicScale, '\n');
  file.WriteParameter("kLeftThighMass", gFile3LeftThighMass);
  file.WriteParameter("kLeftThighMOI", 9, (double *)gFile3LeftThighMOI);
  file.WriteParameter("kLeftThighCG", 3, gFile3LeftThighCG);
  file.WriteParameter("kLeftThighMDHA", gFile3LeftThighMDHA);
  file.WriteParameter("kLeftThighMDHAlpha", gFile3LeftThighMDHAlpha);
  file.WriteParameter("kLeftThighMDHD", gFile3LeftThighMDHD);
  file.WriteParameter("kLeftThighMDHTheta", gFile3LeftThighMDHTheta);
  file.WriteParameter("kLeftThighInitialJointVelocity", gFile3LeftThighInitialJointVelocity);
  file.WriteParameter("kLeftThighJointMin", gFile3LeftThighJointMin);
  file.WriteParameter("kLeftThighJointMax", gFile3LeftThighJointMax);
  file.WriteParameter("kLeftThighJointLimitSpringConstant", gFile3LeftThighJointLimitSpringConstant);
  file.WriteParameter("kLeftThighJointLimitDamperConstant", gFile3LeftThighJointLimitDamperConstant);
  file.WriteParameter("kLeftThighJointFriction", gFile3LeftThighJointFriction);

  // -------------------------------------------
  // Right Thigh
  file.WriteQuotedStringParameter("kRightThighPartName", gFile3RightThighPartName);
  file.WriteQuotedStringParameter("kRightThighGraphicFile", gFile3RightThighGraphicFile);
  file.WriteNext(gFile3RightThighGraphicScale, '\n');
  file.WriteParameter("kRightThighMass", gFile3RightThighMass);
  file.WriteParameter("kRightThighMOI", 9, (double *)gFile3RightThighMOI);
  file.WriteParameter("kRightThighCG", 3, gFile3RightThighCG);
  file.WriteParameter("kRightThighMDHA", gFile3RightThighMDHA);
  file.WriteParameter("kRightThighMDHAlpha", gFile3RightThighMDHAlpha);
  file.WriteParameter("kRightThighMDHD", gFile3RightThighMDHD);
  file.WriteParameter("kRightThighMDHTheta", gFile3RightThighMDHTheta);
  file.WriteParameter("kRightThighInitialJointVelocity", gFile3RightThighInitialJointVelocity);
  file.WriteParameter("kRightThighJointMin", gFile3RightThighJointMin);
  file.WriteParameter("kRightThighJointMax", gFile3RightThighJointMax);
  file.WriteParameter("kRightThighJointLimitSpringConstant", gFile3RightThighJointLimitSpringConstant);
  file.WriteParameter("kRightThighJointLimitDamperConstant", gFile3RightThighJointLimitDamperConstant);
  file.WriteParameter("kRightThighJointFriction", gFile3RightThighJointFriction);

  // -------------------------------------------
  // Left Leg
  file.WriteQuotedStringParameter("kLeftLegPartName", gFile3LeftLegPartName);
  file.WriteQuotedStringParameter("kLeftLegGraphicFile", gFile3LeftLegGraphicFile);
  file.WriteNext(gFile3LeftLegGraphicScale, '\n');
  file.WriteParameter("kLeftLegMass", gFile3LeftLegMass);
  file.WriteParameter("kLeftLegMOI", 9, (double *)gFile3LeftLegMOI);
  file.WriteParameter("kLeftLegCG", 3, gFile3LeftLegCG);
  file.WriteParameter("kLeftLegMDHA", gFile3LeftLegMDHA);
  file.WriteParameter("kLeftLegMDHAlpha", gFile3LeftLegMDHAlpha);
  file.WriteParameter("kLeftLegMDHD", gFile3LeftLegMDHD);
  file.WriteParameter("kLeftLegMDHTheta", gFile3LeftLegMDHTheta);
  file.WriteParameter("kLeftLegInitialJointVelocity", gFile3LeftLegInitialJointVelocity);
  file.WriteParameter("kLeftLegJointMin", gFile3LeftLegJointMin);
  file.WriteParameter("kLeftLegJointMax", gFile3LeftLegJointMax);
  file.WriteParameter("kLeftLegJointLimitSpringConstant", gFile3LeftLegJointLimitSpringConstant);
  file.WriteParameter("kLeftLegJointLimitDamperConstant", gFile3LeftLegJointLimitDamperConstant);
  file.WriteParameter("kLeftLegJointFriction", gFile3LeftLegJointFriction);

  // -------------------------------------------
  // Right Leg
  file.WriteQuotedStringParameter("kRightLegPartName", gFile3RightLegPartName);
  file.WriteQuotedStringParameter("kRightLegGraphicFile", gFile3RightLegGraphicFile);
  file.WriteNext(gFile3RightLegGraphicScale, '\n');
  file.WriteParameter("kRightLegMass", gFile3RightLegMass);
  file.WriteParameter("kRightLegMOI", 9, (double *)gFile3RightLegMOI);
  file.WriteParameter("kRightLegCG", 3, gFile3RightLegCG);
  file.WriteParameter("kRightLegMDHA", gFile3RightLegMDHA);
  file.WriteParameter("kRightLegMDHAlpha", gFile3RightLegMDHAlpha);
  file.WriteParameter("kRightLegMDHD", gFile3RightLegMDHD);
  file.WriteParameter("kRightLegMDHTheta", gFile3RightLegMDHTheta);
  file.WriteParameter("kRightLegInitialJointVelocity", gFile3RightLegInitialJointVelocity);
  file.WriteParameter("kRightLegJointMin", gFile3RightLegJointMin);
  file.WriteParameter("kRightLegJointMax", gFile3RightLegJointMax);
  file.WriteParameter("kRightLegJointLimitSpringConstant", gFile3RightLegJointLimitSpringConstant);
  file.WriteParameter("kRightLegJointLimitDamperConstant", gFile3RightLegJointLimitDamperConstant);
  file.WriteParameter("kRightLegJointFriction", gFile3RightLegJointFriction);

  // -------------------------------------------
  // Left Foot
  file.WriteQuotedStringParameter("kLeftFootPartName", gFile3LeftFootPartName);
  file.WriteQuotedStringParameter("kLeftFootGraphicFile", gFile3LeftFootGraphicFile);
  file.WriteNext(gFile3LeftFootGraphicScale, '\n');
  file.WriteParameter("kLeftFootMass", gFile3LeftFootMass);
  file.WriteParameter("kLeftFootMOI", 9, (double *)gFile3LeftFootMOI);
  file.WriteParameter("kLeftFootCG", 3, gFile3LeftFootCG);
  file.WriteParameter("kLeftFootMDHA", gFile3LeftFootMDHA);
  file.WriteParameter("kLeftFootMDHAlpha", gFile3LeftFootMDHAlpha);
  file.WriteParameter("kLeftFootMDHD", gFile3LeftFootMDHD);
  file.WriteParameter("kLeftFootMDHTheta", gFile3LeftFootMDHTheta);
  file.WriteParameter("kLeftFootInitialJointVelocity", gFile3LeftFootInitialJointVelocity);
  file.WriteParameter("kLeftFootJointMin", gFile3LeftFootJointMin);
  file.WriteParameter("kLeftFootJointMax", gFile3LeftFootJointMax);
  file.WriteParameter("kLeftFootJointLimitSpringConstant", gFile3LeftFootJointLimitSpringConstant);
  file.WriteParameter("kLeftFootJointLimitDamperConstant", gFile3LeftFootJointLimitDamperConstant);
  file.WriteParameter("kLeftFootJointFriction", gFile3LeftFootJointFriction);

  // -------------------------------------------
  // Right Foot
  file.WriteQuotedStringParameter("kRightFootPartName", gFile3RightFootPartName);
  file.WriteQuotedStringParameter("kRightFootGraphicFile", gFile3RightFootGraphicFile);
  file.WriteNext(gFile3RightFootGraphicScale, '\n');
  file.WriteParameter("kRightFootMass", gFile3RightFootMass);
  file.WriteParameter("kRightFootMOI", 9, (double *)gFile3RightFootMOI);
  file.WriteParameter("kRightFootCG", 3, gFile3RightFootCG);
  file.WriteParameter("kRightFootMDHA", gFile3RightFootMDHA);
  file.WriteParameter("kRightFootMDHAlpha", gFile3RightFootMDHAlpha);
  file.WriteParameter("kRightFootMDHD", gFile3RightFootMDHD);
  file.WriteParameter("kRightFootMDHTheta", gFile3RightFootMDHTheta);
  file.WriteParameter("kRightFootInitialJointVelocity", gFile3RightFootInitialJointVelocity);
  file.WriteParameter("kRightFootJointMin", gFile3RightFootJointMin);
  file.WriteParameter("kRightFootJointMax", gFile3RightFootJointMax);
  file.WriteParameter("kRightFootJointLimitSpringConstant", gFile3RightFootJointLimitSpringConstant);
  file.WriteParameter("kRightFootJointLimitDamperConstant", gFile3RightFootJointLimitDamperConstant);
  file.WriteParameter("kRightFootJointFriction", gFile3RightFootJointFriction);

  // -------------------------------------------
  // Muscles
  // -------------------------------------------

  // Left Hip Extensor
  file.WriteQuotedStringParameter("kLeftHipExtensorName", gFile3LeftHipExtensorName);
  file.WriteParameter("kLeftHipExtensorOrigin", 3, gFile3LeftHipExtensorOrigin);
  file.WriteParameter("kLeftHipExtensorInsertion", 3, gFile3LeftHipExtensorInsertion);
  file.WriteParameter("kLeftHipExtensorPCA", gFile3LeftHipExtensorPCA);
  file.WriteParameter("kLeftHipExtensorLength", gFile3LeftHipExtensorLength);

  // Left Hip Flexor
  file.WriteQuotedStringParameter("kLeftHipFlexorName", gFile3LeftHipFlexorName);
  file.WriteParameter("kLeftHipFlexorOrigin", 3, gFile3LeftHipFlexorOrigin);
  file.WriteParameter("kLeftHipFlexorInsertion", 3, gFile3LeftHipFlexorInsertion);
  file.WriteParameter("kLeftHipFlexorPCA", gFile3LeftHipFlexorPCA);
  file.WriteParameter("kLeftHipFlexorLength", gFile3LeftHipFlexorLength);

  // Right Hip Extensor
  file.WriteQuotedStringParameter("kRightHipExtensorName", gFile3RightHipExtensorName);
  file.WriteParameter("kRightHipExtensorOrigin", 3, gFile3RightHipExtensorOrigin);
  file.WriteParameter("kRightHipExtensorInsertion", 3, gFile3RightHipExtensorInsertion);
  file.WriteParameter("kRightHipExtensorPCA", gFile3RightHipExtensorPCA);
  file.WriteParameter("kRightHipExtensorLength", gFile3RightHipExtensorLength);

  // Right Hip Flexor
  file.WriteQuotedStringParameter("kRightHipFlexorName", gFile3RightHipFlexorName);
  file.WriteParameter("kRightHipFlexorOrigin", 3, gFile3RightHipFlexorOrigin);
  file.WriteParameter("kRightHipFlexorInsertion", 3, gFile3RightHipFlexorInsertion);
  file.WriteParameter("kRightHipFlexorPCA", gFile3RightHipFlexorPCA);
  file.WriteParameter("kRightHipFlexorLength", gFile3RightHipFlexorLength);

  // Left Knee Extensor
  file.WriteQuotedStringParameter("kLeftKneeExtensorName", gFile3LeftKneeExtensorName);
  file.WriteParameter("kLeftKneeExtensorOrigin", 3, gFile3LeftKneeExtensorOrigin);
  file.WriteParameter("kLeftKneeExtensorMidPoint", 3, gFile3LeftKneeExtensorMidPoint);
  file.WriteParameter("kLeftKneeExtensorInsertion", 3, gFile3LeftKneeExtensorInsertion);
  file.WriteParameter("kLeftKneeExtensorPCA", gFile3LeftKneeExtensorPCA);
  file.WriteParameter("kLeftKneeExtensorLength", gFile3LeftKneeExtensorLength);

  // Left Knee Flexor
  file.WriteQuotedStringParameter("kLeftKneeFlexorName", gFile3LeftKneeFlexorName);
  file.WriteParameter("kLeftKneeFlexorOrigin", 3, gFile3LeftKneeFlexorOrigin);
  file.WriteParameter("kLeftKneeFlexorMidPoint", 3, gFile3LeftKneeFlexorMidPoint);
  file.WriteParameter("kLeftKneeFlexorInsertion", 3, gFile3LeftKneeFlexorInsertion);
  file.WriteParameter("kLeftKneeFlexorPCA", gFile3LeftKneeFlexorPCA);
  file.WriteParameter("kLeftKneeFlexorLength", gFile3LeftKneeFlexorLength);

  // Right Knee Extensor
  file.WriteQuotedStringParameter("kRightKneeExtensorName", gFile3RightKneeExtensorName);
  file.WriteParameter("kRightKneeExtensorOrigin", 3, gFile3RightKneeExtensorOrigin);
  file.WriteParameter("kRightKneeExtensorMidPoint", 3, gFile3RightKneeExtensorMidPoint);
  file.WriteParameter("kRightKneeExtensorInsertion", 3, gFile3RightKneeExtensorInsertion);
  file.WriteParameter("kRightKneeExtensorPCA", gFile3RightKneeExtensorPCA);
  file.WriteParameter("kRightKneeExtensorLength", gFile3RightKneeExtensorLength);

  // Right Knee Flexor
  file.WriteQuotedStringParameter("kRightKneeFlexorName", gFile3RightKneeFlexorName);
  file.WriteParameter("kRightKneeFlexorOrigin", 3, gFile3RightKneeFlexorOrigin);
  file.WriteParameter("kRightKneeFlexorMidPoint", 3, gFile3RightKneeFlexorMidPoint);
  file.WriteParameter("kRightKneeFlexorInsertion", 3, gFile3RightKneeFlexorInsertion);
  file.WriteParameter("kRightKneeFlexorPCA", gFile3RightKneeFlexorPCA);
  file.WriteParameter("kRightKneeFlexorLength", gFile3RightKneeFlexorLength);

  // Left Ankle Extensor
  file.WriteQuotedStringParameter("kLeftAnkleExtensorName", gFile3LeftAnkleExtensorName);
  file.WriteParameter("kLeftAnkleExtensorOrigin", 3, gFile3LeftAnkleExtensorOrigin);
  file.WriteParameter("kLeftAnkleExtensorInsertion", 3, gFile3LeftAnkleExtensorInsertion);
  file.WriteParameter("kLeftAnkleExtensorPCA", gFile3LeftAnkleExtensorPCA);
  file.WriteParameter("kLeftAnkleExtensorLength", gFile3LeftAnkleExtensorLength);

  // Left Ankle Flexor
  file.WriteQuotedStringParameter("kLeftAnkleFlexorName", gFile3LeftAnkleFlexorName);
  file.WriteParameter("kLeftAnkleFlexorOrigin", 3, gFile3LeftAnkleFlexorOrigin);
  file.WriteParameter("kLeftAnkleFlexorInsertion", 3, gFile3LeftAnkleFlexorInsertion);
  file.WriteParameter("kLeftAnkleFlexorPCA", gFile3LeftAnkleFlexorPCA);
  file.WriteParameter("kLeftAnkleFlexorLength", gFile3LeftAnkleFlexorLength);

  // Right Ankle Extensor
  file.WriteQuotedStringParameter("kRightAnkleExtensorName", gFile3RightAnkleExtensorName);
  file.WriteParameter("kRightAnkleExtensorOrigin", 3, gFile3RightAnkleExtensorOrigin);
  file.WriteParameter("kRightAnkleExtensorInsertion", 3, gFile3RightAnkleExtensorInsertion);
  file.WriteParameter("kRightAnkleExtensorPCA", gFile3RightAnkleExtensorPCA);
  file.WriteParameter("kRightAnkleExtensorLength", gFile3RightAnkleExtensorLength);

  // Right Ankle Flexor
  file.WriteQuotedStringParameter("kRightAnkleFlexorName", gFile3RightAnkleFlexorName);
  file.WriteParameter("kRightAnkleFlexorOrigin", 3, gFile3RightAnkleFlexorOrigin);
  file.WriteParameter("kRightAnkleFlexorInsertion", 3, gFile3RightAnkleFlexorInsertion);
  file.WriteParameter("kRightAnkleFlexorPCA", gFile3RightAnkleFlexorPCA);
  file.WriteParameter("kRightAnkleFlexorLength", gFile3RightAnkleFlexorLength);

  // -------------------------------------------
  // Contact Models
  // -------------------------------------------

  // Left Foot

  file.WriteQuotedStringParameter("kLeftFootContactName", gFile3LeftFootContactName);
  file.WriteParameter("kLeftFootNumContactPoints", gFile3LeftFootNumContactPoints);
  file.WriteParameter("kLeftFootContactPositions", gFile3LeftFootNumContactPoints * 3, (double *)gFile3LeftFootContactPositions);
  
  // Right Foot

  file.WriteQuotedStringParameter("kRightFootContactName", gFile3RightFootContactName);
  file.WriteParameter("kRightFootNumContactPoints", gFile3RightFootNumContactPoints);
  file.WriteParameter("kRightFootContactPositions", gFile3RightFootNumContactPoints * 3, (double *)gFile3RightFootContactPositions);

  file.WriteFile(fileName3);
}

void Merge(double a, double b, double fraction, double &c)
{
  c = a + (b - a) * fraction;
}

void Merge(double *a, double *b, int n, double fraction, double *c)
{
  int i;
  for (i = 0; i < n; i++)
  {
    c[i] = a[i] + (b[i] - a[i]) * fraction;
  }
}

void Merge(char * a, char * b, char *c)
{
  if (strcmp(a, b) != 0)
  {
    cerr << "String mismatch (1) " << a << " (2) " << b << "\n";
  }
  strcpy(c, a);
}

void Merge(int a, int b, double fraction, int &c)
{
  if (a != b)
  {
    cerr << "Integer mismatch (1) " << a << " (2) " << b << "\n";
  }
  c = a + (int)((b - a) * fraction);
}

