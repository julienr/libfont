#include "ftlib.h"

static int nextPowerOf2 (int a)
{
  int rval = 1;
  while (rval<a) rval*=2;
  return rval;
}

//Copy the bitmap of the given glyph to the texture atlas at the location (atlasX, atlasY). If drawBorder is true, a white border is drawn around the glyph square
static void copyGlyphToTex (FT_GlyphSlot glyph, GLubyte* texture, int atlasX, int atlasY, int texSize, int squareSize, bool drawBorder) {
  const int baseOffset = atlasX*squareSize + atlasY*squareSize*texSize;

  if (drawBorder) { //Draw a white border around the glyph
    for (int w=0; w<squareSize; w++)
      texture[baseOffset+w] = 255;

    for (int h=1; h<squareSize; h++)
      for (int w=0; w<squareSize; w++)
        texture[baseOffset+(w+h*texSize)] = (w==0||w==squareSize-1)?255:(h==squareSize-1)?255:0;
  }

  //The bitmap might be smaller than our square of [squareSize*squareSize]. So we first
  //copy the glyph bitmap and then add black padding if needed
  const int gr = glyph->bitmap.rows;
  const int gw = glyph->bitmap.width;
  for (int h=0; h<gr; h++) {
    for (int w=0; w<gw; w++) {
      //We are using a two channel opengl texture (one for luminance one for alpha), 
      //but we assign the same value to both
      texture[baseOffset+(w+h*texSize)] = glyph->bitmap.buffer[w+h*gw];
    }
  }

  if (!drawBorder) { //with borders, padding has already been done 
    for (int h=0; h<squareSize; h++) {
      if (h < gr) {
        //right padding
        for (int w=gw; w<squareSize; w++)
          texture[baseOffset+(w+h*texSize)] = 0;
      } else {
        //bottom padding
        for (int w=0; w<squareSize; w++)
          texture[baseOffset+(w+h*texSize)] = 0;
      }
    }
  }
}

FTLib* FTLib::instance = NULL;

FTLib::FTLib () : error(false) {
  if (FT_Init_FreeType(&library)) {
    LOGE("Failed to initialize freetype library");
    error = true;
    return;
  }
}

FTLib::~FTLib () {
  FT_Done_FreeType(library);
}

Font* FTLib::loadFont (const char* filename, int size) {
  //TODO: To avoid loading ALL glyphs from a particular typeFace, we might
  //let the user provides one or more "character ranges" that we should load

  //TODO: If we have a font face with a HUGE amount of glyphs, maybe it would
  //be better to generate multiple atlas (and a glyph would be found by using
  //its atlas coordinates AND an atlas index)
  
  if (error) {
    LOGE("loadFont : freetype library initialization failed");
    return NULL;
  }

  FT_Face fontFace;
  if (FT_New_Face(library, filename, 0, &fontFace)) {
    LOGE("Error loading font face %s", filename);
    return NULL;
  }  
  
  //Each character will be rendered in a square of size*size pixels
  FT_Set_Pixel_Sizes(fontFace, size, size);
  /*
   * We build a single texture (the "atlas") to store all our glyphs. Each glyph
   * has an atlasCoord, which represent its position in the atlas (NOT in pixels, 
   * but in glyphs).
   * For example, G1 will have the atlas coords (0,0), G2(1,0) and G20(0,1)
   ------------------------------
   |G1|G2|G3|.......            |
   |----------------------------|
   |G20|                        |
              ...
   |                            |
   |                       |Gn-1|
   ------------------------------
   |Gn|                         |
   ------------------------------ 
   */
  const int numGlyphs = fontFace->num_glyphs;

  //Calculate the size of the texture we'll need to store these glyphs
  const int numGlyphsPerRow = (int)sqrt(numGlyphs); //=numRows (texture is a square)
  //We add 1 to take into account the last row of glyph's heights
  const int texSize = (numGlyphsPerRow+1)*size;
  //OpenGL ES requires power of 2 textures 
  const int realTexSize = nextPowerOf2(texSize);

  GLubyte* textureData = new GLubyte[realTexSize*realTexSize];

  GLuint atlasTex;
  glGenTextures(1, &atlasTex);
  glBindTexture(GL_TEXTURE_2D, atlasTex);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);


  LOGI("num glyphs : %i => texSize = %i (nextpowerof2 = %i), numGlyphsPerRow = %i", numGlyphs, texSize, realTexSize, numGlyphsPerRow);

  map<unsigned long, Font::Glyph> glyphs;

  int texAtlasX = 0;
  int texAtlasY = 0;

  //Loop through all the characters in the font
  FT_UInt gindex;
  for (FT_ULong charcode=FT_Get_First_Char(fontFace, &gindex); 
       gindex != 0; 
       charcode=FT_Get_Next_Char(fontFace, charcode, &gindex)) {
    //Load the corresponding glyph, rendering it on the fly
    if (FT_Load_Glyph(fontFace, gindex, FT_LOAD_DEFAULT)) {
      LOGE("Error loading glyph with index %i and charcode %i. Skipping.", gindex, charcode);
      continue;
    }
    FT_GlyphSlot glyph = fontFace->glyph;
    FT_Render_Glyph(glyph, FT_RENDER_MODE_NORMAL);

    Font::Glyph glyphInfo;
    glyphInfo.leftMargin = glyph->bitmap_left;
    glyphInfo.topMargin = glyph->bitmap_top;
    glyphInfo.atlasX = texAtlasX;
    glyphInfo.atlasY = texAtlasY;
    //advance is stored in fractional pixel format (=1/64 pixels)
    glyphInfo.advance = glyph->advance.x >> 6;

    glyphs.insert(charcode, glyphInfo);

    //Copy the bitmap to the atlas
    copyGlyphToTex(glyph, textureData, texAtlasX, texAtlasY, realTexSize, size, false);
 
    texAtlasX++;
    if (texAtlasX >= numGlyphsPerRow) {
      texAtlasX=0;
      texAtlasY++;
    }
  }

  glTexImage2D (GL_TEXTURE_2D, 0, GL_RGBA, realTexSize, realTexSize, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, textureData);
  delete [] textureData;

  FT_Done_Face(fontFace);

  return new Font (atlasTex, glyphs);
}

