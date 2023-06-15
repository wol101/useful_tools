#include "EulerAngles.h"  
#include <stdio.h>
#include <math.h>

main()
{
	int i;
	HMatrix myHMatrix;
	EulerAngles myEulerAngles;
	
	myEulerAngles.x = M_PI/4;
	myEulerAngles.y = 0;
	myEulerAngles.z = 0;
	myEulerAngles.w = EulOrdXYZs;
	
	Eul_ToHMatrix(myEulerAngles, myHMatrix);
	
	for (i = 0; i < 4; i++)
		printf("[%5.2f %5.2f %5.2f %5.2f]\n", 
				myHMatrix[0][i], myHMatrix[1][i],
				myHMatrix[2][i], myHMatrix[3][i]);
}
