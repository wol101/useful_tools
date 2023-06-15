#ifndef CALLBACKS_H
#define CALLBACKS_H

// these are the callback functions that GAUL needs for various operations during optimisation

#include "gaul.h"

boolean score(population *pop, entity *entity);
boolean iteration_callback(int iteration, entity *solution);
boolean generation_callback(int generation, population *pop);
boolean seed(population *pop, entity *adam);
void ga_mutate_double_multipoint_usersd(population *pop, entity *father, entity *son);
boolean mutate_allele(population *pop, entity *father, entity *son, const int chromo_id, const int allele_id);
boolean to_double(population *pop, entity *entity, double *array);
boolean from_double(population *pop, entity *entity, double *array);

#endif // CALLBACKS_H

