#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mpi.h>

int main(int argc, char **argv)
{
	int nprocs, myrank, rc;
        char command[100000];
        int i;
	int dummyArgc = 1;
	if (argc < 2)
	{
		printf("Usage: mpiw \"program command line (full path)\"\n");
		return 1;
	}

        printf("argc %d\n", argc);
        for (i = 0; i < argc; i++) printf("%s\n", argv[i]);
        
	rc = MPI_Init(&argc, &argv);
	rc = MPI_Comm_size(MPI_COMM_WORLD, &nprocs);
	rc = MPI_Comm_rank(MPI_COMM_WORLD, &myrank);
        
	/* create the new command string */
	strcpy(command, "");
        for (i = 1; i < argc; i++) 
	{
	    strcat(command, argv[i]);
	    strcat(command, " ");
	}
	
        printf("nprocs = %d\nmyrank = %d\n%s\n", nprocs, myrank, command);
        system(command);
        
	rc = MPI_Finalize();
        
        return 0;
}

