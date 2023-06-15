// Euler X, Y, Z static angle to Matrix representation

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
  HMatrix M;

  if (argc <= 1)
  {
    printf("Convert Euler Angles to Matrix\n");
    printf("X, Y, Z rotations in degrees around static axes\n");

    while(1)
    {
      printf("Input 3 Euler Angles (X,Y,Z degrees):\n");
      scanf("%lf %lf %lf",&inAngs.x,&inAngs.y,&inAngs.z);

      inAngs.x = TORAD(inAngs.x);
      inAngs.y = TORAD(inAngs.y);
      inAngs.z = TORAD(inAngs.z);

      Eul_ToHMatrix(inAngs, M);

      printf("Matrix:\n");
#ifdef MATRIX4X4
      printf("[ %19.16f %19.16f %19.16f %19.16f ]\n", M[0][0], M[0][1], M[0][2], M[0][3]);
      printf("[ %19.16f %19.16f %19.16f %19.16f ]\n", M[1][0], M[1][1], M[1][2], M[1][3]);
      printf("[ %19.16f %19.16f %19.16f %19.16f ]\n", M[2][0], M[2][1], M[2][2], M[2][3]);
      printf("[ %19.16f %19.16f %19.16f %19.16f ]\n", M[3][0], M[3][1], M[3][2], M[3][3]);
#else
      printf("[ %19.16f %19.16f %19.16f ]\n", M[0][0], M[0][1], M[0][2]);
      printf("[ %19.16f %19.16f %19.16f ]\n", M[1][0], M[1][1], M[1][2]);
      printf("[ %19.16f %19.16f %19.16f ]\n", M[2][0], M[2][1], M[2][2]);
#endif
    }

    return 0;
  }
  
  if (argc != 4)
  {
    printf("Usage: EulerToMatrix x y z\n");
    return 1;
  }
  
  inAngs.x = strtod(argv[1], 0);
  inAngs.y = strtod(argv[2], 0);
  inAngs.z = strtod(argv[3], 0);
  
  inAngs.x = TORAD(inAngs.x);
  inAngs.y = TORAD(inAngs.y);
  inAngs.z = TORAD(inAngs.z);

  Eul_ToHMatrix(inAngs, M);

  printf("Matrix:\n");
#ifdef MATRIX4X4
      printf("[ %19.16f %19.16f %19.16f %19.16f ]\n", M[0][0], M[0][1], M[0][2], M[0][3]);
      printf("[ %19.16f %19.16f %19.16f %19.16f ]\n", M[1][0], M[1][1], M[1][2], M[1][3]);
      printf("[ %19.16f %19.16f %19.16f %19.16f ]\n", M[2][0], M[2][1], M[2][2], M[2][3]);
      printf("[ %19.16f %19.16f %19.16f %19.16f ]\n", M[3][0], M[3][1], M[3][2], M[3][3]);
#else
      printf("[ %19.16f %19.16f %19.16f ]\n", M[0][0], M[0][1], M[0][2]);
      printf("[ %19.16f %19.16f %19.16f ]\n", M[1][0], M[1][1], M[1][2]);
      printf("[ %19.16f %19.16f %19.16f ]\n", M[2][0], M[2][1], M[2][2]);
#endif
  
  return 0;
}  
