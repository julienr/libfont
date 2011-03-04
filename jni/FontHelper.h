#ifndef _FONT_HELPER_H
#define _FONT_HELPER_H

#include "Font.h"

namespace fontlib {

//A helper class to draw fonts
class FontHelper {
  public:

    //Draw a text string of given size centered on coordinates (x,y)
    //if autoGLState is on, will enable vertex and texcoords array before drawing and DISABLE them after drawing
    static void drawCenteredAt (Font* font, const char* str, float size, float x, float y, bool autoGLState) {
      glPushMatrix();
      float fw, fh;
      font->getExtent(str, size, &fw, &fh);
      glTranslatef(x-fw/2.0f, y-fh/2.0f, 0);
      font->draw(str,size,autoGLState);
      glPopMatrix();
    }

  private:
    FontHelper() {}
};

}

#endif
