#ifndef _FONT_H
#define _FONT_H

#include "def.h"
#include "map.h"

class FTLib;

class Font {
  friend class FTLib;
  public:
    struct Glyph {
      float leftMargin;
      float topMargin;
      //horizontal advance
      float advance;

      float texCoordX;
      float texCoordY;

      //tex coordinates in the texture atlas
      float atlasX;
      float atlasY;
    };

    void draw (const char* str, float size);

    void drawSquare (float size);

  private:
    Font (const GLuint atlasTex, const map<unsigned long, Glyph>& glyphMap, float glyphSize);

    void drawGlyph (const Glyph& gi);
    
    GLuint tex;
    map<unsigned long, Glyph> glyphMap;
    float glyphSize;
};

#endif
