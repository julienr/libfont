#include "FTLib.h"

namespace fontlib {

static int nextPowerOf2 (int a)
{
  int rval = 1;
  while (rval<a) rval*=2;
  return rval;
}


//We use a two-channel texture (one for luminance, one for alpha). We assigne the same value to both
//because we want black(0 luminance) to be transparent(0 alpha)

static void setPixel (GLubyte* texture, int offset, int size, int x, int y, GLubyte val) {
  texture[2*(offset+x+y*size)] = texture[2*(offset+x+y*size)+1] = val;
}

//Copy the bitmap of the given glyph to the texture atlas at the location (atlasX, atlasY). If drawBorder is true, a white border is drawn around the glyph square
static void copyGlyphToTex (FT_GlyphSlot glyph, GLubyte* texture, int atlasX, int atlasY, int texSize, int resolution, int marginSize, bool drawBorder) {
  const int squareSize = resolution + marginSize*2;
  const int baseOffset = atlasX*squareSize + atlasY*squareSize*texSize;

  if (drawBorder) { //Draw a white border around the glyph
    for (int w=0; w<squareSize; w++)
      setPixel(texture, baseOffset, texSize, w, 0, 255);

    for (int h=1; h<squareSize; h++)
      for (int w=0; w<squareSize; w++)
        setPixel(texture,baseOffset,texSize,w,h, (w==0||w==squareSize-1)?255:(h==squareSize-1)?255:0);
  }

  //The bitmap might be smaller than our square of [squareSize*squareSize]. So we first
  //copy the glyph bitmap and then add black padding if needed
  const int gr = glyph->bitmap.rows;
  const int gw = glyph->bitmap.width;
  for (int h=0; h<gr; h++) {
    for (int w=0; w<gw; w++) {
      //Need to add the top margin (marginSize*texSize offset) and the left margin (marginSize)
      setPixel(texture, baseOffset+marginSize, texSize, w, marginSize+h, glyph->bitmap.buffer[w+h*gw]);
    }
  }

  if (!drawBorder) { //with borders, padding has already been done 
    for (int h=0; h<squareSize; h++) {
      if (h < marginSize || h >= marginSize+gr) {
         //bottom padding => whole rows
        for (int w=0; w<squareSize; w++) {
          setPixel(texture, baseOffset, texSize, w, h, 0);
        }
      } else {
        //left padding
        for (int w=0; w<marginSize; w++)
          setPixel(texture, baseOffset, texSize, w, h, 0);
        //right padding
        for (int w=gw+marginSize; w<squareSize; w++)
          setPixel(texture, baseOffset, texSize, w, h, 0);
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

#define GLERR do {int err=glGetError();if (err != 0) LOGE("GL Error at %s:%i : %i", __FILE__, __LINE__, err); } while(0)

Font* FTLib::readFont (const FT_Face& fontFace, int resolution, int glyphMargin) {
  //Each character will be rendered in a square of resolution*resolution pixels
  FT_Set_Pixel_Sizes(fontFace, resolution, resolution);
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

  //The actual size of a side of the square that will contain our glyph
  const int squareSize = resolution + glyphMargin*2;

  //Calculate the size of the texture we'll need to store these glyphs
  const int numGlyphsPerRow = (int)ceilf(sqrt(numGlyphs)); //=numRows (texture is a square)
  const int texSize = (numGlyphsPerRow)*squareSize;
  //OpenGL ES requires power of 2 textures 
  const int realTexSize = nextPowerOf2(texSize);

  //we use two channels (one for luminance, one for alpha)
  GLubyte* textureData = new GLubyte[realTexSize*realTexSize*2];

  GLuint atlasTex;
  glGenTextures(1, &atlasTex);
  glBindTexture(GL_TEXTURE_2D, atlasTex);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  GLERR;

  LOGI("num glyphs : %i => texSize = %i (nextpowerof2 = %i), numGlyphsPerRow = %i, margin = %i", numGlyphs, texSize, realTexSize, numGlyphsPerRow, glyphMargin);

  map<unsigned long, Font::Glyph> glyphs;

  int texAtlasX = 0;
  int texAtlasY = 0;

  //Loop through all the characters in the font
  FT_UInt gindex;
  for (FT_ULong charcode=FT_Get_First_Char(fontFace, &gindex); 
       gindex != 0; 
       charcode=FT_Get_Next_Char(fontFace, charcode, &gindex)) {
    //LOGI("Load glyph for charcode %li", charcode);
    //Load the corresponding glyph, rendering it on the fly
    if (FT_Load_Glyph(fontFace, gindex, FT_LOAD_DEFAULT)) {
      LOGE("Error loading glyph with index %i and charcode %i. Skipping.", gindex, (int)charcode);
      continue;
    }
    FT_GlyphSlot glyph = fontFace->glyph;
    FT_Render_Glyph(glyph, FT_RENDER_MODE_NORMAL);

    //Calculate glyph informations 
    Font::Glyph glyphInfo;
    glyphInfo.leftMargin = glyph->bitmap_left/(float)squareSize;
    glyphInfo.topMargin = -(glyph->bitmap_top/(float)squareSize);
    glyphInfo.atlasX = texAtlasX*squareSize/(float)realTexSize;
    //add 1 to avoid border aliasing issues 
    glyphInfo.atlasY = (texAtlasY*squareSize+1)/(float)realTexSize;
    //advance is stored in fractional pixel format (=1/64 pixels)
    glyphInfo.advance = (glyph->advance.x)/(float)(64.0f*squareSize);

    glyphs.insert(charcode, glyphInfo);

    //Copy the bitmap to the atlas
    copyGlyphToTex(glyph, textureData, texAtlasX, texAtlasY, realTexSize, resolution, glyphMargin, false);
 
    texAtlasX++;
    if (texAtlasX >= numGlyphsPerRow) {
      texAtlasX=0;
      texAtlasY++;
    }
  }

  glTexImage2D (GL_TEXTURE_2D, 0, GL_LUMINANCE_ALPHA, realTexSize, realTexSize, 0, GL_LUMINANCE_ALPHA, GL_UNSIGNED_BYTE, textureData);
  delete [] textureData;
  GLERR;

  FT_Done_Face(fontFace);

  return new Font (atlasTex, glyphs, squareSize/(float)realTexSize);
}


Font* FTLib::loadMemoryFont (const char* memBase, size_t memLength, int resolution) const {
  const int margin = (int)ceil(resolution*0.1f);
  return loadMemoryFont(memBase, memLength, resolution, margin);
}

Font* FTLib::loadMemoryFont (const char* memBase, size_t memLength, int resolution, int glyphMargin) const {
  if (error) {
    LOGE("loadMemoryFont : freetype library initialization failed");
    return NULL;
  }

  FT_Face fontFace;
  if (FT_New_Memory_Face(library, (FT_Byte*)memBase, (FT_Long)memLength, 0, &fontFace)) {
    LOGE("Error loading font face");
    return NULL;
  }

  return readFont(fontFace, resolution, glyphMargin);
}

Font* FTLib::loadFont(const char* filename, int resolution) const {
  const int margin = (int)ceil(resolution*0.1f);
  return loadFont(filename, resolution, margin);
}


Font* FTLib::loadFont (const char* filename, int resolution, int glyphMargin) const {
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

  return readFont(fontFace, resolution, glyphMargin);
}


}
