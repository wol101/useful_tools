/*
 *  main.cpp
 *  GaiSymGaul
 *
 *  Created by Bill Sellers on 14/04/2010.
 *  Copyright 2010 Bill Sellers. All rights reserved.
 *
 */

#include "do_genetic_algorithm.h"
#include "do_next_ascent_hillclimbing.h"
#include "do_random_ascent_hillclimbing.h"
#include "do_simplex_search.h"
#include "do_simulated_annealling.h"
#include "do_tabu_search.h"

#include "PGDMath.h"
#include "XMLConverter.h"
#include "Simulation.h"
#include "Util.h"

#include "gaul.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <sys/stat.h>
#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <libxml/xmlmemory.h>
#include <libxml/parser.h>

double g_mutation_sd = 1.0; // global mutation rate

XMLConverter g_XMLConverter;

// Simulation global
extern Simulation *gSimulation;

int main(int argc, char **argv)
{
    const char *config_file_name = "config.txt";

    // I like line buffering on stdout
    setlinebuf(stdout);
    // this would remove buffering completely
    // setvbuf(stdout, NULL, _IONBF, 0);

#if HAVE_MPI == 1
    int rc = MPI_Init(&argc, &argv);
    // std::cout << "MPI_Init " << rc << "\n";
#endif

    try
    {
        int i = 1;
        while (i < argc)
        {
            if (strcmp(argv[i], "-i") == 0)
            {
                i++;
                if (i >= argc) throw __LINE__;
                config_file_name = argv[i];
                i++;
                continue;
            }

            throw __LINE__;
        }
    }

    catch (int e)
    {
        dief("Error on line %d\nUsage: %s [-i config_file_name]", e, argv[0]);
    }

    DataFile config_file;
    config_file.SetExitOnError(false);
    if (config_file.ReadFile(config_file_name)) dief("ReadFile(%s) error", config_file_name);

    char search_function[512];
    if (config_file.RetrieveParameter("search_function", search_function, 512))
        dief("Must specify required search_function");

    if (strcmp(search_function, "genetic_algorithm") == 0) do_genetic_algorithm(&config_file);
    else if (strcmp(search_function, "simplex_search") == 0) do_simplex_search(&config_file);
    else if (strcmp(search_function, "tabu_search") == 0) do_tabu_search(&config_file);
    else if (strcmp(search_function, "simulated_annealling") == 0) do_simulated_annealling(&config_file);
    else if (strcmp(search_function, "next_ascent_hillclimbing") == 0) do_next_ascent_hillclimbing(&config_file);
    else if (strcmp(search_function, "random_ascent_hillclimbing") == 0) do_random_ascent_hillclimbing(&config_file);
    else dief("seach_function %s not recognised", search_function);

    exit(EXIT_SUCCESS);
}




