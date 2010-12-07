#include "font.h"

extern "C" {
#include <ft2build.h>
#include <freetype/freetype.h>
#include <freetype/ftglyph.h>
#include <freetype/ftoutln.h>
#include <freetype/fttrigon.h>
}

Font::Font (const GLuint atlasTex, const map<unsigned long, Glyph>& glyphMap, float glyphSize) 
  : tex(atlasTex), glyphMap(glyphMap), glyphSize(glyphSize) {
}

void Font::drawSquare (float size) {
  glBindTexture(GL_TEXTURE_2D,tex);
  glBegin (GL_QUADS);
    glTexCoord2f(0.0f,0.0f); glVertex2f(0.0f,0.0f);
    glTexCoord2f(0.0f,1.0f); glVertex2f(0.0f,size);
    glTexCoord2f(1.0f,1.0f); glVertex2f(size, size);
    glTexCoord2f(1.0f,0.0f); glVertex2f(size,0.0f);
  glEnd ();
}

void Font::drawGlyph (const Glyph& gi) {
  //LOGI("Drawing glyph at coords (%f,%f) with glyphSize %f", gi.atlasX, gi.atlasY, glyphSize);

  glPushMatrix();
  glTranslatef(gi.leftMargin, gi.topMargin, 0.0f);

  glBegin (GL_QUADS);
    glTexCoord2f(gi.atlasX,gi.atlasY); glVertex2f(0.0f,0.0f);
    glTexCoord2f(gi.atlasX,gi.atlasY+glyphSize); glVertex2f(0.0f,1.0f);
    glTexCoord2f(gi.atlasX+glyphSize,gi.atlasY+glyphSize); glVertex2f(1.0f,1.0f);
    glTexCoord2f(gi.atlasX+glyphSize,gi.atlasY); glVertex2f(1.0f,0.0f);
  glEnd ();
  glPopMatrix();
}

void Font::draw (const char* str, float size) {
  glBindTexture(GL_TEXTURE_2D, tex);

  glPushMatrix();
  glScalef(size, size, 1.0f);
  for (const char* c = str; *c!='\0'; c++) {
    const Glyph& gi = glyphMap.get((unsigned long)*c);
    drawGlyph(gi);
    glTranslatef(gi.advance, 0, 0);
  }
  glPopMatrix();
}

