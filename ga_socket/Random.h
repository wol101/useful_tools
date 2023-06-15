// Random.h - some random number routines

#ifndef RANDOM_H
#define RANDOM_H

#define USE_DRAND48

// RANDOM_SEED and RANDOM_FLOAT can be defined as system random
// number generators (eg. srand48 and drand48) or one of the 3
// inbuilt random number generators
// sran2 and ran2 are recommended for general use...
// RANDOM_SEED_TYPE needs to match the required type for the random seed

#ifdef USE_DRAND48
#define RANDOM_SEED_TYPE long
#define RANDOM_SEED srand48
#define RANDOM_FLOAT drand48
#else
#define RANDOM_SEED_TYPE unsigned int
#define RANDOM_SEED sran2
#define RANDOM_FLOAT ran2
#endif

void RandomSeed(RANDOM_SEED_TYPE randomSeed);
double RandomDouble(double lowBound, double highBound);
int RandomInt(int lowBound, int highBound);
bool CoinFlip(double chanceOfReturningTrue = 0.5);
int SqrtBiasedRandomInt(int lowBound, int highBound);
double RandomUnitGaussian();
int RankBiasedRandomInt(int lowBound, int highBound);

#endif // RANDOM_H
