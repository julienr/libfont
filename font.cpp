#include "font.h"

extern "C" {
#include <ft2build.h>
#include <freetype/freetype.h>
#include <freetype/ftglyph.h>
#include <freetype/ftoutln.h>
#include <freetype/fttrigon.h>
}

Font::Font (const GLuint atlasTex, const map<unsigned long, Glyph>& glyphMap) 
  : tex(atlasTex), glyphMap(glyphMap) {
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

