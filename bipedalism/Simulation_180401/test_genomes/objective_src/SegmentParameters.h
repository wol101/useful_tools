// SegmentParameters.h
// This file contains the required segment parameters for the model

#ifndef SEGMENTPARAMETERS_H
#define SEGMENTPARAMETERS_H

// Human values from Delph's model (see the ISB website)
// and Lazlo's thesis for CM and MOI
// used Mathematica and parallel axis theorem to adjust
// for required CM

// some generic values

const double kDefaultJointLimitSpringConstant = 1000.0;
const double kDefaultJointLimitDamperConstant = 100.0;
const double kDefaultJointFriction = 0.35;

// -------------------------------------------
// Links
// -------------------------------------------

// -------------------------------------------
// Torso
const char * const kTorsoPartName = "Torso";
const char * const kTorsoGraphicFile = "Torso.xan";
const double kTorsoMass = 51.8;
const CartesianTensor kTorsoMOI =     {{0.44, 0.00, 0.00},
                                       {0.00, 0.99, 0.00},
                                       {0.00, 0.00, 0.99}};
const CartesianVector kTorsoCG =      {0.00, 0.00, 0.00};
const double kTorsoPosition[7]=       {0.5, 0.5, -0.5, 0.5, // orientation in first 4 parameters
                                       5.0, 5.0, 1.27};    // position in last 3
const SpatialVector kTorsoVelocity =  {0.0, 0.0, 0.0, // angular velocities
                                       0.0, 0.0, 0.0};// translational velocities
// -------------------------------------------
// Left Thigh
const char * const kLeftThighPartName = "LeftThigh";
const char * const kLeftThighGraphicFile = "LeftThigh.xan";
const double kLeftThighMass = 8.35;
const CartesianTensor kLeftThighMOI = {{0.025, 0.00, 0.00},
                                       {0.00, 0.49, 0.00},
                                       {0.00, 0.00, 0.49}};
const CartesianVector kLeftThighCG =  {0.198, 0.00, 0.00};
const double kLeftThighMDHA = 0.4;
const double kLeftThighMDHAlpha = 0.0;
const double kLeftThighMDHD = 0.0;
const double kLeftThighMDHTheta = 0.0;
const double kLeftThighInitialJointVelocity = 0.0;
// const double kLeftThighJointMin = -0.175; // realistic
const double kLeftThighJointMin = -0.52; // generous
// const double kLeftThighJointMax = 1.57; // fairly realistic
const double kLeftThighJointMax = 0.52; // restricted
const double kLeftThighJointLimitSpringConstant = kDefaultJointLimitSpringConstant;
const double kLeftThighJointLimitDamperConstant = kDefaultJointLimitDamperConstant;
const double kLeftThighJointFriction = kDefaultJointFriction;

// -------------------------------------------
// Right Thigh
const char * const kRightThighPartName = "RightThigh";
const char * const kRightThighGraphicFile = "RightThigh.xan";
const double kRightThighMass = 8.35;
const CartesianTensor kRightThighMOI = {{0.025, 0.00, 0.00},
                                        {0.00, 0.49, 0.00},
                                        {0.00, 0.00, 0.49}};
const CartesianVector kRightThighCG =  {0.198, 0.00, 0.00};
const double kRightThighMDHA = 0.4;
const double kRightThighMDHAlpha = 0.0;
const double kRightThighMDHD = 0.0;
const double kRightThighMDHTheta = 0.0;
const double kRightThighInitialJointVelocity = 0.0;
// const double kRightThighJointMin = -0.175; // realistic
const double kRightThighJointMin = -0.52; // generous
// const double kRightThighJointMax = 1.57; // fairly realistic
const double kRightThighJointMax = 0.52; // restricted
const double kRightThighJointLimitSpringConstant = kDefaultJointLimitSpringConstant;
const double kRightThighJointLimitDamperConstant = kDefaultJointLimitDamperConstant;
const double kRightThighJointFriction = kDefaultJointFriction;

// -------------------------------------------
// Left Leg
const char * const kLeftLegPartName = "LeftLeg";
const char * const kLeftLegGraphicFile = "LeftLeg.xan";
const double kLeftLegMass = 4.16;
const CartesianTensor kLeftLegMOI = {{0.007, 0.00, 0.00},
                                     {0.00, 0.25, 0.00},
                                     {0.00, 0.00, 0.25}};
const CartesianVector kLeftLegCG =  {0.215, 0.00, 0.00};
const double kLeftLegMDHA = 0.396;
const double kLeftLegMDHAlpha = 0.0;
const double kLeftLegMDHD = 0.0;
const double kLeftLegMDHTheta = 0.0;
const double kLeftLegInitialJointVelocity = 0.0;
// const double kLeftLegJointMin = -1.57; // fairly realistic
const double kLeftLegJointMin = -0.79; // restricted
const double kLeftLegJointMax = 0.00;
const double kLeftLegJointLimitSpringConstant = kDefaultJointLimitSpringConstant;
const double kLeftLegJointLimitDamperConstant = kDefaultJointLimitDamperConstant;
const double kLeftLegJointFriction = kDefaultJointFriction;

// -------------------------------------------
// Right Leg
const char * const kRightLegPartName = "RightLeg";
const char * const kRightLegGraphicFile = "RightLeg.xan";
const double kRightLegMass = 4.16;
const CartesianTensor kRightLegMOI = {{0.007, 0.00, 0.00},
                                      {0.00, 0.25, 0.00},
                                      {0.00, 0.00, 0.25}};
const CartesianVector kRightLegCG =  {0.215, 0.00, 0.00};
const double kRightLegMDHA = 0.396;
const double kRightLegMDHAlpha = 0.0;
const double kRightLegMDHD = 0.0;
const double kRightLegMDHTheta = 0.0;
const double kRightLegInitialJointVelocity = 0.0;
// const double kRightLegJointMin = -1.57; // fairly realistic
const double kRightLegJointMin = -0.79; // restricted
const double kRightLegJointMax = 0.00;
const double kRightLegJointLimitSpringConstant = kDefaultJointLimitSpringConstant;
const double kRightLegJointLimitDamperConstant = kDefaultJointLimitDamperConstant;
const double kRightLegJointFriction = kDefaultJointFriction;

// -------------------------------------------
// Left Foot
const char * const kLeftFootPartName = "LeftFoot";
const char * const kLeftFootGraphicFile = "LeftFoot.xan";
const double kLeftFootMass = 1.20;
const CartesianTensor kLeftFootMOI = {{0.023, 0.00,   0.00},
                                      {0.00,  0.0018, 0.00},
                                      {0.00,  0.00,   0.023}};
const CartesianVector kLeftFootCG =  {0.00, 0.115, 0.00};
const double kLeftFootMDHA = 0.43;
const double kLeftFootMDHAlpha = 0.0;
const double kLeftFootMDHD = 0.0;
const double kLeftFootMDHTheta = 0.0;
const double kLeftFootInitialJointVelocity = 0.0;
const double kLeftFootJointMin = -1.047;
const double kLeftFootJointMax = 0.576;
const double kLeftFootJointLimitSpringConstant = kDefaultJointLimitSpringConstant;
const double kLeftFootJointLimitDamperConstant = kDefaultJointLimitDamperConstant;
const double kLeftFootJointFriction = kDefaultJointFriction;

// -------------------------------------------
// Right Foot
const char * const kRightFootPartName = "RightFoot";
const char * const kRightFootGraphicFile = "RightFoot.xan";
const double kRightFootMass = 1.20;
const CartesianTensor kRightFootMOI = {{0.023, 0.00,   0.00},
                                       {0.00,  0.0018, 0.00},
                                       {0.00,  0.00,   0.023}};
const CartesianVector kRightFootCG =  {0.00, 0.115, 0.00};
const double kRightFootMDHA = 0.43;
const double kRightFootMDHAlpha = 0.0;
const double kRightFootMDHD = 0.0;
const double kRightFootMDHTheta = 0.0;
const double kRightFootInitialJointVelocity = 0.0;
const double kRightFootJointMin = -1.047;
const double kRightFootJointMax = 0.576;
const double kRightFootJointLimitSpringConstant = kDefaultJointLimitSpringConstant;
const double kRightFootJointLimitDamperConstant = kDefaultJointLimitDamperConstant;
const double kRightFootJointFriction = kDefaultJointFriction;

// -------------------------------------------
// Muscles
// -------------------------------------------

// Left Hip Extensor
const char * const kLeftHipExtensorName = "LeftHipExtensor";
const CartesianVector kLeftHipExtensorOrigin = {0.452, -0.076,  0.0};
const CartesianVector kLeftHipExtensorInsertion = {0.30, 0.005, 0.0};
const double kLeftHipExtensorMinLength = 0.0;
const double kLeftHipExtensorDampingFactor = 40.0;
const double kLeftHipExtensorControlFactor = 3000.0;
const double kLeftHipExtensorSpringConstant = 1000.0;

// Left Hip Flexor
const char * const kLeftHipFlexorName = "LeftHipFlexor";
const CartesianVector kLeftHipFlexorOrigin = {0.319, 0.071,  0.0};
const CartesianVector kLeftHipFlexorInsertion = {0.16, 0.029, 0.0};
const double kLeftHipFlexorMinLength = 0.0;
const double kLeftHipFlexorDampingFactor = 40.0;
const double kLeftHipFlexorControlFactor = 3000.0;
const double kLeftHipFlexorSpringConstant = 1000.0;

// Right Hip Extensor
const char * const kRightHipExtensorName = "RightHipExtensor";
const CartesianVector kRightHipExtensorOrigin = {0.452, -0.076,  0.0};
const CartesianVector kRightHipExtensorInsertion = {0.30, 0.005, 0.0};
const double kRightHipExtensorMinLength = 0.0;
const double kRightHipExtensorDampingFactor = 40.0;
const double kRightHipExtensorControlFactor = 3000.0;
const double kRightHipExtensorSpringConstant = 1000.0;

// Right Hip Flexor
const char * const kRightHipFlexorName = "RightHipFlexor";
const CartesianVector kRightHipFlexorOrigin = {0.319, 0.071,  0.0};
const CartesianVector kRightHipFlexorInsertion = {0.16, 0.029, 0.0};
const double kRightHipFlexorMinLength = 0.0;
const double kRightHipFlexorDampingFactor = 40.0;
const double kRightHipFlexorControlFactor = 3000.0;
const double kRightHipFlexorSpringConstant = 1000.0;

// Left Knee Extensor
const char * const kLeftKneeExtensorName = "LeftKneeExtensor";
const CartesianVector kLeftKneeExtensorOrigin = {0.44, 0.077,  0.0};
const CartesianVector kLeftKneeExtensorInsertion = {0.076, 0.043, 0.0};
const double kLeftKneeExtensorMinLength = 0.0;
const double kLeftKneeExtensorDampingFactor = 40.0;
const double kLeftKneeExtensorControlFactor = 3000.0;
const double kLeftKneeExtensorSpringConstant = 1000.0;

// Left Knee Flexor
const char * const kLeftKneeFlexorName = "LeftKneeFlexor";
const CartesianVector kLeftKneeFlexorOrigin = {0.32, -0.068,  0.0};
const CartesianVector kLeftKneeFlexorInsertion = {0.08, -0.01, 0.0};
const double kLeftKneeFlexorMinLength = 0.0;
const double kLeftKneeFlexorDampingFactor = 40.0;
const double kLeftKneeFlexorControlFactor = 3000.0;
const double kLeftKneeFlexorSpringConstant = 1000.0;

// Right Knee Extensor
const char * const kRightKneeExtensorName = "RightKneeExtensor";
const CartesianVector kRightKneeExtensorOrigin = {0.44, 0.077,  0.0};
const CartesianVector kRightKneeExtensorInsertion = {0.076, 0.043, 0.0};
const double kRightKneeExtensorMinLength = 0.0;
const double kRightKneeExtensorDampingFactor = 40.0;
const double kRightKneeExtensorControlFactor = 3000.0;
const double kRightKneeExtensorSpringConstant = 1000.0;

// Right Knee Flexor
const char * const kRightKneeFlexorName = "RightKneeFlexor";
const CartesianVector kRightKneeFlexorOrigin = {0.32, -0.068,  0.0};
const CartesianVector kRightKneeFlexorInsertion = {0.08, -0.01, 0.0};
const double kRightKneeFlexorMinLength = 0.0;
const double kRightKneeFlexorDampingFactor = 40.0;
const double kRightKneeFlexorControlFactor = 3000.0;
const double kRightKneeFlexorSpringConstant = 1000.0;

// Left Ankle Extensor (dorsiflexion)
const char * const kLeftAnkleExtensorName = "LeftAnkleExtensor";
const CartesianVector kLeftAnkleExtensorOrigin = {0.14, 0.033, 0.0};
const CartesianVector kLeftAnkleExtensorInsertion = {0.0, 0.057, 0.0};
const double kLeftAnkleExtensorMinLength = 0.0;
const double kLeftAnkleExtensorDampingFactor = 40.0;
const double kLeftAnkleExtensorControlFactor = 3000.0;
const double kLeftAnkleExtensorSpringConstant = 1000.0;

// Left Ankle Flexor (plantarflexion)
const char * const kLeftAnkleFlexorName = "LeftAnkleFlexor";
const CartesianVector kLeftAnkleFlexorOrigin = {0.057, -0.029,  0.0};
const CartesianVector kLeftAnkleFlexorInsertion = {0.038, -0.057, 0.0};
const double kLeftAnkleFlexorMinLength = 0.0;
const double kLeftAnkleFlexorDampingFactor = 40.0;
const double kLeftAnkleFlexorControlFactor = 3000.0;
const double kLeftAnkleFlexorSpringConstant = 1000.0;

// Right Ankle Extensor (dorsiflexion)
const char * const kRightAnkleExtensorName = "RightAnkleExtensor";
const CartesianVector kRightAnkleExtensorOrigin = {0.14, 0.033, 0.0};
const CartesianVector kRightAnkleExtensorInsertion = {0.0, 0.057, 0.0};
const double kRightAnkleExtensorMinLength = 0.0;
const double kRightAnkleExtensorDampingFactor = 40.0;
const double kRightAnkleExtensorControlFactor = 3000.0;
const double kRightAnkleExtensorSpringConstant = 1000.0;

// Right Ankle Flexor (plantarflexion)
const char * const kRightAnkleFlexorName = "RightAnkleFlexor";
const CartesianVector kRightAnkleFlexorOrigin = {0.057, -0.029,  0.0};
const CartesianVector kRightAnkleFlexorInsertion = {0.038, -0.057, 0.0};
const double kRightAnkleFlexorMinLength = 0.0;
const double kRightAnkleFlexorDampingFactor = 40.0;
const double kRightAnkleFlexorControlFactor = 3000.0;
const double kRightAnkleFlexorSpringConstant = 1000.0;

// -------------------------------------------
// Controller
// -------------------------------------------

const double kControllerStepTime = 0.01;
const char * const kControllerSuffix = "Controller";

// -------------------------------------------
// Contact Models
// -------------------------------------------

// Left Foot

const char * const kLeftFootContactName = "LeftFootContact";
const int kLeftFootNumContactPoints = 4;
const CartesianVector kLeftFootContactPositions[kLeftFootNumContactPoints] = 
                                                   {{0.05, -0.05, -0.044},
                                                    {0.05, -0.05,  0.044},
                                                    {0.05,  0.10, -0.044},
                                                    {0.05,  0.10,  0.044}};

// Right Foot

const char * const kRightFootContactName = "RightFootContact";
const int kRightFootNumContactPoints = 4;
const CartesianVector kRightFootContactPositions[kRightFootNumContactPoints] = 
                                                   {{0.05, -0.05, -0.044},
                                                    {0.05, -0.05,  0.044},
                                                    {0.05,  0.10, -0.044},
                                                    {0.05,  0.10,  0.044}};

#endif // SEGMENTPARAMETERS_H
