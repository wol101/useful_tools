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
  EulerAngles inAngs;
  HMatrix M;

  if (argc <= 1)
  {
    printf("Convert Matrix to Euler Angle\n");
    printf("X, Y, Z rotations in degrees around static axes\n");

    while(1)
    {
      printf("Matrix by rows (r1c1 r1c2 r1c3 r2c1 r2c2 r2c3 r3c1 r3c2 r3c3):\n");
      scanf("%lf%lf%lf%lf%lf%lf%lf%lf%lf", &M[0][0], &M[0][1], &M[0][2], &M[1][0], &M[1][1], &M[1][2], &M[2][0], &M[2][1], &M[2][2]);
      M[0][3] = M[1][3] = M[2][3] = 0;
      M[3][0] = M[3][1] = M[3][2] = 0;
      M[3][3] = 1;

      inAngs = Eul_FromHMatrix(M, EulOrdXYZs);

      inAngs.x = TODEG(inAngs.x);
      inAngs.y = TODEG(inAngs.y);
      inAngs.z = TODEG(inAngs.z);

      printf("Euler Angles (X,Y,Z degrees):\n");
      printf("%.16f %.16f %.16f\n", inAngs.x, inAngs.y, inAngs.z);
    }

    return 0;
  }
  
  if (argc != 10)
  {
    printf("Usage: MatrixToEuler r1c1 r1c2 r1c3 r2c1 r2c2 r2c3 r3c1 r3c2 r3c3 \n");
    return 1;
  }
  
  M[0][0] = strtod(argv[1], 0);
  M[0][1] = strtod(argv[2], 0);
  M[0][2] = strtod(argv[3], 0);
  M[1][0] = strtod(argv[4], 0);
  M[1][1] = strtod(argv[5], 0);
  M[1][2] = strtod(argv[6], 0);
  M[2][0] = strtod(argv[7], 0);
  M[2][1] = strtod(argv[8], 0);
  M[2][2] = strtod(argv[9], 0);

  M[0][3] = M[1][3] = M[2][3] = 0;
  M[3][0] = M[3][1] = M[3][2] = 0;
  M[3][3] = 1;
  
  inAngs = Eul_FromHMatrix(M, EulOrdXYZs);

  inAngs.x = TODEG(inAngs.x);
  inAngs.y = TODEG(inAngs.y);
  inAngs.z = TODEG(inAngs.z);

  printf("%.16f %.16f %.16f\n", inAngs.x, inAngs.y, inAngs.z);
  
  return 0;
}  
