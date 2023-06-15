/*
 *  MyVertex.h
 *  OBJMangler
 *
 *  Created by Bill Sellers on 05/03/2009.
 *  Copyright 2009 Bill Sellers. All rights reserved.
 *
 */

#ifndef MyVertex_h
#define MyVertex_h

// including this automatically defines the boolean operators based on operator== and operator<
#include <utility>


struct MyVertex
{
    double x;
    double y;
    double z;
    
    static double epsilon;
    
    inline MyVertex()
    {
        x = y = z = 0;
    }
    
    inline MyVertex(double xx, double yy, double zz)
    {
        x = xx; y = yy; z = zz;
    }
    
    inline MyVertex& operator=(const MyVertex &u)
    {
        x = u.x;
        y = u.y;
        z = u.z;
        return *this;
    }

    inline static void SetEpsilon(double e)
    {
        epsilon = e;
    }

    // operator< needed for map to work
    inline bool operator<(const MyVertex &u) const
    {
        if (this->x < u.x - epsilon) return true;
        if (this->x > u.x + epsilon) return false;
        if (this->y < u.y - epsilon) return true;
        if (this->y > u.y + epsilon) return false;
        if (this->z < u.z - epsilon) return true;
        // if (this->z > u.z + epsilon) return false;
        return false;
    }
 
    inline bool operator==(const MyVertex &u) const
    {
        if (this->x < u.x - epsilon) return false;
        if (this->x > u.x + epsilon) return false;
        if (this->y < u.y - epsilon) return false;
        if (this->y > u.y + epsilon) return false;
        if (this->z < u.z - epsilon) return false;
        if (this->z > u.z + epsilon) return false;
        return true;
    }
    
    
};



#endif
