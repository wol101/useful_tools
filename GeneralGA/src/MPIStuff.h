/*
 *  MPIStuff.h
 *  GeneralGA
 *
 *  Created by Bill Sellers on 06/09/2008.
 *  Copyright 2008 Bill Sellers. All rights reserved.
 *
 */

struct MPIRunSpecifier
{
    double startTime;
    int mpiSource;
};

#define MPI_MESSAGE_ID_RELOAD_MODELCONFIG 100
#define MPI_MESSAGE_ID_SEND_GENOME_DATA   101
#define MPI_MESSAGE_ID_SEND_RESULTS       102
#define MPI_MESSAGE_ID_SEND_TIMINGS       103
#define MPI_MESSAGE_ID_ABORT_CLIENTS      104
