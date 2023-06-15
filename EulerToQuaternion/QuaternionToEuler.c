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
  EulerAngles inAngs;
  Quat qu;

  if (argc <= 1)
  {
    printf("Convert Quaternion Euler Angle\n");
    printf("X, Y, Z rotations in degrees around static axes\n");

    while(1)
    {
      printf("Quaternion (ODE order) (w, x, y, z):\n");
      scanf("%lf %lf %lf %lf", &qu.w, &qu.x, &qu.y, &qu.z);

      inAngs = Eul_FromQuat(qu, EulOrdXYZs);

      inAngs.x = TODEG(inAngs.x);
      inAngs.y = TODEG(inAngs.y);
      inAngs.z = TODEG(inAngs.z);

      printf("Euler Angles (X,Y,Z degrees):\n");
      printf("%.16f %.16f %.16f\n", inAngs.x, inAngs.y, inAngs.z);
    }

    return 0;
  }
  
  if (argc != 5)
  {
    printf("Usage: QuaternionToEuler x y z w\n");
    return 1;
  }
  
  qu.x = strtod(argv[1], 0);
  qu.y = strtod(argv[2], 0);
  qu.z = strtod(argv[3], 0);
  qu.w = strtod(argv[4], 0);
  
  inAngs = Eul_FromQuat(qu, EulOrdXYZs);

  inAngs.x = TODEG(inAngs.x);
  inAngs.y = TODEG(inAngs.y);
  inAngs.z = TODEG(inAngs.z);

  printf("%.16f %.16f %.16f\n", inAngs.x, inAngs.y, inAngs.z);
  
  return 0;
}  
