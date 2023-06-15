#include <stdio.h>
#include <stdarg.h>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <typeinfo>

#include <libxml/xmlmemory.h>
#include <libxml/parser.h>

#include "DataFile.h"
#include "AnyFolder.h"
#include "AnySeg.h"
#include "AnyRefNode.h"
#include "AnyJoint.h"
#include "AnyMuscle.h"
#include "AnyDrawSurf.h"
#include "FacetedObject.h"
#include "AnySurfCylinder.h"
#include "AnyMuscleModel.h"
#include "AnyVec.h"
#include "AnyVar.h"
#include "AnyMat.h"

static void PrintExit(const char *templ, ...);
static char *Preprocess(const char *filename);
static void WriteXML(std::string filename, std::vector<AnyFolder *> &list);
static void ParallelAxis(double x, double y, double z, double mass, pgd::Matrix3x3 *moi);
static void ParallelAxis(double x, double y, double z, // transformation from centre of mass to new location (m)
                         double mass, // mass (kg)
                         double ixx, double iyy, double izz, double ixy, double iyz, double izx, // moments of inertia kgm2
                         double ang, // rotation angle (radians)
                         double ax, double ay, double az, // axis of rotation - must be unit length
                         double *ixxp, double *iyyp, double *izzp, double *ixyp, double *iyzp, double *izxp); // transformed moments of inertia about new coordinate system


int gDebug = 0;
std::vector<std::string> gHatSegments;

int main (int argc, char * const argv[]) 
{
    char *inputFileName = 0;
    char *hatSegmentsFileName = 0;
    int i;
    unsigned int j;
    
    // parse the command line
    try
    {
        for (i = 1; i < argc; i++)
        {
            if (strcmp(argv[i], "-i") == 0)
            {
                i++;
                if (i >= argc) throw(1);
                inputFileName = argv[i];
            }
            
            else if (strcmp(argv[i], "-h") == 0)
            {
                i++;
                if (i >= argc) throw(1);
                hatSegmentsFileName = argv[i];
            }
        }
    }
    
    catch (int e)
    {
        std::cerr << argv[0] << " program\n";
        std::cerr << "Build " << __DATE__ << " " << __TIME__ << "\n";
        std::cerr << "-i inputFileName\n";
        std::cerr << "-h hatSegmentsFileName\n";
        return 1;
    }
    
    if (hatSegmentsFileName)
    {
        DataFile hatSegmentsFile;
        hatSegmentsFile.SetExitOnError(true);
        hatSegmentsFile.ReadFile(hatSegmentsFileName);
        char *buf = hatSegmentsFile.GetRawData();
        AnyFolder::Tokenizer(buf, gHatSegments);
    }
    
    char *buf = Preprocess(inputFileName);
    
    std::ofstream prepout;
    prepout.open("preprocess.any");
    prepout << buf;
    prepout.close();
    
    std::vector<std::string> tokens;
    AnyFolder::Tokenizer(buf, tokens);
    unsigned int numberUnresolved = 0;
    unsigned int numberUnresolvedLastTime = 0;
    AnyFolder *currentRoot = 0;
    AnyFolder *lastRoot = 0;
    unsigned int index;
    std::vector<AnyFolder *> list;
    std::map<std::string, std::string> unresolved;
    do
    {
        numberUnresolvedLastTime = numberUnresolved;
        lastRoot = currentRoot;
        currentRoot = new AnyFolder("AnyFolder", "Main", 0);
        currentRoot->SetAlternateTree(lastRoot);
        index = 0;
        currentRoot->ParseTokens(tokens, index);
        list.clear();
        unresolved.clear();
        currentRoot->GetAllChildren(&list);
        for (j = 0; j < list.size(); j++) if (list[j]->GetStatus() != 0) unresolved[list[j]->GetPath()] = list[j]->GetContent();
        std::cerr << "unresolved.size() = " << unresolved.size() << "\n";
        numberUnresolved = unresolved.size();
        
        AnyVec *anyVec = dynamic_cast<AnyVec *>(currentRoot->Search("Main.Model.HumanModel.Trunk.SegmentsLumbar.PelvisSeg.r0", false));
        if (anyVec)
            std::cerr << anyVec << " " << *anyVec << "\n";
        if (lastRoot) anyVec = dynamic_cast<AnyVec *>(lastRoot->Search("Main.Model.HumanModel.Trunk.SegmentsLumbar.PelvisSeg.r0", false));
        if (anyVec)
            std::cerr << anyVec << " " << *anyVec << "\n";
        
    } while (numberUnresolved != numberUnresolvedLastTime);
                
    std::map<std::string, std::string>::iterator iter;
    std::ofstream unres;
    unres.open("Unresolved.txt");
    iter = unresolved.begin();
    while (iter != unresolved.end())
    {
        unres << iter->first << " " << iter->second << "\n";
        iter++;
    }
    unres.close();
        
    std::ofstream created;
    created.open("Created.txt");
    std::ofstream segs;
    segs.open("Bodies.txt");
    std::ofstream joints;
    joints.open("Joint.txt");
    std::ofstream refnodes;
    refnodes.open("RefNodes.txt");
    std::ofstream muscles;
    muscles.open("Muscles.txt");
    for (j = 0; j < list.size(); j++)
    {
        AnyFolder *anyFolder = list[j];
        created << typeid(*anyFolder).name() << " " << anyFolder->GetType() << " " << anyFolder->GetPath() << "\n";
        AnySeg *anySeg = dynamic_cast<AnySeg *>(anyFolder);
        if (anySeg)
            segs << *anySeg << "\n";
        AnyJoint *anyJoint = dynamic_cast<AnyJoint *>(anyFolder);
        if (anyJoint)
            joints << *anyJoint << "\n";
        AnyRefNode *anyRefNode = dynamic_cast<AnyRefNode *>(anyFolder);
        if (anyRefNode)
        {
            refnodes << *anyRefNode << "\n";
            pgd::Vector gp = anyRefNode->GetGlobalPosition();
            refnodes << "Global Position = " << gp.x << " " << gp.y << " " << gp.z << "\n";
        }
        AnyMuscle *anyMuscle = dynamic_cast<AnyMuscle *>(anyFolder);
        if (anyMuscle)
            muscles << *anyMuscle << "\n";
    }
    created.close();
    segs.close();
    joints.close();
    refnodes.close();
    muscles.close();
    
    if (gHatSegments.size())
    {
        // merge the HAT segment
        std::map<std::string, int> sortedHatSegments;
        std::map<std::string, int>::iterator sortedHatSegmentsIter;
        std::vector<AnySeg *> hatList;
        for (j = 0; j < gHatSegments.size(); j++) sortedHatSegments[gHatSegments[j]] = j;
        AnyFolder *anyFolder;
        AnySeg *anySeg;
        std::vector<AnyFolder *>::iterator listIterator = list.begin();
        while (listIterator != list.end())
        {
            anyFolder = *listIterator;
            anySeg = dynamic_cast<AnySeg *>(anyFolder);
            if (anySeg)
            {
                sortedHatSegmentsIter = sortedHatSegments.find(anySeg->GetPath());
                if (sortedHatSegmentsIter != sortedHatSegments.end())
                {
                    hatList.push_back(anySeg);
                    list.erase(listIterator);
                }
            }
            listIterator++;
        }
        // now find the centre of mass of the HAT
        pgd::Vector cm(0, 0, 0);
        double totalMass = 0;
        double mass;
        for (j = 0; j < hatList.size(); j++)
        {
            mass = hatList[j]->GetMass();
            totalMass += mass;
            cm += (hatList[j]->GetCMPosition() * mass);
        }
        cm /= totalMass;
        // now find the moment of inertia about the cm
        pgd::Matrix3x3 totalMoi(0,0,0,0,0,0,0,0,0);
        pgd::Matrix3x3 moi;
        pgd::Matrix3x3 rot;
        pgd::Quaternion qrot;
        pgd::Vector position, offset, axis;
        double ixxp, iyyp, izzp, ixyp, iyzp, izxp;
        for (j = 0; j < hatList.size(); j++)
        {
            position = hatList[j]->GetCMPosition();
            mass = hatList[j]->GetMass();
            offset = cm - position;
            moi = hatList[j]->GetMoIAroundCM();
            rot = hatList[j]->GetGlobalRotationMatrix();
            qrot = pgd::MakeQfromM(rot);
            axis = pgd::QGetAxis(qrot);
            ParallelAxis(offset.x, offset.y, offset.z, // transformation from centre of mass to new location (m)
                         mass, // mass (kg)
                         moi.e11, moi.e22, moi.e33, moi.e12, moi.e23, moi.e13, // moments of inertia kgm2
                         pgd::QGetAngle(qrot), // rotation angle (radians)
                         axis.x, axis.y, axis.z, // axis of rotation - must be unit length
                         &ixxp, &iyyp, &izzp, &ixyp, &iyzp, &izxp);  // transformed moments of inertia about new coordinate system
            moi.e11 = ixxp;
            moi.e22 = iyyp;
            moi.e33 = izzp;
            moi.e12 = moi.e21 = ixyp; 
            moi.e23 = moi.e32 = iyzp;
            moi.e13 = moi.e31 = izxp;
            ParallelAxis(offset.x, offset.y, offset.z, mass, &moi);
            totalMoi += moi;
        }
        
        // now create the HAT graphic
        FacetedObject *hatFacetedObject = new FacetedObject();
        for (j = 0; j < hatList.size(); j++)
        {
            position = hatList[j]->GetCMPosition();
            rot = hatList[j]->GetGlobalRotationMatrix();
            offset = cm - position;
            AnyDrawSurf *anyDrawSurf = hatList[j]->GetAnyDrawSurf();
            pgd::Matrix3x3 mat;
            if (anyDrawSurf)
            {
                std::string surfFilename;
                anyDrawSurf->GetProperty("FileName", &surfFilename);
                FacetedObject *facetedObject = new FacetedObject();
                facetedObject->ParseANYSURFFile(surfFilename.c_str());
                AnyFunTransform3D *anyFunTransform3D = anyDrawSurf->GetAnyFunTransform3D();
                if (anyFunTransform3D) facetedObject->ApplyAnyFunction(anyFunTransform3D);
                pgd::Vector vector;
                anyDrawSurf->GetProperty("ScaleXYZ", &vector);
                facetedObject->Scale(vector.x, vector.y, vector.z);
                if (dynamic_cast<AnyRefNode *>(anyDrawSurf->GetParent()))
                {
                    anyDrawSurf->GetParent()->GetProperty("ARel", &mat);
                    facetedObject->ApplyMatrix3x3(&mat);
                    anyDrawSurf->GetParent()->GetProperty("sRel", &vector);
                    facetedObject->Move(vector.x, vector.y, vector.z);
                }
                hatList[j]->GetProperty("sCoM", &vector);
                facetedObject->Move(-vector.x, -vector.y, -vector.z);
                facetedObject->ApplyMatrix3x3(&rot);
                facetedObject->Move(-offset.x, -offset.y, -offset.z);
                hatFacetedObject->Concatenate(facetedObject);
                delete facetedObject;
            }
        }
        hatFacetedObject->SwapAxes(1, 2);
        hatFacetedObject->Mirror(false, true, false);
        hatFacetedObject->WriteOBJFile("HAT.obj");
        delete hatFacetedObject;
        
        char buffer[1000];
        tokens.clear();
        index = 0;
        anySeg = new AnySeg("AnySeg", "HAT", 0);
        sprintf(buffer, "HAT={Mass=%f;};", totalMass);
        AnyFolder::Tokenizer(buffer, tokens);
        anySeg->ParseTokens(tokens, index);
        
        tokens.clear();
        index = 0;
        sprintf(buffer, "HAT={Jii={%g,%g,%g};};", totalMoi.e11,totalMoi.e22,totalMoi.e33);
        AnyFolder::Tokenizer(buffer, tokens);
        anySeg->ParseTokens(tokens, index);
        
        tokens.clear();
        index = 0;
        sprintf(buffer, "HAT={r0={%g,%g,%g};};", cm.x,cm.y,cm.z);
        AnyFolder::Tokenizer(buffer, tokens);
        anySeg->ParseTokens(tokens, index);
        
        // note Jij is Ixy,Ixz,Iyz
        tokens.clear();
        index = 0;
        sprintf(buffer, "HAT={Jij={%g,%g,%g};};", totalMoi.e12,totalMoi.e13,totalMoi.e23);
        AnyFolder::Tokenizer(buffer, tokens);
        anySeg->ParseTokens(tokens, index);
        
        list.push_back(anySeg);
    }
    
    WriteXML("Output.xml", list);
        
    return 0;
}

void PrintExit(const char *format, ...)
{
    va_list ap;
    va_start (ap, format);
    vfprintf (stderr, format, ap);
    va_end (ap);
    fprintf(stderr, "\n");
    exit(1);
}

// removes comments (C and C++ style) and handles include files
char *Preprocess(const char *filename)
{
    int i;
    DataFile inputFile;
    inputFile.SetExitOnError(true);
    inputFile.ReadFile(filename);
    
    std::string parent(filename);
    unsigned int p = parent.rfind("/");
    if (p != std::string::npos) parent.resize(p + 1);
    else parent.resize(0);
    
    // dirty hack
    // convert filenames into full pathnames
    char *dhp = inputFile.GetRawData();
    std::string newBuffer;
    while (*dhp != 0)
    {
        if (strncmp(dhp, "FileName", 8) == 0)
        {
            while (*dhp != '"') { 
                if (*dhp == ';') goto l1; 
                newBuffer.push_back(*dhp); dhp++; 
            }
            newBuffer.push_back(*dhp); dhp++;
            newBuffer += parent;
            while (*dhp != '"') { 
                if (*dhp == '\\') newBuffer.push_back('/'); 
                else newBuffer.push_back(*dhp); dhp++; 
            }
l1:         newBuffer.push_back(*dhp); dhp++;
        }
        else
        {
            newBuffer.push_back(*dhp); dhp++;
        }
    }
    
    char *buf = (char *)malloc(newBuffer.size() + 1);
    strcpy(buf, newBuffer.c_str());
    inputFile.ClearData();
    newBuffer.clear();
    
    // handle '//'
    
    bool inComment = false;
    char *ptr = buf;
    while (*ptr)
    {
        if (inComment == false)
        {
            if (*ptr == '/' && *(ptr + 1) == '/')
            {
                *ptr = ' ';
                ptr++;
                *ptr = ' ';
                inComment = true;
            }
        }
        else
        {
            if (*ptr == '\n' || *ptr == '\r') inComment = false;
            else *ptr = ' ';
        }
        
        ptr++;
    }
    
    // handle '/*' and */'
    
    inComment = false;
    ptr = buf;
    while (*ptr)
    {
        if (inComment == false)
        {
            if (*ptr == '/' && *(ptr + 1) == '*')
            {
                *ptr = ' ';
                ptr++;
                *ptr = ' ';
                inComment = true;
            }
        }
        else
        {
            if (*ptr == '*' && *(ptr + 1) == '/') 
            {
                *ptr = ' ';
                ptr++;
                *ptr = ' ';
                inComment = false;
            }
            else *ptr = ' ';
        }
        
        ptr++;
    }    
    
    // handle #include
    
    ptr = buf;
    bool newline = true;
    while (*ptr)
    {
        if (*ptr == '\n' || *ptr == '\r')
        {
            newline = true;
            ptr++;
            continue;
        }
        if (newline == false)
        {
            ptr++;
            continue;
        }
        if (*ptr < 33)
        {
            ptr++;
            continue;
        }
        if (*ptr != '#')
        {
            newline = false;
            ptr++;
            continue;
        }
        
        // must have found a starting '#' if get here
        *ptr = ' ';
        ptr++;
        while (*ptr > 0 && *ptr < 33) ptr++;
        if (strncmp(ptr, "include", 7) != 0) PrintExit("%s line %d Parse error: 'include' expected", __FILE__, __LINE__);
        for (i = 0; i < 7; i++) { *ptr = ' '; ptr++; }
        while (*ptr > 0 && *ptr < 33) ptr++;
        if (*ptr != '"') PrintExit("%s line %d Parse error: missing start \"", __FILE__, __LINE__);
        *ptr = 0;
        ptr++;
        char *p1 = ptr;
        while (*ptr > 0 && *ptr != '"') ptr++;
        if (*ptr != '"') PrintExit("%s line %d Parse error: missing end \"", __FILE__, __LINE__);
        *ptr = 0;
        ptr++;
        
        std::string path(p1);
        for (unsigned int j = 0; j < path.size(); j++) if (path[j] == '\\') path[j] = '/';
        std::string fullPath;
        fullPath = parent;
        fullPath.append(path);
        
        char *newBuf = Preprocess(fullPath.c_str());
        char *repBuf = (char *)malloc(strlen(buf) + strlen(newBuf) + strlen(ptr) + 1);
        strcpy(repBuf, buf);
        strcat(repBuf, newBuf);
        strcat(repBuf, ptr);
        ptr = repBuf + strlen(buf) + strlen(newBuf) + 1;
        free(buf);
        free(newBuf);
        buf = repBuf;
    }
    
    return buf;
}

void
WriteXML(std::string filename, std::vector<AnyFolder *> &list)
{
    xmlDocPtr doc;
    xmlNodePtr rootNode;
    xmlNodePtr newNode;
    xmlAttrPtr newAttr;
    double v;
    pgd::Vector vec;
    pgd::Matrix3x3 mat;
    pgd::Quaternion quat;
    char buf1[256];
    char buf2[256];
    unsigned int i, j;
    
    doc = xmlNewDoc((xmlChar *)"1.0");
    if (doc == 0) return;
    
    rootNode = xmlNewDocNode(doc, 0, (xmlChar *)"GAITSYMODE", 0);
    xmlDocSetRootElement(doc, rootNode);
    
    newNode = xmlNewTextChild(rootNode, 0, (xmlChar *)"IOCONTROL", 0);
    newAttr = xmlNewProp(newNode, (xmlChar *)"OldStyleInputs", (xmlChar *)"false");

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

    AnySeg *anySeg;
    for (j = 0; j < list.size(); j++)
    {
        anySeg = dynamic_cast<AnySeg *>(list[j]);
        if (anySeg)
        {
            if (anySeg->GetMass() == 0) continue;
            break;
        }
    }
    newAttr = xmlNewProp(newNode, (xmlChar *)"DistanceTravelledBodyID", (xmlChar *)anySeg->GetPath().c_str());

    // Note coordinate system differences mean that the y and z values are swapped

    // bodies
    std::map<std::string, AnySeg *> bodyList;
    for (j = 0; j < list.size(); j++)
    {
        anySeg = dynamic_cast<AnySeg *>(list[j]);
        if (anySeg)
        {
            std::cerr << anySeg->GetName() << " " << anySeg->GetPath() << "\n";
            if (anySeg->GetMass() == 0) continue;
            bodyList[anySeg->GetPath()] = anySeg;
            
            if (anySeg->GetName() == "SkullSeg")
                std::cerr << anySeg->GetPath() << "\n";
            
            newNode = xmlNewTextChild(rootNode, 0, (xmlChar *)"BODY", 0);
            newAttr = xmlNewProp(newNode, (xmlChar *)"ID", (xmlChar *)anySeg->GetPath().c_str());
            
            v = anySeg->GetMass();
            sprintf(buf1, "%.17g", v);
            newAttr = xmlNewProp(newNode, (xmlChar *)"Mass", (xmlChar *)buf1);
            
            mat = anySeg->GetMoIAroundCM();
            sprintf(buf1, "%.17g %.17g %.17g %.17g %.17g %.17g", mat.e11, mat.e33, mat.e22, mat.e13, mat.e12, mat.e23);
            newAttr = xmlNewProp(newNode, (xmlChar *)"MOI", (xmlChar *)buf1);      
            // note: inertial matrix is as follows
            // [ I11 I12 I13 ]
            // [ I12 I22 I23 ]
            // [ I13 I23 I33 ]
            // MOI="I11 I22 I33 I12 I13 I23"

            vec = anySeg->GetCMPosition();
            sprintf(buf1, "World %.17g %.17g %.17g", vec.x, -vec.z, vec.y);
            newAttr = xmlNewProp(newNode, (xmlChar *)"Position", (xmlChar *)buf1);      
            
            quat = MakeQfromM(anySeg->GetGlobalRotationMatrix());
            sprintf(buf1, "World %.17g %.17g %.17g %.17g", quat.n, quat.v.x, -quat.v.z, quat.v.y);
            newAttr = xmlNewProp(newNode, (xmlChar *)"Quaternion", (xmlChar *)buf1);
            // Quaternion="qs qx qy qz"
            
            // currently we set velocities to zero
            sprintf(buf1, "%.17g %.17g %.17g", 0.0, 0.0, 0.0);
            newAttr = xmlNewProp(newNode, (xmlChar *)"LinearVelocity", (xmlChar *)buf1);
            sprintf(buf1, "%.17g %.17g %.17g", 0.0, 0.0, 0.0);
            newAttr = xmlNewProp(newNode, (xmlChar *)"AngularVelocity", (xmlChar *)buf1);   
            
            // now handle the graphic file
            AnyDrawSurf *anyDrawSurf = anySeg->GetAnyDrawSurf();
            if (anyDrawSurf == 0)
            {
                std::string surfFilename;
                surfFilename = anySeg->GetName() + ".obj";
                newAttr = xmlNewProp(newNode, (xmlChar *)"GraphicFile", (xmlChar *)surfFilename.c_str());
                newAttr = xmlNewProp(newNode, (xmlChar *)"Scale", (xmlChar *)"1.0");
                newAttr = xmlNewProp(newNode, (xmlChar *)"Offset", (xmlChar *)"0 0 0");
            }
            else
            {
                std::string surfFilename;
                anyDrawSurf->GetProperty("FileName", &surfFilename);
                FacetedObject *facetedObject = new FacetedObject();
                facetedObject->ParseANYSURFFile(surfFilename.c_str());
                AnyFunTransform3D *anyFunTransform3D = anyDrawSurf->GetAnyFunTransform3D();
                if (anyFunTransform3D) facetedObject->ApplyAnyFunction(anyFunTransform3D);
                pgd::Vector vector;
                anyDrawSurf->GetProperty("ScaleXYZ", &vector);
                facetedObject->Scale(vector.x, vector.y, vector.z);
                if (dynamic_cast<AnyRefNode *>(anyDrawSurf->GetParent()))
                {
                    anyDrawSurf->GetParent()->GetProperty("ARel", &mat);
                    facetedObject->ApplyMatrix3x3(&mat);
                    anyDrawSurf->GetParent()->GetProperty("sRel", &vector);
                    facetedObject->Move(vector.x, vector.y, vector.z);
                }
                anySeg->GetProperty("sCoM", &vector);
                facetedObject->Move(-vector.x, -vector.y, -vector.z);
                facetedObject->SwapAxes(1, 2);
                facetedObject->Mirror(false, true, false);
                surfFilename = anySeg->GetPath() + ".obj";
                facetedObject->WriteOBJFile(surfFilename.c_str());
                newAttr = xmlNewProp(newNode, (xmlChar *)"GraphicFile", (xmlChar *)surfFilename.c_str());
                newAttr = xmlNewProp(newNode, (xmlChar *)"Scale", (xmlChar *)"1.0");
                newAttr = xmlNewProp(newNode, (xmlChar *)"Offset", (xmlChar *)"0 0 0");
            }
            
        }
    }
        
    // joints
    for (j = 0; j < list.size(); j++)
    {
        AnyJoint *anyJoint = dynamic_cast<AnyJoint *>(list[j]);
        if (anyJoint)
        {
            if (bodyList.find(anyJoint->GetBody1ID()) == bodyList.end() || bodyList.find(anyJoint->GetBody2ID()) == bodyList.end())
                continue;
            
            newNode = xmlNewTextChild(rootNode, 0, (xmlChar *)"JOINT", 0);
            newAttr = xmlNewProp(newNode, (xmlChar *)"ID", (xmlChar *)anyJoint->GetPath().c_str());
            
            newAttr = xmlNewProp(newNode, (xmlChar *)"Type", (xmlChar *)"Hinge");
            
            newAttr = xmlNewProp(newNode, (xmlChar *)"Body1ID", (xmlChar *)anyJoint->GetBody1ID().c_str());
            newAttr = xmlNewProp(newNode, (xmlChar *)"Body2ID", (xmlChar *)anyJoint->GetBody2ID().c_str());
            
            vec = anyJoint->GetHingeAnchor();
            sprintf(buf1, "%s %.17g %.17g %.17g", "World", vec.x, -vec.z, vec.y);
            newAttr = xmlNewProp(newNode, (xmlChar *)"HingeAnchor", (xmlChar *)buf1);
            
            vec = anyJoint->GetHingeAxis();
            sprintf(buf1, "%s %.17g %.17g %.17g", "World", vec.x, -vec.z, vec.y);
            newAttr = xmlNewProp(newNode, (xmlChar *)"HingeAxis", (xmlChar *)buf1);
            
            newAttr = xmlNewProp(newNode, (xmlChar *)"ParamLoStop", (xmlChar *)"-100");
            newAttr = xmlNewProp(newNode, (xmlChar *)"ParamHiStop", (xmlChar *)"+100");
            newAttr = xmlNewProp(newNode, (xmlChar *)"StartAngleReference", (xmlChar *)"0");
            
        }
    }
    
    // muscles
    for (j = 0; j < list.size(); j++)
    {
        AnyMuscle *anyMuscle = dynamic_cast<AnyMuscle *>(list[j]);
        if (anyMuscle)
        {
            unsigned int numNodes = anyMuscle->GetNumNodes();
            if (numNodes < 2) continue;
            bool fail = false;
            for (i = 0; i < numNodes; i++)
            {
                if (bodyList.find(anyMuscle->GetNodeID(i)) == bodyList.end()) 
                {
                    fail = true;
                    break;
                }
            }
            if (fail) continue;
            
            newNode = xmlNewTextChild(rootNode, 0, (xmlChar *)"MUSCLE", 0);
            newAttr = xmlNewProp(newNode, (xmlChar *)"ID", (xmlChar *)anyMuscle->GetPath().c_str());
            
            newAttr = xmlNewProp(newNode, (xmlChar *)"Type", (xmlChar *)"MinettiAlexanderExtended");
            
            newAttr = xmlNewProp(newNode, (xmlChar *)"OriginBodyID", (xmlChar *)anyMuscle->GetOriginID().c_str());
            vec = anyMuscle->GetOriginGlobalPosition();
            sprintf(buf1, "%s %.17g %.17g %.17g", "World", vec.x, -vec.z, vec.y);
            newAttr = xmlNewProp(newNode, (xmlChar *)"Origin", (xmlChar *)buf1);
            
            newAttr = xmlNewProp(newNode, (xmlChar *)"InsertionBodyID", (xmlChar *)anyMuscle->GetInsertionID().c_str());
            vec = anyMuscle->GetInsertionGlobalPosition();
            sprintf(buf1, "%s %.17g %.17g %.17g", "World", vec.x, -vec.z, vec.y);
            newAttr = xmlNewProp(newNode, (xmlChar *)"Insertion", (xmlChar *)buf1);
            
            AnyMuscleModel *anyMuscleModel = anyMuscle->GetAnyMuscleModel();
            double forcePerUnitArea = 300000;
            sprintf(buf1, "%.17g", forcePerUnitArea);
            newAttr = xmlNewProp(newNode, (xmlChar *)"ForcePerUnitArea", (xmlChar *)buf1);
            anyMuscleModel->GetProperty("F0", &v);
            sprintf(buf1, "%.17g", v / forcePerUnitArea);
            newAttr = xmlNewProp(newNode, (xmlChar *)"PCA", (xmlChar *)buf1);
            anyMuscleModel->GetProperty("Lfbar", &v);
            sprintf(buf1, "%.17g", v);
            newAttr = xmlNewProp(newNode, (xmlChar *)"FibreLength", (xmlChar *)buf1); // this is ignoring pennation
            anyMuscleModel->GetProperty("Lt0", &v);
            sprintf(buf1, "%.17g", v);
            newAttr = xmlNewProp(newNode, (xmlChar *)"TendonLength", (xmlChar *)buf1);
            anyMuscleModel->GetProperty("K1", &v);
            sprintf(buf1, "%.17g", v);
            newAttr = xmlNewProp(newNode, (xmlChar *)"VMaxFactor", (xmlChar *)buf1);
            anyMuscleModel->GetProperty("Epsilonbar", &v);
            sprintf(buf1, "%.17g", v);
            newAttr = xmlNewProp(newNode, (xmlChar *)"SerialStrainAtFmax", (xmlChar *)buf1);

            // these values are not really available
            newAttr = xmlNewProp(newNode, (xmlChar *)"ParallelStrainAtFmax", (xmlChar *)"0.5");
            newAttr = xmlNewProp(newNode, (xmlChar *)"ActivationK", (xmlChar *)"0.17");
            newAttr = xmlNewProp(newNode, (xmlChar *)"ActivationKinetics", (xmlChar *)"false");
            
            std::vector<AnySurfCylinder *> anySurfCylinderList;
            anyMuscle->GetAnySurfCylinderList(&anySurfCylinderList);
            if (anySurfCylinderList.size() == 1)
            {
                newAttr = xmlNewProp(newNode, (xmlChar *)"Strap", (xmlChar *)"CylinderWrap");
                AnySurfCylinder *anySurfCylinder = anySurfCylinderList[0];
                vec = anySurfCylinder->GetParent()->GetGlobalPosition();
                sprintf(buf1, "%s %.17g %.17g %.17g", "World", vec.x, -vec.z, vec.y);
                newAttr = xmlNewProp(newNode, (xmlChar *)"CylinderPosition", (xmlChar *)buf1);
                mat = anySurfCylinder->GetParent()->GetGlobalRotationMatrix();
                pgd::Vector zVec = mat * pgd::Vector(0, 0, 1); zVec.Normalize();
                pgd::Vector axis = zVec ^ pgd::Vector(0, 0, 1); axis.Normalize();
                double angle = acos(zVec * pgd::Vector(0, 0, 1));
                // quat = pgd::MakeQFromAxis(axis.x, axis.y, axis.z, angle); // need to get the right sense for my implementation
                quat = pgd::MakeQFromAxis(axis.x, axis.y, axis.z, angle + M_PI);
                sprintf(buf1, "World %.17g %.17g %.17g %.17g", quat.n, quat.v.x, -quat.v.z, quat.v.y); 
                newAttr = xmlNewProp(newNode, (xmlChar *)"CylinderQuaternion", (xmlChar *)buf1);
                anySurfCylinder->GetProperty("Radius", &v);
                sprintf(buf1, "%.17g", v);
                newAttr = xmlNewProp(newNode, (xmlChar *)"CylinderRadius", (xmlChar *)buf1);
                anySeg = dynamic_cast<AnyRefNode *>(anySurfCylinder->GetParent())->GetParentSeg();
                newAttr = xmlNewProp(newNode, (xmlChar *)"CylinderBodyID", (xmlChar *)anySeg->GetPath().c_str());
            }
            else if (numNodes == 2)
            {
                newAttr = xmlNewProp(newNode, (xmlChar *)"Strap", (xmlChar *)"TwoPoint");
            }
            else if (numNodes == 3)
            {
                newAttr = xmlNewProp(newNode, (xmlChar *)"Strap", (xmlChar *)"ThreePoint");
                newAttr = xmlNewProp(newNode, (xmlChar *)"MidpointBodyID", (xmlChar *)anyMuscle->GetNodeID(1).c_str());
                std::cerr << anyMuscle->GetNodeID(1) << "\n";
                vec = anyMuscle->GetOriginGlobalPosition(1);
                sprintf(buf1, "%s %.17g %.17g %.17g", "World", vec.x, -vec.z, vec.y);
                newAttr = xmlNewProp(newNode, (xmlChar *)"Midpoint", (xmlChar *)buf1);
            }
            else if (numNodes > 3)
            {
                newAttr = xmlNewProp(newNode, (xmlChar *)"Strap", (xmlChar *)"NPoint");
                for (i = 1; i < numNodes - 1; i++)
                {
                    sprintf(buf1, "ViaPointBody%d", i - 1);
                    newAttr = xmlNewProp(newNode, (xmlChar *)buf1, (xmlChar *)anyMuscle->GetNodeID(i).c_str());
                    vec = anyMuscle->GetOriginGlobalPosition(i);
                    sprintf(buf1, "%s %.17g %.17g %.17g", "World", vec.x, -vec.z, vec.y);
                    sprintf(buf2, "ViaPoint%d", i - 1);
                    newAttr = xmlNewProp(newNode, (xmlChar *)buf2, (xmlChar *)buf1);
                }
            }
        }
    }
    
    xmlThrDefIndentTreeOutput(1);
    xmlSaveFormatFile(filename.c_str(), doc, 1);
    
    xmlFreeDoc(doc);
}

// x, y, z is offset
// note matrix uses Mathematica convention
// {{Ixx, Ixy, Izx}, 
//  {Ixy, Iyy, Iyz}, 
//  {Izx, Iyz, Izz}} 
void ParallelAxis(double x, double y, double z, double mass,
                  pgd::Matrix3x3 *moi)
{
    double ixx = moi->e11;
    double ixy = moi->e12;
    double izx = moi->e13;
    //double ixy = moi->e21;
    double iyy = moi->e22;
    double iyz = moi->e23;
    //double izx = moi->e31;
    //double iyz = moi->e32;
    double izz = moi->e33;
    
    double ixxp = ixx - mass*(-(y*y) - (z*z));
    double iyyp = iyy - mass*(-(x*x) - (z*z));
    double izzp = izz - mass*(-(x*x) - (y*y));
    double ixyp = ixy - mass*x*y;
    double iyzp = iyz - mass*y*z;
    double izxp = izx - mass*x*z;
    
    moi->e11 = ixxp;
    moi->e12 = ixyp;
    moi->e13 = izxp;
    moi->e21 = ixyp;
    moi->e22 = iyyp;
    moi->e23 = iyzp;
    moi->e31 = izxp;
    moi->e32 = iyzp;
    moi->e33 = izzp;
}

// a utility function to calculate moments of interia given an arbitrary translation and rotation
void ParallelAxis(double x, double y, double z, // transformation from centre of mass to new location (m)
                  double mass, // mass (kg)
                  double ixx, double iyy, double izz, double ixy, double iyz, double izx, // moments of inertia kgm2
                  double ang, // rotation angle (radians)
                  double ax, double ay, double az, // axis of rotation - must be unit length
                  double *ixxp, double *iyyp, double *izzp, double *ixyp, double *iyzp, double *izxp) // transformed moments of inertia about new coordinate system
{
    double cosang = cos(ang);
    double sinang = sin(ang);
    
    *ixxp = -(mass*(-(y*y) - (z*z))) + ((ax*ax)*(1 - cosang) + cosang)*
    (ixx*((ax*ax)*(1 - cosang) + cosang) + izx*(ax*az*(1 - cosang) + ay*sinang) + 
     ixy*(ax*ay*(1 - cosang) - az*sinang)) + (ax*ay*(1 - cosang) - az*sinang)*
    (ixy*((ax*ax)*(1 - cosang) + cosang) + iyz*(ax*az*(1 - cosang) + ay*sinang) + 
     iyy*(ax*ay*(1 - cosang) - az*sinang)) + (ax*az*(1 - cosang) + ay*sinang)*
    (izx*((ax*ax)*(1 - cosang) + cosang) + izz*(ax*az*(1 - cosang) + ay*sinang) + 
     iyz*(ax*ay*(1 - cosang) - az*sinang));
    
    *iyyp = -(mass*(-(x*x) - (z*z))) + (ax*ay*(1 - cosang) + az*sinang)*
    (ixy*((ay*ay)*(1 - cosang) + cosang) + izx*(ay*az*(1 - cosang) - ax*sinang) + 
     ixx*(ax*ay*(1 - cosang) + az*sinang)) + ((ay*ay)*(1 - cosang) + cosang)*
    (iyy*((ay*ay)*(1 - cosang) + cosang) + iyz*(ay*az*(1 - cosang) - ax*sinang) + 
     ixy*(ax*ay*(1 - cosang) + az*sinang)) + (ay*az*(1 - cosang) - ax*sinang)*
    (iyz*((ay*ay)*(1 - cosang) + cosang) + izz*(ay*az*(1 - cosang) - ax*sinang) + 
     izx*(ax*ay*(1 - cosang) + az*sinang));
    
    *izzp = -(mass*(-(x*x) - (y*y))) + (ax*az*(1 - cosang) - ay*sinang)*
    (izx*((az*az)*(1 - cosang) + cosang) + ixy*(ay*az*(1 - cosang) + ax*sinang) + 
     ixx*(ax*az*(1 - cosang) - ay*sinang)) + (ay*az*(1 - cosang) + ax*sinang)*
    (iyz*((az*az)*(1 - cosang) + cosang) + iyy*(ay*az*(1 - cosang) + ax*sinang) + 
     ixy*(ax*az*(1 - cosang) - ay*sinang)) + ((az*az)*(1 - cosang) + cosang)*
    (izz*((az*az)*(1 - cosang) + cosang) + iyz*(ay*az*(1 - cosang) + ax*sinang) + 
     izx*(ax*az*(1 - cosang) - ay*sinang));
    
    *ixyp = -(mass*x*y) + (ax*ay*(1 - cosang) + az*sinang)*
    (ixx*((ax*ax)*(1 - cosang) + cosang) + izx*(ax*az*(1 - cosang) + ay*sinang) + 
     ixy*(ax*ay*(1 - cosang) - az*sinang)) + ((ay*ay)*(1 - cosang) + cosang)*
    (ixy*((ax*ax)*(1 - cosang) + cosang) + iyz*(ax*az*(1 - cosang) + ay*sinang) + 
     iyy*(ax*ay*(1 - cosang) - az*sinang)) + (ay*az*(1 - cosang) - ax*sinang)*
    (izx*((ax*ax)*(1 - cosang) + cosang) + izz*(ax*az*(1 - cosang) + ay*sinang) + 
     iyz*(ax*ay*(1 - cosang) - az*sinang));
    
    *iyzp = -(mass*y*z) + (ax*az*(1 - cosang) - ay*sinang)*
    (ixy*((ay*ay)*(1 - cosang) + cosang) + izx*(ay*az*(1 - cosang) - ax*sinang) + 
     ixx*(ax*ay*(1 - cosang) + az*sinang)) + (ay*az*(1 - cosang) + ax*sinang)*
    (iyy*((ay*ay)*(1 - cosang) + cosang) + iyz*(ay*az*(1 - cosang) - ax*sinang) + 
     ixy*(ax*ay*(1 - cosang) + az*sinang)) + ((az*az)*(1 - cosang) + cosang)*
    (iyz*((ay*ay)*(1 - cosang) + cosang) + izz*(ay*az*(1 - cosang) - ax*sinang) + 
     izx*(ax*ay*(1 - cosang) + az*sinang));
    
    *izxp = -(mass*x*z) + (ax*az*(1 - cosang) - ay*sinang)*
    (ixx*((ax*ax)*(1 - cosang) + cosang) + izx*(ax*az*(1 - cosang) + ay*sinang) + 
     ixy*(ax*ay*(1 - cosang) - az*sinang)) + (ay*az*(1 - cosang) + ax*sinang)*
    (ixy*((ax*ax)*(1 - cosang) + cosang) + iyz*(ax*az*(1 - cosang) + ay*sinang) + 
     iyy*(ax*ay*(1 - cosang) - az*sinang)) + ((az*az)*(1 - cosang) + cosang)*
    (izx*((ax*ax)*(1 - cosang) + cosang) + izz*(ax*az*(1 - cosang) + ay*sinang) + 
     iyz*(ax*ay*(1 - cosang) - az*sinang));
}

