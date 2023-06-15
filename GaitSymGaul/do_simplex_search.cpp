// function to perform a simplex search

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

#include "PGDMath.h"
#include "XMLConverter.h"
#include "Simulation.h"
#include "Util.h"

#include "gaul.h"

#include "utilities.h"
#include "callbacks.h"

#include "do_simplex_search.h"

extern double g_mutation_sd;
extern XMLConverter g_XMLConverter;
extern Simulation *gSimulation;

void do_simplex_search(DataFile *config_file)
{
    int population_size;
    int chromosome_size;
    char model_config_file[512];
    int simplex_max_iterations;
    double simplex_initial_stepsize;

    int gaul_log_level = 4; // 0 none, 1 fatal, 2, warnings, 3 normal, 4 verbose, 5 fixme, 6 debug
    char starting_population_file[512];
    starting_population_file[0] = 0;

    try
    {
        THROWIF(config_file->RetrieveQuotedStringParameter("model_config_file", model_config_file, 512));
        THROWIF(config_file->RetrieveParameter("population_size", &population_size));
        THROWIF(config_file->RetrieveParameter("chromosome_size", &chromosome_size));
        THROWIF(config_file->RetrieveParameter("simplex_max_iterations", &simplex_max_iterations));
        THROWIF(config_file->RetrieveParameter("simplex_initial_stepsize", &simplex_initial_stepsize));

        config_file->RetrieveParameter("starting_population_file", starting_population_file, 512);
        config_file->RetrieveParameter("gaul_log_level", &gaul_log_level);
    }

    catch (int e)
    {
        dief("Error on line %d\nMissing parameter in config_file", e);
    }

    random_seed(config_file);

    if (g_XMLConverter.LoadBaseXMLFile(model_config_file)) dief("LoadBaseXMLFile \"%s\"error", model_config_file);

    // turn on logging
    char log_filename[512];
    strcpy(log_filename,"SimplexLog.txt");
    log_init((log_level_type)gaul_log_level, log_filename, 0, 1);

    population          *pop = 0;                   /* Population of solutions. */
    entity              *solution = 0;              /* Optimised solution. */

    char dir_name[256], output_file[256];
    create_output_folder(dir_name, sizeof(dir_name));

    // set up the initial population
    pop = ga_genesis_double(
            population_size,                    /* const int                population_size */
            1,                                  /* const int                num_chromo */
            chromosome_size,                    /* const int                len_chromo */
            NULL,                               /* GAgeneration_hook        generation_hook */
            iteration_callback,                 /* GAiteration_hook         iteration_hook */
            NULL,                               /* GAdata_destructor        data_destructor */
            NULL,                               /* GAdata_ref_incrementor   data_ref_incrementor */
            score,                              /* GAevaluate               evaluate */
            seed,                               /* GAseed                   seed */
            NULL,                               /* GAadapt                  adapt */
            NULL,                               /* GAselect_one             select_one */
            NULL,                               /* GAselect_two             select_two */
            NULL,                               /* GAmutate                 mutate */
            NULL,                               /* GAcrossover              crossover */
            NULL,                               /* GAreplace                replace */
            NULL                                /* vpointer                 User data */
            );

    ga_population_set_simplex_parameters(
               pop,                                /* population               *pop */
               chromosome_size,                    /* const int                num_dimensions */
               simplex_initial_stepsize,           /* const double             Initial step size. */
               to_double,                          /* const GAto_double        to_double */
               from_double                         /* const GAfrom_double      from_double */
               );

    // allele ranges needed for setting the search limits
    ga_population_set_allele_min_double(pop, 0.0);
    ga_population_set_allele_max_double(pop, 1.0);

    ga_population_seed(pop);

    // optionally load up a user population
    if (starting_population_file[0]) load_population(pop, starting_population_file);

    /* Evaluate and sort the initial population members (i.e. select best of the random solutions. */
    // this may actually not be needed for user supplied populations if they are already sorted by fitness
    ga_population_score_and_sort(pop);
    plog(LOG_NORMAL, "Best starting fitness =  %g", pop->entity_iarray[0]->fitness);

    // now we loop through the population using each member in turn as the starting point

    for (int i_ent = 0; i_ent < pop->stable_size; i_ent++)
    {
        solution = ga_get_entity_from_rank(pop, i_ent);

        ga_simplex(
                    pop,                                /* population               *pop */
                    solution,                           /* entity                   *solution */
                    simplex_max_iterations              /* const int                max_iterations */
                    );

        plog(LOG_NORMAL, "RESULT %d Simplex Fitness = %g", i_ent, solution->fitness);

        // output the best solution
        double *array = new double[pop->len_chromosomes];
        for (int i = 0; i < pop->len_chromosomes; i++)
            array[i] = ((double *)solution->chromosome[0])[i];
        g_XMLConverter.ApplyGenome(pop->len_chromosomes, array);
        int docTxtLen;
        xmlChar* xml = g_XMLConverter.GetFormattedXML(&docTxtLen);

        sprintf(output_file, "%s/%05dBestGenome.xml", dir_name, i_ent);
        plog(LOG_VERBOSE, "Creating %s", output_file);
        std::ofstream best_genome_xml(output_file);
        best_genome_xml << xml;
        best_genome_xml.close();

        sprintf(output_file, "%s/%05dBestGenome.txt", dir_name, i_ent);
        plog(LOG_VERBOSE, "Creating %s", output_file);
        std::ofstream best_genome_text(output_file);
        best_genome_text.precision(17);
        best_genome_text.setf(std::ios::scientific, std:: ios::floatfield);
        best_genome_text << "-2\n" << pop->len_chromosomes << "\n";
        for (int i = 0; i < pop->len_chromosomes; i++)
            best_genome_text << array[i] << " 0.0 1.0 0.1 0\n";
        best_genome_text << solution->fitness << " 0 0 0 0\n";
        best_genome_text.close();
        delete [] array;
    }

    ga_extinction(pop);

}

