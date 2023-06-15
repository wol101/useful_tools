#ifndef __UTIL_H__
#define __UTIL_H__

#include <math.h>

#define square(a) ((a) * (a))

class Util {
public:

    // INVERSION OF A MATRIX WITH GAUSS-JORDAN'S METHOD
    // modified from Numerical Recipes and fixed at 3 by 3
    // returns 0 if successful

    static int Gaussj3x3(double a[3][3]);

    // calculate cross product (vector product)
    inline static void CrossProduct3x1(const double *a, const double *b, double *c)
    {
        c[0] = a[1] * b[2] - a[2] * b[1];
        c[1] = a[2] * b[0] - a[0] * b[2];
        c[2] = a[0] * b[1] - a[1] * b[0];
    };

    // calculate dot product (scalar product)
    inline static double DotProduct3x1(const double *a, const double *b)
    {
        return a[0] * b[0] + a[1] * b[1] + a[2] * b[2];
    };

    // calculate length of vector
    inline static double Magnitude3x1(const double *a)
    {
        return sqrt(square(a[0]) + square(a[1]) + square(a[2]));
    };

    // calculate distance between two points
    inline static double Distance3x1(const double *a, const double *b)
    {
        return sqrt(square(a[0] - b[0]) + square(a[1] - b[1]) + square(a[2] - b[2]));
    };

    // calculate unit vector
    inline static void Unit3x1(double *a)
    {
        double len = sqrt(square(a[0]) + square(a[1]) + square(a[2]));
        // default fixup for zero length vectors
        if (fabs(len) < 1e-30)
        {
            a[0] = 1;
            a[1] = 0;
            a[2] = 0;
        }
        else
        {
            a[0] /= len;
            a[1] /= len;
            a[2] /= len;
        }
    };

    // c = a + b vectors
    inline static void Add3x1(const double *a, const double *b, double *c)
    {
        c[0] = a[0] + b[0];
        c[1] = a[1] + b[1];
        c[2] = a[2] + b[2];
    };

    // c = a - b vectors
    inline static void Subtract3x1(const double *a, const double *b, double *c)
    {
        c[0] = a[0] - b[0];
        c[1] = a[1] - b[1];
        c[2] = a[2] - b[2];
    };

    // c = scalar * a
    inline static void ScalarMultiply3x1(const double scalar, const double *a, double *c)
    {
        c[0] = a[0] * scalar;
        c[1] = a[1] * scalar;
        c[2] = a[2] * scalar;
    };

    // b = a
    inline static void Copy3x1(const double *a, double *b)
    {
        b[0] = a[0];
        b[1] = a[1];
        b[2] = a[2];
    };


    inline static void MDHTransform(double a, double alpha, double d, double theta,
                                    double *location)
    {
        double internal[3];

        // translate back to origin

        internal[0] = location[0] - a;
        internal[1] = location[1];
        internal[2] = location[2] - d;

        // rotation code from dmMDHLink::rtxToInboard

        double calpha = cos(alpha);
        double salpha = sin(alpha);
        double ctheta = cos(theta);
        double stheta = sin(theta);
        double temp1;

        // z planar rotation:
        location[0] = internal[0]*ctheta - internal[1]*stheta;
        temp1 = internal[1]*ctheta + internal[0]*stheta;

        // x planar rotation
        location[1] = temp1*calpha - internal[2]*salpha;
        location[2] = internal[2]*calpha + temp1*salpha;

        // translate back to original position

        location[0] += a;
        location[2] += d;
    };

    inline static bool OutRange(double v, double l, double h)
    {
        if (v < l)
            return true;
        if (v > h)
            return true;
        return false;
    }

    // convert a link local coordinate to a world coordinate

    inline static void
        ConvertLocalToWorldP(const dmABForKinStruct *m, const CartesianVector local, CartesianVector world)
    {
            for (int j = 0; j < 3; j++)
            {

                world[j] = m->p_ICS[j] +
                m->R_ICS[j][0] * local[0] +
                m->R_ICS[j][1] * local[1] +
                m->R_ICS[j][2] * local[2];
            }
    }

    // convert a link local coordinate to a world coordinate
    // don't know if this is actually corect for the rotations (are they
    // global or local?)

    inline static void
        ConvertLocalToWorldV(const dmABForKinStruct *m, const SpatialVector local, SpatialVector world)
    {
            for (int j = 0; j < 3; j++)
            {
                world[j] =
                m->R_ICS[j][0] * local[0] +
                m->R_ICS[j][1] * local[1] +
                m->R_ICS[j][2] * local[2];

                world[j + 3] =
                    m->R_ICS[j][0] * local[0 + 3] +
                    m->R_ICS[j][1] * local[1 + 3] +
                    m->R_ICS[j][2] * local[2 + 3];
            }
    }

};



#endif                   // __UTIL_H__
