#include "FTLib.h"
#include <stdio.h>
#include <stdlib.h>
#include <SDL/SDL.h>
#include <SDL/SDL_opengl.h>
#include <SDL/SDL_ttf.h>

#define SCREEN_WIDTH 800 
#define SCREEN_HEIGHT 800

SDL_Surface* screen;

using namespace fontlib;

static void initVideo () {
  if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) != 0) {
    LOGE("Error initializing SDL");
    exit(-1);
  }

  const SDL_VideoInfo* vidInfo = SDL_GetVideoInfo();
  screen = SDL_SetVideoMode(SCREEN_WIDTH, SCREEN_HEIGHT, vidInfo->vfmt->BitsPerPixel, SDL_OPENGL | SDL_HWPALETTE | SDL_HWSURFACE);
  if (!screen) {
    LOGE("Error initializing SDL screen");
    exit(-1);
  }

  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

  //OpenGL init
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
//  glClearColor(0.7f,0.7f,0.7f,0.0f);
  glClearColor(0,0,0,0);
  glOrtho(0, SCREEN_WIDTH, SCREEN_HEIGHT, 0, 0, 1);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  glDisable(GL_DEPTH_TEST);
  glColor4f(1,1,1,1);
  glEnable(GL_TEXTURE_2D);

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

static void releaseVideo () {
  SDL_FreeSurface(screen);
}

static void renderTTF (TTF_Font* font, const char* str, int x, int y) {
	SDL_Color color = {255, 255, 255};
	SDL_Surface *message = TTF_RenderText_Blended(font, str, color);
	unsigned texture = 0;
 
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
 
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
 
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, message->w, message->h, 0, GL_BGRA, GL_UNSIGNED_BYTE, message->pixels);
 
	glBegin(GL_QUADS);
		glTexCoord2d(0, 0); glVertex3d(x, y, 0);
		glTexCoord2d(1, 0); glVertex3d(x+message->w, y, 0);
		glTexCoord2d(1, 1); glVertex3d(x+message->w, y+message->h, 0);
		glTexCoord2d(0, 1); glVertex3d(x, y+message->h, 0);
	glEnd();
 
	/*Clean up.*/
	glDeleteTextures(1, &texture);
	SDL_FreeSurface(message);
}

static void renderTexAtlas (Font* font) {
  font->drawSquare(SCREEN_WIDTH/2);
}

static void drawText (Font* font, const char* msg, int x, int y, int size) {
  glPushMatrix();

  glTranslatef(x, y, 0);
  font->draw(msg, size, true);

  glPopMatrix();
}


bool done = false;

static void handleEvents () {
  SDL_Event event;
  while (SDL_PollEvent(&event)) {
    switch(event.type) {
      case SDL_KEYDOWN:
        if (event.key.keysym.sym == SDLK_ESCAPE)
          done = true;
        break;
      case SDL_QUIT:
        done = true;
        break;
      default:
        break;
    }
  }
}

size_t fileSize (FILE* f) {
  size_t size = 0;
  char buf[256];
  size_t numRead;
  fseek(f, 0, SEEK_SET);
  while ((numRead = fread(buf, 1, 256, f)) > 0) {
    size += numRead;
  }
  fseek(f, 0, SEEK_SET);
  return size;
}

//Read a file to a buffer
char* readToBuf (const char* filename, size_t* bufSize) {
  FILE* f = fopen(filename, "rb");
  size_t size = fileSize(f);
  char* buffer = new char[size];
  LOGI("file (%s) size : %d", filename, size);
  fread(buffer, 1, size, f);
  *bufSize = size;
  fclose(f);
  return buffer;

}

int main (int argc, char** argv) {
  if (argc < 2) {
    fprintf(stderr, "usage: %s <ttf file>\n", argv[0]);
    return -1;
  }

  const char* message = "hello everybody in the world !";

  initVideo();
  Font* font15 = FTLib::getInstance()->loadFont(argv[1], 15);

  //Load same font but from memory and with better resolution
  size_t bufSize;
  char* buffer = readToBuf(argv[1], &bufSize);
  Font* font50 = FTLib::getInstance()->loadMemoryFont(buffer, bufSize, 50);
  delete [] buffer;

  if(TTF_Init() == -1) {
    printf("Error initializing SDL_TTF\n");
    return -1;
  }

  TTF_Font* ttfFont = TTF_OpenFont(argv[1], 40);
  if (!ttfFont) {
    printf("Error loading SDL_TTF font : %s\n", SDL_GetError());
  }

  while (!done) {
    handleEvents();

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    renderTexAtlas(font50);

    drawText(font15, message, 0, SCREEN_WIDTH/2+60, 60);

    drawText(font50, message, 0, SCREEN_WIDTH/2+2*60, 60);

    renderTTF(ttfFont, message, 0, SCREEN_WIDTH/2+3*60);

    drawText(font50, "first\nsecond line", 0, SCREEN_WIDTH/2+5*60, 60);
    glFlush();
    SDL_GL_SwapBuffers();
  }
  releaseVideo();
  SDL_Quit();
  return 0;
}
