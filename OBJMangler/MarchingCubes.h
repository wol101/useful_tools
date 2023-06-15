/*
 * Polygonising A Scalar Field
 *
 * Also known as: "3D Contouring", "Marching Cubes", "Surface Reconstruction"
 * Written by Paul Bourke
 * May 1994
 *
 * Based on tables by Cory Gene Bloyd
 * 
 * http://local.wasp.uwa.edu.au/~pbourke/geometry/polygonise/
 *
 */

typedef struct {
    double x,y,z;
} XYZ;

typedef struct {
    XYZ p[3];
} TRIANGLE;

typedef struct {
    XYZ p[8];
    double val[8];
} GRIDCELL;

int Polygonise(GRIDCELL grid, double isolevel, TRIANGLE *triangles);
XYZ VertexInterp(double isolevel, XYZ p1, XYZ p2, double valp1, double valp2);
int PolygoniseTri(GRIDCELL g, double iso, TRIANGLE *tri, int v0, int v1, int v2, int v3);

