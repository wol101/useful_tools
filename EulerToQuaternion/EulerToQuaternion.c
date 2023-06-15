// Euler X, Y, Z static angle to Quaternion representation

/* EulerSample.c - Read angles as quantum mechanics, write as aerospace */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "EulerAngles.h"

#define TODEG(x)    ((x) * 180 / M_PI);
#define TORAD(x)    ((x) / 180 * M_PI);

int main(int argc, char *argv[])
{
  EulerAngles inAngs = {0,0,0,EulOrdXYZs};
  Quat qu;

  if (argc <= 1)
  {
    printf("Convert Euler Angles to Quaternion\n");
    printf("X, Y, Z rotations in degrees around static axes\n");

    while(1)
    {
      printf("Input 3 Euler Angles (X,Y,Z degrees):\n");
      scanf("%lf %lf %lf",&inAngs.x,&inAngs.y,&inAngs.z);

      inAngs.x = TORAD(inAngs.x);
      inAngs.y = TORAD(inAngs.y);
      inAngs.z = TORAD(inAngs.z);

      qu = Eul_ToQuat(inAngs);

      printf("Quaternion (ODE order) (w, x, y, z):\n");
      printf("%.16f %.16f %.16f %.16f\n", qu.w, qu.x, qu.y, qu.z);
    }

    return 0;
  }
  
  if (argc != 4)
  {
    printf("Usage: EulerToQuaternion x y z\n");
    return 1;
  }
  
  inAngs.x = strtod(argv[1], 0);
  inAngs.y = strtod(argv[2], 0);
  inAngs.z = strtod(argv[3], 0);
  
  inAngs.x = TORAD(inAngs.x);
  inAngs.y = TORAD(inAngs.y);
  inAngs.z = TORAD(inAngs.z);

  qu = Eul_ToQuat(inAngs);

  printf("%.16f %.16f %.16f %.16f\n", qu.x, qu.y, qu.z, qu.w);
  
  return 0;
}  
