#!/bin/sh
#  -masterq ulgbc1.q -cwd -V -pe score 7
#$ -masterq ulgbc1.q -V -pe score 7 -l h_rt=24:00:00
scout -wait -F /users/rsavage/.score/ndfile.$JOB_ID -e /tmp/scrun.$JOB_ID -nodes=6x2 /users/rsavage/mpi/run_sh
