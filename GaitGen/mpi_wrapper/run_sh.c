#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

int main(int argc, char **argv)
{
	int nprocs, myrank, rc;
        char command[1024];
        int i;

        printf("argc %d\n", argc);
        for (i = 0; i < argc; i++) printf("%s\n", argv[i]);
        
	rc = MPI_Init(&argc, &argv);
	rc = MPI_Comm_size(MPI_COMM_WORLD, &nprocs);
	rc = MPI_Comm_rank(MPI_COMM_WORLD, &myrank);
        
        sprintf(command, "%s/cvs/GaitGen/bin/objective_socket --hostname ulgbc1.liv.ac.uk --runTimeLimit 85000", getenv("HOME"));
        printf("nprocs = %d\nmyrank = %d\n%s\n", nprocs, myrank, command);
        system(command);
        
	rc = MPI_Finalize();
        
        return 0;
}

