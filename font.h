#ifndef _FONT_H
#define _FONT_H

#include "def.h"
#include "map.h"

class FTLib;

class Font {
  friend class FTLib;
  public:
    struct Glyph {
      int leftMargin;
      int topMargin;
      //horizontal advance
      int advance;

      //coordinates in the texture atlas
      int atlasX;
      int atlasY;
    };

    void draw (const char* str);

    void drawSquare (float size);
  private:
    Font (const GLuint atlasTex, const map<unsigned long, Glyph>& glyphMap);
    
    GLuint tex;
    map<unsigned long, Glyph> glyphMap;
};

#endif
