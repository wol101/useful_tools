#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mpi.h>

int main(int argc, char **argv)
{
    int nprocs, myrank, rc;
    char *command;
    char *serverCommand = 0;
    char *clientCommand = 0;
    int i;
    int dummyArgc = 1;
    FILE *serverStream;
    int debug = 0;

    for (i = 1; i < argc; i++)
    {
        // do something with arguments

        if (strcmp(argv[i], "--client") == 0)
        {
            i++;
            if (i >= argc) goto err;
            clientCommand = argv[i];
        }

        else if (strcmp(argv[i], "--server") == 0)
        {
            i++;
            if (i >= argc) goto err;
            serverCommand = argv[i];
        }

        else if (strcmp(argv[i], "--debug") == 0)
        {
            debug++;
        }
        
        else 
        {
err:     
            printf("Usage: MPIClientServer args\n");
            printf("--client \"client command line (full path)\"\n");
            printf("--server \"server command line (full path)\"\n");
            printf("--debug\n");
            return 1;
        }
    }

    if (serverCommand == 0) goto err;
    if (clientCommand == 0) goto err;

    rc = MPI_Init(&argc, &argv);
    rc = MPI_Comm_size(MPI_COMM_WORLD, &nprocs);
    rc = MPI_Comm_rank(MPI_COMM_WORLD, &myrank);

    command = malloc(strlen(serverCommand) + strlen(clientCommand) + 256); // guaranteed big enough!

    /* run server command */
    if (myrank == 0) 
    {
        if (debug == 0)
            sprintf(command, "%s --MPI_Comm_size %d --MPI_Comm_rank %d", serverCommand, nprocs, myrank);
        else
            sprintf(command, "%s --MPI_Comm_size %d --MPI_Comm_rank %d > server_stdout%04d.log 2> server_stderr%04d.log", serverCommand, nprocs, myrank, myrank, myrank);
        printf("%s\n", command);
        system(command);
    }

    /* run client command */
    else
    {
        if (debug == 0)
            sprintf(command, "%s --MPI_Comm_size %d --MPI_Comm_rank %d", clientCommand, nprocs, myrank);
        else
            sprintf(command, "%s --MPI_Comm_size %d --MPI_Comm_rank %d > client_stdout%04d.log 2> client_stderr%04d.log", clientCommand, nprocs, myrank, myrank, myrank);
        printf("%s\n", command);
        system(command);
    }

    rc = MPI_Finalize();

    free(command);

    return 0;
}

