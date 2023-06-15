#!/usr/bin/python -u

import os
import time
import sys
import re

usageString = "Usage:\n%s [--nCores n] [--nHours n] [--nRepeats n]" % sys.argv[0]
nCores = 16
nHours = 24
nRepeats = 1

i = 1;
while i < len(sys.argv):
    if sys.argv[i] == "--nCores":
        i = i + 1
        if i >= len(sys.argv): 
            sys.stderr.write(usageString + "\n")
            sys.exit(1)
        nCores = int(sys.argv[i])
        i = i + 1
        continue

    if sys.argv[i] == "--nHours":
        i = i + 1
        if i >= len(sys.argv): 
            sys.stderr.write(usageString + "\n")
            sys.exit(1)
        nHours = int(sys.argv[i])
        i = i + 1
        continue

    if sys.argv[i] == "--nRepeats":
        i = i + 1
        if i >= len(sys.argv): 
            sys.stderr.write(usageString + "\n")
            sys.exit(1)
        nRepeats = int(sys.argv[i])
        i = i + 1
        continue

    sys.stderr.write(usageString + "\n")
    sys.exit(1)

f  = '#!/bin/sh\n'
f += '#submit.sh file generated by do_submit.py\n'    
f += '#$ -l np=NCORES\n'
f += '#$ -cwd -V\n' 
f += '#$ -l h_rt=NHOURS:00:00\n' 
f += '#$ -o OUTPUTFILE\n'
f += 'ulimit -c 0\n'
f += 'EXEC="$HOME/Unix/cvs/UnifiedBootstrap2016/bin/UnifiedBootstrapAsync2016 --repeats %d --serverArgs -d 0 --clientArgs -d 0 -q "\n' % (nRepeats)
f += 'mpirun $EXEC\n'

# NCORES is the number of cores to use (should be a multiple of 8)
# NHOURS is the number of hours to run (max walltime)
# OUTPUTFILE is the name of the output file
jobSubmitFile = f
jobSubmitFile = jobSubmitFile.replace("NCORES", str(nCores))
jobSubmitFile = jobSubmitFile.replace("NHOURS", str(nHours))

theTime = time.localtime(time.time())
outputFile = "n8_%03dCore_%03dHour_%04d-%02d-%02d_%02d-%02d-%02d.txt" % (nCores, nHours, theTime[0], theTime[1], theTime[2], theTime[3], theTime[4], theTime[5])
            
jobSubmitFile = jobSubmitFile.replace("OUTPUTFILE", outputFile)

out = open("n8_submit.sh", "w")
out.write(jobSubmitFile);
out.close()

qsub_command = "qsub n8_submit.sh"
print qsub_command
qsub_text = os.popen(qsub_command).read()
print qsub_text

qstat_command = "qstat"
print qstat_command
qstat_text = os.popen(qstat_command).read()
print qstat_text

job_number = int(re.findall(r'\d+', qsub_text)[0])
current_time = time.localtime()
logfile_name = 'Submit_Log_%s_%d.txt' % (time.strftime("%Y-%m-%d_%H.%M.%S", current_time), job_number)
fout = open(logfile_name, 'w')
fout.write('%s\n\n' % time.ctime(time.mktime(current_time)))
fout.write(" ".join(sys.argv) + '\n\n')
fout.write(qsub_command + '\n')
fout.write(qsub_text + '\n')
fout.write(qstat_command + '\n')
fout.write(qstat_text + '\n')
fout.close()


