// some useful routines

#include <math.h>

#include "Util.h"

#define SWAP(a,b) {double temp=(a);(a)=(b);(b)=temp;}

// INVERSION OF A MATRIX WITH GAUSS-JORDAN'S METHOD
// modified from Numerical Recipes (gausj.c) and fixed at 3 by 3
// returns 0 if successful

// matrix order is:

/*

( a[0][0] a[1][0] a[2][0] )
( a[0][1] a[1][1] a[2][1] )
( a[0][2] a[1][2] a[2][2] )

*/

int
 Util::Gaussj3x3(double a[3][3])
{
	int indxc[3], indxr[3], ipiv[3];
	int i, icol = 0, irow = 0, j, k, l, ll;
	double big, dum, pivinv;

	for (j = 0; j < 3; j++)
		ipiv[j] = 0;
	for (i = 0; i < 3; i++)
	{
		big = 0.0;
		for (j = 0; j < 3; j++)
			if (ipiv[j] != 1)
				for (k = 0; k < 3; k++)
				{
					if (ipiv[k] == 0)
					{
						if (fabs(a[j][k]) >= big)
						{
							big = fabs(a[j][k]);
							irow = j;
							icol = k;
						}
					} else if (ipiv[k] > 1)
						return 1;
				}
		++(ipiv[icol]);
		if (irow != icol)
		{
			for (l = 0; l < 3; l++)
				SWAP(a[irow][l], a[icol][l]);
		}
		indxr[i] = irow;
		indxc[i] = icol;
		if (a[icol][icol] == 0.0)
			return 1;
		pivinv = 1.0 / a[icol][icol];
		a[icol][icol] = 1.0;
		for (l = 0; l < 3; l++)
			a[icol][l] *= pivinv;
		for (ll = 0; ll < 3; ll++)
			if (ll != icol)
			{
				dum = a[ll][icol];
				a[ll][icol] = 0.0;
				for (l = 0; l < 3; l++)
					a[ll][l] -= a[icol][l] * dum;
			}
	}
	for (l = 2; l >= 0; l--)
	{
		if (indxr[l] != indxc[l])
			for (k = 0; k < 3; k++)
				SWAP(a[k][indxr[l]], a[k][indxc[l]]);
	}
	// successful
	return 0;
}

#undef SWAP
