#include "ftlib.h"
#include <stdio.h>
#include <stdlib.h>
#include <SDL/SDL.h>
#include <SDL/SDL_opengl.h>

#define SCREEN_WIDTH 800 
#define SCREEN_HEIGHT 800

SDL_Surface* screen;

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
  glClearColor(0.7f,0.7f,0.7f,0.0f);
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

static void render (Font* font) {
  glClear (GL_COLOR_BUFFER_BIT);

  glLoadIdentity();

  //glDisable(GL_TEXTURE_2D);

  font->drawSquare(SCREEN_WIDTH/2);

  glTranslatef(0,SCREEN_WIDTH/2+70, 0);

//  font->draw("abcdefghijklmnop", 50);
  font->draw("hello everybody in the world !", 60);
  glFlush();
  SDL_GL_SwapBuffers();
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
      default:
        break;
    }
  }
}

int main (int argc, char** argv) {
  if (argc < 2) {
    fprintf(stderr, "usage: %s <ttf file>\n", argv[0]);
    return -1;
  }

  initVideo();
  Font* font = FTLib::getInstance()->loadFont(argv[1], 15, 2);

  while (!done) {
    handleEvents();
    render(font);
  }
  releaseVideo();
  SDL_Quit();
  return 0;
}
