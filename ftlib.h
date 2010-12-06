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

    ///Loads a font. Returns NULL on error
    //TODO: rename size to resolution
    Font* loadFont (const char* filename, int size);


  private:
    static FTLib* instance;

    FTLib ();
    ~FTLib ();

    bool error; //this flag is set to true if any error occurs during loading
    FT_Library library;
};

#endif
