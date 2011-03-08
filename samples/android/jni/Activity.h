#ifndef DEMO_ACTIVITY_H
#define DEMO_ACTIVITY_H

#include <jni.h>
#include <errno.h>

#include <EGL/egl.h>
#include <GLES/gl.h>

#include <android/log.h>

#include <android_native_app_glue.h>


//A native activity helper class
class Activity {
  public:
    Activity (android_app* app);

    //Should be called from android_main just after this
    //Activity has been created. Will enter the main activity 
    //loop and return when the activity is exited
    void run ();

    bool handleInput (AInputEvent* event);
    void handleCmd (int32_t cmd);

    android_app* getApp () const { return app; }

    int getWidth () const { return width; }
    int getHeight() const { return height; }
  protected:
    //called on every frame to draw the world. 
    virtual void drawImpl () = 0;

    //called on every frame to simulate the world's events
    //elapsedS is the number of elapsed seconds since last frame
    virtual void simulate (double elapsedS) = 0;

    //called after GL initialization, allows the activity
    //to perform resources loading
    virtual void postInit () {}

    //can be overloaded to handle state saving when the activity gets paused
    //after the call, *loc should contain a pointer to an object
    //allocated by malloc
    //savedStateSize should contains its size
    virtual void saveState(void** loc, size_t* savedStateSize) {}

    //restore the state from the given location 'loc'. The state
    //of the saved state is given by savedStateSize.
    virtual void restoreState(void* loc, size_t savedStateSize) {}
  private:
    bool _initGL ();
    void _deinitGL ();

    void _draw ();

    uint64_t _getTimeMillis ();

    android_app* app;

    EGLDisplay display;
    EGLSurface surface;
    EGLContext context;
    int32_t width;
    int32_t height;

    uint64_t lastSimulate;
};


#endif
