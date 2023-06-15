#!/usr/bin/python -u

import sys
import os
import math
import shutil
import string

def EditConfig():

    usageString = ("Edit the values of a config.txt file \n"
                   "Usage:\n%s [options] inputFile [outputFile]"
                   "\n\n"
                   "inputFile                       file name for input config file\n"
                   "outputFile                      file name for output config file (overwrites input if not given)\n"
                   "\n"
                   "Options:\n"
                   "-c --modelConfigFile path       sets the modelConfigFile to path\n"
                   "-p --modelPopulationFile path   sets the modelPopulationFile to path\n"
                   "-d --driverFile path            sets the driverFile to path\n"
                   "-f --force                      allows the output and backup files to overwrite\n"
                   "\n"
                   % sys.argv[0])
    
    if len(sys.argv) < 2: PrintExit(usageString)
    
    inputFile = ""
    outputFile = ""
    modelConfigFile = ""
    modelPopulationFile = ""
    driverFile = ""
    force = False;
    bare_args = []
    
    i = 1
    while i < len(sys.argv):

        if sys.argv[i] == '--modelConfigFile' or sys.argv[i] == '-c':
            i = i + 1
            if (i >= len(sys.argv)): PrintExit(usageString)
            modelConfigFile = sys.argv[i]
            i = i + 1
            continue

        if sys.argv[i] == '--modelPopulationFile' or sys.argv[i] == '-p':
            i = i + 1
            if (i >= len(sys.argv)): PrintExit(usageString)
            modelPopulationFile = sys.argv[i]
            i = i + 1
            continue

        if sys.argv[i] == '--driverFile' or sys.argv[i] == '-d':
            i = i + 1
            if (i >= len(sys.argv)): PrintExit(usageString)
            driverFile = sys.argv[i]
            i = i + 1
            continue

        if sys.argv[i] == '--force' or sys.argv[i] == '-f':
            force = True
            i = i + 1
            continue

        bare_args.append(sys.argv[i])
        i = i + 1

    if (len(bare_args) < 1 or len(bare_args) > 2): PrintExit(usageString)
    inputFile = bare_args[0]
    if not os.path.exists(inputFile):
        PrintExit('%s not found.' % (inputFile))
    
    if (len(bare_args) == 2):
        outputFile = bare_args[1]
        if force == False:
            if os.path.exists(outputFile):
                PrintExit('%s exists. Use --force to overwrite.' % (outputFile))
    else:
        backup_file = inputFile + '.bak'
        if force == False:
            if os.path.exists(backup_file):
                PrintExit('%s exists. Use --force to overwrite.' % (backup_file))
        shutil.copyfile(inputFile, backup_file)
        shutil.copystat(inputFile, backup_file)
        outputFile = inputFile
    
    print 'Reading %s' % (inputFile)
    substring_list = ConvertFileListOfSubstrings(inputFile)
    
    # handle modelConfigFile
    if len(modelConfigFile):
        for i in range(0, len(substring_list) - 2):
            if substring_list[i] == "modelConfigFile":
                print 'Changing %s to %s' % (substring_list[i + 2], modelConfigFile)
                substring_list[i + 2] = modelConfigFile
                break
    
    # handle modelPopulationFile
    if len(modelPopulationFile):
        for i in range(0, len(substring_list) - 1):
            if substring_list[i] == "modelPopulationFile":
                print 'Changing %s to %s' % (substring_list[i + 2], modelPopulationFile)
                substring_list[i + 2] = modelPopulationFile
                break
    
    # handle driverFile
    if len(driverFile):
        for i in range(0, len(substring_list) - 1):
            if substring_list[i] == "driverFile":
                print 'Changing %s to %s' % (substring_list[i + 2], driverFile)
                substring_list[i + 2] = driverFile
                break
    
    print 'Writing %s' % (outputFile)
    WriteListOfSubstrings(outputFile, substring_list)

def PrintExit(value):
    """exits with error message"""
    sys.stderr.write(str(value) + "\n");
    sys.exit(1)

def WriteListOfSubstrings(filename, substring_list):
    """writes a list of strings (and things that can be converted to strings) to a file"""

    theOutput = open(filename, 'w')
    for i in range(0, len(substring_list)):
        theOutput.write(str(substring_list[i]))
    theOutput.close()

def ConvertFileListOfSubstrings(filename):
    """reads in a file and converts into a list of substrings consisting of the tokens and the whitespace"""
    
    input = open(filename, 'r')
    theBuffer = input.read()
    input.close()
    
    return ConvertToListOfSubstrings(theBuffer)

def ConvertToListOfSubstrings(charList):
    """converts a string into a list of substrings consisting of the tokens and the whitespace"""
    
    i = 0
    substringList = []
    whitespace_flag = True
    whitespace_string = ''
    token_string = ''
    while i < len(charList):
        byte = charList[i]
        if byte in string.whitespace and whitespace_flag:
            whitespace_string = whitespace_string + byte
            i = i + 1
            continue
            
        if not (byte in string.whitespace) and not whitespace_flag:
            token_string = token_string + byte
            i = i + 1
            continue
            
        if byte in string.whitespace and not whitespace_flag:
            if len(token_string) > 0:
                substringList.append(token_string)
                token_string = ''
            whitespace_string = whitespace_string + byte
            whitespace_flag = True
            i = i + 1
            continue
            
        if not (byte in string.whitespace) and whitespace_flag:
            if len(whitespace_string) > 0:
                substringList.append(whitespace_string)
                whitespace_string = ''
            token_string = token_string + byte
            whitespace_flag = False
            i = i + 1
            continue
    
    if len(token_string) > 0:
        substringList.append(token_string)
    if len(whitespace_string) > 0:
        substringList.append(whitespace_string)
    return substringList

# program starts here

if __name__ == '__main__':
    EditConfig()

