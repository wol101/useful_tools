#!/usr/bin/python -u

import os
import sys
import time
import re

usageString = "Usage:\n%s [options]\n" % sys.argv[0]
usageString = usageString + "Options:\n" + \
    "    --nRepeats n       The repeats argument to UnifiedBootstrap\n" + \
    "    --nCores n         The number of cores to use (8192 default, must be a multiple of nCoresPerNode)\n" + \
    "    --nNodes n         The number of nodes to use (overrides nCores)\n" + \
    "    --nCoresPerNode n  The number of cores per node (16 should not normally be changed)\n" + \
    "    --nHours n         The number of hours to run (4 default)\n" + \
    "    --nMinutes n       The number of minutes to run (0 default)\n" + \
    "    --test             20 minute queue\n" + \
    "    --short            1 hour queue\n" + \
    "    --medium           4 hour queue\n" + \
    "    --long             12 hour queue\n"

defaultHours = 4
nRepeats = 1
nHours = 0
nMinutes = 0
nCoresPerNode = 16 # current recommended value for Blue Gene/Q
nCores = 16384 # must be a multiple of nCoresPerNode
nNodes = 0

i = 1;
while i < len(sys.argv):
    if sys.argv[i] == "--nRepeats":
        i = i + 1
        if i >= len(sys.argv): PrintExit(usageString)
        nRepeats = int(sys.argv[i])
        i = i + 1
        continue

    if sys.argv[i] == "--nCores":
        i = i + 1
        if i >= len(sys.argv): PrintExit(usageString)
        nCores = int(sys.argv[i])
        i = i + 1
        continue

    if sys.argv[i] == "--nNodes":
        i = i + 1
        if i >= len(sys.argv): PrintExit(usageString)
        nNodes = int(sys.argv[i])
        i = i + 1
        continue

    if sys.argv[i] == "--nCoresPerNode":
        i = i + 1
        if i >= len(sys.argv): PrintExit(usageString)
        nCoresPerNode = int(sys.argv[i])
        i = i + 1
        continue

    if sys.argv[i] == "--nHours":
        i = i + 1
        if i >= len(sys.argv): PrintExit(usageString)
        nHours = int(sys.argv[i])
        i = i + 1
        continue

    if sys.argv[i] == "--nMinutes":
        i = i + 1
        if i >= len(sys.argv): PrintExit(usageString)
        nMinutes = int(sys.argv[i])
        if nMinutes < 0:
            nMinutes = 0
        if nMinutes > 59:
            nHours = nHours + int(nMinutes / 60)
            nMinutes = nMinutes % 60
        i = i + 1
        continue

    if sys.argv[i] == "--test":
        i = i + 1
        nMinutes = 20
        nHours = 0
        nNodes = 128
        continue
        
    if sys.argv[i] == "--short":
        i = i + 1
        nMinutes = 0
        nHours = 1
        continue
        
    if sys.argv[i] == "--medium" or sys.argv[i] == "--med":
        i = i + 1
        nMinutes = 0
        nHours = 4
        continue
        
    if sys.argv[i] == "--long":
        i = i + 1
        nMinutes = 0
        nHours = 12
        continue
        

    sys.stderr.write(usageString + "\n");
    sys.exit(1)

if (nHours <= 0 and nMinutes <= 0):
    nHours = defaultHours
    nMinutes = 0

if (nNodes == 0):
    nNodes = nCores / nCoresPerNode
else:
    nCores = nNodes * nCoresPerNode

if (nCores % nCoresPerNode) != 0:
    sys.stderr.write(usageString + "\n");
    sys.stderr.write('nCores must be a multiple of %d\n' % (nCoresPerNode));
    exit(1)
    
executable = "/gpfs/home/PRACE/txs10/wws65-txs10/Unix/bin/UnifiedBootstrapAsync"

print "Blue Gene/Q Joule 16 cores/node 128-4096 nodes (6144 if booked)"
print "Requested %d Cores (%d nodes at %d cores per node)" % (nCores, nNodes, nCoresPerNode)
print "Requested %02d:%02d:00\n" % (nHours, nMinutes)

f  = '#@bg_size=%d\n' % nNodes
f += '#@job_type=bluegene\n'
f += '#@input=/dev/null\n'
f += '#@output=stdout.$(jobid).txt\n'
f += '#@error=stderr.$(jobid).txt\n'
f += '#@wall_clock_limit=%02d:%02d:00\n' % (nHours, nMinutes)
f += '#@executable=/bgsys/drivers/ppcfloor/hlcs/bin/runjob\n'
f += '#@arguments= --exe "%s" --ranks-per-node %d --np %d --args "--repeats" --args %d --args "--serverArgs" --args "-d" --args "0" --args "--clientArgs" --args "-q"\n' % (executable, nCoresPerNode, nCores, nRepeats)
f += '#@class=prod\n'
f += '#@notification=complete\n'
f += '#@queue\n'

jobSubmitFile = f

out = open("bg_submit.txt", "w")
out.write(jobSubmitFile);
out.close()

qsub_command = "llsubmit bg_submit.txt"
print qsub_command
qsub_text = os.popen(qsub_command).read()
print qsub_text

qstat_command = "llq -u wws65-txs10"
print qstat_command
qstat_text = os.popen(qstat_command).read()
print qstat_text

job_number = int(re.findall(r'\d+', qsub_text)[1])
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

