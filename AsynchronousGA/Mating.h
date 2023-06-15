/*
 *  Mating.h
 *  AsynchronousGA
 *
 *  Created by Bill Sellers on 19/10/2010.
 *  Copyright 2010 Bill Sellers. All rights reserved.
 *
 */

#ifndef MATING_H
#define MATING_H

namespace AsynchronousGA
{

class Genome;
class Population;

enum CrossoverType
{
    OnePoint = 0,
    Average = 1
};

int Mate(Genome *parent1, Genome *parent2, Genome *offspring, CrossoverType type);
int GaussianMutate(Genome *genome, double mutationChance);
int MultipleGaussianMutate(Genome *genome, double mutationChance);
int FrameShiftMutate(Genome *genome, double mutationChance);
int DuplicationMutate(Genome *genome, double mutationChance);

}

#endif // MATING_H
