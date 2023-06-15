cd /home/horace/mqbssws2/cvs/MPIWrapper  
thedate=`date +%Y-%m-%d_%H.%M.%S`
#BSUB -n 64
#BSUB -W 23:59
#BSUB -J gaitsym 
#BSUB -o stdout.%J
#BSUB -e stderr.%J
prun -n 64 ./mpiw /home/horace/mqbssws2/cvs/GaitSymODE/bin/gaitsym_socket -L /home/horace/mqbssws2/remote/hosts.txt
