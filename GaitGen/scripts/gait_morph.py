#!/usr/bin/python -u

import sys
import string
import array
import os
import re
import time

def CopyFile(theInputFileName, theOutputFileName):
    """Copies the contents of a file"""
    theInput = open(theInputFileName, 'r')
    theData = theInput.read()
    theInput.close()
    theOutput = open(theOutputFileName, 'w')
    theOutput.write(theData)
    theOutput.close()

def StartsWith(theString, thePrefix):
    if theString[0: len(thePrefix)] == thePrefix:
        return 1
    return 0

def PrintExit(value):
    """exits with error message"""
    sys.stderr.write(str(value) + "\n");
    sys.exit(1)

def ConvertToListOfTokens(theBuffer):
    """converts a string into a list of tokens delimited by whitespace"""
    
    charList = list(theBuffer)
    i = 0
    tokenList = []
    while i < len(charList):
        byte = charList[i]
        if byte in string.whitespace: 
            i = i + 1
            continue
        word = ""
        if byte == '"':
            i = i + 1
            byte = charList[i]
            while byte != '"':
                word = word + byte
                i = i + 1
                if i >= len(charList): PrintExit("Unmatched \" error")
                byte = charList[i]
        else:
            while (byte in string.whitespace) == 0:
                word = word + byte
                i = i + 1
                if i >= len(charList): break
                byte = charList[i]
        
        if len(word) > 0: tokenList.append(word)
        i = i + 1
            
    return tokenList        

def ConvertFileToTokens(filename):
    """reads in a file and converts to a list of whitespace delimited tokens"""
    
    input = open(filename, 'r')
    theBuffer = input.read()
    input.close()
    
    return ConvertToListOfTokens(theBuffer)
   
def GetValueFromTokenList(tokenList, match):
    """returns value after the matching token in the tokenList (match + 1)
       raises NoMatch on error"""
    for i in range(0, len(tokenList) - 1):
        if tokenList[i] == match: return tokenList[i + 1]
        
    raise "NoMatch", match

def IsANumber(theString):
    """checks to see whether a string is a valid number"""
    if re.match('([+-]?)(?=\d|\.\d)\d*(\.\d*)?([Ee]([+-]?\d+))?', theString) == None: return 0
    return 1
     
def CorrectQuaternion(q0, q1, q2, q3):
    """corrects an orientation quaternion to be in the y = 0 plane
       EXPERIMENTAL"""
       
    # covert to Euler
    command = "%s %f %f %f %f > temp.txt" % (quaternionToEulerCommand, q0, q1, q2, q3)
    print command
    os.system(command)
    theTokenList = ConvertFileToTokens("temp.txt")

    # convert back to Quaternion with x = -90 & z = -180

    command = "%s %f %f %f > temp.txt" % (eulerToQuaternionCommand, -90, theTokenList[1], 180)
    print command
    os.system(command);
    theTokenList = ConvertFileToTokens("temp.txt")

    return theTokenList;
  

def main():
    usageString = "Usage:\n%s [--controlFile file] [--jumpPercent x]" % sys.argv[0]

    controlFile = "control.txt";
    jumpFlag = 0;

    i = 1;
    while i < len(sys.argv):
        if sys.argv[i] == "--controlFile":
            i = i + 1
            if i >= len(sys.argv): PrintExit(usageString)
            controlFile = sys.argv[i] 
            i = i + 1
            continue 

        if sys.argv[i] == "--jumpPercent":
            i = i + 1
            if i >= len(sys.argv): PrintExit(usageString)
            jumpFlag = 1
            jumpPercent = float(sys.argv[i]) 
            i = i + 1
            continue 

        PrintExit(usageString)


    tokenList = ConvertFileToTokens(controlFile)

    try:
        kRangeControlStartPercent = float(GetValueFromTokenList(tokenList, "startPercent"))
        kRangeControlEndPercent = float(GetValueFromTokenList(tokenList, "endPercent"))
        kRangeControlIncrementPercent = float(GetValueFromTokenList(tokenList, "incrementPercent"))
        rootConfigFile1 = GetValueFromTokenList(tokenList, "rootConfigFile1")
        rootConfigFile2 = GetValueFromTokenList(tokenList, "rootConfigFile2")
        rootParameterFile = GetValueFromTokenList(tokenList, "rootParameterFile")
        rootGenomeFile = GetValueFromTokenList(tokenList, "rootGenomeFile")
        rootPopulationFile = GetValueFromTokenList(tokenList, "rootPopulationFile")
        rootModelStateFile = GetValueFromTokenList(tokenList, "rootModelStateFile")
        workingConfigFile = GetValueFromTokenList(tokenList, "workingConfigFile")
        workingParameterFile = GetValueFromTokenList(tokenList, "workingParameterFile")
        workingModelState = GetValueFromTokenList(tokenList, "workingModelState")
        gaCommand = GetValueFromTokenList(tokenList, "gaCommand")
        objectiveCommand = GetValueFromTokenList(tokenList, "objectiveCommand")
        mergeCommand = GetValueFromTokenList(tokenList, "mergeCommand")
        mergeParameterFile = GetValueFromTokenList(tokenList, "mergeParameterFile")
        workingGenome = GetValueFromTokenList(tokenList, "workingGenome")
        workingPopulation = GetValueFromTokenList(tokenList, "workingPopulation")
        usePopulationFlag = int(GetValueFromTokenList(tokenList, "usePopulationFlag"))
        repeatsAtEachValue = int(GetValueFromTokenList(tokenList, "repeatsAtEachValue"))

    except "NoMatch", data:
        PrintExit("Error: %s undefined" % data)

    if jumpFlag:
        kRangeControlStartPercent = jumpPercent
    else:
        if usePopulationFlag:
            print "Copying %s to %s" % (rootPopulationFile, workingPopulation)
            theInput = open(rootPopulationFile, 'r')
            theOutput = open(workingPopulation, 'w')
            theData = theInput.read();
            theOutput.write(theData);
            theInput.close();
            theOutput.close();
        else:
            print "Copying %s to %s" % (rootGenomeFile, workingGenome)
            theInput = open(rootGenomeFile, 'r')
            theOutput = open(workingGenome, 'w')
            theData = theInput.read();
            theOutput.write(theData);
            theInput.close();
            theOutput.close();

        print "Copying %s to %s" % (rootModelStateFile, workingModelState)
        theInput = open(rootModelStateFile, 'r')
        theOutput = open(workingModelState, 'w')
        theData = theInput.read();
        theOutput.write(theData);
        theInput.close();
        theOutput.close();

    dirName = range(0, repeatsAtEachValue)
    repeatCount = 0
    kRangeControlPercent = kRangeControlStartPercent
    while kRangeControlPercent <= kRangeControlEndPercent:
        kRangeControl = kRangeControlPercent / 100.0 

        # build the working config file

        command = "%s %s %s %s %s tempConfigFile.txt" % (mergeCommand, rootConfigFile1, rootConfigFile2, kRangeControl, mergeParameterFile)
        print command
        os.system(command)

        # prepend the model state
        theOutput = open(workingConfigFile, 'w')
        theInput = open(workingModelState, 'r')
        theData = theInput.read()
        theOutput.write(theData)
        theInput.close()
        
        theOutput.write("\n\n")

        theInput = open("tempConfigFile.txt", 'r');
        theData = theInput.read();
        theInput.close();
        theOutput.write(theData);
        theOutput.close();

        # build the working parameter file

        theOutput = open(workingParameterFile, 'w')
        if usePopulationFlag: theOutput.write("startingPopulation %s\n\n" % workingPopulation)
        else: theOutput.write("modelGenome %s\n\n" % workingGenome)
        theOutput.write("extraDataFile %s\n\n" % workingConfigFile)
        theInput = open(rootParameterFile, 'r');
        theData = theInput.read();
        theInput.close();
        theOutput.write(theData);
        theOutput.close();

        # create the directory name
        theTime = time.localtime(time.time())
        dirName[repeatCount] = "%.4f/Run_%04d-%02d-%02d_%02d.%02d.%02d" % (kRangeControl, theTime[0], theTime[1], theTime[2], theTime[3], theTime[4], theTime[5])
        os.makedirs(dirName[repeatCount])
        command = "%s -p %s -b %s -l %s -o %s" % (gaCommand, workingParameterFile, workingGenome, workingPopulation, dirName[repeatCount])
        print command
        os.system(command)  

        # run the objective

        command = "%s -c %s -g %s" % (objectiveCommand, workingConfigFile, workingGenome)
        print command
        os.system(command)  

        repeatCount = repeatCount + 1;
        if repeatCount >= repeatsAtEachValue:
            repeatCount = 0;
            kRangeControlPercent = kRangeControlPercent + kRangeControlIncrementPercent

            # find the best score from the previour block
            maxScore = -1e30
            bestDir = 0
            for i in range(0, repeatsAtEachValue):
                fileList = os.listdir(dirName[i])
                fileList.sort()
                fileList.reverse()
                for name in fileList:
                    if StartsWith(name, "BestGenome_"): break
                tokenList = ConvertFileToTokens(dirName[i] + "/" + name)
                if int(tokenList[0]) != -1:
                    print "Error reading genome: %s" % (dirName[i] + "/" + name)
                    continue
                genomeLength = int(tokenList[1])
                score = float(tokenList[genomeLength * 4 + 2])
                if score > maxScore: 
                    maxScore = score
                    bestDir = i
            
            print "Best score %f found in %s" % (maxScore, dirName[bestDir])
            
            # copy the files over
            fileList = os.listdir(dirName[bestDir])
            fileList.sort()
            fileList.reverse()
            for name in fileList:
                if StartsWith(name, "BestGenome_"): 
                    print "Copying %s to %s" % (dirName[bestDir] + "/" + name, workingGenome)
                    CopyFile(dirName[bestDir] + "/" + name, workingGenome)
                    break
            
            for name in fileList:
                if StartsWith(name, "Population_"): 
                    print "Copying %s to %s" % (dirName[bestDir] + "/" + name, workingPopulation)
                    CopyFile(dirName[bestDir] + "/" + name, workingPopulation)
                    break
                    
            print "Copying %s to %s" % (dirName[bestDir] + "/" + "config_file.txt", workingConfigFile) 
            CopyFile(dirName[bestDir] + "/" + "config_file.txt", workingConfigFile)
            
            # run the objective

            command = "%s -c %s -g %s" % (objectiveCommand, workingConfigFile, workingGenome)
            print command
            os.system(command)  

# program starts here

main()
   

