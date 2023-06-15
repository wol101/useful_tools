// Segments.cc - the various segment objects

#include <string.h>

#include "Segments.h"
#include "Simulation.h"
#include "ModifiedContactModel.h"
#include "StrapForceAnchor.h"
#include "DebugControl.h"
#include "Util.h"
#include "SideStabilizer.h"
#include "StrapForce.h"

#ifdef USE_OPENGL
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
#endif

// Skull Defined Here

Skull::Skull():
dmMobileBaseLink()
{
    // set the link name
    setName(kSkullPartName);

    // load up a graphics model if required
#ifdef USE_OPENGL
    if (gSimulation->UseGraphics())
    {
        GLuint *dlist = new GLuint;
        *dlist = LoadObj(kSkullGraphicFile, kSkullGraphicScale, kSkullGraphicScale, kSkullGraphicScale);
        setUserData((void *) dlist);
    }
#endif

    setInertiaParameters(kSkullMass, kSkullMOI, kSkullCG);
    setState(kSkullPosition, kSkullVelocity);

    // Contact Points
    ModifiedContactModel *contactModel = new ModifiedContactModel();
    CartesianVector contactPositions[4] = {{100,100,0},{100,-100,0},{-100,100,0},{-100,-100,0}};
    contactModel->setContactPoints(4, contactPositions);
    contactModel->setName("Skull supports");
    addForce(contactModel);
    gSimulation->GetForceList()->AddForce(contactModel);
    
    // add the muscle forces

    StrapForceAnchor *strapForceAnchor;
    StrapForce *strapForce;

    // LeftTemporalis

    strapForce = gSimulation->GetCPG()->GetLeftTemporalis();
    strapForce->SetNumAnchors(2);
    strapForce->setName(kLeftTemporalisName);

    strapForceAnchor = new StrapForceAnchor();
    strapForceAnchor->setName(strcat(kLeftTemporalisName, "Origin"));
    strapForceAnchor->SetStrapForce(this, kLeftTemporalisOrigin, strapForce, 0);
    addForce(strapForceAnchor);
    gSimulation->GetForceList()->AddForce(strapForceAnchor);

    // LeftMasseter

    strapForce = gSimulation->GetCPG()->GetLeftMasseter();
    strapForce->SetNumAnchors(2);
    strapForce->setName(kLeftMasseterName);

    strapForceAnchor = new StrapForceAnchor();
    strapForceAnchor->setName(strcat(kLeftMasseterName, "Origin"));
    strapForceAnchor->SetStrapForce(this, kLeftMasseterOrigin, strapForce, 0);
    addForce(strapForceAnchor);
    gSimulation->GetForceList()->AddForce(strapForceAnchor);

    // LeftMedialPterygoid

    strapForce = gSimulation->GetCPG()->GetLeftMedialPterygoid();
    strapForce->SetNumAnchors(2);
    strapForce->setName(kLeftMedialPterygoidName);

    strapForceAnchor = new StrapForceAnchor();
    strapForceAnchor->setName(strcat(kLeftMedialPterygoidName, "Origin"));
    strapForceAnchor->SetStrapForce(this, kLeftMedialPterygoidOrigin, strapForce, 0);
    addForce(strapForceAnchor);
    gSimulation->GetForceList()->AddForce(strapForceAnchor);

    // LeftLateralPterygoid

    strapForce = gSimulation->GetCPG()->GetLeftLateralPterygoid();
    strapForce->SetNumAnchors(2);
    strapForce->setName(kLeftLateralPterygoidName);

    strapForceAnchor = new StrapForceAnchor();
    strapForceAnchor->setName(strcat(kLeftLateralPterygoidName, "Origin"));
    strapForceAnchor->SetStrapForce(this, kLeftLateralPterygoidOrigin, strapForce, 0);
    addForce(strapForceAnchor);
    gSimulation->GetForceList()->AddForce(strapForceAnchor);

    // RightTemporalis

    strapForce = gSimulation->GetCPG()->GetRightTemporalis();
    strapForce->SetNumAnchors(2);
    strapForce->setName(kRightTemporalisName);

    strapForceAnchor = new StrapForceAnchor();
    strapForceAnchor->setName(strcat(kRightTemporalisName, "Origin"));
    strapForceAnchor->SetStrapForce(this, kRightTemporalisOrigin, strapForce, 0);
    addForce(strapForceAnchor);
    gSimulation->GetForceList()->AddForce(strapForceAnchor);

    // RightMasseter

    strapForce = gSimulation->GetCPG()->GetRightMasseter();
    strapForce->SetNumAnchors(2);
    strapForce->setName(kRightMasseterName);

    strapForceAnchor = new StrapForceAnchor();
    strapForceAnchor->setName(strcat(kRightMasseterName, "Origin"));
    strapForceAnchor->SetStrapForce(this, kRightMasseterOrigin, strapForce, 0);
    addForce(strapForceAnchor);
    gSimulation->GetForceList()->AddForce(strapForceAnchor);

    // RightMedialPterygoid

    strapForce = gSimulation->GetCPG()->GetRightMedialPterygoid();
    strapForce->SetNumAnchors(2);
    strapForce->setName(kRightMedialPterygoidName);

    strapForceAnchor = new StrapForceAnchor();
    strapForceAnchor->setName(strcat(kRightMedialPterygoidName, "Origin"));
    strapForceAnchor->SetStrapForce(this, kRightMedialPterygoidOrigin, strapForce, 0);
    addForce(strapForceAnchor);
    gSimulation->GetForceList()->AddForce(strapForceAnchor);

    // RightLateralPterygoid

    strapForce = gSimulation->GetCPG()->GetRightLateralPterygoid();
    strapForce->SetNumAnchors(2);
    strapForce->setName(kRightLateralPterygoidName);

    strapForceAnchor = new StrapForceAnchor();
    strapForceAnchor->setName(strcat(kRightLateralPterygoidName, "Origin"));
    strapForceAnchor->SetStrapForce(this, kRightLateralPterygoidOrigin, strapForce, 0);
    addForce(strapForceAnchor);
    gSimulation->GetForceList()->AddForce(strapForceAnchor);

    // Springs

    DampedSpringStrap *spring;


    // Tooth
    spring = gSimulation->GetTooth();
    spring->SetNumAnchors(2);
    spring->setName(kToothName);
    spring->SetUnloadedLength(kToothLength);
    spring->SetDamping(kToothDamping);
    spring->SetSpringConstant(kToothSpringConstant);

    strapForceAnchor = new StrapForceAnchor();
    strapForceAnchor->setName(strcat(kToothName, "Origin"));
    strapForceAnchor->SetStrapForce(this, kToothOrigin, spring, 0);
    addForce(strapForceAnchor);
    gSimulation->GetForceList()->AddForce(strapForceAnchor);

    // LeftTMJ
    spring = gSimulation->GetLeftTMJ();
    spring->SetNumAnchors(2);
    spring->setName(kLeftTMJName);
    spring->SetUnloadedLength(kLeftTMJLength);
    spring->SetDamping(kLeftTMJDamping);
    spring->SetSpringConstant(kLeftTMJSpringConstant);

    strapForceAnchor = new StrapForceAnchor();
    strapForceAnchor->setName(strcat(kLeftTMJName, "Origin"));
    strapForceAnchor->SetStrapForce(this, kLeftTMJOrigin, spring, 0);
    addForce(strapForceAnchor);
    gSimulation->GetForceList()->AddForce(strapForceAnchor);

    // RightTMJ
    spring = gSimulation->GetRightTMJ();
    spring->SetNumAnchors(2);
    spring->setName(kRightTMJName);
    spring->SetUnloadedLength(kRightTMJLength);
    spring->SetDamping(kRightTMJDamping);
    spring->SetSpringConstant(kRightTMJSpringConstant);

    strapForceAnchor = new StrapForceAnchor();
    strapForceAnchor->setName(strcat(kRightTMJName, "Origin"));
    strapForceAnchor->SetStrapForce(this, kRightTMJOrigin, spring, 0);
    addForce(strapForceAnchor);
    gSimulation->GetForceList()->AddForce(strapForceAnchor);

    if (gDebug == SegmentsDebug)
    {
        *gDebugStream << "Skull::Skull()\tgetName()\t" << getName() << "\n";
    }

}

Skull::~Skull()
{
#ifdef USE_OPENGL
    delete (GLuint *)m_user_data;
#endif
}

void
Skull::draw() const
{
#ifdef USE_OPENGL
dmMobileBaseLink::draw();
    DrawAxes();
#endif
}

// Mandible defined here

Mandible::Mandible():
dmMobileBaseLink()
{
    // set the link name
    setName(kMandiblePartName);

    // load up a graphics model if required
#ifdef USE_OPENGL
    if (gSimulation->UseGraphics())
    {
        GLuint *dlist = new GLuint;
        *dlist = LoadObj(kMandibleGraphicFile, kMandibleGraphicScale, kMandibleGraphicScale, kMandibleGraphicScale);
        setUserData((void *) dlist);
    }
#endif

    setInertiaParameters(kMandibleMass, kMandibleMOI, kMandibleCG);
    setState(kMandiblePosition, kMandibleVelocity);

    // add the muscle forces

    StrapForceAnchor *strapForceAnchor;
    StrapForce *strapForce;

    // LeftTemporalis

    strapForce = gSimulation->GetCPG()->GetLeftTemporalis();

    strapForceAnchor = new StrapForceAnchor();
    strapForceAnchor->setName(strcat(kLeftTemporalisName, "Insertion"));
    strapForceAnchor->SetStrapForce(this, kLeftTemporalisInsertion, strapForce, 1);
    addForce(strapForceAnchor);
    gSimulation->GetForceList()->AddForce(strapForceAnchor);

    // LeftMasseter

    strapForce = gSimulation->GetCPG()->GetLeftMasseter();

    strapForceAnchor = new StrapForceAnchor();
    strapForceAnchor->setName(strcat(kLeftMasseterName, "Insertion"));
    strapForceAnchor->SetStrapForce(this, kLeftMasseterInsertion, strapForce, 1);
    addForce(strapForceAnchor);
    gSimulation->GetForceList()->AddForce(strapForceAnchor);

    // LeftMedialPterygoid

    strapForce = gSimulation->GetCPG()->GetLeftMedialPterygoid();

    strapForceAnchor = new StrapForceAnchor();
    strapForceAnchor->setName(strcat(kLeftMedialPterygoidName, "Insertion"));
    strapForceAnchor->SetStrapForce(this, kLeftMedialPterygoidInsertion, strapForce, 1);
    addForce(strapForceAnchor);
    gSimulation->GetForceList()->AddForce(strapForceAnchor);

    // LeftLateralPterygoid

    strapForce = gSimulation->GetCPG()->GetLeftLateralPterygoid();

    strapForceAnchor = new StrapForceAnchor();
    strapForceAnchor->setName(strcat(kLeftLateralPterygoidName, "Insertion"));
    strapForceAnchor->SetStrapForce(this, kLeftLateralPterygoidInsertion, strapForce, 1);
    addForce(strapForceAnchor);
    gSimulation->GetForceList()->AddForce(strapForceAnchor);

    // RightTemporalis

    strapForce = gSimulation->GetCPG()->GetRightTemporalis();

    strapForceAnchor = new StrapForceAnchor();
    strapForceAnchor->setName(strcat(kRightTemporalisName, "Insertion"));
    strapForceAnchor->SetStrapForce(this, kRightTemporalisInsertion, strapForce, 1);
    addForce(strapForceAnchor);
    gSimulation->GetForceList()->AddForce(strapForceAnchor);

    // RightMasseter

    strapForce = gSimulation->GetCPG()->GetRightMasseter();

    strapForceAnchor = new StrapForceAnchor();
    strapForceAnchor->setName(strcat(kRightMasseterName, "Insertion"));
    strapForceAnchor->SetStrapForce(this, kRightMasseterInsertion, strapForce, 1);
    addForce(strapForceAnchor);
    gSimulation->GetForceList()->AddForce(strapForceAnchor);

    // RightMedialPterygoid

    strapForce = gSimulation->GetCPG()->GetRightMedialPterygoid();

    strapForceAnchor = new StrapForceAnchor();
    strapForceAnchor->setName(strcat(kRightMedialPterygoidName, "Insertion"));
    strapForceAnchor->SetStrapForce(this, kRightMedialPterygoidInsertion, strapForce, 1);
    addForce(strapForceAnchor);
    gSimulation->GetForceList()->AddForce(strapForceAnchor);

    // RightLateralPterygoid

    strapForce = gSimulation->GetCPG()->GetRightLateralPterygoid();

    strapForceAnchor = new StrapForceAnchor();
    strapForceAnchor->setName(strcat(kRightLateralPterygoidName, "Insertion"));
    strapForceAnchor->SetStrapForce(this, kRightLateralPterygoidInsertion, strapForce, 1);
    addForce(strapForceAnchor);
    gSimulation->GetForceList()->AddForce(strapForceAnchor);

    // Springs

    DampedSpringStrap *spring;

    // Tooth
    spring = gSimulation->GetTooth();
    strapForceAnchor = new StrapForceAnchor();
    strapForceAnchor->setName(strcat(kToothName, "Insertion"));
    strapForceAnchor->SetStrapForce(this, kToothInsertion, spring, 1);
    addForce(strapForceAnchor);
    gSimulation->GetForceList()->AddForce(strapForceAnchor);

    // LeftTMJ
    spring = gSimulation->GetLeftTMJ();
    strapForceAnchor = new StrapForceAnchor();
    strapForceAnchor->setName(strcat(kLeftTMJName, "Insertion"));
    strapForceAnchor->SetStrapForce(this, kLeftTMJInsertion, spring, 1);
    addForce(strapForceAnchor);
    gSimulation->GetForceList()->AddForce(strapForceAnchor);

    // RightTMJ
    spring = gSimulation->GetRightTMJ();
    strapForceAnchor = new StrapForceAnchor();
    strapForceAnchor->setName(strcat(kRightTMJName, "Insertion"));
    strapForceAnchor->SetStrapForce(this, kRightTMJInsertion, spring, 1);
    addForce(strapForceAnchor);
    gSimulation->GetForceList()->AddForce(strapForceAnchor);

    
    if (gDebug == SegmentsDebug)
    {
        *gDebugStream << "Mandible::Mandible()\tgetName()\t" << getName() << "\n";
    }

}

Mandible::~Mandible()
{
#ifdef USE_OPENGL
    delete (GLuint *)m_user_data;
#endif
}

void
Mandible::draw() const
{
#ifdef USE_OPENGL
    dmMobileBaseLink::draw();
    DrawAxes();
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

    // -------------------------------------------
    // Links
    // -------------------------------------------

    // -------------------------------------------
    // Mandible
    file.RetrieveParameter("kMandiblePartName", kMandiblePartName, 256);
    file.RetrieveParameter("kMandibleGraphicFile", kMandibleGraphicFile, 256);
    file.ReadNext(&kMandibleGraphicScale);
    file.RetrieveParameter("kMandibleMass", &kMandibleMass);
    file.RetrieveParameter("kMandibleMOI", 9, (double *)kMandibleMOI);
    file.RetrieveParameter("kMandibleCG", 3, kMandibleCG);
    file.RetrieveParameter("kMandiblePosition", 7, kMandiblePosition);
    file.RetrieveParameter("kMandibleVelocity", 6, kMandibleVelocity);

    // -------------------------------------------
    // Skull
    file.RetrieveParameter("kSkullPartName", kSkullPartName, 256);
    file.RetrieveParameter("kSkullGraphicFile", kSkullGraphicFile, 256);
    file.ReadNext(&kSkullGraphicScale);
    file.RetrieveParameter("kSkullMass", &kSkullMass);
    file.RetrieveParameter("kSkullMOI", 9, (double *)kSkullMOI);
    file.RetrieveParameter("kSkullCG", 3, kSkullCG);
    file.RetrieveParameter("kSkullPosition", 7, kSkullPosition);
    file.RetrieveParameter("kSkullVelocity", 6, kSkullVelocity);
    
    // -------------------------------------------
    // Muscles
    // -------------------------------------------

    // LeftTemporalis
    file.RetrieveParameter("kLeftTemporalisName", kLeftTemporalisName, 256);
    file.RetrieveParameter("kLeftTemporalisOrigin", 3, kLeftTemporalisOrigin);
    file.RetrieveParameter("kLeftTemporalisInsertion", 3, kLeftTemporalisInsertion);

    // LeftMasseter
    file.RetrieveParameter("kLeftMasseterName", kLeftMasseterName, 256);
    file.RetrieveParameter("kLeftMasseterOrigin", 3, kLeftMasseterOrigin);
    file.RetrieveParameter("kLeftMasseterInsertion", 3, kLeftMasseterInsertion);

    // LeftMedialPterygoid
    file.RetrieveParameter("kLeftMedialPterygoidName", kLeftMedialPterygoidName, 256);
    file.RetrieveParameter("kLeftMedialPterygoidOrigin", 3, kLeftMedialPterygoidOrigin);
    file.RetrieveParameter("kLeftMedialPterygoidInsertion", 3, kLeftMedialPterygoidInsertion);

    // LeftLateralPterygoid
    file.RetrieveParameter("kLeftLateralPterygoidName", kLeftLateralPterygoidName, 256);
    file.RetrieveParameter("kLeftLateralPterygoidOrigin", 3, kLeftLateralPterygoidOrigin);
    file.RetrieveParameter("kLeftLateralPterygoidInsertion", 3, kLeftLateralPterygoidInsertion);

    // RightTemporalis
    file.RetrieveParameter("kRightTemporalisName", kRightTemporalisName, 256);
    file.RetrieveParameter("kRightTemporalisOrigin", 3, kRightTemporalisOrigin);
    file.RetrieveParameter("kRightTemporalisInsertion", 3, kRightTemporalisInsertion);

    // RightMasseter
    file.RetrieveParameter("kRightMasseterName", kRightMasseterName, 256);
    file.RetrieveParameter("kRightMasseterOrigin", 3, kRightMasseterOrigin);
    file.RetrieveParameter("kRightMasseterInsertion", 3, kRightMasseterInsertion);

    // RightMedialPterygoid
    file.RetrieveParameter("kRightMedialPterygoidName", kRightMedialPterygoidName, 256);
    file.RetrieveParameter("kRightMedialPterygoidOrigin", 3, kRightMedialPterygoidOrigin);
    file.RetrieveParameter("kRightMedialPterygoidInsertion", 3, kRightMedialPterygoidInsertion);

    // RightLateralPterygoid
    file.RetrieveParameter("kRightLateralPterygoidName", kRightLateralPterygoidName, 256);
    file.RetrieveParameter("kRightLateralPterygoidOrigin", 3, kRightLateralPterygoidOrigin);
    file.RetrieveParameter("kRightLateralPterygoidInsertion", 3, kRightLateralPterygoidInsertion);
    
    // -------------------------------------------
    // Springs
    // -------------------------------------------

    // Tooth
    file.RetrieveParameter("kToothName", kToothName, 256);
    file.RetrieveParameter("kToothOrigin", 3, kToothOrigin);
    file.RetrieveParameter("kToothInsertion", 3, kToothInsertion);
    file.RetrieveParameter("kToothLength", &kToothLength);
    file.RetrieveParameter("kToothDamping", &kToothDamping);
    file.RetrieveParameter("kToothSpringConstant", &kToothSpringConstant);

    // LeftTMJ
    file.RetrieveParameter("kLeftTMJName", kLeftTMJName, 256);
    file.RetrieveParameter("kLeftTMJOrigin", 3, kLeftTMJOrigin);
    file.RetrieveParameter("kLeftTMJInsertion", 3, kLeftTMJInsertion);
    file.RetrieveParameter("kLeftTMJLength", &kLeftTMJLength);
    file.RetrieveParameter("kLeftTMJDamping", &kLeftTMJDamping);
    file.RetrieveParameter("kLeftTMJSpringConstant", &kLeftTMJSpringConstant);

    // RightTMJ
    file.RetrieveParameter("kRightTMJName", kRightTMJName, 256);
    file.RetrieveParameter("kRightTMJOrigin", 3, kRightTMJOrigin);
    file.RetrieveParameter("kRightTMJInsertion", 3, kRightTMJInsertion);
    file.RetrieveParameter("kRightTMJLength", &kRightTMJLength);
    file.RetrieveParameter("kRightTMJDamping", &kRightTMJDamping);
    file.RetrieveParameter("kRightTMJSpringConstant", &kRightTMJSpringConstant);

    // -------------------------------------------
    // Kinematic Goal Parameters
    // -------------------------------------------

    if (gSimulation->GetFitnessType() == KinematicMatch)
    {
        double v;
        if (file.RetrieveParameter("kTargetTime", &v) == false) gTimeLimit = v;
        file.RetrieveParameter("kMandiblePositionTarget", 7, kMandiblePositionTarget);
        file.RetrieveParameter("kMandibleVelocityTarget", 6, kMandibleVelocityTarget);
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
#endif

