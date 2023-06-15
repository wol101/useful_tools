#include "EulerAngles.h"  /* to convert static X, Y, Z angles to static Z, Y, X */
#include <stdio.h>
#include <math.h>

#define TODEG(x)    x = x * 180 / M_PI;
#define TORAD(x)    x = x / 180 * M_PI;

main() {
    HMatrix joe;
    EulerAngles sa, ra;
    
    printf("enter static angles (X, Y, Z)-> ");
    while(scanf("%f%f%f", &sa.x, &sa.y, &sa.z)==3)
      {
	TORAD(sa.x);
	TORAD(sa.y);
	TORAD(sa.z);

	sa.w = EulOrdXYZs;
	Eul_ToHMatrix(sa, joe);
	ra = Eul_FromHMatrix(joe, EulOrdZYXs);
	TODEG(ra.x);
	TODEG(ra.y);
	TODEG(ra.z);
	printf("static angles (Z, Y, X) = %.16f %.16f %.16f\n", ra.x, ra.y, ra.z);

	printf("\nenter angles -> ");
      }
}

