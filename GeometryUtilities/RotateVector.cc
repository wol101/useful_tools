// Command line routine to multiply a vector by a quaternion

#include <stdlib.h>
#include <float.h>
#include <iostream>

#include "mymath.h"

static const char *usage = "RotateVector vx vy vz qx qy qz qw";

int main(int argc, char *argv[])
{
    if (argc != 8)
    {
        std::cerr << usage << "\n";
        return 1;
    }

    double vx = strtod(argv[1], 0);
    double vy = strtod(argv[2], 0);
    double vz = strtod(argv[3], 0);
    
    double qx = strtod(argv[4], 0);
    double qy = strtod(argv[5], 0);
    double qz = strtod(argv[6], 0);
    double qw = strtod(argv[7], 0);
    
    Vector v1(vx, vy, vz);
    Quaternion q1(qw, qx, qy, qz);
    
    Vector v2 = QVRotate(q1, v1);
    
    std::cout.precision(DBL_DIG + 2);
    std::cout << v2.x << " " << v2.y << " " << v2.z << "\n";
}
