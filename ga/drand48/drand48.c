/*
 * Copyright (c) 1993 Martin Birgmeier
 * All rights reserved.
 *
 * You may redistribute unmodified or modified versions of this source
 * code provided that the above copyright notice and this and the
 * following conditions are retained.
 *
 * This software is provided ``as is'', and comes with no warranties
 * of any kind. I shall in no event be liable for anything that happens
 * to anyone/anything when using this software.
 */
 
/*
 * modified to compile in MetroWerks C++, D. Eppstein, April 1997
 * (my changes marked with "DE")
 */

#include "rand48.h"

/* declare erand48 - DE */
extern double erand48(unsigned short xseed[3]);

extern unsigned short _rand48_seed[3];

double
drand48(void)
{
	return erand48(_rand48_seed);
}
