#ifndef _DEF_H
#define _DEF_H

extern "C" {
#include <stdint.h>
#include <stdio.h>
#include <math.h>
#include <assert.h>
}

#define LOG_TAG    __FILE__ ":" STR(__LINE__)
#define STR(x) STRINGIFY(x)
#define STRINGIFY(x) # x

#ifdef __ANDROID__ //Android build
  #include <GLES/gl.h>
  #include <android/log.h>
  #if 1
    #define LOGI(...)  __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)
    #define LOGE(...)  __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)
  #else
    #define LOGI(...)
    #define LOGE(...)
  #endif
#else
  #include <GL/gl.h>
  #define LOGI(...) do {printf(__VA_ARGS__);printf("\n");} while(0)
  #define LOGE(...) do {printf(__VA_ARGS__);printf("\n");} while(0)
#endif

//BEGIN opengl-related defs
//Since the desktop version of opengl doesn't support fixed-point,
#ifdef PLATFORM_SDL
  #define MGL_DATATYPE float
  #define MGL_TYPE GL_FLOAT
  #define iX(x) (x)
  #define fX(x) (x)
#else
  #define MGL_DATATYPE int
  #define MGL_TYPE GL_FIXED
  //int to fixed point
  #define iX(x) (x<<16)
  //float to fixed point
  #define fX(x) ((int)(x * (1  << 16)))
#endif

//END


#define MAX_TEXTURE_SIZE 512

#endif
