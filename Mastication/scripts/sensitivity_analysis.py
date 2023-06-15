#!/usr/bin/python -u

import sys
import string
import array
import os
import re
import time

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
    
    return ConvertToListOfTokens(theBuffer)
   
def GetIndexFromTokenList(tokenList, match):
    """returns the index pointing to the value in the tokenList (match + 1)
       raises NoMatch on error"""
    for i in range(0, len(tokenList) - 1):
        if tokenList[i] == match: return i + 1
        
    raise "NoMatch", match
    
def GetValueFromTokenList(tokenList, match):
    """returns the value after the matching token in the tokenList (match + 1)
       raises NoMatch on error"""
    for i in range(0, len(tokenList) - 1):
        if tokenList[i] == match: return tokenList[i + 1]
        
    raise "NoMatch", match

def IsANumber(theString):
    """checks to see whether a string is a valid number"""
    if re.match('([+-]?)(?=\d|\.\d)\d*(\.\d*)?([Ee]([+-]?\d+))?', theString) == None: return 0
    return 1
   
def main():
    usageString = "Usage:\n%s [--controlFile file] [--objectiveCommand file] [--genomeFile file] [--rootConfigFile file] [--outputDirectory dir]" % sys.argv[0]

    controlFile = "model/control.txt"
    objectiveCommand = "bin/objective --numStartupPhases 25600"
    genomeFile = "model/genome3.txt"
    rootConfigFile = "model/SimulationData.txt"
    workingConfigFile = "workingConfigFile.txt"
    outputDirectory = "output"
    
    i = 1;
    while i < len(sys.argv):
        if sys.argv[i] == "--controlFile":
            i = i + 1
            if i >= len(sys.argv): PrintExit(usageString)
            controlFile = sys.argv[i] 
            i = i + 1
            continue 

        if sys.argv[i] == "--objectiveCommand":
            i = i + 1
            if i >= len(sys.argv): PrintExit(usageString)
            objectiveCommand = sys.argv[i] 
            i = i + 1
            continue 

        if sys.argv[i] == "--genomeFile":
            i = i + 1
            if i >= len(sys.argv): PrintExit(usageString)
            genomeFile = sys.argv[i] 
            i = i + 1
            continue 

        if sys.argv[i] == "--rootConfigFile":
            i = i + 1
            if i >= len(sys.argv): PrintExit(usageString)
            rootConfigFile = sys.argv[i] 
            i = i + 1
            continue 

        if sys.argv[i] == "--outputDirectory":
            i = i + 1
            if i >= len(sys.argv): PrintExit(usageString)
            outputDirectory = sys.argv[i] 
            i = i + 1
            continue 

        PrintExit(usageString)

    # create the output directory
    if os.path.exists(outputDirectory) == 0: os.makedirs(outputDirectory)

    # read the control file line by line
    input = open(controlFile, 'r')
    listOfLines = input.readlines()
    input.close()
    i = 0;
    while i < len(listOfLines):
        if listOfLines[i].startswith("expt"):
            # note: need to remove the end of line marker
            theExptName = (listOfLines[i])[0:len(listOfLines[i]) - 1]
            j = 1
            newBlock = ""
            while (i + j) < len(listOfLines):
                if listOfLines[i + j].startswith("expt"):
                    break;
                newBlock = newBlock + listOfLines[i + j] 
                j = j + 1
            i = i + j    
            
            # copy the default config file but put the new block first

            print "Copying %s to %s" % (rootConfigFile, workingConfigFile)
            print newBlock,
            theInput = open(rootConfigFile, 'r')
            theOutput = open(workingConfigFile, 'w')
            theOutput.write(newBlock)
            theData = theInput.read()
            theOutput.write(theData)
            theInput.close()
            theOutput.close()

            # build the working command line and run it
            outputFile = "%s.txt" % (theExptName)
            command = "%s -c \"%s\" -g \"%s\" > \"%s/%s\"" % (objectiveCommand, workingConfigFile, genomeFile, outputDirectory, outputFile)
            print command
            os.system(command)


# program starts here

main()
   

