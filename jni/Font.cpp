#include "Font.h"

extern "C" {
#include <ft2build.h>
#include <freetype/freetype.h>
#include <freetype/ftglyph.h>
#include <freetype/ftoutln.h>
#include <freetype/fttrigon.h>
}

namespace fontlib {

static MGL_DATATYPE squareVerts[8] = {
/*    fX(-0.5), fX(-0.5), 0,
    fX(0.5), fX(-0.5), 0,
    fX(-0.5), fX(0.5), 0,
    fX(0.5), fX(0.5), 0*/
    0,0,
    fX(1),0,
    0,fX(1),
    fX(1),fX(1)
};

Font::Font (const GLuint atlasTex, const map<unsigned long, Glyph>& glyphMap, float glyphSize) 
  : tex(atlasTex), glyphMap(glyphMap), glyphSize(glyphSize) {
}

void Font::drawSquare (float size) {
  MGL_DATATYPE texCoords[8] = {
    0,0,
    fX(1),0,
    0,fX(1),
    fX(1),fX(1),
  };

  glEnableClientState(GL_VERTEX_ARRAY);
  glEnableClientState(GL_TEXTURE_COORD_ARRAY);

  glBindTexture(GL_TEXTURE_2D,tex);

  glPushMatrix();
  glScalef(size, size, 1);
  glVertexPointer(2, MGL_TYPE, 0, squareVerts);
  glTexCoordPointer(2, MGL_TYPE, 0, texCoords);
  glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
  glPopMatrix();

  glDisableClientState(GL_VERTEX_ARRAY);
  glDisableClientState(GL_TEXTURE_COORD_ARRAY);
}

void Font::drawGlyph (const Glyph& gi) {
  glPushMatrix();
  glTranslatef(gi.leftMargin, gi.topMargin, 0.0f);
 
  float texCoords[8] = {
    gi.atlasX, gi.atlasY,
    gi.atlasX+glyphSize, gi.atlasY,
    gi.atlasX, gi.atlasY+glyphSize,
    gi.atlasX+glyphSize, gi.atlasY+glyphSize
  };
  glVertexPointer(2, MGL_TYPE, 0, squareVerts);
  glTexCoordPointer(2, GL_FLOAT, 0, texCoords);
  glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

  glPopMatrix();
}

void Font::draw (const char* str, float size, bool autoGLState) {
  glBindTexture(GL_TEXTURE_2D, tex);

  if (autoGLState) {
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
  }

  glPushMatrix();
  glScalef(size, size, 1.0f);

  float advanceAccum = 0;
  for (const char* c = str; *c!='\0'; c++) {
    if (*c == '\n') {
      glTranslatef(-advanceAccum, 1, 0);
      advanceAccum = 0;
    } else {
      const Glyph& gi = glyphMap.get((unsigned long)*c);
      drawGlyph(gi);
      glTranslatef(gi.advance, 0, 0);
      advanceAccum += gi.advance;
    }
  }

  if (autoGLState) {
    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
  }

  glPopMatrix();
}

void Font::getExtent (const char* str, float size, float* w, float* h) {
  float longestLine = 0; //longest line length
  int numLines = 1;

  float currLineLength = 0;
  for (const char* c = str; *c!='\0'; c++) {
    if (*c == '\n') {
      numLines++;
      if (currLineLength > longestLine)
        longestLine = currLineLength;
      currLineLength = 0;
    } else {
      const Glyph& gi = glyphMap.get((unsigned long)*c);
      currLineLength += glyphSize*size + gi.leftMargin*size + gi.advance*size;
    }
  }
  if (currLineLength > longestLine)
    longestLine = currLineLength;

  *w = longestLine;
  *h = numLines*glyphSize*size;
}

}
