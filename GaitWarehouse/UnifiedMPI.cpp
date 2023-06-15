/*
 *  UnifiedMPI.cpp
 *  GaitSymODE
 *
 *  Created by Bill Sellers on 11/05/2014.
 *  Copyright 2014 Bill Sellers. All rights reserved.
 *
 */

#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <iostream>

#include "../GaitSymODE/src/MPIStuff.h"

int WarehouseProcessor(int argc, char *argv[]);
int GaitSymMain(int argc, char *argv[]);

static int CountTokens(char *string);
static int ReturnTokens(char *string, char *ptrs[], int size);

int gMPI_Comm_size = -1;
int gMPI_Comm_rank = -1;

int main(int argc, char **argv)
{
    int rc;
    char serverCommand[10000];
    char clientCommand[10000];
    int i;
    int debug = 0;
    int serverHousekeepingTime = 10;
    int repeats = 1;

    rc = MPI_Init(&argc, &argv);
    if (debug > 0) std::cout << "MPI_Init " << rc << "\n";

    serverCommand[0] = 0;
    clientCommand[0] = 0;
    bool serverCommandFlag = false;
    bool clientCommandFlag = false;
    for (i = 1; i < argc; i++)
    {
        if (strcmp(argv[i], "--clientArgs") == 0)
        {
            clientCommandFlag = true;
            serverCommandFlag = false;
            continue;
        }

        if (strcmp(argv[i], "--serverArgs") == 0)
        {
            clientCommandFlag = false;
            serverCommandFlag = true;
            continue;
        }

        if (strcmp(argv[i], "--debug") == 0)
        {
            debug++;
            continue;
        }

        if (strcmp(argv[i], "--repeats") == 0)
        {
            i = i + 1;
            if (i >= argc) break;
            repeats = strtol(argv[i], 0, 10);
            continue;
        }

        if (clientCommandFlag)
        {
            strcat(clientCommand, argv[i]);
            strcat(clientCommand, " ");
            continue;
        }

        if (serverCommandFlag)
        {
            strcat(serverCommand, argv[i]);
            strcat(serverCommand, " ");
            continue;
        }

err:
        printf("Usage: UnifiedMPI args\n");
        printf("--clientArgs \"client command line arguments\"\n");
        printf("--serverArgs \"server command line arguments\"\n");
        printf("--debug\n");
        printf("--repeats n\n");
        return 1;
    }

    // it isn't an error if there are no arguments
    //if (serverCommand[0] == 0) goto err;
    //if (clientCommand[0] == 0) goto err;

    char *serverArgs = new char[strlen(serverCommand) + 256];
    char *clientArgs = new char[strlen(clientCommand) + 256];
    sprintf(serverArgs, "server %s", serverCommand);
    sprintf(clientArgs, "client %s", clientCommand);
    char *pServerArgs[256];
    char *pClientArgs[256];
    int nServerArgs = ReturnTokens(serverArgs, pServerArgs, 256);
    int nClientArgs = ReturnTokens(clientArgs, pClientArgs, 256);

    if (debug > 0)
    {
        for (int i = 0; i < nServerArgs; i++) std::cout << pServerArgs[i] << "\n";
        for (int i = 0; i < nClientArgs; i++) std::cout << pClientArgs[i] << "\n";
    }

    rc = MPI_Comm_size(MPI_COMM_WORLD, &gMPI_Comm_size);
    rc = MPI_Comm_rank(MPI_COMM_WORLD, &gMPI_Comm_rank);
    if (debug > 0) std::cout << "Size " << gMPI_Comm_size << " Rank " << gMPI_Comm_rank << "\n";

    if (debug > 1)
    {
        time_t theTime = time(0);
        struct tm *theLocalTime = localtime(&theTime);
        char logname[256];
        sprintf(logname, "%04d_%04d-%02d-%02d_%02d.%02d.%02d.log",
                    gMPI_Comm_rank,
                    theLocalTime->tm_year + 1900, theLocalTime->tm_mon + 1,
                    theLocalTime->tm_mday,
                    theLocalTime->tm_hour, theLocalTime->tm_min,
                    theLocalTime->tm_sec);
        freopen(logname, "w", stderr);
    }

    /* run server command */
    if (gMPI_Comm_rank == 0)
    {
        if (debug > 0) std::cout << "Starting server\n";
        for (int iRepeats = 0; iRepeats < repeats; iRepeats++)
        {
            int err = WarehouseProcessor(nServerArgs, pServerArgs);
            if (err) std::cerr << "WarehouseProcessor error on line " << err << "\n";
        }
        if (debug > 0) std::cout << "Ending server\n";
#ifdef USE_MPI_ABORT
        MPI_Abort(MPI_COMM_WORLD, 1); // this causes problems on some setups due to logging it as an error
#else
        std::cerr << "Send abort message\n";
        int message = MPI_MESSAGE_ID_ABORT_CLIENTS;
        int message_size = sizeof(message);
        for (i = 1; i < gMPI_Comm_size; i++)
        {
            MPI_Send(&message,                  /* message buffer */
                     message_size,              /* number of data item */
                     MPI_BYTE,                  /* data items are bytes */
                     i,                         /* destination */
                     0,                         /* user chosen message tag */
                     MPI_COMM_WORLD);           /* default communicator */
        }

#endif
    }

    /* run client command */
    else
    {
        // the server needs to do some housekeeping before the clients start
        sleep(serverHousekeepingTime);
        if (debug > 0) std::cout << "Starting client\n";
        GaitSymMain(nClientArgs, pClientArgs);
        if (debug > 0) std::cout << "Ending client\n";
    }

    rc = MPI_Finalize();

    delete [] serverArgs;
    delete [] clientArgs;

    return 0;
}

// Count token utility
int CountTokens(char *string)
{
    char *p = string;
    bool inToken = false;
    int count = 0;

    while (*p != 0)
    {
        if (inToken == false && *p > 32)
        {
            inToken = true;
            count++;
            if (*p == '"')
            {
                p++;
                while (*p != '"')
                {
                    p++;
                    if (*p == 0) return count;
                }
            }
        }
        else if (inToken == true && *p <= 32)
        {
            inToken = false;
        }
        p++;
    }
    return count;
}

// Return tokens utility
// note string is altered by this routine
// if returned count is >= size then there are still tokens
// (this is probably an error status)
// recommend that tokens are counted first
int ReturnTokens(char *string, char *ptrs[], int size)
{
    char *p = string;
    bool inToken = false;
    int count = 0;

    while (*p != 0)
    {
        if (inToken == false && *p > 32)
        {
            inToken = true;
            if (count >= size) return count;
            ptrs[count] = p;
            count++;
            if (*p == '"')
            {
                p++;
                ptrs[count - 1] = p;
                while (*p != '"')
                {
                    p++;
                    if (*p == 0) return count;
                }
                *p = 0;
            }
        }
        else if (inToken == true && *p <= 32)
        {
            inToken = false;
            *p = 0;
        }
        p++;
    }
    return count;
}

