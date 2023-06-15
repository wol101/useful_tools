#include "EulerAngles.h"  /* to convert static angles to rotating ones */
#include <stdio.h>
#include <math.h>

#define TODEG(x)    x = x * 180 / M_PI;
#define TORAD(x)    x = x / 180 * M_PI;

main() {
    HMatrix joe;
    EulerAngles sa, ra;
    
    printf("enter angles -> ");
    while(scanf("%f%f%f", &sa.x, &sa.y, &sa.z)==3)
      {
	TORAD(sa.x);
	TORAD(sa.y);
	TORAD(sa.z);

	sa.w = EulOrdXYZs;
	Eul_ToHMatrix(sa, joe);
	ra = Eul_FromHMatrix(joe, EulOrdXYZr);
	TODEG(ra.x);
	TODEG(ra.y);
	TODEG(ra.z);
	printf("rotating angles = %10.3f %10.3f %10.3f\n", ra.x, ra.y, ra.z);

	sa.w = EulOrdXYZr;
	Eul_ToHMatrix(sa, joe);
	ra = Eul_FromHMatrix(joe, EulOrdXYZs);
	TODEG(ra.x);
	TODEG(ra.y);
	TODEG(ra.z);
	printf("  static angles = %10.3f %10.3f %10.3f\n", ra.x, ra.y, ra.z);
	printf("\nenter angles -> ");
      }
}

