#ifndef _FONT_H
#define _FONT_H

#include "fontdef.h"
#include "map.h"

namespace fontlib {

class FTLib;

class Font {
  friend class FTLib;
  public:
    ~Font () {
      //Delete GL texture
    }

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

    //Render the given string with the given size (size is the length of a glyph quad side)
    //If autoGLState, VERTEX and TEXCOORDS array will be enabled before drawing and disable after.
    void draw (const char* str, float size, bool autoGLState);

    //Return the width/height of the rendered text at given size
    void getExtent (const char* str, float size, float* w, float* h);

    //Draw a square containing the whole texture atlas (mostly for debug purposes)
    void drawSquare (float size);

  private:
    //glyphSize is the size of a glyph quad in texture coordinates [0-1]
    Font (const GLuint atlasTex, const map<unsigned long, Glyph>& glyphMap, float glyphSize);

    void drawGlyph (const Glyph& gi);
    
    GLuint tex;
    map<unsigned long, Glyph> glyphMap;
    float glyphSize;
};

}

#endif
