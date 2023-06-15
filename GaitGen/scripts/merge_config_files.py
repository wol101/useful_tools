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

def GetIndexFromTokenList(tokenList, match):
    """returns index of the matching token in the tokenList
       raises NoMatch on error"""
    for i in range(0, len(tokenList) - 1):
        if tokenList[i] == match: return i
        
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
    usageString = "Usage:\n%s fileName1 fileName2 proportionFile2 parameterFile outputFileName\nList of parameters in " % sys.argv[0]

    if (len(sys.argv) != 6): PrintExit(usageString)
    
    fileName1 = sys.argv[1]
    fileName2 = sys.argv[2]
    proportionFile2 = float(sys.argv[3])
    parameterFile = sys.argv[4]
    outputFileName = sys.argv[5]
    
    # convert the data files to tokens
    tokenListFileName1 = ConvertFileToTokens(fileName1)
    tokenListFileName2 = ConvertFileToTokens(fileName2)
    
    # read the lines in the parameterFile
    theInput = open(parameterFile, 'r')
    lineListParameterFile = theInput.readlines()
    theInput.close()
    
    # loop through the parameter list handling each parameter
    outputParameterList = []    
    for line in lineListParameterFile:
        theList = []
        theTokenList = ConvertToListOfTokens(line)
        
        # check for blank lines or comments
        if (len(theTokenList) == 0 or theTokenList[0] == "#"): continue
        if (len(theTokenList) < 2): PrintExit("Trouble parsing %s" % (theTokenList[0]))
        parameter = theTokenList[0]
        theList.append(parameter)
        count = int(theTokenList[1])
        try:
            index1 = GetIndexFromTokenList(tokenListFileName1, parameter)
            index2 = GetIndexFromTokenList(tokenListFileName2, parameter)
            for i in range(1, count + 1):
                if (IsANumber(tokenListFileName1[index1 + i]) and IsANumber(tokenListFileName2[index2 + i])):
                    value = \
                        float(tokenListFileName1[index1 + i]) + \
                        (float(tokenListFileName2[index2 + i]) - \
                        float(tokenListFileName1[index1 + i])) * proportionFile2
                    theList.append("%0.19g" % (value))
                else:
                    theList.append("\"" + tokenListFileName1[index1 + i] + "\"")
            outputParameterList.append(theList)
        except:
            print "WARNING %s not found" % (parameter)
        
    # now output the merged file
    
    theOutput = open(outputFileName, 'w')
    for theList in outputParameterList:
        for theItem in theList:
            theOutput.write(theItem)
            theOutput.write(" ")
        theOutput.write("\n")   
    theOutput.close()

# program starts here

main()
   

