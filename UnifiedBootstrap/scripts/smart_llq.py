#!/usr/bin/python -u

import sys
import os
import string
import subprocess
import re
import getpass
import time
import dateutil.parser

def smart_llq():
    
    usage_string = "Parses the output from llq to give more useful user information\nUsage:\n%s [user_name]" % sys.argv[0]

    n_nodes = 4096 # this seems about right for this machine

    if len(sys.argv) > 2:
        print usage_string
        exit(1)
    
    username = getpass.getuser()
    if len(sys.argv) == 2:
        username = sys.argv[1]
        if (sys.argv[1] == '-h' or sys.argv[1] == '--help'):
            print usage_string
            exit(1)
    
    # use llq to get the order that the processes will run
    # queue_data = subprocess.check_output(["llq"])
    p = subprocess.Popen(["llq"], stdout=subprocess.PIPE)
    out, err = p.communicate()
    queue_data = out
    queue_lines = queue_data.splitlines()
    id_list = []
    for i in range(2, len(queue_lines)):
        tokens = to_tokens(queue_lines[i])
        if len(tokens) < 7:
            break
        id_list.append(tokens[0])

    # now use llq -l to get the actual run information
    # queue_data = subprocess.check_output(["llq", "-l"])
    p = subprocess.Popen(["llq", "-l"], stdout=subprocess.PIPE)
    out, err = p.communicate()
    queue_data = out

    print '%-3s %-24s %-16s %-10s %-5s %-5s' % ('N','Id','Owner','Status','Nodes','Time')
    print '--- ------------------------ ---------------- ---------- ----- -----'
    
    lines = queue_data.splitlines()
    starts = []
    for line_index in range(0, len(lines)):
        if (lines[line_index].startswith('=====')):
            starts.append(line_index)
    
    job_list = {}
    for i in range(0, len(starts)):
    
        line_index = starts[i] + 1
        job_info = {}
        while line_index < len(lines):
            current_line = lines[line_index]
            colon_index = current_line.find(':')
            if (colon_index < 0):
                break
            key = current_line[0: colon_index].strip()
            data = current_line[colon_index + 1:].strip()
            job_info[key] = data
            line_index = line_index + 1
        
        job_list[job_info['Job Step Id']] = job_info
    
    #print job_list
    #job_list.reverse()
    node_hours = 0.0
    for i in range(0, len(id_list)):
        job_info = job_list[id_list[i]]
        Id = job_info['Job Step Id']
        Owner = job_info['Owner']
        Status = job_info['Status']
        Size = job_info['BG Size Requested']
        if len(Size) == 0:
            tokens = to_tokens(job_info['Class'])
            class_split = re.findall(r"[0123456789]+", tokens[-1])
            Size = int(class_split[-1])
        else:
            Size = int(Size)
        tokens = to_tokens(job_info['Wall Clk Hard Limit'])
        Walltime = float(tokens[-2][1:]) / (60 * 60) # hours not seconds
        if len(job_info['Dispatch Time']) > 0:
            # Dispatch_Time = time.mktime(time.strptime(job_info['Dispatch Time'], '%a %d %b %Y %H:%M:%S %p %Z')) # this format string isn't always right
            datetime_rep = dateutil.parser.parse(job_info['Dispatch Time'])
            Dispatch_Time = time.mktime(datetime_rep.timetuple())
        else:
            Dispatch_Time = 0
        
        if Owner == username[0: len(Owner)]:
            if Status == "Running":
                print '%3d %-24s %-16s %-10s %-5d %-5s Start %s' % (i,Id,Owner,Status,Size,to_hours_minutes_str(Walltime),time.ctime(Dispatch_Time))
            else:
                estimated_time = node_hours / n_nodes
                print '%3d %-24s %-16s %-10s %-5d %-5s Delay %.3f hours' % (i,Id,Owner,Status,Size,to_hours_minutes_str(Walltime),estimated_time)
        
        if Status == "Running":
            elapsed_time = (time.time() - Dispatch_Time) / (60 * 60) # hours not seconds;
            time_left = Walltime - elapsed_time
            if (time_left > 0):
                node_hours = node_hours + Size * time_left
        else:
            node_hours = node_hours + Size * Walltime
        #print node_hours
   
def to_hours_minutes_str(hours):
    h = int(hours)
    rem = hours % 1
    m = int(rem * 60.0)
    str = '%02d:%02d' % (h, m) 
    return str

def to_tokens(charList):
    """converts a string into a list of tokens delimited by whitespace"""
    
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
                if i >= len(charList):
                    print('Unmatched " error')
                    exit(1)
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

def find_matching_line_prefix(lines, match_str, start_index):
    """finds the line that starts with match_str characters"""
    for i in range(start_index, len(lines)):
        stripped_line = lines[i].lstrip()
        if match_str == stripped_line[0: len(match_str)]:
            return i
    return -1

def is_number(theString):
    """checks to see whether a string is a valid number"""
    if re.match('([+-]?)(?=\d|\.\d)\d*(\.\d*)?([Ee]([+-]?\d+))?', theString) == None: return False
    return True

# program starts here

if __name__ == '__main__':
    smart_llq()   

