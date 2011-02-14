#ifndef _FTLIB_H
#define _FTLIB_H

#include "Font.h"

extern "C" {
#include <ft2build.h>
#include <freetype/freetype.h>
#include <freetype/ftglyph.h>
#include <freetype/ftoutln.h>
#include <freetype/fttrigon.h>
}

namespace fontlib {

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
    Font* loadFont(const char* filename, int resolution) const;

    /**
     * Loads a font with a specified glyphMargin. 
     * @param filename the filename from which to load the font
     * @param resolution the size (in pixel) of a glyph quad in the opengl texture 
     * @param glyphMargin additionnal margin added to the glyph size to avoid artifacts due
     *                    to aliasing
     * @return the newly loaded font, NULL on error
     */
    Font* loadFont (const char* filename, int resolution, int glyphMargin) const;

    /**
     * Loads a font from an in-memory buffer containing the TTF file.
     * @see loadFont
     * @param name an arbitrary name to uniquely identify this font
     * @param memBase pointer to the beginning of the font data
     * @param memLength size of memory chunk used by font data
     */
    Font* loadMemoryFont (const char* memBase, size_t memLength, int resolution) const; 
    Font* loadMemoryFont (const char* memBase, size_t memLength, int resolution, int glyphMargin) const; 


  private:
    static FTLib* instance;

    /**
     * Read the given freetype font and convert it to an OpenGL texture atlas
     */
    static Font* readFont (const FT_Face& fontFace, int resolution, int glyphMargin);


    FTLib ();
    ~FTLib ();

    bool error; //this flag is set to true if any error occurs during freetype initialization
    FT_Library library;
};

}

#endif
