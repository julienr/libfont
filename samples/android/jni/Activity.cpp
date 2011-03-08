#include "Activity.h"


#define LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO, "native-activity", __VA_ARGS__))
#define LOGE(...) ((void)__android_log_print(ANDROID_LOG_ERROR, "native-activity", __VA_ARGS__))

Activity::Activity (android_app* app)
  : app(app),
    display(EGL_NO_DISPLAY), 
    surface(EGL_NO_SURFACE), 
    context(EGL_NO_CONTEXT) {}


bool Activity::_initGL () {
  /*
   * Here specify the attributes of the desired configuration.
   * Below, we select an EGLConfig with at least 8 bits per color
   * component compatible with on-screen windows
   */
  const EGLint attribs[] = {
    EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
    EGL_BLUE_SIZE, 8,
    EGL_GREEN_SIZE, 8,
    EGL_RED_SIZE, 8,
    EGL_NONE
  };
  EGLint dummy, format;
  EGLint numConfigs;
  EGLConfig config;

  display = eglGetDisplay(EGL_DEFAULT_DISPLAY);

  eglInitialize(display, 0, 0);

  /* Here, the application chooses the configuration it desires. In this
   * sample, we have a very simplified selection process, where we pick
   * the first EGLConfig that matches our criteria */
  eglChooseConfig(display, attribs, &config, 1, &numConfigs);

  /* EGL_NATIVE_VISUAL_ID is an attribute of the EGLConfig that is
   * guaranteed to be accepted by ANativeWindow_setBuffersGeometry().
   * As soon as we picked a EGLConfig, we can safely reconfigure the
   * ANativeWindow buffers to match, using EGL_NATIVE_VISUAL_ID. */
  eglGetConfigAttrib(display, config, EGL_NATIVE_VISUAL_ID, &format);

  ANativeWindow_setBuffersGeometry(app->window, 0, 0, format);

  surface = eglCreateWindowSurface(display, config, app->window, NULL);
  context = eglCreateContext(display, config, NULL, NULL);

  if (eglMakeCurrent(display, surface, surface, context) == EGL_FALSE) {
    LOGE("Unable to eglMakeCurrent");
    return false;
  }

  eglQuerySurface(display, surface, EGL_WIDTH, &width);
  eglQuerySurface(display, surface, EGL_HEIGHT, &height);

  glViewport(0,0, width, height);

  postInit();

  return true;
}

void Activity::_deinitGL () {
  if (display != EGL_NO_DISPLAY) {
    eglMakeCurrent(display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
    if (context != EGL_NO_CONTEXT)
      eglDestroyContext(display, context);
    if (surface != EGL_NO_SURFACE)
      eglDestroySurface(display, surface);
    eglTerminate(display);
  }
  display = EGL_NO_DISPLAY;
  surface = EGL_NO_SURFACE;
  context = EGL_NO_CONTEXT;
}

uint64_t Activity::_getTimeMillis () {
    timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec*1000 + tv.tv_usec/1000;
}

void Activity::_draw () {
  if (display == EGL_NO_DISPLAY)
    return;

  drawImpl();

  eglSwapBuffers(display, surface);
}

bool Activity::handleInput (AInputEvent* event) {
  return false;
}

void Activity::handleCmd (int32_t cmd) {
  switch (cmd) {
    case APP_CMD_SAVE_STATE:
      saveState(&app->savedState, &app->savedStateSize);
      break;
    case APP_CMD_INIT_WINDOW:
      if (app->window != NULL) {
        _initGL();
        _draw();
      }
      break;
    case APP_CMD_TERM_WINDOW:
      _deinitGL();
      break;
  }
}

static void handle_cmd(struct android_app* app, int32_t cmd) {
  ((Activity*)app->userData)->handleCmd(cmd);
}

static int32_t handle_input(struct android_app* app, AInputEvent* event) {
  return ((Activity*)app->userData)->handleInput(event);
}

void Activity::run () {
  app->userData = this;
  app->onAppCmd = handle_cmd;
  app->onInputEvent = handle_input;

  if (app->savedState != NULL) {
    restoreState(app->savedState, app->savedStateSize);
    //State restored, free the memory
    free(app->savedState);
    app->savedState = NULL;
    app->savedStateSize = 0;
  }

  lastSimulate = _getTimeMillis();

  while (1) {
    int ident;
    int events;
    android_poll_source* source;
    while ((ident = ALooper_pollAll(0, NULL, &events, (void**)&source)) >= 0) {
      if (source != NULL)
        source->process(app, source);

      //We are exiting
      if (app->destroyRequested != 0) {
        _deinitGL();
        return;
      }
    }

    //Simulate
    const uint64_t now = _getTimeMillis();
    const double elapsedS = (now-lastSimulate)/1000.0;
    simulate(elapsedS);
    lastSimulate = now;

    //Draw
    _draw();
  }

}

