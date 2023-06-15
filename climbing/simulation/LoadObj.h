// LoadObj.h - routine to load a Wavefront .obj model for the
// geometry of a shape

// based on glLoadModels.cpp

// NB. Only works with obj files with triangular meshes, absolute
// references and no extra normal information. 

// This is quite restrictive but it's the only sort of obj file that
// I use.

#ifndef LoadObj_h
#define LoadObj_h

#include <gl.h>

GLuint LoadObj(const char *model_filename);
void SetGLColour(GLfloat red, GLfloat green, GLfloat blue,
      GLfloat ambient, GLfloat diffuse, GLfloat specular,
      GLfloat specularPower);

#endif // LoadObj_h
