#include "Circle.h"
#include <stdlib.h>
#include <math.h>
#include <GLES/gl.h>

#define MGL_DATATYPE int
#define MGL_TYPE GL_FIXED
//int to fixed point
#define iX(x) (x<<16)
//float to fixed point
#define fX(x) ((int)(x * (1  << 16)))

static size_t nverts;
static MGL_DATATYPE* verts = NULL;
static float* texcoords = NULL;

void Circle::create (unsigned numSubdiv) {
  if (verts != NULL) {
    delete [] verts;
    delete [] texcoords;
  }
  nverts = numSubdiv+2;
  verts = new MGL_DATATYPE[nverts*3];
  texcoords = new float[nverts*2];

  const float angleIncr = 2.0f * M_PI / (float) numSubdiv;

  verts[0] = 0;
  verts[1] = 0;
  verts[2] = 0;
  texcoords[0] = texcoords[1] = 0;
  //texcoords[0] = texcoords[1] = 0.5f;
  float angle = 0;
  for (size_t i=1; i<nverts; i++, angle+=angleIncr) {
    verts[3 * i] = fX(0.5f*cosf(angle));
    verts[3 * i + 1] = fX(0.5f*sinf(angle));
    verts[3 * i + 2] = 0;

    texcoords[2 * i] = 1.0f;
    texcoords[2 * i + 1] = 0;
   }
}

void Circle::draw (bool enableTexture) {
  glVertexPointer(3, MGL_TYPE, 0, verts);
  if (enableTexture)
    glTexCoordPointer(2, MGL_TYPE, 0, texcoords);
  glDrawArrays(GL_TRIANGLE_FAN, 0, nverts);
}
