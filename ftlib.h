#ifndef _FTLIB_H
#define _FTLIB_H

#include "font.h"

extern "C" {
#include <ft2build.h>
#include <freetype/freetype.h>
#include <freetype/ftglyph.h>
#include <freetype/ftoutln.h>
#include <freetype/fttrigon.h>
}

class FTLib {
  public:
    static FTLib* getInstance () {
      if (instance == NULL)
        instance = new FTLib();
      return instance;
    }

    static void free () {
      delete instance;
      instance = NULL;
    }

    bool getError () {
      return error;
    }

    /**
     * Loads a font. Glyph margin is autocalculated (10% of resolution). 
     */
    Font* loadFont(const char* filename, int resolution);

    /**
     * Loads a font with a specified glyphMargin. 
     * @param filename the filename from which to load the font
     * @param resolution the size (in pixel) of a glyph quad in the opengl texture 
     * @param glyphMargin additionnal margin added to the glyph size to avoid artifacts due
     *                    to aliasing
     * @return the newly loaded font, NULL on error
     */
    Font* loadFont (const char* filename, int resolution, int glyphMargin);


  private:
    static FTLib* instance;

    FTLib ();
    ~FTLib ();

    bool error; //this flag is set to true if any error occurs during loading
    FT_Library library;
};

#endif
