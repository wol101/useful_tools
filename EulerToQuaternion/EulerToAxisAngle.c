// Euler X, Y, Z static angle to Axis Angle representation

/* EulerSample.c - Read angles as quantum mechanics, write as aerospace */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "EulerAngles.h"

#define TODEG(x)    ((x) * 180 / M_PI)
#define TORAD(x)    ((x) / 180 * M_PI)

void QuaternionToAxisAngle(Quat qu, double *ax, double *ay, double *az, double *radians);

int main(int argc, char *argv[])
{
    EulerAngles inAngs = {0,0,0,EulOrdXYZs};
    Quat qu;
    double ax, ay, az, radians;

    if (argc <= 1)
    {
        printf("Convert Euler Angles to Quaternion\n");
        printf("X, Y, Z rotations in degrees around static axes\n");

        while(1)
        {
            printf("Input 3 Euler Angles (X,Y,Z degrees):\n");
            scanf("%lf %lf %lf\n",&inAngs.x,&inAngs.y,&inAngs.z);

            inAngs.x = TORAD(inAngs.x);
            inAngs.y = TORAD(inAngs.y);
            inAngs.z = TORAD(inAngs.z);

            qu = Eul_ToQuat(inAngs);
            QuaternionToAxisAngle(qu, &ax, &ay, &az, &radians);

            printf("Axis (x, y, z); Angle (degrees):\n");
            printf("%.16f %.16f %.16f %.16f\n", ax, ay, az, TODEG(radians));
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
    QuaternionToAxisAngle(qu, &ax, &ay, &az, &radians);

    printf("%.16f %.16f %.16f %.16f\n", ax, ay, az, TODEG(radians));

    return 0;
}  

void QuaternionToAxisAngle(Quat qu, double *ax, double *ay, double *az, double *radians)
{
    double magnitude;
    *radians = (2 * acos(qu.w));
    magnitude = sqrt(qu.x * qu.x + qu.y * qu.y + qu.z * qu.z);
    *ax = qu.x / magnitude;
    *ay = qu.y / magnitude;
    *az = qu.z / magnitude;
}
