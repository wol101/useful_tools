#!/usr/bin/python -u

import os
import time
import sys

usageString = "Usage:\n%s [--nCores n] [--nHours n]" % sys.argv[0]
nCores = 16
nHours = 24

i = 1;
while i < len(sys.argv):
    if sys.argv[i] == "--nCores":
        i = i + 1
        if i >= len(sys.argv): PrintExit(usageString)
        nCores = int(sys.argv[i])
        i = i + 1
        continue

    if sys.argv[i] == "--nHours":
        i = i + 1
        if i >= len(sys.argv): PrintExit(usageString)
        nHours = int(sys.argv[i])
        i = i + 1
        continue

    sys.stderr.write(usageString + "\n");
    sys.exit(1)

f  = '#!/bin/sh\n'
f += '#submit.sh file generated by do_submit.py\n'    
f += '#$ -l quad\n' 
f += '#$ -pe openmpi NCORES\n'
f += '#$ -cwd -V -j y\n' 
f += '#$ -l h_rt=NHOURS:00:00\n' 
f += '#$ -o OUTPUTFILE\n'
f += 'EXEC="/panfs/li/staging/users/mcws/cvs/UnifiedBootstrap/bin/UnifiedBootstrapAsync --serverArgs -d 0 --clientArgs -d 0 -q "\n'
f += '. /usr/share/Modules/init/bash\n' 
f += 'module load mpi/openmpi-gcc/1.5.3\n'
f += '# $NSLOTS is defined automatically from the number requested in the parallel environment.\n'
f += 'mpirun -np $NSLOTS $EXEC\n'

# NCORES is the number of cores to use (should be a multiple of 8)
# NHOURS is the number of hours to run (max walltime)
# OUTPUTFILE is the name of the output file
jobSubmitFile = f
jobSubmitFile = jobSubmitFile.replace("NCORES", str(nCores))
jobSubmitFile = jobSubmitFile.replace("NHOURS", str(nHours))

theTime = time.localtime(time.time())
outputFile = "ulgbc5_%03dCore_%03dHour_%04d-%02d-%02d_%02d-%02d-%02d.txt" % (nCores, nHours, theTime[0], theTime[1], theTime[2], theTime[3], theTime[4], theTime[5])
            
jobSubmitFile = jobSubmitFile.replace("OUTPUTFILE", outputFile)

out = open("submit.sh", "w")
out.write(jobSubmitFile);
out.close()

command = "qsub submit.sh"
print command

jobnum = os.popen(command).read()
print jobnum

