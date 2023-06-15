/*
 *  WarehouseProcessor.cpp
 *  GaitSymODE
 *
 *  Created by Bill Sellers on 11/05/2014.
 *  Copyright 2014 Bill Sellers. All rights reserved.
 *
 */

#include <mpi.h>
#include <stdio.h>
#include <limits.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <alloca.h>
#include <unistd.h>
#include <time.h>
#include <vector>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <string>
#include <libxml/xmlmemory.h>
#include <libxml/parser.h>

#include "../GeneralGA/src/DataFile.h"
#include "../GeneralGA/src/Random.h"
#include "../GaitSymODE/src/MPIStuff.h"
#include "../GaitSymODE/src/XMLConverter.h"
#include "../GaitSymODE/src/Warehouse.h"

extern int gMPI_Comm_size;
extern int gMPI_Comm_rank;

int GAMain(int argc, char *argv[]);
int GaitSymMain(int argc, char *argv[]);
xmlDocPtr MergeDriversAndWarehouse(const char *modelDefinitionFile, const char *driverFile, const char *warehouseFile);
xmlDocPtr MergeDrivers(const char *modelDefinitionFile, const char *driverFile);
xmlAttr *DoXmlReplaceProp(xmlNode *cur, const xmlChar *name, const xmlChar *newValue);
xmlDocPtr AddExtraWarehouseUnit(const std::string &inputConfigFile, const std::string &extraWarehouse, bool alwaysCreateNewWarehouseUnitFlag);
void ChangeAttribute(xmlDocPtr doc, const char *tag, const char *attribute, const char *newValue, bool firstMatch);
void ApplyGenome(const char *inputXML, const char *inputGenome, const char *outputXML, double *fitness);
void CreateOutputFolder(int count, std::string &newFolderName);
void CopyFile(const char *src, const char *dest);
void CheckCPUIOUsage();
int FindBest(std::string folder, std::string &bestPopulationFile, std::string &bestGenomeFile);
bool Replace(std::string& str, const std::string& from, const std::string& to);
int ReadLines(const char *filename, std::vector<std::string> &lines, bool ignoreEmptyLinesFlag);
int WriteLines(const char *filename, std::vector<std::string> &lines);


#define NUM_ELEMENTS(arrayname) (sizeof(arrayname) / sizeof(*arrayname))

// returns zero on no erro, or the line number of an error
int WarehouseProcessor(int argc, char *argv[])
{
    // read the config file
    const char *configFileName = "config.txt";
    DataFile file;
    file.SetExitOnError(true);
    file.ReadFile(configFileName);

    char modelDefinitionFile[512];
    char muscleDriverFile[512];
    char gaParameterFile[512];
    char modelPopulationFile[512];
    double outputModelStateAtWarehouseDistance;
    double newWarehouseSimulationTimeLimit;
    double newWarehouseSimulationCreationTimeLimit;
    double newWarehouseSimulationCreationWarehouseFailDistanceAbort;
    bool alwaysCreateNewWarehouseUnitFlag;

    file.RetrieveParameter("modelDefinitionFile", modelDefinitionFile, sizeof(modelDefinitionFile) - 1, true);
    file.RetrieveParameter("muscleDriverFile", muscleDriverFile, sizeof(muscleDriverFile) - 1, true);
    file.RetrieveParameter("gaParameterFile", gaParameterFile, sizeof(gaParameterFile) - 1, true);
    file.RetrieveParameter("modelPopulationFile", modelPopulationFile, sizeof(modelPopulationFile) - 1, true);
    file.RetrieveParameter("outputModelStateAtWarehouseDistance", &outputModelStateAtWarehouseDistance, true);
    file.RetrieveParameter("newWarehouseSimulationTimeLimit", &newWarehouseSimulationTimeLimit, true);
    file.RetrieveParameter("newWarehouseSimulationCreationTimeLimit", &newWarehouseSimulationCreationTimeLimit, true);
    file.RetrieveParameter("newWarehouseSimulationCreationWarehouseFailDistanceAbort", &newWarehouseSimulationCreationWarehouseFailDistanceAbort, true);
    file.RetrieveParameter("alwaysCreateNewWarehouseUnitFlag", &alwaysCreateNewWarehouseUnitFlag, true);

    char buffer[10000];
    char *pArgs[256];
    int nArgs, mpi_big_buffer_size;
    char *mpi_big_buffer;
    std::string initialModelDefinitionFile = "01_initialModelDefinitionFile.xml";
    std::string warehouseDistanceModelStateFile = "02_warehouseDistanceModelStateFile.xml";
    std::string postMergeFile = "03_postMergeFile.xml";
    std::string extraWarehouseFile = "04_extraWarehouseFile.txt";
    std::string newModelDefinitionFile = "05_newModelDefinitionFile.xml";
    struct stat fileStat;
    std::string outputFolder, workingModelDefinitionFile, workingWarehouseFile, workingWarehouseDistanceModelStateFile;
    std::string workingModelDefinitionPostMergeFile, workingExtraWarehouseFile;
    std::string bestPopulationFile, bestGenomeFile, bestGenomeFileXML;
    std::string workingNewModelDefinitionFile;

    std::string startModelDefinitionFile = modelDefinitionFile;
    int loopCount = 0;
    while (true)
    {
        std::cerr << "Create the output folder\n";
        CreateOutputFolder(loopCount, outputFolder);
        std::cerr << "Set up the standard file names\n";
        workingModelDefinitionFile = outputFolder + "/" + initialModelDefinitionFile;
        workingWarehouseDistanceModelStateFile = outputFolder + "/" + warehouseDistanceModelStateFile;
        workingModelDefinitionPostMergeFile = outputFolder + "/" + postMergeFile;
        workingExtraWarehouseFile = outputFolder + "/" + extraWarehouseFile;
        workingNewModelDefinitionFile = outputFolder + "/" + newModelDefinitionFile;

        std::cerr << "Copy the starting files to the required locations\n";
        CopyFile(startModelDefinitionFile.c_str(), workingModelDefinitionFile.c_str());

        std::cerr << "Run the initial warehouse until it fails\n";
        sprintf(buffer, "gaitsym --config \"%s\" --outputModelStateFile \"%s\" --outputModelStateAtWarehouseDistance %g",
                workingModelDefinitionFile.c_str(), workingWarehouseDistanceModelStateFile.c_str(), outputModelStateAtWarehouseDistance);
        std::cerr << "Executing command:\n" << buffer << "\n";
        nArgs = DataFile::ReturnTokens(buffer, pArgs, NUM_ELEMENTS(pArgs));
        GaitSymMain(nArgs, pArgs);
        std::cerr << "GaitSymMain finished\n";

        std::cerr << "Check that we have a new " << workingWarehouseDistanceModelStateFile << " file\n";
        if (stat(workingWarehouseDistanceModelStateFile.c_str(), &fileStat))
        {
            std::cerr << "Successful warehouse created.\nExiting\n";
            return(0);
        }

        std::cerr << "Merge " << muscleDriverFile << " into " << workingWarehouseDistanceModelStateFile << "\n";
        //xmlDocPtr newDoc = MergeDriversAndWarehouse(workingWarehouseDistanceModelStateFile.c_str(), muscleDriverFile, workingWarehouseFile.c_str());
        xmlDocPtr newDoc = MergeDrivers(workingWarehouseDistanceModelStateFile.c_str(), muscleDriverFile);
        std::cerr << "Change GLOBAL values for ClosestWarehouse\n";
        ChangeAttribute(newDoc, "GLOBAL", "FitnessType", "ClosestWarehouse", true);
        sprintf(buffer, "%g", newWarehouseSimulationTimeLimit);
        ChangeAttribute(newDoc, "GLOBAL", "TimeLimit", buffer, true);
        std::cerr << "Output the new XML file " << workingModelDefinitionPostMergeFile << "\n";
        xmlSaveFormatFile(workingModelDefinitionPostMergeFile.c_str(), newDoc, 1);
        xmlFreeDoc(newDoc);

        std::cerr << "Set up the optimisation to find the activities required to get back on course\n";

        std::cerr << "Reading workingModelDefinitionFile: " << workingModelDefinitionPostMergeFile << "\n";
        file.ReadFile(workingModelDefinitionPostMergeFile.c_str());
        std::cerr << "Force a reload of the workingModelDefinitionFile: " << workingModelDefinitionPostMergeFile << "\n";
        mpi_big_buffer_size = file.GetSize() + 1 + 2 * sizeof(int);
        mpi_big_buffer = new char[mpi_big_buffer_size];
        *((int *)mpi_big_buffer) = MPI_MESSAGE_ID_RELOAD_MODELCONFIG;
        *((int *)(mpi_big_buffer + sizeof(int))) = file.GetSize();
        strcpy(mpi_big_buffer + 2 * sizeof(int), file.GetRawData());
        for (int j = 1; j < gMPI_Comm_size; j++) MPI_Send(mpi_big_buffer, mpi_big_buffer_size, MPI_BYTE, j, 0, MPI_COMM_WORLD);
        delete [] mpi_big_buffer;

        std::cerr << "run the GA command\n";
        sprintf(buffer, "ga -p %s -o %s -s %s", gaParameterFile, outputFolder.c_str(), modelPopulationFile);
        std::cerr << "Executing command:\n";
        std::cerr << buffer << "\n";
        nArgs = DataFile::ReturnTokens(buffer, pArgs, NUM_ELEMENTS(pArgs));
        GAMain(nArgs, pArgs);
        std::cerr << "GAMain finished\n";

        CheckCPUIOUsage();

        std::cerr << "create a new warehouse file using the best genome\n";
        if (FindBest(outputFolder, bestPopulationFile, bestGenomeFile)) return __LINE__;
        std::cerr << "create the XML file from the best genome\n";
        bestGenomeFileXML = bestGenomeFile;
        Replace(bestGenomeFileXML, ".txt", ".xml");
        std::cerr << "Apply genome " <<  bestGenomeFile << " to " << workingModelDefinitionPostMergeFile << " and create " << bestGenomeFileXML << "\n";
        double fitness;
        ApplyGenome(workingModelDefinitionPostMergeFile.c_str(), bestGenomeFile.c_str(), bestGenomeFileXML.c_str(), &fitness);
#ifdef TOTAL_DISTANCE_WAREHOUSE_METRIC // this version calculates a total distance from warehouse metric
        sprintf(buffer, "gaitsym --config \"%s\" --outputWarehouse \"%s\" --simulationTimeLimit %g", bestGenomeFileXML.c_str(), workingExtraWarehouseFile.c_str(), newWarehouseSimulationCreationTimeLimit);
#else // and this is a minimum distance metric
        if (newWarehouseSimulationCreationTimeLimit > 0)
            sprintf(buffer, "gaitsym --config \"%s\" --outputWarehouse \"%s\" --simulationTimeLimit %g", bestGenomeFileXML.c_str(), workingExtraWarehouseFile.c_str(), newWarehouseSimulationCreationTimeLimit);
        else
            // this should work but the value of fitness is often not reached - rounding error maybe?
            // sprintf(buffer, "gaitsym --config \"%s\" --outputWarehouse \"%s\" --warehouseFailDistanceAbort %g", bestGenomeFileXML.c_str(), workingExtraWarehouseFile.c_str(), fitness * 1.000001);
            // so replace with a manually optimised value
            sprintf(buffer, "gaitsym --config \"%s\" --outputWarehouse \"%s\" --warehouseFailDistanceAbort %g", bestGenomeFileXML.c_str(), workingExtraWarehouseFile.c_str(), newWarehouseSimulationCreationWarehouseFailDistanceAbort);
#endif
        std::cerr << "Executing command:\n" << buffer << "\n";
        nArgs = DataFile::ReturnTokens(buffer, pArgs, NUM_ELEMENTS(pArgs));
        GaitSymMain(nArgs, pArgs);
        std::cerr << "GaitSymMain finished\n";

        std::cerr << "add the new warehouse to the old one\n";
        newDoc = AddExtraWarehouseUnit(workingModelDefinitionFile, workingExtraWarehouseFile, alwaysCreateNewWarehouseUnitFlag);
        xmlSaveFormatFile(workingNewModelDefinitionFile.c_str(), newDoc, 1);
        xmlFreeDoc(newDoc);

        std::cerr << "and set things up for the next loop with the new warehouse file: " << workingNewModelDefinitionFile << "\n";
        startModelDefinitionFile = workingNewModelDefinitionFile;
        loopCount++;
    }

    return 0;
}

// create the output folder based on the current time
void CreateOutputFolder(int count, std::string &newFolderName)
{
    int err;
    time_t theTime;
    struct tm *theLocalTime;
    const int nAttempts = 10;
    char folderName[128];
    for (int i = 0; i < nAttempts; i++)
    {
        theTime = time(0);
        theLocalTime = localtime(&theTime);
        sprintf(folderName, "%04d_%04d-%02d-%02d_%02d.%02d.%02d",
                        count,
                        theLocalTime->tm_year + 1900,
                        theLocalTime->tm_mon + 1,
                        theLocalTime->tm_mday,
                        theLocalTime->tm_hour, theLocalTime->tm_min,
                        theLocalTime->tm_sec);
        std::cerr << "Creating folder " << folderName << "\n";
        err = mkdir(folderName, 0777);
        if (err != 0) sleep(2);
        else break;
    }
    if (err != 0) newFolderName = "";
    else newFolderName  = folderName;
}

// Copy a file
void CopyFile(const char *src, const char *dest)
{
    std::cerr << "copy " << src << " to " << dest << "\n";
    DataFile work;
    work.SetExitOnError(true);
    work.ReadFile(src);
    work.WriteFile(dest);
}

// Add the newly generated warehouse as a new Unit in the existing warehouse
xmlDocPtr AddExtraWarehouseUnit(const std::string &inputConfigFile, const std::string &extraWarehouse, bool alwaysCreateNewWarehouseUnitFlag)
{
    xmlDocPtr modelDefinitionDoc = 0;
    xmlNodePtr cur, modelDefinitionCurrentElement, newTextNode;
    int err;

    std::cerr << "Create the new XML document\n";
    xmlNodePtr newNode;
    xmlAttrPtr newAttr;
    xmlDocPtr newDoc = xmlNewDoc((xmlChar *)"1.0");
    xmlNodePtr newRootNode = xmlNewDocNode(newDoc, 0, (xmlChar *)"GAITSYMODE", 0);
    xmlDocSetRootElement(newDoc, newRootNode);

    std::cerr << "Read the inputConfigFile " << inputConfigFile << "\n";
    DataFile modelDefinitionDataFile;
    modelDefinitionDataFile.SetExitOnError(true);
    modelDefinitionDataFile.ReadFile(inputConfigFile.c_str());
    char *modelDefinitionXMLDataBuffer = modelDefinitionDataFile.GetRawData();

    std::cerr << "Do the basic parsing of " << inputConfigFile << "\n";
    modelDefinitionDoc = xmlReadMemory(modelDefinitionXMLDataBuffer, modelDefinitionDataFile.GetSize(), 0, 0, XML_PARSE_HUGE);
    if (modelDefinitionDoc == 0) { std::cerr << "Error parsing " << inputConfigFile << "\n"; goto EXIT_LABEL; }
    modelDefinitionCurrentElement = xmlDocGetRootElement(modelDefinitionDoc);
    if (modelDefinitionCurrentElement == 0) { std::cerr << inputConfigFile << ": Document empty\n"; goto EXIT_LABEL; }
    if (xmlStrcmp(modelDefinitionCurrentElement->name, (const xmlChar *) "GAITSYMODE")) {
        std::cerr << inputConfigFile << ": Document of the wrong type, root node != GAITSYMODE\n"; goto EXIT_LABEL; }

    cur = modelDefinitionCurrentElement->xmlChildrenNode;
    while (cur != 0)
    {
        if (xmlStrcmp(cur->name, (const xmlChar *)"WAREHOUSE") == 0) // if it is a warehouse append the new material
        {
            Warehouse warehouse;
            warehouse.XMLLoad(cur);
            if (warehouse.GetNumWarehouseUnits() == 1 || alwaysCreateNewWarehouseUnitFlag == true)
            {
                std::cerr << "Create a new WarehouseUnit from " << extraWarehouse << "\n";
                WarehouseUnit *warehouseUnit = warehouse.NewWarehouseUnit(warehouse.GetNumWarehouseUnits());
                err = warehouseUnit->ImportWarehouseUnit(extraWarehouse.c_str(), false);
                int numBodies, numValuesPerBody;
                double *unit0Weights = warehouse.GetWarehouseUnit(0)->GetWeights(&numBodies, &numValuesPerBody);
                std::cerr << "numBodies = " << numBodies << " numValuesPerBody = " << numValuesPerBody << "\n";
                double *unit1Weights = warehouse.GetWarehouseUnit(1)->GetWeights(&numBodies, &numValuesPerBody);
                std::cerr << "numBodies = " << numBodies << " numValuesPerBody = " << numValuesPerBody << "\n";
                for (int i = 0; i < numBodies * numValuesPerBody; i++) unit1Weights[i] = unit0Weights[i];
            }
            else
            {
                std::cerr << "Append to WarehouseUnit from " << extraWarehouse << "\n";
                WarehouseUnit *warehouseUnit = warehouse.GetWarehouseUnit(1);
                err = warehouseUnit->ImportWarehouseUnit(extraWarehouse.c_str(), true);
            }
            if (err) std::cerr << "Error reading " << extraWarehouse << "\n";

            std::cerr << __LINE__ << "\n";
            newNode = warehouse.XMLSave();
            std::cerr << __LINE__ << "\n";
            xmlAddChild(newRootNode, newNode);
        }
        else // otherwise just copy the node
        {
            newNode = xmlCopyNode(cur, 1);
            xmlAddChild(newRootNode, newNode);
        }
        cur = cur->next;
    }

    if (modelDefinitionDoc) xmlFreeDoc(modelDefinitionDoc);
    return newDoc;

EXIT_LABEL:
    if (modelDefinitionDoc) xmlFreeDoc(modelDefinitionDoc);
    xmlFreeDoc(newDoc);
    return 0;
}

// merge drivers in from a separate XML file
xmlDocPtr MergeDrivers(const char *modelDefinitionFile, const char *driverFile)
{
    xmlDocPtr modelDefinitionDoc = 0, driverDoc = 0;
    xmlNodePtr cur, modelDefinitionCurrentElement, driverCurrentElement, newTextNode;

    std::cerr << "Create the new XML document\n";
    xmlNodePtr newNode;
    xmlAttrPtr newAttr;
    xmlDocPtr newDoc = xmlNewDoc((xmlChar *)"1.0");
    xmlNodePtr newRootNode = xmlNewDocNode(newDoc, 0, (xmlChar *)"GAITSYMODE", 0);
    xmlDocSetRootElement(newDoc, newRootNode);

    std::cerr << "Read the modelDefinitionFile " << modelDefinitionFile << "\n";
    DataFile modelDefinitionDataFile;
    modelDefinitionDataFile.SetExitOnError(true);
    modelDefinitionDataFile.ReadFile(modelDefinitionFile);
    char *modelDefinitionXMLDataBuffer = modelDefinitionDataFile.GetRawData();

    std::cerr << "Read the driverFile " << driverFile << "\n";
    DataFile driverDataFile;
    driverDataFile.SetExitOnError(true);
    driverDataFile.ReadFile(driverFile);
    char *driverXMLDataBuffer = driverDataFile.GetRawData();

    std::cerr << "Do the basic parsing of " << modelDefinitionFile << "\n";
    modelDefinitionDoc = xmlReadMemory(modelDefinitionXMLDataBuffer, modelDefinitionDataFile.GetSize(), 0, 0, XML_PARSE_HUGE);
    if (modelDefinitionDoc == 0) { std::cerr << "Error parsing " << modelDefinitionFile << "\n"; goto EXIT_LABEL; }
    modelDefinitionCurrentElement = xmlDocGetRootElement(modelDefinitionDoc);
    if (modelDefinitionCurrentElement == 0) { std::cerr << modelDefinitionFile << ": Document empty\n"; goto EXIT_LABEL; }
    if (xmlStrcmp(modelDefinitionCurrentElement->name, (const xmlChar *) "GAITSYMODE")) {
        std::cerr << modelDefinitionFile << ": Document of the wrong type, root node != GAITSYMODE\n"; goto EXIT_LABEL; }

    std::cerr << "Do the basic parsing of " << driverFile << "\n";
    driverDoc = xmlReadMemory(driverXMLDataBuffer, driverDataFile.GetSize(), 0, 0, XML_PARSE_HUGE);
    if (driverDoc == 0) { std::cerr << "Error parsing " << driverFile << "\n"; goto EXIT_LABEL; }
    driverCurrentElement = xmlDocGetRootElement(driverDoc);
    if (driverCurrentElement == 0) { std::cerr << driverFile << ": Document empty\n"; goto EXIT_LABEL; }
    if (xmlStrcmp(driverCurrentElement->name, (const xmlChar *) "GAITSYMODE")) {
        std::cerr << driverFile << ": Document of the wrong type, root node != GAITSYMODE\n"; goto EXIT_LABEL; }

    std::cerr << "Parse the elements in " << modelDefinitionFile << "\n";
    cur = modelDefinitionCurrentElement->xmlChildrenNode;
    while (cur != 0)
    {
        if (xmlStrcmp(cur->name, (const xmlChar *)"DRIVER") != 0) // if it isn't a driver, just copy the node
        {
            newNode = xmlCopyNode(cur, 1);
            xmlAddChild(newRootNode, newNode);
        }
        cur = cur->next;
    }

    std::cerr << "Parse the elements in " << driverFile << "\n";
    cur = driverCurrentElement->xmlChildrenNode;
    while (cur != 0)
    {
        if (xmlStrcmp(cur->name, (const xmlChar *)"DRIVER") == 0) // if it is a driver, just copy the node
        {
            newNode = xmlCopyNode(cur, 1);
            xmlAddChild(newRootNode, newNode);
            newTextNode = xmlNewText((const xmlChar *)"\n" ); // this just adds a newline after each driver
            xmlAddChild(newRootNode, newTextNode);
        }
        cur = cur->next;
    }

EXIT_LABEL:
    if (driverDoc) xmlFreeDoc(driverDoc);
    if (modelDefinitionDoc) xmlFreeDoc(modelDefinitionDoc);
    return newDoc;
}

// merge drivers in from a separate XML file
// and warehouse in from the export file
xmlDocPtr MergeDriversAndWarehouse(const char *modelDefinitionFile, const char *driverFile, const char *warehouseFile)
{
    xmlDocPtr modelDefinitionDoc = 0, driverDoc = 0;
    xmlNodePtr cur, modelDefinitionCurrentElement, driverCurrentElement, newTextNode;

    std::cerr << "Create the new XML document\n";
    xmlNodePtr newNode;
    xmlAttrPtr newAttr;
    xmlDocPtr newDoc = xmlNewDoc((xmlChar *)"1.0");
    xmlNodePtr newRootNode = xmlNewDocNode(newDoc, 0, (xmlChar *)"GAITSYMODE", 0);
    xmlDocSetRootElement(newDoc, newRootNode);

    std::cerr << "Read the modelDefinitionFile " << modelDefinitionFile << "\n";
    DataFile modelDefinitionDataFile;
    modelDefinitionDataFile.SetExitOnError(true);
    modelDefinitionDataFile.ReadFile(modelDefinitionFile);
    char *modelDefinitionXMLDataBuffer = modelDefinitionDataFile.GetRawData();

    std::cerr << "Read the driverFile " << driverFile << "\n";
    DataFile driverDataFile;
    driverDataFile.SetExitOnError(true);
    driverDataFile.ReadFile(driverFile);
    char *driverXMLDataBuffer = driverDataFile.GetRawData();

    DataFile warehouse;
    char *ptr;

    std::cerr << "Do the basic parsing of " << modelDefinitionFile << "\n";
    modelDefinitionDoc = xmlReadMemory(modelDefinitionXMLDataBuffer, modelDefinitionDataFile.GetSize(), 0, 0, XML_PARSE_HUGE);
    if (modelDefinitionDoc == 0) { std::cerr << "Error parsing " << modelDefinitionFile << "\n"; goto EXIT_LABEL; }
    modelDefinitionCurrentElement = xmlDocGetRootElement(modelDefinitionDoc);
    if (modelDefinitionCurrentElement == 0) { std::cerr << modelDefinitionFile << ": Document empty\n"; goto EXIT_LABEL; }
    if (xmlStrcmp(modelDefinitionCurrentElement->name, (const xmlChar *) "GAITSYMODE")) {
        std::cerr << modelDefinitionFile << ": Document of the wrong type, root node != GAITSYMODE\n"; goto EXIT_LABEL; }

    std::cerr << "Do the basic parsing of " << driverFile << "\n";
    driverDoc = xmlReadMemory(driverXMLDataBuffer, driverDataFile.GetSize(), 0, 0, XML_PARSE_HUGE);
    if (driverDoc == 0) { std::cerr << "Error parsing " << driverFile << "\n"; goto EXIT_LABEL; }
    driverCurrentElement = xmlDocGetRootElement(driverDoc);
    if (driverCurrentElement == 0) { std::cerr << driverFile << ": Document empty\n"; goto EXIT_LABEL; }
    if (xmlStrcmp(driverCurrentElement->name, (const xmlChar *) "GAITSYMODE")) {
        std::cerr << driverFile << ": Document of the wrong type, root node != GAITSYMODE\n"; goto EXIT_LABEL; }

    std::cerr << "Parse the elements in " << modelDefinitionFile << "\n";
    cur = modelDefinitionCurrentElement->xmlChildrenNode;
    while (cur != 0)
    {
        if (xmlStrcmp(cur->name, (const xmlChar *)"DRIVER") != 0 && xmlStrcmp(cur->name, (const xmlChar *)"WAREHOUSE") != 0) // if it isn't a driver or a warehouse, just copy the node
        {
            newNode = xmlCopyNode(cur, 1);
            xmlAddChild(newRootNode, newNode);
        }
        cur = cur->next;
    }

    std::cerr << "Parse the elements in " << driverFile << "\n";
    cur = driverCurrentElement->xmlChildrenNode;
    while (cur != 0)
    {
        if (xmlStrcmp(cur->name, (const xmlChar *)"DRIVER") == 0) // if it is a driver, just copy the node
        {
            newNode = xmlCopyNode(cur, 1);
            xmlAddChild(newRootNode, newNode);
            newTextNode = xmlNewText((const xmlChar *)"\n" ); // this just adds a newline after each driver
            xmlAddChild(newRootNode, newTextNode);
        }
        cur = cur->next;
    }

    std::cerr << "Parse " << warehouseFile << "\n";
    warehouse.ReadFile(warehouseFile);
    ptr = warehouse.GetRawData();
    while (*ptr) { if (*ptr < 32 && *ptr > 0) *ptr = 32; ptr++; } // converts all whitespace to spaces
    newNode = xmlNewTextChild(newRootNode, 0, (const xmlChar *)"WAREHOUSE", 0);
    xmlNewProp(newNode, (const xmlChar *)"ID", (const xmlChar *)"DefaultWarehouse");
    xmlNewProp(newNode, (const xmlChar *)"Data", (const xmlChar *)warehouse.GetRawData());
    xmlAddChild(newRootNode, newNode);
    newTextNode = xmlNewText((const xmlChar *)"\n" ); // this just adds a newline after the warehouse
    xmlAddChild(newRootNode, newTextNode);

EXIT_LABEL:
    if (driverDoc) xmlFreeDoc(driverDoc);
    if (modelDefinitionDoc) xmlFreeDoc(modelDefinitionDoc);
    return newDoc;
}

// change the values of an attribute
void ChangeAttribute(xmlDocPtr doc, const char *tag, const char *attribute, const char *newValue, bool firstMatch)
{
    xmlNodePtr cur;
    cur = xmlDocGetRootElement(doc);
    cur = cur->xmlChildrenNode;
    while (cur != 0)
    {
        if (xmlStrcmp(cur->name, (const xmlChar *)tag) == 0)
        {
            DoXmlReplaceProp(cur, (const xmlChar *)attribute, (const xmlChar *)newValue);
            if (firstMatch) break;
        }
        cur = cur->next;
    }
}

xmlAttr *DoXmlReplaceProp(xmlNode *cur, const xmlChar *name, const xmlChar *newValue)
{
    xmlAttr *ptr;
#ifdef USE_CASE_SENSITIVE_XML_ATTRIBUTES
    ptr = xmlHasProp(cur, name);
#else
    for (ptr = cur->properties; ptr != 0; ptr = ptr->next)
    {
        if (strcasecmp((const char *)ptr->name, (const char *)name) == 0)
        {
            break;
        }
    }
#endif
    if (ptr) xmlRemoveProp(ptr);

    ptr = xmlNewProp(cur, name, newValue);
    return ptr;
}

void ApplyGenome(const char *inputXML, const char *inputGenome, const char *outputXML, double *fitness)
{
    DataFile genomeData;
    double val;
    int ival, genomeSize;
    genomeData.ReadFile(inputGenome);
    genomeData.ReadNext(&ival);
    genomeData.ReadNext(&genomeSize);
    double *data = new double[genomeSize];
    for (int i = 0; i < genomeSize; i++)
    {
        genomeData.ReadNext(&val);
        data[i] = val;
        genomeData.ReadNext(&val); genomeData.ReadNext(&val); genomeData.ReadNext(&val);
        if (ival == -2) genomeData.ReadNext(&val); // skip the extra parameter
    }
    genomeData.ReadNext(fitness);

    XMLConverter myXMLConverter;
    myXMLConverter.LoadBaseXMLFile(inputXML);
    myXMLConverter.ApplyGenome(genomeSize, data);
    int len;
    char *buf = (char *)myXMLConverter.GetFormattedXML(&len);

    DataFile outputXMLData;
    outputXMLData.SetRawData(buf);
    outputXMLData.WriteFile(outputXML);

    delete [] data;
}

// check how much CPU and IO time has been used
void CheckCPUIOUsage()
{
    std::cerr << "Checking CPU and IO Usage\n";
    MPI_Status status;
    int mpi_buffer[16];
    double *ptr;
    double CPUTime = 0;
    double IOTime = 0;
    for (int j = 1; j < gMPI_Comm_size; j++)
    {
        mpi_buffer[0] = MPI_MESSAGE_ID_SEND_TIMINGS;
        MPI_Send(mpi_buffer,                /* message buffer */
                 sizeof(int),               /* number of data item */
                 MPI_BYTE,                  /* data items are bytes */
                 j,                         /* destination */
                 0,                         /* user chosen message tag */
                 MPI_COMM_WORLD);           /* default communicator */

        MPI_Recv(mpi_buffer,            /* message buffer */
                 sizeof(mpi_buffer),    /* max number of data items */
                 MPI_BYTE,              /* of type BYTE */
                 j,                     /* receive from this sender */
                 MPI_ANY_TAG,           /* any type of message */
                 MPI_COMM_WORLD,        /* default communicator */
                 &status);              /* info about the received message */

        // std::cerr << "MPI target " << j << " Message ID " << ((int *)mpi_buffer)[0] << "\n";
        if (((int *)mpi_buffer)[0] == MPI_MESSAGE_ID_SEND_TIMINGS)
        {
            ptr = (double *)&(mpi_buffer[2]);
            CPUTime += *ptr++;
            IOTime += *ptr;
        }
    }
    std::cerr << "CPUTime = " << CPUTime << " IOTime = " << IOTime << " CPU Use Proportion " << CPUTime / (CPUTime + IOTime) << "\n";
}

// find the best genome in a folder
// returns 0 on success and line number on failure
int FindBest(std::string folder, std::string &bestPopulationFile, std::string &bestGenomeFile)
{
    DIR *dir;
    struct dirent* entry;
    std::vector<std::string> fileList;
    int j;

    std::cerr << "read the output folder to find the best genome and the best population\n";
    dir = opendir(folder.c_str());
    fileList.clear();
    while( (entry = readdir(dir))) fileList.push_back(entry->d_name);
    closedir(dir);
    std::sort(fileList.begin(), fileList.end());

    for (j = fileList.size() - 1; j >= 0; j--)
    {
        if (fileList[j].find("Population") == 0) break;
    }
    if (j < 0) return __LINE__;
    bestPopulationFile = folder + "/" + fileList[j];
    std::cerr << "bestPopulationFile is now " << bestPopulationFile << "\n";

    for (j = fileList.size() - 1; j >= 0; j--)
    {
        if (fileList[j].find("BestGenome") == 0)
        {
            if (fileList[j].find(".txt") != std::string::npos)
                break;
        }
    }
    if (j < 0) return __LINE__;
    bestGenomeFile = folder + "/" + fileList[j];
    std::cerr << "bestGenomeFile is now " << bestGenomeFile << "\n";
    return 0;
}

// implement a search and replace for std::string
bool Replace(std::string& str, const std::string& from, const std::string& to)
{
    size_t start_pos = str.find(from);
    if (start_pos == std::string::npos) return false;
    str.replace(start_pos, from.length(), to);
    return true;
}

// read a file into lines
int ReadLines(const char *filename, std::vector<std::string> &lines, bool ignoreEmptyLinesFlag)
{
    std::cerr << "ReadLines: " << filename << "\n";
    lines.clear();
    std::string line;
    std::ifstream myfile(filename);
    if (myfile.good())
    {
        while (std::getline(myfile, line).good())
        {
            if (ignoreEmptyLinesFlag)
            {
                if (line.length() > 0) lines.push_back(line);
            }
            else
            {
                lines.push_back(line);
            }
        }
        myfile.close();
        std::cerr << lines.size() << " read\n";
    }
    else
    {
        return __LINE__;
    }
    return 0;
}

// write lines into a file
int WriteLines(const char *filename, std::vector<std::string> &lines)
{
    std::cerr << "WriteLines: " << filename << " lines = " << lines.size() << "\n";
    std::ofstream myfile(filename);
    if (myfile.good())
    {
        for (unsigned int j = 0; j < lines.size(); j++) myfile << lines[j] << "\n";
        myfile.close();
    }
    else
    {
        return __LINE__;
    }
    return 0;
}


