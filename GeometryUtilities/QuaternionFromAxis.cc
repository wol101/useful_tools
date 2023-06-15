// Command line routine to create a quaternion from an axis and an angle

#include <stdlib.h>
#include <float.h>
#include <math.h>
#include <iostream>

#include "mymath.h"

static const char *usage = "QuaternionFromAxis x y z angle_degrees";

int main(int argc, char *argv[])
{
    if (argc != 5)
    {
        std::cerr << usage << "\n";
        return 1;
    }

    double x = strtod(argv[1], 0);
    double y = strtod(argv[2], 0);
    double z = strtod(argv[3], 0);
    
    Vector v(x, y, z);
    v.Normalize();
    
    double angle = (M_PI / 180) * strtod(argv[4], 0);
    
    double sin_a = sin( angle / 2 );
    double cos_a = cos( angle / 2 );
        
    double qx    = v.x * sin_a;
    double qy    = v.y * sin_a;
    double qz    = v.z * sin_a;    
    double qw    = cos_a;
    
    std::cout.precision(DBL_DIG + 2);
    std::cout << qx << " " << qy << " " << qz << " " << qw << "\n";
}
