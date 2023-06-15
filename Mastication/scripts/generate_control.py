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

    configFile = "model/SimulationData.txt"
    rangeFile = "model/RangeData.txt"
    
    # read the config file
    configTokenList = ConvertFileToTokens(configFile)
    rangeTokenList = ConvertFileToTokens(rangeFile)
    
    # loop through the various combinations
    for attachment in ("Origin", "Insertion"):
        for muscle in ("Temporalis",  "Masseter", "MedialPterygoid", "LateralPterygoid"):
            count = 0;
            for xOffset in (-1.0, 0.0, 1.0):
                for yOffset in (-1.0, 0.0, 1.0):
                    for zOffset in (-1.0, 0.0, 1.0):
                        print "expt%s%s%02d" % (muscle, attachment, count)
                        count = count + 1
                        name = "kLeft" + muscle + attachment
                        valueIndex = GetIndexFromTokenList(configTokenList, name)
                        rangeIndex = GetIndexFromTokenList(rangeTokenList, name)
                        print "%s %f %f %f" % (name, float(configTokenList[valueIndex]) + xOffset * float(rangeTokenList[rangeIndex]), float(configTokenList[valueIndex+1]) + yOffset * float(rangeTokenList[rangeIndex+1]), float(configTokenList[valueIndex+2]) + zOffset * float(rangeTokenList[rangeIndex+2]))
                        name = "kRight" + muscle + attachment;
                        valueIndex = GetIndexFromTokenList(configTokenList, name)
                        rangeIndex = GetIndexFromTokenList(rangeTokenList, name)
                        print "%s %f %f %f" % (name, float(configTokenList[valueIndex]) + xOffset * float(rangeTokenList[rangeIndex]), float(configTokenList[valueIndex+1]) - yOffset * float(rangeTokenList[rangeIndex+1]), float(configTokenList[valueIndex+2]) + zOffset * float(rangeTokenList[rangeIndex+2]))

    # special cases for constraints
    for i in range(0, 5):
        name = "ToothSpringConstant"
        print "expt%s%02d" % (name, i)
        rangeIndex = GetIndexFromTokenList(rangeTokenList, "k" + name)
        print "%s %f" % (name, float(rangeTokenList[rangeIndex + i]))
        name = "TMJSpringConstant"
        print "expt%s%02d" % (name, i)
        rangeIndex = GetIndexFromTokenList(rangeTokenList, "kLeft" + name)
        print "%s %f" % ("kLeft" + name, float(rangeTokenList[rangeIndex + i]))
        rangeIndex = GetIndexFromTokenList(rangeTokenList, "kRight" + name)
        print "%s %f" % ("kRight" + name, float(rangeTokenList[rangeIndex + i]))
    
    count = 0;
    for xOffset in (-1.0, 0.0, 1.0):
        for yOffset in (-1.0, 0.0, 1.0):
            for zOffset in (-1.0, 0.0, 1.0):
                print "exptTMJPosition%02d" % (count)
                count = count + 1
                name = "kLeftTMJOrigin" 
                valueIndex = GetIndexFromTokenList(configTokenList, name)
                rangeIndex = GetIndexFromTokenList(rangeTokenList, name)
                print "%s %f %f %f" % (name, float(configTokenList[valueIndex]) + xOffset * float(rangeTokenList[rangeIndex]), float(configTokenList[valueIndex+1]) + yOffset * float(rangeTokenList[rangeIndex+1]), float(configTokenList[valueIndex+2]) + zOffset * float(rangeTokenList[rangeIndex+2]))
                name = "kLeftTMJInsertion" 
                valueIndex = GetIndexFromTokenList(configTokenList, name)
                rangeIndex = GetIndexFromTokenList(rangeTokenList, name)
                print "%s %f %f %f" % (name, float(configTokenList[valueIndex]) + xOffset * float(rangeTokenList[rangeIndex]), float(configTokenList[valueIndex+1]) + yOffset * float(rangeTokenList[rangeIndex+1]), float(configTokenList[valueIndex+2]) + zOffset * float(rangeTokenList[rangeIndex+2]))
                name = "kRightTMJOrigin" 
                valueIndex = GetIndexFromTokenList(configTokenList, name)
                rangeIndex = GetIndexFromTokenList(rangeTokenList, name)
                print "%s %f %f %f" % (name, float(configTokenList[valueIndex]) + xOffset * float(rangeTokenList[rangeIndex]), float(configTokenList[valueIndex+1]) - yOffset * float(rangeTokenList[rangeIndex+1]), float(configTokenList[valueIndex+2]) + zOffset * float(rangeTokenList[rangeIndex+2]))
                name = "kRightTMJInsertion" 
                valueIndex = GetIndexFromTokenList(configTokenList, name)
                rangeIndex = GetIndexFromTokenList(rangeTokenList, name)
                print "%s %f %f %f" % (name, float(configTokenList[valueIndex]) + xOffset * float(rangeTokenList[rangeIndex]), float(configTokenList[valueIndex+1]) - yOffset * float(rangeTokenList[rangeIndex+1]), float(configTokenList[valueIndex+2]) + zOffset * float(rangeTokenList[rangeIndex+2]))

# program starts here

main()
   
           
