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
            i += 1
            continue
        word = ""
        if byte == '"':
            i += 1
            byte = charList[i]
            while byte != '"':
                word += byte
                i += 1
                if i >= len(charList): PrintExit("Unmatched \" error")
                byte = charList[i]
        else:
            while (byte in string.whitespace) == 0:
                word += byte
                i += 1
                if i >= len(charList): break
                byte = charList[i]
        
        if len(word) > 0: tokenList.append(word)
        i += 1
            
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
    """analyses the output produced by the simulator"""
    usageString = "Usage:\n%s file1 file2 ..." % sys.argv[0]

    if len(sys.argv) < 2: PrintExit(usageString)

    maxToothForceX = []
    maxToothForceY = []
    maxToothForceZ = []
    minToothForceX = []
    minToothForceY = []
    minToothForceZ = []
    maxLeftTMJForceX = []
    maxLeftTMJForceY = []
    maxLeftTMJForceZ = []
    minLeftTMJForceX = []
    minLeftTMJForceY = []
    minLeftTMJForceZ = []
    
    i = 1;
    while i < len(sys.argv):
        inputFile = sys.argv[i]
        i = i + 1
        
        # read the input file line by line
        input = open(inputFile, 'r')
        inputData = input.read()
        input.close()
        lines = inputData.splitlines();
        toothForceX = []
        toothForceY = []
        toothForceZ = []
        leftTMJForceX = []
        leftTMJForceY = []
        leftTMJForceZ = []
        for line in lines:
            l = line.split("\t")
            if (len(line) > 8):
              toothForceX.append(float(l[2]))
              toothForceY.append(float(l[3]))
              toothForceZ.append(float(l[4]))
              leftTMJForceX.append(float(l[6]))
              leftTMJForceY.append(float(l[7]))
              leftTMJForceZ.append(float(l[8]))
         
        maxToothForceX.append(max(toothForceX))
        maxToothForceY.append(max(toothForceY))
        maxToothForceZ.append(max(toothForceZ))
        minToothForceX.append(min(toothForceX))
        minToothForceY.append(min(toothForceY))
        minToothForceZ.append(min(toothForceZ))
        maxLeftTMJForceX.append(max(leftTMJForceX))
        maxLeftTMJForceY.append(max(leftTMJForceY))
        maxLeftTMJForceZ.append(max(leftTMJForceZ))
        minLeftTMJForceX.append(min(leftTMJForceX))
        minLeftTMJForceY.append(min(leftTMJForceY))
        minLeftTMJForceZ.append(min(leftTMJForceZ))
         
    print "minToothForce\t%f\t%f\t%f\tto\t%f\t%f\t%f\t" % (min(minToothForceX), min(minToothForceY), min(minToothForceZ), max(minToothForceX), max(minToothForceY), max(minToothForceZ)),
    print "maxToothForce\t%f\t%f\t%f\tto\t%f\t%f\t%f\t" % (min(maxToothForceX), min(maxToothForceY), min(maxToothForceZ), max(maxToothForceX), max(maxToothForceY), max(maxToothForceZ)),
    print "minLeftTMJForce\t%f\t%f\t%f\tto\t%f\t%f\t%f\t" % (min(minLeftTMJForceX), min(minLeftTMJForceY), min(minLeftTMJForceZ), max(minLeftTMJForceX), max(minLeftTMJForceY), max(minLeftTMJForceZ)),
    print "maxLeftTMJForce\t%f\t%f\t%f\tto\t%f\t%f\t%f\n" % (min(maxLeftTMJForceX), min(maxLeftTMJForceY), min(maxLeftTMJForceZ), max(maxLeftTMJForceX), max(maxLeftTMJForceY), max(maxLeftTMJForceZ)),
    
# program starts here

main()
        
        
