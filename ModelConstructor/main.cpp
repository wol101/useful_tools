/*
 *  main.cpp
 *  ModelConstructor
 *
 *  Created by Bill Sellers on 11/02/2009.
 *  Copyright 2007 Bill Sellers. All rights reserved.
 *
 */

#include <stdio.h>
#include <stdarg.h>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <typeinfo>
#include <cstring>

#include <libxml/xmlmemory.h>
#include <libxml/parser.h>

#include "DataFile.h"
#include "PGDMath.h"

struct Muscle
{
    pgd::Vector origin;
    pgd::Vector insertion;
    std::string originName;
    std::string insertionName;
    std::vector<pgd::Vector> viaPoints;
    std::vector<std::string> viaNames;
    std::string cylinderName;
    pgd::Vector cylinderPosition;
    pgd::Vector cylinderAxis;
    double cylinderRadius;
    std::string cylinder1Name;
    pgd::Vector cylinder1Position;
    pgd::Vector cylinder1Axis;
    double cylinder1Radius;
    std::string cylinder2Name;
    pgd::Vector cylinder2Position;
    pgd::Vector cylinder2Axis;
    double cylinder2Radius;
    
/* surprisingly I don't seem to need the deeep copy defined here */
/* const Muscle &operator=(const Muscle &m)
    {
        origin = m.origin;
        insertion = m.insertion;
        originName = m.originName;
        insertionName = m.insertionName;
        cylinderName = m.cylinderName;
        cylinderPosition = m.cylinderPosition;
        cylinderPosition = m.cylinderPosition;
        cylinderRadius = m.cylinderRadius;
        unsigned int i;
        for (i = 0; i < m.viaPoints.size(); i++) 
        {
            viaPoints.push_back(m.viaPoints[i]);
            std::cerr << "Copy " << m.viaPoints[i].x << "\n";
        }
        for (i = 0; i < m.viaNames.size(); i++) 
        {
            viaNames.push_back(m.viaNames[i]);
            std::cerr << "Copy " << m.viaNames[i] << "\n";
        }
        return m;
    }*/
};

struct Joint
{
    pgd::Vector anchor;
    pgd::Vector axis;
    std::string body1;
    std::string body2;
};

struct Body
{
    std::string graphicFile;
    double mass;
    pgd::Vector moi;
    pgd::Vector poi;
    pgd::Vector position;
};

struct Geom
{
    pgd::Vector position;
    std::string body;
    double radius;
};

std::map<std::string, Muscle> muscleList;
std::map<std::string, Muscle> springList;
std::map<std::string, Joint> jointList;
std::map<std::string, Body> bodyList;
std::map<std::string, Geom> geomList;
std::vector<std::string> bodyNames;

std::string distanceTravelledBodyID;

static int ParseMuscles(const char *muscleFileName);
static int ParseSprings(const char *springFileName);
static int ParseJoints(const char *jointFileName);
static int ParseBodies(const char *bodyFileName);
static int ParseGeoms(const char *geomFileName);
static int WriteConfigFile(const char *outputFileName);

int main (int argc, char * const argv[]) 
{
    const char *jointFileName = "Joints.txt";
    const char *bodyFileName = "Bodies.txt";
    const char *muscleFileName = "Muscles.txt";
    const char *geomFileName = "Geoms.txt";
    const char *springFileName = "Springs.txt";
    const char *outputFileName = "Output.xml";
    int i;
    int err;
    
    // parse the command line
    try
    {
        for (i = 1; i < argc; i++)
        {
            // do something with arguments
            
            if (strcmp(argv[i], "--joints") == 0)
            {
                i++;
                if (i >= argc) throw(1);
                jointFileName = argv[i];
            }
            
            else if (strcmp(argv[i], "--bodies") == 0)
            {
                i++;
                if (i >= argc) throw(1);
                bodyFileName = argv[i];
            }
            
            else if (strcmp(argv[i], "--muscles") == 0)
            {
                i++;
                if (i >= argc) throw(1);
                muscleFileName = argv[i];
            }

            else if (strcmp(argv[i], "--geoms") == 0)
            {
                i++;
                if (i >= argc) throw(1);
                geomFileName = argv[i];
            }
            
            else if (strcmp(argv[i], "--springs") == 0)
            {
                i++;
                if (i >= argc) throw(1);
                springFileName = argv[i];
            }
            
            else if (strcmp(argv[i], "--output") == 0)
            {
                i++;
                if (i >= argc) throw(1);
                outputFileName = argv[i];
            }
            
            else
            {
                throw(1);
            }
        }
        if (jointFileName == 0 || bodyFileName == 0 || muscleFileName == 0 || geomFileName == 0 || outputFileName == 0) throw(2);
    }
    
    // catch argument errors
    catch (int e)
    {
        std::cerr << "\nModelConstructor program\n";
        std::cerr << "Build " << __DATE__ << " " << __TIME__ << "\n";
        std::cerr << "Supported options:\n\n";
        std::cerr << "--joints jointFileName\n";
        std::cerr << "--bodies bodyFileName\n";
        std::cerr << "--muscles muscleFileName\n";
        std::cerr << "--geoms geomFileName\n";
        std::cerr << "--springs springFileName\n";
        std::cerr << "--output outputFileName\n";
        return __LINE__;
    }
    
    err = ParseMuscles(muscleFileName);
    std::cerr << "ParseMuscles err = " << err << "\n";
    err = ParseJoints(jointFileName);
    std::cerr << "ParseJoints err = " << err << "\n";
    err = ParseBodies(bodyFileName);
    std::cerr << "ParseBodies err = " << err << "\n";
    err = ParseGeoms(geomFileName);
    std::cerr << "ParseGeoms err = " << err << "\n";
    err = ParseSprings(springFileName);
    std::cerr << "ParseSprings err = " << err << "\n";
    
    err = WriteConfigFile(outputFileName);
    std::cerr << "WriteConfigFile err = " << err << "\n";
    
    return 0;
}

int ParseMuscles(const char *muscleFileName)
{
    char buffer[256];
    std::string buf;
    int n;
    Muscle muscle;
    std::string name;
    pgd::Vector viaPoint;
    std::string viaName;
    
    DataFile file;
    if (file.ReadFile(muscleFileName)) return __LINE__;
    
    while (file.ReadNext(buffer, 256) == false)
    {
        buf = buffer;
        n = buf.rfind("Origin");
        if (n != std::string::npos)
        {
            std::cerr << buf << "\n";
            name.assign(buf, 0, n);
            if (file.ReadNext(buffer, 256)) return __LINE__;
            muscle.origin.x = strtod(buffer, 0);
            if (file.ReadNext(buffer, 256)) return __LINE__;
            muscle.origin.y = strtod(buffer, 0);
            if (file.ReadNext(buffer, 256)) return __LINE__;
            muscle.origin.z = strtod(buffer, 0);
            if (file.ReadNext(buffer, 256)) return __LINE__;
            muscle.originName = buffer;
            continue;
        }
        
        n = buf.rfind("ViaPoint");
        if (n != std::string::npos)
        {
            std::cerr << buf << "\n";
           if (file.ReadNext(buffer, 256)) return __LINE__;
            viaPoint.x = strtod(buffer, 0);
            if (file.ReadNext(buffer, 256)) return __LINE__;
            viaPoint.y = strtod(buffer, 0);
            if (file.ReadNext(buffer, 256)) return __LINE__;
            viaPoint.z = strtod(buffer, 0);
            if (file.ReadNext(buffer, 256)) return __LINE__;
            viaName = buffer;
            muscle.viaPoints.push_back(viaPoint);
            muscle.viaNames.push_back(viaName);
            continue;
        }
        
        n = buf.rfind("Cylinder1");
        if (n != std::string::npos)
        {
            std::cerr << buf << "\n";
            if (file.ReadNext(buffer, 256)) return __LINE__;
            muscle.cylinder1Position.x = strtod(buffer, 0);
            if (file.ReadNext(buffer, 256)) return __LINE__;
            muscle.cylinder1Position.y = strtod(buffer, 0);
            if (file.ReadNext(buffer, 256)) return __LINE__;
            muscle.cylinder1Position.z = strtod(buffer, 0);
            if (file.ReadNext(buffer, 256)) return __LINE__;
            muscle.cylinder1Name = buffer;
            if (file.ReadNext(buffer, 256)) return __LINE__;
            muscle.cylinder1Radius = strtod(buffer, 0);
            
            muscle.cylinder1Axis.x = 0;
            muscle.cylinder1Axis.y = 1;
            muscle.cylinder1Axis.z = 0;
            char *index = file.GetIndex();
            if (file.ReadNext(buffer, 256) == 0)
            {
                if (strchr("0123456789+-.", buffer[0]) != 0)
                {
                    muscle.cylinder1Axis.x = strtod(buffer, 0);
                    if (file.ReadNext(buffer, 256)) return __LINE__;
                    muscle.cylinder1Axis.y = strtod(buffer, 0);
                    if (file.ReadNext(buffer, 256)) return __LINE__;
                    muscle.cylinder1Axis.z = strtod(buffer, 0);
                }
                else
                {
                    file.SetIndex(index);
                }
            }
            else
            {
                file.SetIndex(index);
            }            
            
            
            
            continue;
        }
        
        n = buf.rfind("Cylinder2");
        if (n != std::string::npos)
        {
            std::cerr << buf << "\n";
            if (file.ReadNext(buffer, 256)) return __LINE__;
            muscle.cylinder2Position.x = strtod(buffer, 0);
            if (file.ReadNext(buffer, 256)) return __LINE__;
            muscle.cylinder2Position.y = strtod(buffer, 0);
            if (file.ReadNext(buffer, 256)) return __LINE__;
            muscle.cylinder2Position.z = strtod(buffer, 0);
            if (file.ReadNext(buffer, 256)) return __LINE__;
            muscle.cylinder2Name = buffer;
            if (file.ReadNext(buffer, 256)) return __LINE__;
            muscle.cylinder2Radius = strtod(buffer, 0);
            
            muscle.cylinder2Axis.x = 0;
            muscle.cylinder2Axis.y = 1;
            muscle.cylinder2Axis.z = 0;
            char *index = file.GetIndex();
            if (file.ReadNext(buffer, 256) == 0)
            {
                if (strchr("0123456789+-.", buffer[0]) != 0)
                {
                    muscle.cylinder2Axis.x = strtod(buffer, 0);
                    if (file.ReadNext(buffer, 256)) return __LINE__;
                    muscle.cylinder2Axis.y = strtod(buffer, 0);
                    if (file.ReadNext(buffer, 256)) return __LINE__;
                    muscle.cylinder2Axis.z = strtod(buffer, 0);
                }
                else
                {
                    file.SetIndex(index);
                }
            }
            else
            {
                file.SetIndex(index);
            }            
            
            
            
            continue;
        }
        
        n = buf.rfind("Cylinder");
        if (n != std::string::npos)
        {
            std::cerr << buf << "\n";
            if (file.ReadNext(buffer, 256)) return __LINE__;
            muscle.cylinderPosition.x = strtod(buffer, 0);
            if (file.ReadNext(buffer, 256)) return __LINE__;
            muscle.cylinderPosition.y = strtod(buffer, 0);
            if (file.ReadNext(buffer, 256)) return __LINE__;
            muscle.cylinderPosition.z = strtod(buffer, 0);
            if (file.ReadNext(buffer, 256)) return __LINE__;
            muscle.cylinderName = buffer;
            if (file.ReadNext(buffer, 256)) return __LINE__;
            muscle.cylinderRadius = strtod(buffer, 0);
            
            muscle.cylinderAxis.x = 0;
            muscle.cylinderAxis.y = 1;
            muscle.cylinderAxis.z = 0;
            char *index = file.GetIndex();
            if (file.ReadNext(buffer, 256) == 0)
            {
                if (strchr("0123456789+-.", buffer[0]) != 0)
                {
                    muscle.cylinderAxis.x = strtod(buffer, 0);
                    if (file.ReadNext(buffer, 256)) return __LINE__;
                    muscle.cylinderAxis.y = strtod(buffer, 0);
                    if (file.ReadNext(buffer, 256)) return __LINE__;
                    muscle.cylinderAxis.z = strtod(buffer, 0);
                }
                else
                {
                    file.SetIndex(index);
                }
            }
            else
            {
                file.SetIndex(index);
            }            
            
            
            
            continue;
        }
        
        n = buf.rfind("Insertion");
        if (n != std::string::npos)
        {
            std::cerr << buf << "\n";
            if (file.ReadNext(buffer, 256)) return __LINE__;
            muscle.insertion.x = strtod(buffer, 0);
            if (file.ReadNext(buffer, 256)) return __LINE__;
            muscle.insertion.y = strtod(buffer, 0);
            if (file.ReadNext(buffer, 256)) return __LINE__;
            muscle.insertion.z = strtod(buffer, 0);
            if (file.ReadNext(buffer, 256)) return __LINE__;
            muscle.insertionName = buffer;
            
            muscleList[name] = muscle;
            muscle.viaPoints.clear();
            muscle.viaNames.clear();
            muscle.originName = "";
            muscle.insertionName = "";
            muscle.cylinderName = "";
            muscle.cylinder1Name = "";
            muscle.cylinder2Name = "";
            std::cerr << name << " completed\n";
            continue;
        }
        
    }
    
    return 0;
}

int ParseSprings(const char *springFileName)
{
    char buffer[256];
    std::string buf;
    int n;
    Muscle muscle;
    std::string name;
    pgd::Vector viaPoint;
    std::string viaName;
    
    DataFile file;
    if (file.ReadFile(springFileName)) return __LINE__;
    
    while (file.ReadNext(buffer, 256) == false)
    {
        buf = buffer;
        n = buf.rfind("Origin");
        if (n != std::string::npos)
        {
            std::cerr << buf << "\n";
            name.assign(buf, 0, n);
            if (file.ReadNext(buffer, 256)) return __LINE__;
            muscle.origin.x = strtod(buffer, 0);
            if (file.ReadNext(buffer, 256)) return __LINE__;
            muscle.origin.y = strtod(buffer, 0);
            if (file.ReadNext(buffer, 256)) return __LINE__;
            muscle.origin.z = strtod(buffer, 0);
            if (file.ReadNext(buffer, 256)) return __LINE__;
            muscle.originName = buffer;
            continue;
        }
        
        n = buf.rfind("ViaPoint");
        if (n != std::string::npos)
        {
            std::cerr << buf << "\n";
            if (file.ReadNext(buffer, 256)) return __LINE__;
            viaPoint.x = strtod(buffer, 0);
            if (file.ReadNext(buffer, 256)) return __LINE__;
            viaPoint.y = strtod(buffer, 0);
            if (file.ReadNext(buffer, 256)) return __LINE__;
            viaPoint.z = strtod(buffer, 0);
            if (file.ReadNext(buffer, 256)) return __LINE__;
            viaName = buffer;
            muscle.viaPoints.push_back(viaPoint);
            muscle.viaNames.push_back(viaName);
            continue;
        }
        
        n = buf.rfind("Cylinder");
        if (n != std::string::npos)
        {
            std::cerr << buf << "\n";
            if (file.ReadNext(buffer, 256)) return __LINE__;
            muscle.cylinderPosition.x = strtod(buffer, 0);
            if (file.ReadNext(buffer, 256)) return __LINE__;
            muscle.cylinderPosition.y = strtod(buffer, 0);
            if (file.ReadNext(buffer, 256)) return __LINE__;
            muscle.cylinderPosition.z = strtod(buffer, 0);
            if (file.ReadNext(buffer, 256)) return __LINE__;
            muscle.cylinderName = buffer;
            if (file.ReadNext(buffer, 256)) return __LINE__;
            muscle.cylinderRadius = strtod(buffer, 0);
            
            muscle.cylinderAxis.x = 0;
            muscle.cylinderAxis.y = 1;
            muscle.cylinderAxis.z = 0;
            char *index = file.GetIndex();
            if (file.ReadNext(buffer, 256) == 0)
            {
                if (strchr("0123456789+-.", buffer[0]) != 0)
                {
                    muscle.cylinderAxis.x = strtod(buffer, 0);
                    if (file.ReadNext(buffer, 256)) return __LINE__;
                    muscle.cylinderAxis.y = strtod(buffer, 0);
                    if (file.ReadNext(buffer, 256)) return __LINE__;
                    muscle.cylinderAxis.z = strtod(buffer, 0);
                }
                else
                {
                    file.SetIndex(index);
                }
            }
            else
            {
                file.SetIndex(index);
            }            
            
            
            
            continue;
        }
        
        n = buf.rfind("Insertion");
        if (n != std::string::npos)
        {
            std::cerr << buf << "\n";
            if (file.ReadNext(buffer, 256)) return __LINE__;
            muscle.insertion.x = strtod(buffer, 0);
            if (file.ReadNext(buffer, 256)) return __LINE__;
            muscle.insertion.y = strtod(buffer, 0);
            if (file.ReadNext(buffer, 256)) return __LINE__;
            muscle.insertion.z = strtod(buffer, 0);
            if (file.ReadNext(buffer, 256)) return __LINE__;
            muscle.insertionName = buffer;
            
            springList[name] = muscle;
            muscle.viaPoints.clear();
            muscle.viaNames.clear();
            muscle.originName = "";
            muscle.insertionName = "";
            muscle.cylinderName = "";
            std::cerr << name << " completed\n";
            continue;
        }
        
    }
    
    return 0;
}

int ParseJoints(const char *jointFileName)
{
    char buffer[256];
    std::string buf;
    int n;
    Joint joint;
    std::string name;
    
    DataFile file;
    if (file.ReadFile(jointFileName)) return __LINE__;
    
    while (file.ReadNext(buffer, 256) == false)
    {
        buf = buffer;
        n = buf.rfind("Centre");
        if (n != std::string::npos)
        {
            std::cerr << buf << "\n";
            name.assign(buf, 0, n);
            if (file.ReadNext(buffer, 256)) return __LINE__;
            joint.anchor.x = strtod(buffer, 0);
            if (file.ReadNext(buffer, 256)) return __LINE__;
            joint.anchor.y = strtod(buffer, 0);
            if (file.ReadNext(buffer, 256)) return __LINE__;
            joint.anchor.z = strtod(buffer, 0);
            if (file.ReadNext(buffer, 256)) return __LINE__;
            joint.body1 = buffer;
            if (file.ReadNext(buffer, 256)) return __LINE__;
            joint.body2 = buffer;
            
            
            joint.axis.x = 0;
            joint.axis.y = 1;
            joint.axis.z = 0;
            char *index = file.GetIndex();
            if (file.ReadNext(buffer, 256) == 0)
            {
                if (strchr("0123456789+-.", buffer[0]) != 0)
                {
                    joint.axis.x = strtod(buffer, 0);
                    if (file.ReadNext(buffer, 256)) return __LINE__;
                    joint.axis.y = strtod(buffer, 0);
                    if (file.ReadNext(buffer, 256)) return __LINE__;
                    joint.axis.z = strtod(buffer, 0);
                }
                else
                {
                    file.SetIndex(index);
                }
            }
            else
            {
                file.SetIndex(index);
            }            
            
            jointList[name] = joint;
            std::cerr << name << " completed\n";
            continue;
        }
    }
    
    return 0;
}

int ParseBodies(const char *bodyFileName)
{
    char buffer[256];
    Body body;
    std::string name;
    
    DataFile file;
    if (file.ReadFile(bodyFileName)) return __LINE__;
    
    while (file.ReadNext(buffer, 256) == false)
    {
        name = buffer;
        std::cerr << name << "\n";
        if (file.ReadNext(buffer, 256)) return __LINE__;
        body.mass = strtod(buffer, 0);
        if (file.ReadNext(buffer, 256)) return __LINE__;
        body.moi.x = body.moi.y = body.moi.z = strtod(buffer, 0);
        if (file.ReadNext(buffer, 256)) return __LINE__;
        body.position.x = strtod(buffer, 0);
        if (file.ReadNext(buffer, 256)) return __LINE__;
        body.position.y = strtod(buffer, 0);
        if (file.ReadNext(buffer, 256)) return __LINE__;
        body.position.z = strtod(buffer, 0);
        if (file.ReadNext(buffer, 256)) return __LINE__;
        body.graphicFile = buffer;
            
        bodyList[name] = body;
        bodyNames.push_back(name);
        std::cerr << name << " completed\n";
        
        if (distanceTravelledBodyID.size() == 0) distanceTravelledBodyID = name;
        continue;
    }
    
    return 0;
}

int ParseGeoms(const char *geomFileName)
{
    char buffer[256];
    Geom geom;
    std::string name;
    
    DataFile file;
    if (file.ReadFile(geomFileName)) return __LINE__;
    
    while (file.ReadNext(buffer, 256) == false)
    {
        name = buffer;
        std::cerr << name << "\n";
        if (file.ReadNext(buffer, 256)) return __LINE__;
        geom.position.x = strtod(buffer, 0);
        if (file.ReadNext(buffer, 256)) return __LINE__;
        geom.position.y = strtod(buffer, 0);
        if (file.ReadNext(buffer, 256)) return __LINE__;
        geom.position.z = strtod(buffer, 0);
        if (file.ReadNext(buffer, 256)) return __LINE__;
        geom.body = buffer;
        if (file.ReadNext(buffer, 256)) return __LINE__;
        geom.radius = strtod(buffer, 0);
        
        geomList[name] = geom;
        std::cerr << name << " completed\n";
        continue;
    }
    
    return 0;
}


int
WriteConfigFile(const char *outputFileName)
{
    xmlDocPtr doc;
    xmlNodePtr rootNode;
    xmlNodePtr newNode;
    xmlAttrPtr newAttr;
    pgd::Vector vec;
    pgd::Matrix3x3 mat;
    pgd::Quaternion quat;
    char buf1[256];
    char buf2[256];
    unsigned int i;
    
    doc = xmlNewDoc((xmlChar *)"1.0");
    if (doc == 0) return __LINE__;
    
    rootNode = xmlNewDocNode(doc, 0, (xmlChar *)"GAITSYMODE", 0);
    xmlDocSetRootElement(doc, rootNode);
    
    newNode = xmlNewTextChild(rootNode, 0, (xmlChar *)"IOCONTROL", 0);
    newAttr = xmlNewProp(newNode, (xmlChar *)"OldStyleInputs", (xmlChar *)"false");
    newAttr = xmlNewProp(newNode, (xmlChar *)"OldStyleOutputs", (xmlChar *)"false");
    
    newNode = xmlNewTextChild(rootNode, 0, (xmlChar *)"GLOBAL", 0);
    newAttr = xmlNewProp(newNode, (xmlChar *)"IntegrationStepSize", (xmlChar *)"1e-4");
    newAttr = xmlNewProp(newNode, (xmlChar *)"GravityVector", (xmlChar *)"0.0 0.0 -9.81");
    newAttr = xmlNewProp(newNode, (xmlChar *)"ERP", (xmlChar *)"0.2");
    newAttr = xmlNewProp(newNode, (xmlChar *)"CFM", (xmlChar *)"1e-10");
    newAttr = xmlNewProp(newNode, (xmlChar *)"ContactMaxCorrectingVel", (xmlChar *)"100");
    newAttr = xmlNewProp(newNode, (xmlChar *)"ContactSurfaceLayer", (xmlChar *)"0.001");
    newAttr = xmlNewProp(newNode, (xmlChar *)"AllowInternalCollisions", (xmlChar *)"false");
    newAttr = xmlNewProp(newNode, (xmlChar *)"BMR", (xmlChar *)"0");
    newAttr = xmlNewProp(newNode, (xmlChar *)"TimeLimit", (xmlChar *)"3");
    newAttr = xmlNewProp(newNode, (xmlChar *)"MetabolicEnergyLimit", (xmlChar *)"0");
    newAttr = xmlNewProp(newNode, (xmlChar *)"MechanicalEnergyLimit", (xmlChar *)"0");
    newAttr = xmlNewProp(newNode, (xmlChar *)"FitnessType", (xmlChar *)"DistanceTravelled");
    newAttr = xmlNewProp(newNode, (xmlChar *)"DistanceTravelledBodyID", (xmlChar *)distanceTravelledBodyID.c_str());

    newNode = xmlNewTextChild(rootNode, 0, (xmlChar *)"INTERFACE", 0);
    newAttr = xmlNewProp(newNode, (xmlChar *)"EnvironmentAxisSize", (xmlChar *)"1 1 1");
    newAttr = xmlNewProp(newNode, (xmlChar *)"EnvironmentColour", (xmlChar *)"0.5 0.5 1.0 1.0");
    newAttr = xmlNewProp(newNode, (xmlChar *)"BodyAxisSize", (xmlChar *)"0.1 0.1 0.1");
    newAttr = xmlNewProp(newNode, (xmlChar *)"BodyColour", (xmlChar *)".275 .725 .451 1.0");
    newAttr = xmlNewProp(newNode, (xmlChar *)"JointAxisSize", (xmlChar *)"0.05 0.05 0.05");
    newAttr = xmlNewProp(newNode, (xmlChar *)"JointColour", (xmlChar *)"0 1 0 1");
    newAttr = xmlNewProp(newNode, (xmlChar *)"GeomColour", (xmlChar *)"0 0 1 1");
    newAttr = xmlNewProp(newNode, (xmlChar *)"StrapColour", (xmlChar *)"1 0 0 1");
    newAttr = xmlNewProp(newNode, (xmlChar *)"StrapRadius", (xmlChar *)"0.005");
    newAttr = xmlNewProp(newNode, (xmlChar *)"StrapForceColour", (xmlChar *)"1 0 0 0.5");
    newAttr = xmlNewProp(newNode, (xmlChar *)"StrapForceRadius", (xmlChar *)"0.01");
    newAttr = xmlNewProp(newNode, (xmlChar *)"StrapForceScale", (xmlChar *)"0.000001");
    newAttr = xmlNewProp(newNode, (xmlChar *)"StrapCylinderColour", (xmlChar *)"0 1 1 1");
    newAttr = xmlNewProp(newNode, (xmlChar *)"StrapCylinderLength", (xmlChar *)"0.05");
    newAttr = xmlNewProp(newNode, (xmlChar *)"DrawingOrder", (xmlChar *)"Environment Joint Muscle Geom Body");
    newAttr = xmlNewProp(newNode, (xmlChar *)"TrackBodyID", (xmlChar *)distanceTravelledBodyID.c_str());
    
    newNode = xmlNewTextChild(rootNode, 0, (xmlChar *)"ENVIRONMENT", 0);
    newAttr = xmlNewProp(newNode, (xmlChar *)"Plane", (xmlChar *)"0 0 1 0");    
    
#ifdef USE_MARKERS
    // markers
    std::string name;
    std::map<std::string, Body >::const_iterator bodyIter;
    for (bodyIter = bodyList.begin(); bodyIter != bodyList.end(); bodyIter++)
    {
        name = bodyIter->first + std::string("PositionMarker");
        newNode = xmlNewTextChild(rootNode, 0, (xmlChar *)"MARKER", 0);
        newAttr = xmlNewProp(newNode, (xmlChar *)"ID", (xmlChar *)name.c_str());
        newAttr = xmlNewProp(newNode, (xmlChar *)"BodyID", (xmlChar *)"World");
        sprintf(buf1, "World %.17g %.17g %.17g", bodyIter->second.position.x, bodyIter->second.position.y, bodyIter->second.position.z);
        newAttr = xmlNewProp(newNode, (xmlChar *)"Position", (xmlChar *)buf1); 
        // Quaternion
        name = bodyIter->first + std::string("QuaternionMarker");
        newNode = xmlNewTextChild(rootNode, 0, (xmlChar *)"MARKER", 0);
        newAttr = xmlNewProp(newNode, (xmlChar *)"ID", (xmlChar *)name.c_str());
        newAttr = xmlNewProp(newNode, (xmlChar *)"BodyID", (xmlChar *)"World");
        sprintf(buf1, "World %.17g %.17g %.17g %.17g", 1.0, 0.0, 0.0, 0.0);
        newAttr = xmlNewProp(newNode, (xmlChar *)"Quaternion", (xmlChar *)buf1);
    }

    
    // bodies
    for (unsigned int nameCount = 0; nameCount < bodyNames.size(); nameCount++)
    {
        bodyIter = bodyList.find(bodyNames[nameCount]);
        std::cerr << bodyIter->first << "\n";
        
        newNode = xmlNewTextChild(rootNode, 0, (xmlChar *)"BODY", 0);
        newAttr = xmlNewProp(newNode, (xmlChar *)"ID", (xmlChar *)bodyIter->first.c_str());
            
        sprintf(buf1, "%.17g", bodyIter->second.mass);
        newAttr = xmlNewProp(newNode, (xmlChar *)"Mass", (xmlChar *)buf1);
            
        sprintf(buf1, "%.17g %.17g %.17g %.17g %.17g %.17g", bodyIter->second.moi.x, bodyIter->second.moi.y, bodyIter->second.moi.z, 
                bodyIter->second.poi.x, bodyIter->second.poi.y, bodyIter->second.poi.z);
        newAttr = xmlNewProp(newNode, (xmlChar *)"MOI", (xmlChar *)buf1);      
        // note: inertial matrix is as follows
        // [ I11 I12 I13 ]
        // [ I12 I22 I23 ]
        // [ I13 I23 I33 ]
        // MOI="I11 I22 I33 I12 I13 I23"
            
        name = bodyIter->first + std::string("PositionMarker");
        newAttr = xmlNewProp(newNode, (xmlChar *)"Position", (xmlChar *)name.c_str());      
            
        name = bodyIter->first + std::string("QuaternionMarker");
        newAttr = xmlNewProp(newNode, (xmlChar *)"Quaternion", (xmlChar *)name.c_str());
        // Quaternion="qs qx qy qz"
            
        // currently we set velocities to zero
        sprintf(buf1, "%.17g %.17g %.17g", 0.0, 0.0, 0.0);
        newAttr = xmlNewProp(newNode, (xmlChar *)"LinearVelocity", (xmlChar *)buf1);
        sprintf(buf1, "%.17g %.17g %.17g", 0.0, 0.0, 0.0);
        newAttr = xmlNewProp(newNode, (xmlChar *)"AngularVelocity", (xmlChar *)buf1);   
            
        // now handle the graphic file
        newAttr = xmlNewProp(newNode, (xmlChar *)"GraphicFile", (xmlChar *)bodyIter->second.graphicFile.c_str());
        newAttr = xmlNewProp(newNode, (xmlChar *)"Scale", (xmlChar *)"1.0");
        sprintf(buf1, "%.17g %.17g %.17g", -bodyIter->second.position.x, -bodyIter->second.position.y, -bodyIter->second.position.z);
        newAttr = xmlNewProp(newNode, (xmlChar *)"Offset", (xmlChar *)buf1);
        
        newAttr = xmlNewProp(newNode, (xmlChar *)"Density", (xmlChar *)"-1");
        
        newAttr = xmlNewProp(newNode, (xmlChar *)"PositionLowBound", (xmlChar *)"-10 -5 0");
        newAttr = xmlNewProp(newNode, (xmlChar *)"PositionHighBound", (xmlChar *)"1000 5 2");
        newAttr = xmlNewProp(newNode, (xmlChar *)"Clockwise", (xmlChar *)"false");

    }
#else
    // bodies
    std::map<std::string, Body >::const_iterator bodyIter;
    for (unsigned int nameCount = 0; nameCount < bodyNames.size(); nameCount++)
    {
        bodyIter = bodyList.find(bodyNames[nameCount]);
        std::cerr << bodyIter->first << "\n";
        
        newNode = xmlNewTextChild(rootNode, 0, (xmlChar *)"BODY", 0);
        newAttr = xmlNewProp(newNode, (xmlChar *)"ID", (xmlChar *)bodyIter->first.c_str());
        
        sprintf(buf1, "%.17g", bodyIter->second.mass);
        newAttr = xmlNewProp(newNode, (xmlChar *)"Mass", (xmlChar *)buf1);
        
        sprintf(buf1, "%.17g %.17g %.17g %.17g %.17g %.17g", bodyIter->second.moi.x, bodyIter->second.moi.y, bodyIter->second.moi.z, 
                bodyIter->second.poi.x, bodyIter->second.poi.y, bodyIter->second.poi.z);
        newAttr = xmlNewProp(newNode, (xmlChar *)"MOI", (xmlChar *)buf1);      
        // note: inertial matrix is as follows
        // [ I11 I12 I13 ]
        // [ I12 I22 I23 ]
        // [ I13 I23 I33 ]
        // MOI="I11 I22 I33 I12 I13 I23"
        
        sprintf(buf1, "World %.17g %.17g %.17g", bodyIter->second.position.x, bodyIter->second.position.y, bodyIter->second.position.z);
        newAttr = xmlNewProp(newNode, (xmlChar *)"Position", (xmlChar *)buf1);      
        
        sprintf(buf1, "World %.17g %.17g %.17g %.17g", 1.0, 0.0, 0.0, 0.0);
        newAttr = xmlNewProp(newNode, (xmlChar *)"Quaternion", (xmlChar *)buf1);
        // Quaternion="qs qx qy qz"
        
        // currently we set velocities to zero
        sprintf(buf1, "%.17g %.17g %.17g", 0.0, 0.0, 0.0);
        newAttr = xmlNewProp(newNode, (xmlChar *)"LinearVelocity", (xmlChar *)buf1);
        sprintf(buf1, "%.17g %.17g %.17g", 0.0, 0.0, 0.0);
        newAttr = xmlNewProp(newNode, (xmlChar *)"AngularVelocity", (xmlChar *)buf1);   
        
        // now handle the graphic file
        newAttr = xmlNewProp(newNode, (xmlChar *)"GraphicFile", (xmlChar *)bodyIter->second.graphicFile.c_str());
        newAttr = xmlNewProp(newNode, (xmlChar *)"Scale", (xmlChar *)"1.0");
        sprintf(buf1, "%.17g %.17g %.17g", -bodyIter->second.position.x, -bodyIter->second.position.y, -bodyIter->second
                .position.z);
        newAttr = xmlNewProp(newNode, (xmlChar *)"Offset", (xmlChar *)buf1);
        
        newAttr = xmlNewProp(newNode, (xmlChar *)"Density", (xmlChar *)"-1");
        
        newAttr = xmlNewProp(newNode, (xmlChar *)"PositionLowBound", (xmlChar *)"-10 -5 0");
        newAttr = xmlNewProp(newNode, (xmlChar *)"PositionHighBound", (xmlChar *)"1000 5 2");
        newAttr = xmlNewProp(newNode, (xmlChar *)"Clockwise", (xmlChar *)"false");
    }    
#endif
    
    // joints
    std::map<std::string, Joint >::const_iterator jointIter;
    for (jointIter = jointList.begin(); jointIter != jointList.end(); jointIter++)
    {
        std::cerr << jointIter->first << "\n";
            
        newNode = xmlNewTextChild(rootNode, 0, (xmlChar *)"JOINT", 0);
        newAttr = xmlNewProp(newNode, (xmlChar *)"ID", (xmlChar *)jointIter->first.c_str());
        
        newAttr = xmlNewProp(newNode, (xmlChar *)"Type", (xmlChar *)"Hinge");
        
        newAttr = xmlNewProp(newNode, (xmlChar *)"Body1ID", (xmlChar *)jointIter->second.body1.c_str());
        newAttr = xmlNewProp(newNode, (xmlChar *)"Body2ID", (xmlChar *)jointIter->second.body2.c_str());
        
        vec = jointIter->second.anchor;
        sprintf(buf1, "%s %.17g %.17g %.17g", "World", vec.x, vec.y, vec.z);
        newAttr = xmlNewProp(newNode, (xmlChar *)"HingeAnchor", (xmlChar *)buf1);
        
        vec = jointIter->second.axis;
        sprintf(buf1, "%s %.17g %.17g %.17g", "World", vec.x, vec.y, vec.z);
        newAttr = xmlNewProp(newNode, (xmlChar *)"HingeAxis", (xmlChar *)buf1);
        
        newAttr = xmlNewProp(newNode, (xmlChar *)"ParamLoStop", (xmlChar *)"-100");
        newAttr = xmlNewProp(newNode, (xmlChar *)"ParamHiStop", (xmlChar *)"+100");
        newAttr = xmlNewProp(newNode, (xmlChar *)"StartAngleReference", (xmlChar *)"0");
            
    }

    // geoms
    std::map<std::string, Geom >::const_iterator geomIter;
    for (geomIter = geomList.begin(); geomIter != geomList.end(); geomIter++)
    {
        std::cerr << geomIter->first << "\n";
        
        newNode = xmlNewTextChild(rootNode, 0, (xmlChar *)"GEOM", 0);
        newAttr = xmlNewProp(newNode, (xmlChar *)"ID", (xmlChar *)geomIter->first.c_str());
        
        newAttr = xmlNewProp(newNode, (xmlChar *)"Type", (xmlChar *)"Sphere");
        
        newAttr = xmlNewProp(newNode, (xmlChar *)"BodyID", (xmlChar *)geomIter->second.body.c_str());
        
        vec = geomIter->second.position;
        sprintf(buf1, "%s %.17g %.17g %.17g", "World", vec.x, vec.y, vec.z);
        newAttr = xmlNewProp(newNode, (xmlChar *)"Position", (xmlChar *)buf1);
        
        sprintf(buf1, "World %.17g %.17g %.17g %.17g", 1.0, 0.0, 0.0, 0.0);
        newAttr = xmlNewProp(newNode, (xmlChar *)"Quaternion", (xmlChar *)buf1);
        // Quaternion="qs qx qy qz"
        
        sprintf(buf1, "%.17g", geomIter->second.radius);
        newAttr = xmlNewProp(newNode, (xmlChar *)"Radius", (xmlChar *)buf1);

        newAttr = xmlNewProp(newNode, (xmlChar *)"SpringConstant", (xmlChar *)"1e5");
        newAttr = xmlNewProp(newNode, (xmlChar *)"DampingConstant", (xmlChar *)"1e4");
        newAttr = xmlNewProp(newNode, (xmlChar *)"Mu", (xmlChar *)"1.0");
        newAttr = xmlNewProp(newNode, (xmlChar *)"Abort", (xmlChar *)"false");
        
    }
    
    // muscles
    std::map<std::string, Muscle >::const_iterator muscleIter;
    for (muscleIter = muscleList.begin(); muscleIter != muscleList.end(); muscleIter++)
    {
        std::cerr << muscleIter->first << "\n";
        
        newNode = xmlNewTextChild(rootNode, 0, (xmlChar *)"MUSCLE", 0);
        newAttr = xmlNewProp(newNode, (xmlChar *)"ID", (xmlChar *)muscleIter->first.c_str());
            
        newAttr = xmlNewProp(newNode, (xmlChar *)"Type", (xmlChar *)"MinettiAlexanderExtended");
            
        newAttr = xmlNewProp(newNode, (xmlChar *)"OriginBodyID", (xmlChar *)muscleIter->second.originName.c_str());
        vec = muscleIter->second.origin;
        sprintf(buf1, "%s %.17g %.17g %.17g", "World", vec.x, vec.y, vec.z);
        newAttr = xmlNewProp(newNode, (xmlChar *)"Origin", (xmlChar *)buf1);
            
        newAttr = xmlNewProp(newNode, (xmlChar *)"InsertionBodyID", (xmlChar *)muscleIter->second.insertionName.c_str());
        vec = muscleIter->second.insertion;
        sprintf(buf1, "%s %.17g %.17g %.17g", "World", vec.x, vec.y, vec.z);
        newAttr = xmlNewProp(newNode, (xmlChar *)"Insertion", (xmlChar *)buf1);
            
        double forcePerUnitArea = 300000.;
        sprintf(buf1, "%.17g", forcePerUnitArea);
        newAttr = xmlNewProp(newNode, (xmlChar *)"ForcePerUnitArea", (xmlChar *)buf1);
        sprintf(buf1, "%.17g", 1.0);
        newAttr = xmlNewProp(newNode, (xmlChar *)"PCA", (xmlChar *)buf1);
        sprintf(buf1, "%.17g", 1.0);
        newAttr = xmlNewProp(newNode, (xmlChar *)"FibreLength", (xmlChar *)buf1); // this is ignoring pennation
        sprintf(buf1, "%.17g", -1.0);
        newAttr = xmlNewProp(newNode, (xmlChar *)"TendonLength", (xmlChar *)buf1);
        sprintf(buf1, "%.17g", 8.4);
        newAttr = xmlNewProp(newNode, (xmlChar *)"VMaxFactor", (xmlChar *)buf1);
        sprintf(buf1, "%.17g", 0.06);
        newAttr = xmlNewProp(newNode, (xmlChar *)"SerialStrainAtFmax", (xmlChar *)buf1);
            
        // these values are not really available
        newAttr = xmlNewProp(newNode, (xmlChar *)"ParallelStrainAtFmax", (xmlChar *)"0.6");
        newAttr = xmlNewProp(newNode, (xmlChar *)"ActivationK", (xmlChar *)"0.17");
        newAttr = xmlNewProp(newNode, (xmlChar *)"ActivationKinetics", (xmlChar *)"false");
        
        int numNodes = muscleIter->second.viaPoints.size() + 2;
        if (numNodes == 2)
        {
            if ( muscleIter->second.cylinderName.size() == 0 && muscleIter->second.cylinder1Name.size() == 0 && muscleIter->second.cylinder2Name.size() == 0)
            {
                newAttr = xmlNewProp(newNode, (xmlChar *)"Strap", (xmlChar *)"TwoPoint");
            }
            else
            {
                if (muscleIter->second.cylinderName.size() > 0)
                {
                    newAttr = xmlNewProp(newNode, (xmlChar *)"Strap", (xmlChar *)"CylinderWrap");
                    newAttr = xmlNewProp(newNode, (xmlChar *)"CylinderBodyID", (xmlChar *)muscleIter->second.cylinderName.c_str());
                    vec = muscleIter->second.cylinderPosition;
                    sprintf(buf1, "%s %.17g %.17g %.17g", "World", vec.x, vec.y, vec.z);
                    newAttr = xmlNewProp(newNode, (xmlChar *)"CylinderPosition", (xmlChar *)buf1);
                    if (muscleIter->second.cylinderRadius > 0)
                    {
                        sprintf(buf1, "%.17g", muscleIter->second.cylinderRadius);
                        newAttr = xmlNewProp(newNode, (xmlChar *)"CylinderRadius", (xmlChar *)buf1);
                        sprintf(buf1, "World %.17g %.17g %.17g", muscleIter->second.cylinderAxis.x, muscleIter->second.cylinderAxis.y, muscleIter->second.cylinderAxis.z);
                        newAttr = xmlNewProp(newNode, (xmlChar *)"CylinderAxis", (xmlChar *)buf1);
                    }
                    else
                    {
                        sprintf(buf1, "%.17g", -1 * muscleIter->second.cylinderRadius);
                        newAttr = xmlNewProp(newNode, (xmlChar *)"CylinderRadius", (xmlChar *)buf1);
                        sprintf(buf1, "World %.17g %.17g %.17g",-1 * muscleIter->second.cylinderAxis.x, -1 * muscleIter->second.cylinderAxis.y, -1 * muscleIter->second.cylinderAxis.z);
                        newAttr = xmlNewProp(newNode, (xmlChar *)"CylinderAxis", (xmlChar *)buf1);
                    }
                }
                else
                {
                    newAttr = xmlNewProp(newNode, (xmlChar *)"Strap", (xmlChar *)"TwoCylinderWrap");
                    newAttr = xmlNewProp(newNode, (xmlChar *)"CylinderBodyID", (xmlChar *)muscleIter->second.cylinder1Name.c_str());
                    sprintf(buf1, "World %.17g %.17g %.17g", muscleIter->second.cylinder1Axis.x, muscleIter->second.cylinder1Axis.y, muscleIter->second.cylinder1Axis.z);
                    newAttr = xmlNewProp(newNode, (xmlChar *)"CylinderAxis", (xmlChar *)buf1);
                    vec = muscleIter->second.cylinder1Position;
                    sprintf(buf1, "%s %.17g %.17g %.17g", "World", vec.x, vec.y, vec.z);
                    newAttr = xmlNewProp(newNode, (xmlChar *)"Cylinder1Position", (xmlChar *)buf1);
                    vec = muscleIter->second.cylinder2Position;
                    sprintf(buf1, "%s %.17g %.17g %.17g", "World", vec.x, vec.y, vec.z);
                    newAttr = xmlNewProp(newNode, (xmlChar *)"Cylinder2Position", (xmlChar *)buf1);
                    sprintf(buf1, "%.17g", muscleIter->second.cylinder1Radius);
                    newAttr = xmlNewProp(newNode, (xmlChar *)"Cylinder1Radius", (xmlChar *)buf1);
                    sprintf(buf1, "%.17g", muscleIter->second.cylinder2Radius);
                    newAttr = xmlNewProp(newNode, (xmlChar *)"Cylinder2Radius", (xmlChar *)buf1);
                }
            }
        }
        else if (numNodes == 3)
        {
            newAttr = xmlNewProp(newNode, (xmlChar *)"Strap", (xmlChar *)"ThreePoint");
            newAttr = xmlNewProp(newNode, (xmlChar *)"MidpointBodyID", (xmlChar *)muscleIter->second.viaNames[0].c_str());
            vec = muscleIter->second.viaPoints[0];
            sprintf(buf1, "%s %.17g %.17g %.17g", "World", vec.x, vec.y, vec.z);
            newAttr = xmlNewProp(newNode, (xmlChar *)"Midpoint", (xmlChar *)buf1);
        }
        else if (numNodes > 3)
        {
            newAttr = xmlNewProp(newNode, (xmlChar *)"Strap", (xmlChar *)"NPoint");
            for (i = 0; i < muscleIter->second.viaPoints.size(); i++)
            {
                sprintf(buf1, "ViaPointBody%d", i);
                newAttr = xmlNewProp(newNode, (xmlChar *)buf1, (xmlChar *)muscleIter->second.viaNames[i].c_str());
                vec = muscleIter->second.viaPoints[i];
                sprintf(buf1, "%s %.17g %.17g %.17g", "World", vec.x, vec.y, vec.z);
                sprintf(buf2, "ViaPoint%d", i);
                newAttr = xmlNewProp(newNode, (xmlChar *)buf2, (xmlChar *)buf1);
            }
        }
    }

    // springs
    for (muscleIter = springList.begin(); muscleIter != springList.end(); muscleIter++)
    {
        std::cerr << muscleIter->first << "\n";
        
        newNode = xmlNewTextChild(rootNode, 0, (xmlChar *)"MUSCLE", 0);
        newAttr = xmlNewProp(newNode, (xmlChar *)"ID", (xmlChar *)muscleIter->first.c_str());
        
        newAttr = xmlNewProp(newNode, (xmlChar *)"Type", (xmlChar *)"DampedSpring");
        
        newAttr = xmlNewProp(newNode, (xmlChar *)"OriginBodyID", (xmlChar *)muscleIter->second.originName.c_str());
        vec = muscleIter->second.origin;
        sprintf(buf1, "%s %.17g %.17g %.17g", "World", vec.x, vec.y, vec.z);
        newAttr = xmlNewProp(newNode, (xmlChar *)"Origin", (xmlChar *)buf1);
        
        newAttr = xmlNewProp(newNode, (xmlChar *)"InsertionBodyID", (xmlChar *)muscleIter->second.insertionName.c_str());
        vec = muscleIter->second.insertion;
        sprintf(buf1, "%s %.17g %.17g %.17g", "World", vec.x, vec.y, vec.z);
        newAttr = xmlNewProp(newNode, (xmlChar *)"Insertion", (xmlChar *)buf1);
        
        pgd::Vector path = muscleIter->second.insertion - muscleIter->second.origin;
        sprintf(buf1, "%.17g", path.Magnitude());
        newAttr = xmlNewProp(newNode, (xmlChar *)"UnloadedLength", (xmlChar *)buf1);
        sprintf(buf1, "%.17g", 1.5e9);
        newAttr = xmlNewProp(newNode, (xmlChar *)"SpringConstant", (xmlChar *)buf1); // 1.5e9 Pa (Bennett et al. 1986; Ker et al. 1988)
        sprintf(buf1, "%.17g", 0.01 * 0.01);
        newAttr = xmlNewProp(newNode, (xmlChar *)"Area", (xmlChar *)buf1);
        sprintf(buf1, "%.17g", 1.5e6);
        newAttr = xmlNewProp(newNode, (xmlChar *)"Damping", (xmlChar *)buf1);
        
        int numNodes = muscleIter->second.viaPoints.size() + 2;
        if (numNodes == 2)
        {
            if ( muscleIter->second.cylinderName.size() == 0)
            {
                newAttr = xmlNewProp(newNode, (xmlChar *)"Strap", (xmlChar *)"TwoPoint");
            }
            else
            {
                newAttr = xmlNewProp(newNode, (xmlChar *)"Strap", (xmlChar *)"CylinderWrap");
                newAttr = xmlNewProp(newNode, (xmlChar *)"CylinderBodyID", (xmlChar *)muscleIter->second.cylinderName.c_str());
                vec = muscleIter->second.cylinderPosition;
                sprintf(buf1, "%s %.17g %.17g %.17g", "World", vec.x, vec.y, vec.z);
                newAttr = xmlNewProp(newNode, (xmlChar *)"CylinderPosition", (xmlChar *)buf1);
                if (muscleIter->second.cylinderRadius > 0)
                {
                    sprintf(buf1, "%.17g", muscleIter->second.cylinderRadius);
                    newAttr = xmlNewProp(newNode, (xmlChar *)"CylinderRadius", (xmlChar *)buf1);
                    sprintf(buf1, "World %.17g %.17g %.17g", muscleIter->second.cylinderAxis.x, muscleIter->second.cylinderAxis.y, muscleIter->second.cylinderAxis.z);
                    newAttr = xmlNewProp(newNode, (xmlChar *)"CylinderAxis", (xmlChar *)buf1);
                }
                else
                {
                    sprintf(buf1, "%.17g", -1 * muscleIter->second.cylinderRadius);
                    newAttr = xmlNewProp(newNode, (xmlChar *)"CylinderRadius", (xmlChar *)buf1);
                    sprintf(buf1, "World %.17g %.17g %.17g",-1 * muscleIter->second.cylinderAxis.x, -1 * muscleIter->second.cylinderAxis.y, -1 * muscleIter->second.cylinderAxis.z);
                    newAttr = xmlNewProp(newNode, (xmlChar *)"CylinderAxis", (xmlChar *)buf1);
                }
            }
        }
        else if (numNodes == 3)
        {
            newAttr = xmlNewProp(newNode, (xmlChar *)"Strap", (xmlChar *)"ThreePoint");
            newAttr = xmlNewProp(newNode, (xmlChar *)"MidpointBodyID", (xmlChar *)muscleIter->second.viaNames[0].c_str());
            vec = muscleIter->second.viaPoints[0];
            sprintf(buf1, "%s %.17g %.17g %.17g", "World", vec.x, vec.y, vec.z);
            newAttr = xmlNewProp(newNode, (xmlChar *)"Midpoint", (xmlChar *)buf1);
        }
        else if (numNodes > 3)
        {
            newAttr = xmlNewProp(newNode, (xmlChar *)"Strap", (xmlChar *)"NPoint");
            for (i = 0; i < muscleIter->second.viaPoints.size(); i++)
            {
                sprintf(buf1, "ViaPointBody%d", i);
                newAttr = xmlNewProp(newNode, (xmlChar *)buf1, (xmlChar *)muscleIter->second.viaNames[i].c_str());
                vec = muscleIter->second.viaPoints[i];
                sprintf(buf1, "%s %.17g %.17g %.17g", "World", vec.x, vec.y, vec.z);
                sprintf(buf2, "ViaPoint%d", i);
                newAttr = xmlNewProp(newNode, (xmlChar *)buf2, (xmlChar *)buf1);
            }
        }
    }

    // spring drivers
    for (muscleIter = springList.begin(); muscleIter != springList.end(); muscleIter++)
    {
        std::cerr << muscleIter->first << "\n";
        newNode = xmlNewTextChild(rootNode, 0, (xmlChar *)"DRIVER", 0);
        sprintf(buf1, "%s_driver", muscleIter->first.c_str());
        newAttr = xmlNewProp(newNode, (xmlChar *)"ID", (xmlChar *)buf1);
        newAttr = xmlNewProp(newNode, (xmlChar *)"Target", (xmlChar *)muscleIter->first.c_str());
        sprintf(buf1, "%g %g", 1000.0, 1.0);
        newAttr = xmlNewProp(newNode, (xmlChar *)"DurationValuePairs", (xmlChar *)buf1);
        sprintf(buf1, "Cyclic");
        newAttr = xmlNewProp(newNode, (xmlChar *)"Type", (xmlChar *)buf1);
    }
    
    xmlThrDefIndentTreeOutput(1);
    xmlSaveFormatFile(outputFileName, doc, 1);
    
    xmlFreeDoc(doc);
    
    return 0;
}
