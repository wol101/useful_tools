// Segments.cc - the various segment objects

#include <string.h>

#include "Segments.h"
#include "Simulation.h"
#include "ModifiedContactModel.h"
#include "MAMuscle.h"
#include "StrapForceAnchor.h"
#include "DebugControl.h"
#include "Util.h"
#include "SideStabilizer.h"

#ifdef USE_OPENGL
#include <gl.h>
#include <glut.h>
#include "LoadObj.h"
#endif

#include "DataFile.h"

// Note having the single load routine and hundreds of globals is not
// a good way of doing things

// the globals are declare here
#define SEG_PARAM_EXTERN
#include "SegmentParameters.h"

extern Simulation *gSimulation;
extern float gTimeLimit;

#ifdef USE_OPENGL
// axes
extern int gAxisFlag;
extern float gAxisSize;

static void DrawAxes();
static void DrawCOM(double x, double y, double z);
#endif

// Torso defined here

Torso::Torso():
dmMobileBaseLink()
{
    // set the link name
    setName(kTorsoPartName);

    // load up a graphics model if required
#ifdef USE_OPENGL
    if (gSimulation->UseGraphics())
    {
        GLuint *dlist = new GLuint;
        *dlist = LoadObj(kTorsoGraphicFile, kTorsoGraphicScale, kTorsoGraphicScale, kTorsoGraphicScale);
        setUserData((void *) dlist);
    }
#endif

    setInertiaParameters(kTorsoMass, kTorsoMOI, kTorsoCG);
    setState(kTorsoPosition, kTorsoVelocity);

    // add the muscle forces

    StrapForceAnchor *strapForceAnchor;
    MAMuscle *strapForce;

    // Left Hip Extensor

    strapForce = gSimulation->GetCPG()->GetLeftHipExtensor();
    strapForce->SetNumAnchors(2);
    strapForce->setName(kLeftHipExtensorName);
    strapForce->SetUnloadedLength(kLeftHipExtensorLength);
    strapForce->SetSpringConstant(kLeftHipExtensorSpringConstant);
    strapForce->SetVMax(kLeftHipExtensorLength * kDefaultMuscleVMaxFactor);
    strapForce->SetF0(kLeftHipExtensorPCA * kDefaultMuscleForcePerUnitArea);
    strapForce->SetK(kDefaultMuscleActivationK);

    strapForceAnchor = new StrapForceAnchor();
    strapForceAnchor->setName(strcat(kLeftHipExtensorName, "Origin"));
    strapForceAnchor->SetStrapForce(this, kLeftHipExtensorOrigin, strapForce, 0);
    addForce(strapForceAnchor);
    gSimulation->GetForceList()->AddForce(strapForceAnchor);

    // Left Hip Flexor
    strapForce = gSimulation->GetCPG()->GetLeftHipFlexor();
    strapForce->SetNumAnchors(2);
    strapForce->setName(kLeftHipFlexorName);
    strapForce->SetUnloadedLength(kLeftHipFlexorLength);
    strapForce->SetSpringConstant(kLeftHipFlexorSpringConstant);
    strapForce->SetVMax(kLeftHipFlexorLength * kDefaultMuscleVMaxFactor);
    strapForce->SetF0(kLeftHipFlexorPCA * kDefaultMuscleForcePerUnitArea);
    strapForce->SetK(kDefaultMuscleActivationK);

    strapForceAnchor = new StrapForceAnchor();
    strapForceAnchor->setName(strcat(kLeftHipFlexorName, "Origin"));
    strapForceAnchor->SetStrapForce(this, kLeftHipFlexorOrigin, strapForce, 0);
    addForce(strapForceAnchor);
    gSimulation->GetForceList()->AddForce(strapForceAnchor);

    // Right Hip Extensor
    strapForce = gSimulation->GetCPG()->GetRightHipExtensor();
    strapForce->SetNumAnchors(2);
    strapForce->setName(kRightHipExtensorName);
    strapForce->SetUnloadedLength(kRightHipExtensorLength);
    strapForce->SetSpringConstant(kRightHipExtensorSpringConstant);
    strapForce->SetVMax(kRightHipExtensorLength * kDefaultMuscleVMaxFactor);
    strapForce->SetF0(kRightHipExtensorPCA * kDefaultMuscleForcePerUnitArea);
    strapForce->SetK(kDefaultMuscleActivationK);

    strapForceAnchor = new StrapForceAnchor();
    strapForceAnchor->setName(strcat(kRightHipExtensorName, "Origin"));
    strapForceAnchor->SetStrapForce(this, kRightHipExtensorOrigin, strapForce, 0);
    addForce(strapForceAnchor);
    gSimulation->GetForceList()->AddForce(strapForceAnchor);

    // Right Hip Flexor
    strapForce = gSimulation->GetCPG()->GetRightHipFlexor();
    strapForce->SetNumAnchors(2);
    strapForce->setName(kRightHipFlexorName);
    strapForce->SetUnloadedLength(kRightHipFlexorLength);
    strapForce->SetSpringConstant(kRightHipFlexorSpringConstant);
    strapForce->SetVMax(kRightHipFlexorLength * kDefaultMuscleVMaxFactor);
    strapForce->SetF0(kRightHipFlexorPCA * kDefaultMuscleForcePerUnitArea);
    strapForce->SetK(kDefaultMuscleActivationK);

    strapForceAnchor = new StrapForceAnchor();
    strapForceAnchor->setName(strcat(kRightHipFlexorName, "Origin"));
    strapForceAnchor->SetStrapForce(this, kRightHipFlexorOrigin, strapForce, 0);
    addForce(strapForceAnchor);
    gSimulation->GetForceList()->AddForce(strapForceAnchor);

    // and finally add the side stabilizer
    SideStabilizer *sideStabilizer = new SideStabilizer();
    addForce(sideStabilizer);
    gSimulation->GetForceList()->AddForce(sideStabilizer);

    if (gDebug == SegmentsDebug)
    {
        *gDebugStream << "Torso::Torso()\tgetName()\t" << getName() << "\n";
    }

}

Torso::~Torso()
{
#ifdef USE_OPENGL
    delete (GLuint *)m_user_data;
#endif
}

void
Torso::draw() const
{
#ifdef USE_OPENGL
dmMobileBaseLink::draw();
    DrawAxes();
    double mass;
    CartesianTensor inertia;
    CartesianVector cg_pos;
    getInertiaParameters(mass, inertia, cg_pos);
    DrawCOM(cg_pos[0], cg_pos[1], cg_pos[2]);

#endif
}

// Left Limb Segments

LeftThigh::LeftThigh():
dmRevoluteLink()
{
    // set the link name
    setName(kLeftThighPartName);

    // load up a graphics model if required
#ifdef USE_OPENGL
    if (gSimulation->UseGraphics())
    {
        GLuint *dlist = new GLuint;
        *dlist = LoadObj(kLeftThighGraphicFile, kLeftThighGraphicScale, kLeftThighGraphicScale, kLeftThighGraphicScale);
        setUserData((void *) dlist);
    }
#endif

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
    gSimulation->GetForceList()->AddForce(strapForceAnchor);

    // Left Hip Flexor
    strapForce = gSimulation->GetCPG()->GetLeftHipFlexor();

    strapForceAnchor = new StrapForceAnchor();
    strapForceAnchor->setName(strcat(kLeftHipFlexorName, "Insertion"));
    strapForceAnchor->SetStrapForce(this, kLeftHipFlexorInsertion, strapForce, 1);
    addForce(strapForceAnchor);
    gSimulation->GetForceList()->AddForce(strapForceAnchor);

    // Left Knee Extensor
    strapForce = gSimulation->GetCPG()->GetLeftKneeExtensor();
    if (kLeftKneeExtensorMidPoint[0] < 999) strapForce->SetNumAnchors(3);
    else strapForce->SetNumAnchors(2);
    strapForce->setName(kLeftKneeExtensorName);
    strapForce->SetUnloadedLength(kLeftKneeExtensorLength);
    strapForce->SetSpringConstant(kLeftKneeExtensorSpringConstant);
    strapForce->SetVMax(kLeftKneeExtensorLength * kDefaultMuscleVMaxFactor);
    strapForce->SetF0(kLeftKneeExtensorPCA * kDefaultMuscleForcePerUnitArea);
    strapForce->SetK(kDefaultMuscleActivationK);

    strapForceAnchor = new StrapForceAnchor();
    strapForceAnchor->setName(strcat(kLeftKneeExtensorName, "Origin"));
    strapForceAnchor->SetStrapForce(this, kLeftKneeExtensorOrigin, strapForce, 0);
    addForce(strapForceAnchor);
    gSimulation->GetForceList()->AddForce(strapForceAnchor);

    if (kLeftKneeExtensorMidPoint[0] < 999)
    {
        strapForceAnchor = new StrapForceAnchor();
        strapForceAnchor->setName(strcat(kLeftKneeExtensorName, "MidPoint"));
        strapForceAnchor->SetStrapForce(this, kLeftKneeExtensorMidPoint, strapForce, 1);
        addForce(strapForceAnchor);
        gSimulation->GetForceList()->AddForce(strapForceAnchor);
    }

    // Left Knee Flexor
    strapForce = gSimulation->GetCPG()->GetLeftKneeFlexor();
    if (kLeftKneeFlexorMidPoint[0] < 999) strapForce->SetNumAnchors(3);
    else strapForce->SetNumAnchors(2);
    strapForce->setName(kLeftKneeFlexorName);
    strapForce->SetUnloadedLength(kLeftKneeFlexorLength);
    strapForce->SetSpringConstant(kLeftKneeFlexorSpringConstant);
    strapForce->SetVMax(kLeftKneeFlexorLength * kDefaultMuscleVMaxFactor);
    strapForce->SetF0(kLeftKneeFlexorPCA * kDefaultMuscleForcePerUnitArea);
    strapForce->SetK(kDefaultMuscleActivationK);

    strapForceAnchor = new StrapForceAnchor();
    strapForceAnchor->setName(strcat(kLeftKneeFlexorName, "Origin"));
    strapForceAnchor->SetStrapForce(this, kLeftKneeFlexorOrigin, strapForce, 0);
    addForce(strapForceAnchor);
    gSimulation->GetForceList()->AddForce(strapForceAnchor);

    if (kLeftKneeFlexorMidPoint[0] < 999)
    {
        strapForceAnchor = new StrapForceAnchor();
        strapForceAnchor->setName(strcat(kLeftKneeFlexorName, "MidPoint"));
        strapForceAnchor->SetStrapForce(this, kLeftKneeFlexorMidPoint, strapForce, 1);
        addForce(strapForceAnchor);
        gSimulation->GetForceList()->AddForce(strapForceAnchor);
    }

    if (gDebug == SegmentsDebug)
    {
        *gDebugStream << "LeftThigh::LeftThigh()\tgetName()\t" << getName() << "\n";
    }

}

LeftThigh::~LeftThigh()
{
#ifdef USE_OPENGL
    delete (GLuint *)m_user_data;
#endif
}

void
LeftThigh::UpdateMidpoint()
{
    double a, alpha, d, theta;
    CartesianVector location;

    // calculate amount of rotation needed
    // (half the rotation from the start position)
    gSimulation->GetLeftLeg()->getMDHParameters(&a, &alpha, &d, &theta);
    theta = (theta - kLeftLegJointMax) / 2;
    if (gDebug == SegmentsDebug)
    {
        *gDebugStream << "LeftThigh::UpdateMidpoint\ta\t" << a <<
        "\talpha\t" << alpha << "\td\t" << d << "\ttheta\t" << theta << "\n";
    }

    if (kLeftKneeExtensorMidPoint[0] < 999)
    {
        // Left Knee Extensor
Util::Copy3x1(kLeftKneeExtensorMidPoint, location);
Util::MDHTransform(a, alpha, d, theta, location);
        gSimulation->GetCPG()->GetLeftKneeExtensor()->SetAnchorLocation(1, location);

        if (gDebug == SegmentsDebug)
        {
            *gDebugStream << "LeftThigh::UpdateMidpoint" <<
            "\tExtensor location\t" << location[0] << "\t" <<
            location[1] << "\t" << location[2] << "\n";
        }
    }

    if (kLeftKneeFlexorMidPoint[0] < 999)
    {
        // Left Knee Flexor
Util::Copy3x1(kLeftKneeFlexorMidPoint, location);
Util::MDHTransform(a, alpha, d, theta, location);
        gSimulation->GetCPG()->GetLeftKneeFlexor()->SetAnchorLocation(1, location);

        if (gDebug == SegmentsDebug)
        {
            *gDebugStream << "LeftThigh::UpdateMidpoint" <<
            "\tFlexor location\t" << location[0] << "\t" <<
            location[1] << "\t" << location[2] << "\n";
        }
    }
}

void
LeftThigh::draw() const
{
#ifdef USE_OPENGL
dmRevoluteLink::draw();
    DrawAxes();
    double mass;
    CartesianTensor inertia;
    CartesianVector cg_pos;
    getInertiaParameters(mass, inertia, cg_pos);
    DrawCOM(cg_pos[0], cg_pos[1], cg_pos[2]);

#endif
}

LeftLeg::LeftLeg():
dmRevoluteLink()
{
    // set the link name
    setName(kLeftLegPartName);

    // load up a graphics model if required
#ifdef USE_OPENGL
    if (gSimulation->UseGraphics())
    {
        GLuint *dlist = new GLuint;
        *dlist = LoadObj(kLeftLegGraphicFile, kLeftLegGraphicScale, kLeftLegGraphicScale, kLeftLegGraphicScale);
        setUserData((void *) dlist);
    }
#endif

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
    if (kLeftKneeExtensorMidPoint[0] < 999) strapForceAnchor->SetStrapForce(this, kLeftKneeExtensorInsertion, strapForce, 2);
    else strapForceAnchor->SetStrapForce(this, kLeftKneeExtensorInsertion, strapForce, 1);
    addForce(strapForceAnchor);
    gSimulation->GetForceList()->AddForce(strapForceAnchor);

    // Left Knee Flexor
    strapForce = gSimulation->GetCPG()->GetLeftKneeFlexor();

    strapForceAnchor = new StrapForceAnchor();
    strapForceAnchor->setName(strcat(kLeftKneeFlexorName, "Insertion"));
    if (kLeftKneeFlexorMidPoint[0] < 999) strapForceAnchor->SetStrapForce(this, kLeftKneeFlexorInsertion, strapForce, 2);
    else strapForceAnchor->SetStrapForce(this, kLeftKneeFlexorInsertion, strapForce, 1);
    addForce(strapForceAnchor);
    gSimulation->GetForceList()->AddForce(strapForceAnchor);

    // Left Ankle Extensor
    strapForce = gSimulation->GetCPG()->GetLeftAnkleExtensor();
    if (kLeftAnkleExtensorMidPoint[0] < 999) strapForce->SetNumAnchors(3);
    else strapForce->SetNumAnchors(2);
    strapForce->setName(kLeftAnkleExtensorName);
    strapForce->SetUnloadedLength(kLeftAnkleExtensorLength);
    strapForce->SetSpringConstant(kLeftAnkleExtensorSpringConstant);
    strapForce->SetVMax(kLeftAnkleExtensorLength * kDefaultMuscleVMaxFactor);
    strapForce->SetF0(kLeftAnkleExtensorPCA * kDefaultMuscleForcePerUnitArea);
    strapForce->SetK(kDefaultMuscleActivationK);

    strapForceAnchor = new StrapForceAnchor();
    strapForceAnchor->setName(strcat(kLeftAnkleExtensorName, "Origin"));
    strapForceAnchor->SetStrapForce(this, kLeftAnkleExtensorOrigin, strapForce, 0);
    addForce(strapForceAnchor);
    gSimulation->GetForceList()->AddForce(strapForceAnchor);

    if (kLeftAnkleExtensorMidPoint[0] < 999)
    {
        strapForceAnchor = new StrapForceAnchor();
        strapForceAnchor->setName(strcat(kLeftAnkleExtensorName, "MidPoint"));
        strapForceAnchor->SetStrapForce(this, kLeftAnkleExtensorMidPoint, strapForce, 1);
        addForce(strapForceAnchor);
        gSimulation->GetForceList()->AddForce(strapForceAnchor);
    }

    // Left Ankle Flexor
    strapForce = gSimulation->GetCPG()->GetLeftAnkleFlexor();
    if (kLeftAnkleFlexorMidPoint[0] < 999) strapForce->SetNumAnchors(3);
    else strapForce->SetNumAnchors(2);
    strapForce->setName(kLeftAnkleFlexorName);
    strapForce->SetUnloadedLength(kLeftAnkleFlexorLength);
    strapForce->SetSpringConstant(kLeftAnkleFlexorSpringConstant);
    strapForce->SetVMax(kLeftAnkleFlexorLength * kDefaultMuscleVMaxFactor);
    strapForce->SetF0(kLeftAnkleFlexorPCA * kDefaultMuscleForcePerUnitArea);
    strapForce->SetK(kDefaultMuscleActivationK);

    strapForceAnchor = new StrapForceAnchor();
    strapForceAnchor->setName(strcat(kLeftAnkleFlexorName, "Origin"));
    strapForceAnchor->SetStrapForce(this, kLeftAnkleFlexorOrigin, strapForce, 0);
    addForce(strapForceAnchor);
    gSimulation->GetForceList()->AddForce(strapForceAnchor);

    if (kLeftAnkleFlexorMidPoint[0] < 999)
    {
        strapForceAnchor = new StrapForceAnchor();
        strapForceAnchor->setName(strcat(kLeftAnkleFlexorName, "MidPoint"));
        strapForceAnchor->SetStrapForce(this, kLeftAnkleFlexorMidPoint, strapForce, 1);
        addForce(strapForceAnchor);
        gSimulation->GetForceList()->AddForce(strapForceAnchor);
    }

    if (gDebug == SegmentsDebug)
    {
        *gDebugStream << "LeftLeg::LeftLeg()\tgetName()\t" << getName() << "\n";
    }

}

LeftLeg::~LeftLeg()
{
#ifdef USE_OPENGL
    delete (GLuint *)m_user_data;
#endif
}

void
LeftLeg::UpdateMidpoint()
{
    double a, alpha, d, theta;
    CartesianVector location;

    // calculate amount of rotation needed
    // (half the rotation from the start position)
    gSimulation->GetLeftFoot()->getMDHParameters(&a, &alpha, &d, &theta);
    theta = (theta - kLeftFootJointMin) / 2;
    if (gDebug == SegmentsDebug)
    {
        *gDebugStream << "LeftFoot::UpdateMidpoint\ta\t" << a <<
        "\talpha\t" << alpha << "\td\t" << d << "\ttheta\t" << theta << "\n";
    }

    if (kLeftAnkleExtensorMidPoint[0] < 999)
    {
        // Left Ankle Extensor
Util::Copy3x1(kLeftAnkleExtensorMidPoint, location);
Util::MDHTransform(a, alpha, d, theta, location);
        gSimulation->GetCPG()->GetLeftAnkleExtensor()->SetAnchorLocation(1, location);

        if (gDebug == SegmentsDebug)
        {
            *gDebugStream << "LeftFoot::UpdateMidpoint" <<
            "\tExtensor location\t" << location[0] << "\t" <<
            location[1] << "\t" << location[2] << "\n";
        }
    }

    if (kLeftAnkleFlexorMidPoint[0] < 999)
    {
        // Left Ankle Flexor
Util::Copy3x1(kLeftAnkleFlexorMidPoint, location);
Util::MDHTransform(a, alpha, d, theta, location);
        gSimulation->GetCPG()->GetLeftAnkleFlexor()->SetAnchorLocation(1, location);

        if (gDebug == SegmentsDebug)
        {
            *gDebugStream << "LeftFoot::UpdateMidpoint" <<
            "\tFlexor location\t" << location[0] << "\t" <<
            location[1] << "\t" << location[2] << "\n";
        }
    }
}

void
LeftLeg::draw() const
{
#ifdef USE_OPENGL
dmRevoluteLink::draw();
    DrawAxes();
    double mass;
    CartesianTensor inertia;
    CartesianVector cg_pos;
    getInertiaParameters(mass, inertia, cg_pos);
    DrawCOM(cg_pos[0], cg_pos[1], cg_pos[2]);

#endif
}

LeftFoot::LeftFoot():
dmRevoluteLink()
{
    // set the link name
    setName(kLeftFootPartName);

    // load up a graphics model if required
#ifdef USE_OPENGL
    if (gSimulation->UseGraphics())
    {
        GLuint *dlist = new GLuint;
        *dlist = LoadObj(kLeftFootGraphicFile, kLeftFootGraphicScale, kLeftFootGraphicScale, kLeftFootGraphicScale);
        setUserData((void *) dlist);
    }
#endif

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
    if (kLeftAnkleExtensorMidPoint[0] < 999) strapForceAnchor->SetStrapForce(this, kLeftAnkleExtensorInsertion, strapForce, 2);
    else strapForceAnchor->SetStrapForce(this, kLeftAnkleExtensorInsertion, strapForce, 1);
    addForce(strapForceAnchor);
    gSimulation->GetForceList()->AddForce(strapForceAnchor);

    // Left Ankle Flexor
    strapForce = gSimulation->GetCPG()->GetLeftAnkleFlexor();

    strapForceAnchor = new StrapForceAnchor();
    strapForceAnchor->setName(strcat(kLeftAnkleFlexorName, "Insertion"));
    if (kLeftAnkleFlexorMidPoint[0] < 999) strapForceAnchor->SetStrapForce(this, kLeftAnkleFlexorInsertion, strapForce, 2);
    else strapForceAnchor->SetStrapForce(this, kLeftAnkleFlexorInsertion, strapForce, 1);
    addForce(strapForceAnchor);
    gSimulation->GetForceList()->AddForce(strapForceAnchor);

    // Contact Points
    ModifiedContactModel *contactModel = new ModifiedContactModel();
    contactModel->setContactPoints(kLeftFootNumContactPoints, kLeftFootContactPositions);
    contactModel->setName(kLeftFootContactName);
    addForce(contactModel);
    gSimulation->GetForceList()->AddForce(contactModel);

    if (gDebug == SegmentsDebug)
    {
        *gDebugStream << "LeftFoot::LeftFoot()\tgetName()\t" << getName() << "\n";
    }

}

LeftFoot::~LeftFoot()
{
#ifdef USE_OPENGL
    delete (GLuint *)m_user_data;
#endif
}

void
LeftFoot::draw() const
{
#ifdef USE_OPENGL
dmRevoluteLink::draw();
    DrawAxes();
    double mass;
    CartesianTensor inertia;
    CartesianVector cg_pos;
    getInertiaParameters(mass, inertia, cg_pos);
    DrawCOM(cg_pos[0], cg_pos[1], cg_pos[2]);

#endif
}

// Right Limb Segments - these are just the left limb segements
// with a global replace of "Left" to "Right"

RightThigh::RightThigh():
dmRevoluteLink()
{
    // set the link name
    setName(kRightThighPartName);

    // load up a graphics model if required
#ifdef USE_OPENGL
    if (gSimulation->UseGraphics())
    {
        GLuint *dlist = new GLuint;
        *dlist = LoadObj(kRightThighGraphicFile, kRightThighGraphicScale, kRightThighGraphicScale, kRightThighGraphicScale);
        setUserData((void *) dlist);
    }
#endif

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
    gSimulation->GetForceList()->AddForce(strapForceAnchor);

    // Right Hip Flexor
    strapForce = gSimulation->GetCPG()->GetRightHipFlexor();

    strapForceAnchor = new StrapForceAnchor();
    strapForceAnchor->setName(strcat(kRightHipFlexorName, "Insertion"));
    strapForceAnchor->SetStrapForce(this, kRightHipFlexorInsertion, strapForce, 1);
    addForce(strapForceAnchor);
    gSimulation->GetForceList()->AddForce(strapForceAnchor);

    // Right Knee Extensor
    strapForce = gSimulation->GetCPG()->GetRightKneeExtensor();
    if (kRightKneeExtensorMidPoint[0] < 999) strapForce->SetNumAnchors(3);
    else strapForce->SetNumAnchors(2);
    strapForce->setName(kRightKneeExtensorName);
    strapForce->SetUnloadedLength(kRightKneeExtensorLength);
    strapForce->SetSpringConstant(kRightKneeExtensorSpringConstant);
    strapForce->SetVMax(kRightKneeExtensorLength * kDefaultMuscleVMaxFactor);
    strapForce->SetF0(kRightKneeExtensorPCA * kDefaultMuscleForcePerUnitArea);
    strapForce->SetK(kDefaultMuscleActivationK);

    strapForceAnchor = new StrapForceAnchor();
    strapForceAnchor->setName(strcat(kRightKneeExtensorName, "Origin"));
    strapForceAnchor->SetStrapForce(this, kRightKneeExtensorOrigin, strapForce, 0);
    addForce(strapForceAnchor);
    gSimulation->GetForceList()->AddForce(strapForceAnchor);

    if (kRightKneeExtensorMidPoint[0] < 999)
    {
        strapForceAnchor = new StrapForceAnchor();
        strapForceAnchor->setName(strcat(kRightKneeExtensorName, "MidPoint"));
        strapForceAnchor->SetStrapForce(this, kRightKneeExtensorMidPoint, strapForce, 1);
        addForce(strapForceAnchor);
        gSimulation->GetForceList()->AddForce(strapForceAnchor);
    }

    // Right Knee Flexor
    strapForce = gSimulation->GetCPG()->GetRightKneeFlexor();
    if (kRightKneeFlexorMidPoint[0] < 999) strapForce->SetNumAnchors(3);
    else strapForce->SetNumAnchors(2);
    strapForce->setName(kRightKneeFlexorName);
    strapForce->SetUnloadedLength(kRightKneeFlexorLength);
    strapForce->SetSpringConstant(kRightKneeFlexorSpringConstant);
    strapForce->SetVMax(kRightKneeFlexorLength * kDefaultMuscleVMaxFactor);
    strapForce->SetF0(kRightKneeFlexorPCA * kDefaultMuscleForcePerUnitArea);
    strapForce->SetK(kDefaultMuscleActivationK);

    strapForceAnchor = new StrapForceAnchor();
    strapForceAnchor->setName(strcat(kRightKneeFlexorName, "Origin"));
    strapForceAnchor->SetStrapForce(this, kRightKneeFlexorOrigin, strapForce, 0);
    addForce(strapForceAnchor);
    gSimulation->GetForceList()->AddForce(strapForceAnchor);

    if (kRightKneeFlexorMidPoint[0] < 999)
    {
        strapForceAnchor = new StrapForceAnchor();
        strapForceAnchor->setName(strcat(kRightKneeFlexorName, "MidPoint"));
        strapForceAnchor->SetStrapForce(this, kRightKneeFlexorMidPoint, strapForce, 1);
        addForce(strapForceAnchor);
        gSimulation->GetForceList()->AddForce(strapForceAnchor);
    }

    if (gDebug == SegmentsDebug)
    {
        *gDebugStream << "RightThigh::RightThigh()\tgetName()\t" << getName() << "\n";
    }

}

RightThigh::~RightThigh()
{
#ifdef USE_OPENGL
    delete (GLuint *)m_user_data;
#endif
}

void
RightThigh::UpdateMidpoint()
{
    double a, alpha, d, theta;
    CartesianVector location;

    // calculate amount of rotation needed
    // (half the rotation from the start position)
    gSimulation->GetRightLeg()->getMDHParameters(&a, &alpha, &d, &theta);
    theta = (theta - kRightLegJointMax) / 2;
    if (gDebug == SegmentsDebug)
    {
        *gDebugStream << "RightThigh::UpdateMidpoint\ta\t" << a <<
        "\talpha\t" << alpha << "\td\t" << d << "\ttheta\t" << theta << "\n";
    }

    if (kRightKneeExtensorMidPoint[0] < 999)
    {
        // Right Knee Extensor
Util::Copy3x1(kRightKneeExtensorMidPoint, location);
Util::MDHTransform(a, alpha, d, theta, location);
        gSimulation->GetCPG()->GetRightKneeExtensor()->SetAnchorLocation(1, location);

        if (gDebug == SegmentsDebug)
        {
            *gDebugStream << "RightThigh::UpdateMidpoint" <<
            "\tExtensor location\t" << location[0] << "\t" <<
            location[1] << "\t" << location[2] << "\n";
        }
    }

    if (kRightKneeFlexorMidPoint[0] < 999)
    {
        // Right Knee Flexor
Util::Copy3x1(kRightKneeFlexorMidPoint, location);
Util::MDHTransform(a, alpha, d, theta, location);
        gSimulation->GetCPG()->GetRightKneeFlexor()->SetAnchorLocation(1, location);

        if (gDebug == SegmentsDebug)
        {
            *gDebugStream << "RightThigh::UpdateMidpoint" <<
            "\tFlexor location\t" << location[0] << "\t" <<
            location[1] << "\t" << location[2] << "\n";
        }
    }
}

void
RightThigh::draw() const
{
#ifdef USE_OPENGL
dmRevoluteLink::draw();
    DrawAxes();
    double mass;
    CartesianTensor inertia;
    CartesianVector cg_pos;
    getInertiaParameters(mass, inertia, cg_pos);
    DrawCOM(cg_pos[0], cg_pos[1], cg_pos[2]);

#endif
}

RightLeg::RightLeg():
dmRevoluteLink()
{
    // set the link name
    setName(kRightLegPartName);

    // load up a graphics model if required
#ifdef USE_OPENGL
    if (gSimulation->UseGraphics())
    {
        GLuint *dlist = new GLuint;
        *dlist = LoadObj(kRightLegGraphicFile, kRightLegGraphicScale, kRightLegGraphicScale, kRightLegGraphicScale);
        setUserData((void *) dlist);
    }
#endif

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
    if (kRightKneeExtensorMidPoint[0] < 999) strapForceAnchor->SetStrapForce(this, kRightKneeExtensorInsertion, strapForce, 2);
    else strapForceAnchor->SetStrapForce(this, kRightKneeExtensorInsertion, strapForce, 1);
    addForce(strapForceAnchor);
    gSimulation->GetForceList()->AddForce(strapForceAnchor);

    // Right Knee Flexor
    strapForce = gSimulation->GetCPG()->GetRightKneeFlexor();

    strapForceAnchor = new StrapForceAnchor();
    strapForceAnchor->setName(strcat(kRightKneeFlexorName, "Insertion"));
    if (kRightKneeFlexorMidPoint[0] < 999) strapForceAnchor->SetStrapForce(this, kRightKneeFlexorInsertion, strapForce, 2);
    else strapForceAnchor->SetStrapForce(this, kRightKneeFlexorInsertion, strapForce, 1);
    addForce(strapForceAnchor);
    gSimulation->GetForceList()->AddForce(strapForceAnchor);

    // Right Ankle Extensor
    strapForce = gSimulation->GetCPG()->GetRightAnkleExtensor();
    if (kRightAnkleExtensorMidPoint[0] < 999) strapForce->SetNumAnchors(3);
    else strapForce->SetNumAnchors(2);
    strapForce->setName(kRightAnkleExtensorName);
    strapForce->SetUnloadedLength(kRightAnkleExtensorLength);
    strapForce->SetSpringConstant(kRightAnkleExtensorSpringConstant);
    strapForce->SetVMax(kRightAnkleExtensorLength * kDefaultMuscleVMaxFactor);
    strapForce->SetF0(kRightAnkleExtensorPCA * kDefaultMuscleForcePerUnitArea);
    strapForce->SetK(kDefaultMuscleActivationK);

    strapForceAnchor = new StrapForceAnchor();
    strapForceAnchor->setName(strcat(kRightAnkleExtensorName, "Origin"));
    strapForceAnchor->SetStrapForce(this, kRightAnkleExtensorOrigin, strapForce, 0);
    addForce(strapForceAnchor);
    gSimulation->GetForceList()->AddForce(strapForceAnchor);

    if (kRightAnkleExtensorMidPoint[0] < 999)
    {
        strapForceAnchor = new StrapForceAnchor();
        strapForceAnchor->setName(strcat(kRightAnkleExtensorName, "MidPoint"));
        strapForceAnchor->SetStrapForce(this, kRightAnkleExtensorMidPoint, strapForce, 1);
        addForce(strapForceAnchor);
        gSimulation->GetForceList()->AddForce(strapForceAnchor);
    }

    // Right Ankle Flexor
    strapForce = gSimulation->GetCPG()->GetRightAnkleFlexor();
    if (kRightAnkleFlexorMidPoint[0] < 999) strapForce->SetNumAnchors(3);
    else strapForce->SetNumAnchors(2);
    strapForce->setName(kRightAnkleFlexorName);
    strapForce->SetUnloadedLength(kRightAnkleFlexorLength);
    strapForce->SetSpringConstant(kRightAnkleFlexorSpringConstant);
    strapForce->SetVMax(kRightAnkleFlexorLength * kDefaultMuscleVMaxFactor);
    strapForce->SetF0(kRightAnkleFlexorPCA * kDefaultMuscleForcePerUnitArea);
    strapForce->SetK(kDefaultMuscleActivationK);

    strapForceAnchor = new StrapForceAnchor();
    strapForceAnchor->setName(strcat(kRightAnkleFlexorName, "Origin"));
    strapForceAnchor->SetStrapForce(this, kRightAnkleFlexorOrigin, strapForce, 0);
    addForce(strapForceAnchor);
    gSimulation->GetForceList()->AddForce(strapForceAnchor);

    if (kRightAnkleFlexorMidPoint[0] < 999)
    {
        strapForceAnchor = new StrapForceAnchor();
        strapForceAnchor->setName(strcat(kRightAnkleFlexorName, "MidPoint"));
        strapForceAnchor->SetStrapForce(this, kRightAnkleFlexorMidPoint, strapForce, 1);
        addForce(strapForceAnchor);
        gSimulation->GetForceList()->AddForce(strapForceAnchor);
    }

    if (gDebug == SegmentsDebug)
    {
        *gDebugStream << "RightLeg::RightLeg()\tgetName()\t" << getName() << "\n";
    }

}

RightLeg::~RightLeg()
{
#ifdef USE_OPENGL
    delete (GLuint *)m_user_data;
#endif
}

void
RightLeg::UpdateMidpoint()
{
    double a, alpha, d, theta;
    CartesianVector location;

    // calculate amount of rotation needed
    // (half the rotation from the start position)
    gSimulation->GetRightFoot()->getMDHParameters(&a, &alpha, &d, &theta);
    theta = (theta - kRightFootJointMin) / 2;
    if (gDebug == SegmentsDebug)
    {
        *gDebugStream << "RightFoot::UpdateMidpoint\ta\t" << a <<
        "\talpha\t" << alpha << "\td\t" << d << "\ttheta\t" << theta << "\n";
    }

    if (kRightAnkleExtensorMidPoint[0] < 999)
    {
        // Right Ankle Extensor
Util::Copy3x1(kRightAnkleExtensorMidPoint, location);
Util::MDHTransform(a, alpha, d, theta, location);
        gSimulation->GetCPG()->GetRightAnkleExtensor()->SetAnchorLocation(1, location);

        if (gDebug == SegmentsDebug)
        {
            *gDebugStream << "RightFoot::UpdateMidpoint" <<
            "\tExtensor location\t" << location[0] << "\t" <<
            location[1] << "\t" << location[2] << "\n";
        }
    }

    if (kRightAnkleFlexorMidPoint[0] < 999)
    {
        // Right Ankle Flexor
Util::Copy3x1(kRightAnkleFlexorMidPoint, location);
Util::MDHTransform(a, alpha, d, theta, location);
        gSimulation->GetCPG()->GetRightAnkleFlexor()->SetAnchorLocation(1, location);

        if (gDebug == SegmentsDebug)
        {
            *gDebugStream << "RightFoot::UpdateMidpoint" <<
            "\tFlexor location\t" << location[0] << "\t" <<
            location[1] << "\t" << location[2] << "\n";
        }
    }
}

void
RightLeg::draw() const
{
#ifdef USE_OPENGL
dmRevoluteLink::draw();
    DrawAxes();
    double mass;
    CartesianTensor inertia;
    CartesianVector cg_pos;
    getInertiaParameters(mass, inertia, cg_pos);
    DrawCOM(cg_pos[0], cg_pos[1], cg_pos[2]);

#endif
}

RightFoot::RightFoot():
dmRevoluteLink()
{
    // set the link name
    setName(kRightFootPartName);

    // load up a graphics model if required
#ifdef USE_OPENGL
    if (gSimulation->UseGraphics())
    {
        GLuint *dlist = new GLuint;
        *dlist = LoadObj(kRightFootGraphicFile, kRightFootGraphicScale, kRightFootGraphicScale, kRightFootGraphicScale);
        setUserData((void *) dlist);
    }
#endif

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
    if (kRightAnkleExtensorMidPoint[0] < 999) strapForceAnchor->SetStrapForce(this, kRightAnkleExtensorInsertion, strapForce, 2);
    else strapForceAnchor->SetStrapForce(this, kRightAnkleExtensorInsertion, strapForce, 1);
    addForce(strapForceAnchor);
    gSimulation->GetForceList()->AddForce(strapForceAnchor);

    // Right Ankle Flexor
    strapForce = gSimulation->GetCPG()->GetRightAnkleFlexor();

    strapForceAnchor = new StrapForceAnchor();
    strapForceAnchor->setName(strcat(kRightAnkleFlexorName, "Insertion"));
    if (kRightAnkleFlexorMidPoint[0] < 999) strapForceAnchor->SetStrapForce(this, kRightAnkleFlexorInsertion, strapForce, 2);
    else strapForceAnchor->SetStrapForce(this, kRightAnkleFlexorInsertion, strapForce, 1);
    addForce(strapForceAnchor);
    gSimulation->GetForceList()->AddForce(strapForceAnchor);

    // Contact Points
    ModifiedContactModel *contactModel = new ModifiedContactModel();
    contactModel->setContactPoints(kRightFootNumContactPoints, kRightFootContactPositions);
    contactModel->setName(kRightFootContactName);
    addForce(contactModel);
    gSimulation->GetForceList()->AddForce(contactModel);

    if (gDebug == SegmentsDebug)
    {
        *gDebugStream << "RightFoot::RightFoot()\tgetName()\t" << getName() << "\n";
    }

}

RightFoot::~RightFoot()
{
#ifdef USE_OPENGL
    delete (GLuint *)m_user_data;
#endif
}

void
RightFoot::draw() const
{
#ifdef USE_OPENGL
dmRevoluteLink::draw();
    DrawAxes();
    double mass;
    CartesianTensor inertia;
    CartesianVector cg_pos;
    getInertiaParameters(mass, inertia, cg_pos);
    DrawCOM(cg_pos[0], cg_pos[1], cg_pos[2]);

#endif
}


void ReadSegmentGlobals(DataFile &file)
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
    file.RetrieveParameter("kTorsoPartName", kTorsoPartName, 256);
    file.RetrieveParameter("kTorsoGraphicFile", kTorsoGraphicFile, 256);
    file.ReadNext(&kTorsoGraphicScale);
    file.RetrieveParameter("kTorsoMass", &kTorsoMass);
    file.RetrieveParameter("kTorsoMOI", 9, (double *)kTorsoMOI);
    file.RetrieveParameter("kTorsoCG", 3, kTorsoCG);
    file.RetrieveParameter("kTorsoPosition", 7, kTorsoPosition);
    file.RetrieveParameter("kTorsoVelocity", 6, kTorsoVelocity);

    // -------------------------------------------
    // Left Thigh
    file.RetrieveParameter("kLeftThighPartName", kLeftThighPartName, 256);
    file.RetrieveParameter("kLeftThighGraphicFile", kLeftThighGraphicFile, 256);
    file.ReadNext(&kLeftThighGraphicScale);
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
    file.RetrieveParameter("kRightThighPartName", kRightThighPartName, 256);
    file.RetrieveParameter("kRightThighGraphicFile", kRightThighGraphicFile, 256);
    file.ReadNext(&kRightThighGraphicScale);
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
    file.RetrieveParameter("kLeftLegPartName", kLeftLegPartName, 256);
    file.RetrieveParameter("kLeftLegGraphicFile", kLeftLegGraphicFile, 256);
    file.ReadNext(&kLeftLegGraphicScale);
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
    file.RetrieveParameter("kRightLegPartName", kRightLegPartName, 256);
    file.RetrieveParameter("kRightLegGraphicFile", kRightLegGraphicFile, 256);
    file.ReadNext(&kRightLegGraphicScale);
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
    file.RetrieveParameter("kLeftFootPartName", kLeftFootPartName, 256);
    file.RetrieveParameter("kLeftFootGraphicFile", kLeftFootGraphicFile, 256);
    file.ReadNext(&kLeftFootGraphicScale);
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
    file.RetrieveParameter("kRightFootPartName", kRightFootPartName, 256);
    file.RetrieveParameter("kRightFootGraphicFile", kRightFootGraphicFile, 256);
    file.ReadNext(&kRightFootGraphicScale);
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
    file.RetrieveParameter("kLeftHipExtensorName", kLeftHipExtensorName, 256);
    file.RetrieveParameter("kLeftHipExtensorOrigin", 3, kLeftHipExtensorOrigin);
    file.RetrieveParameter("kLeftHipExtensorInsertion", 3, kLeftHipExtensorInsertion);
    file.RetrieveParameter("kLeftHipExtensorPCA", &kLeftHipExtensorPCA);
    file.RetrieveParameter("kLeftHipExtensorLength", &kLeftHipExtensorLength);

    // Left Hip Flexor
    file.RetrieveParameter("kLeftHipFlexorName", kLeftHipFlexorName, 256);
    file.RetrieveParameter("kLeftHipFlexorOrigin", 3, kLeftHipFlexorOrigin);
    file.RetrieveParameter("kLeftHipFlexorInsertion", 3, kLeftHipFlexorInsertion);
    file.RetrieveParameter("kLeftHipFlexorPCA", &kLeftHipFlexorPCA);
    file.RetrieveParameter("kLeftHipFlexorLength", &kLeftHipFlexorLength);

    // Right Hip Extensor
    file.RetrieveParameter("kRightHipExtensorName", kRightHipExtensorName, 256);
    file.RetrieveParameter("kRightHipExtensorOrigin", 3, kRightHipExtensorOrigin);
    file.RetrieveParameter("kRightHipExtensorInsertion", 3, kRightHipExtensorInsertion);
    file.RetrieveParameter("kRightHipExtensorPCA", &kRightHipExtensorPCA);
    file.RetrieveParameter("kRightHipExtensorLength", &kRightHipExtensorLength);

    // Right Hip Flexor
    file.RetrieveParameter("kRightHipFlexorName", kRightHipFlexorName, 256);
    file.RetrieveParameter("kRightHipFlexorOrigin", 3, kRightHipFlexorOrigin);
    file.RetrieveParameter("kRightHipFlexorInsertion", 3, kRightHipFlexorInsertion);
    file.RetrieveParameter("kRightHipFlexorPCA", &kRightHipFlexorPCA);
    file.RetrieveParameter("kRightHipFlexorLength", &kRightHipFlexorLength);

    // Left Knee Extensor
    file.RetrieveParameter("kLeftKneeExtensorName", kLeftKneeExtensorName, 256);
    file.RetrieveParameter("kLeftKneeExtensorOrigin", 3, kLeftKneeExtensorOrigin);
    file.RetrieveParameter("kLeftKneeExtensorMidPoint", 3, kLeftKneeExtensorMidPoint);
    file.RetrieveParameter("kLeftKneeExtensorInsertion", 3, kLeftKneeExtensorInsertion);
    file.RetrieveParameter("kLeftKneeExtensorPCA", &kLeftKneeExtensorPCA);
    file.RetrieveParameter("kLeftKneeExtensorLength", &kLeftKneeExtensorLength);

    // Left Knee Flexor
    file.RetrieveParameter("kLeftKneeFlexorName", kLeftKneeFlexorName, 256);
    file.RetrieveParameter("kLeftKneeFlexorOrigin", 3, kLeftKneeFlexorOrigin);
    file.RetrieveParameter("kLeftKneeFlexorMidPoint", 3, kLeftKneeFlexorMidPoint);
    file.RetrieveParameter("kLeftKneeFlexorInsertion", 3, kLeftKneeFlexorInsertion);
    file.RetrieveParameter("kLeftKneeFlexorPCA", &kLeftKneeFlexorPCA);
    file.RetrieveParameter("kLeftKneeFlexorLength", &kLeftKneeFlexorLength);

    // Right Knee Extensor
    file.RetrieveParameter("kRightKneeExtensorName", kRightKneeExtensorName, 256);
    file.RetrieveParameter("kRightKneeExtensorOrigin", 3, kRightKneeExtensorOrigin);
    file.RetrieveParameter("kRightKneeExtensorMidPoint", 3, kRightKneeExtensorMidPoint);
    file.RetrieveParameter("kRightKneeExtensorInsertion", 3, kRightKneeExtensorInsertion);
    file.RetrieveParameter("kRightKneeExtensorPCA", &kRightKneeExtensorPCA);
    file.RetrieveParameter("kRightKneeExtensorLength", &kRightKneeExtensorLength);

    // Right Knee Flexor
    file.RetrieveParameter("kRightKneeFlexorName", kRightKneeFlexorName, 256);
    file.RetrieveParameter("kRightKneeFlexorOrigin", 3, kRightKneeFlexorOrigin);
    file.RetrieveParameter("kRightKneeFlexorMidPoint", 3, kRightKneeFlexorMidPoint);
    file.RetrieveParameter("kRightKneeFlexorInsertion", 3, kRightKneeFlexorInsertion);
    file.RetrieveParameter("kRightKneeFlexorPCA", &kRightKneeFlexorPCA);
    file.RetrieveParameter("kRightKneeFlexorLength", &kRightKneeFlexorLength);

    // Left Ankle Extensor
    file.RetrieveParameter("kLeftAnkleExtensorName", kLeftAnkleExtensorName, 256);
    file.RetrieveParameter("kLeftAnkleExtensorOrigin", 3, kLeftAnkleExtensorOrigin);
    file.RetrieveParameter("kLeftAnkleExtensorMidPoint", 3, kLeftAnkleExtensorMidPoint);
    file.RetrieveParameter("kLeftAnkleExtensorInsertion", 3, kLeftAnkleExtensorInsertion);
    file.RetrieveParameter("kLeftAnkleExtensorPCA", &kLeftAnkleExtensorPCA);
    file.RetrieveParameter("kLeftAnkleExtensorLength", &kLeftAnkleExtensorLength);

    // Left Ankle Flexor
    file.RetrieveParameter("kLeftAnkleFlexorName", kLeftAnkleFlexorName, 256);
    file.RetrieveParameter("kLeftAnkleFlexorOrigin", 3, kLeftAnkleFlexorOrigin);
    file.RetrieveParameter("kLeftAnkleFlexorMidPoint", 3, kLeftAnkleFlexorMidPoint);
    file.RetrieveParameter("kLeftAnkleFlexorInsertion", 3, kLeftAnkleFlexorInsertion);
    file.RetrieveParameter("kLeftAnkleFlexorPCA", &kLeftAnkleFlexorPCA);
    file.RetrieveParameter("kLeftAnkleFlexorLength", &kLeftAnkleFlexorLength);

    // Right Ankle Extensor
    file.RetrieveParameter("kRightAnkleExtensorName", kRightAnkleExtensorName, 256);
    file.RetrieveParameter("kRightAnkleExtensorOrigin", 3, kRightAnkleExtensorOrigin);
    file.RetrieveParameter("kRightAnkleExtensorMidPoint", 3, kRightAnkleExtensorMidPoint);
    file.RetrieveParameter("kRightAnkleExtensorInsertion", 3, kRightAnkleExtensorInsertion);
    file.RetrieveParameter("kRightAnkleExtensorPCA", &kRightAnkleExtensorPCA);
    file.RetrieveParameter("kRightAnkleExtensorLength", &kRightAnkleExtensorLength);

    // Right Ankle Flexor
    file.RetrieveParameter("kRightAnkleFlexorName", kRightAnkleFlexorName, 256);
    file.RetrieveParameter("kRightAnkleFlexorOrigin", 3, kRightAnkleFlexorOrigin);
    file.RetrieveParameter("kRightAnkleFlexorMidPoint", 3, kRightAnkleFlexorMidPoint);
    file.RetrieveParameter("kRightAnkleFlexorInsertion", 3, kRightAnkleFlexorInsertion);
    file.RetrieveParameter("kRightAnkleFlexorPCA", &kRightAnkleFlexorPCA);
    file.RetrieveParameter("kRightAnkleFlexorLength", &kRightAnkleFlexorLength);


    // extra parameters if present
    bool flag = file.GetExitOnError();
    file.SetExitOnError(false);
    if (file.RetrieveParameter("kLeftHipExtensorSpringConstant", &kLeftHipExtensorSpringConstant) == true)
        kLeftHipExtensorSpringConstant = 0;
    if (file.RetrieveParameter("kLeftHipFlexorSpringConstant", &kLeftHipFlexorSpringConstant) == true)
        kLeftHipFlexorSpringConstant = 0;
    if (file.RetrieveParameter("kLeftKneeExtensorSpringConstant", &kLeftKneeExtensorSpringConstant) == true)
        kLeftKneeExtensorSpringConstant = 0;
    if (file.RetrieveParameter("kLeftKneeFlexorSpringConstant", &kLeftKneeFlexorSpringConstant) == true)
        kLeftKneeFlexorSpringConstant = 0;
    if (file.RetrieveParameter("kLeftAnkleExtensorSpringConstant", &kLeftAnkleExtensorSpringConstant) == true)
        kLeftAnkleExtensorSpringConstant = 0;
    if (file.RetrieveParameter("kLeftAnkleFlexorSpringConstant", &kLeftAnkleFlexorSpringConstant) == true)
        kLeftAnkleFlexorSpringConstant = 0;
    if (file.RetrieveParameter("kRightHipExtensorSpringConstant", &kRightHipExtensorSpringConstant) == true)
        kRightHipExtensorSpringConstant = 0;
    if (file.RetrieveParameter("kRightHipFlexorSpringConstant", &kRightHipFlexorSpringConstant) == true)
        kRightHipFlexorSpringConstant = 0;
    if (file.RetrieveParameter("kRightKneeExtensorSpringConstant", &kRightKneeExtensorSpringConstant) == true)
        kRightKneeExtensorSpringConstant = 0;
    if (file.RetrieveParameter("kRightKneeFlexorSpringConstant", &kRightKneeFlexorSpringConstant) == true)
        kRightKneeFlexorSpringConstant = 0;
    if (file.RetrieveParameter("kRightAnkleExtensorSpringConstant", &kRightAnkleExtensorSpringConstant) == true)
        kRightAnkleExtensorSpringConstant = 0;
    if (file.RetrieveParameter("kRightAnkleFlexorSpringConstant", &kRightAnkleFlexorSpringConstant) == true)
        kRightAnkleFlexorSpringConstant = 0;
    file.SetExitOnError(flag);

    // -------------------------------------------
    // Contact Models
    // -------------------------------------------

    // Left Foot

    file.RetrieveParameter("kLeftFootContactName", kLeftFootContactName, 256);
    file.RetrieveParameter("kLeftFootNumContactPoints", &kLeftFootNumContactPoints);
    assert(kLeftFootNumContactPoints < 256); // hardwired size limit
    file.RetrieveParameter("kLeftFootContactPositions", kLeftFootNumContactPoints * 3, (double *)kLeftFootContactPositions);

    // Right Foot

    file.RetrieveParameter("kRightFootContactName", kRightFootContactName, 256);
    file.RetrieveParameter("kRightFootNumContactPoints", &kRightFootNumContactPoints);
    assert(kRightFootNumContactPoints < 256); // hardwired size limit
    file.RetrieveParameter("kRightFootContactPositions", kRightFootNumContactPoints * 3, (double *)kRightFootContactPositions);

    // -------------------------------------------
    // Kinematic Goal Parameters
    // -------------------------------------------

    if (gSimulation->GetFitnessType() == KinematicMatch)
    {
        double v;
        if (file.RetrieveParameter("kTargetTime", &v) == false) gTimeLimit = v;
        file.RetrieveParameter("kTorsoPositionTarget", 7, kTorsoPositionTarget);
        file.RetrieveParameter("kTorsoVelocityTarget", 6, kTorsoVelocityTarget);
        file.RetrieveParameter("kLeftThighMDHThetaTarget", &kLeftThighMDHThetaTarget);
        file.RetrieveParameter("kLeftThighMDHThetaVTarget", &kLeftThighMDHThetaVTarget);
        file.RetrieveParameter("kLeftLegMDHThetaTarget", &kLeftLegMDHThetaTarget);
        file.RetrieveParameter("kLeftLegMDHThetaVTarget", &kLeftLegMDHThetaVTarget);
        file.RetrieveParameter("kLeftFootMDHThetaTarget", &kLeftFootMDHThetaTarget);
        file.RetrieveParameter("kLeftFootMDHThetaVTarget", &kLeftFootMDHThetaVTarget);
        file.RetrieveParameter("kRightThighMDHThetaTarget", &kRightThighMDHThetaTarget);
        file.RetrieveParameter("kRightThighMDHThetaVTarget", &kRightThighMDHThetaVTarget);
        file.RetrieveParameter("kRightLegMDHThetaTarget", &kRightLegMDHThetaTarget);
        file.RetrieveParameter("kRightLegMDHThetaVTarget", &kRightLegMDHThetaVTarget);
        file.RetrieveParameter("kRightFootMDHThetaTarget", &kRightFootMDHThetaTarget);
        file.RetrieveParameter("kRightFootMDHThetaVTarget", &kRightFootMDHThetaVTarget);
    }

}

#ifdef USE_OPENGL
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

void
DrawCOM(double x, double y, double z)
{
    if (gAxisFlag)
    {
        glPushMatrix();
        glTranslatef(x, y, z);
        glutSolidSphere(gAxisSize / 2, 16, 16);
        glPopMatrix();
    }
}

#endif

