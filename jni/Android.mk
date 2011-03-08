LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

TARGET_PLATFORM := android-4

ANDROID_LIBS := $(LOCAL_PATH)/../../

LOCAL_C_INCLUDES := $(ANDROID_LIBS)/libfreetype/jni/include/

LOCAL_DEFAULT_CPP_EXTENSION := .cpp

LOCAL_MODULE    := libfont
LOCAL_SRC_FILES :=\
	FTLib.cpp \
	Font.cpp 

LOCAL_CFLAGS := -Wall -D__ANDROID__

LIBFREETYPE_PATH := /home/julien/android/libfreetype/obj/local/armeabi/

LOCAL_LDLIBS := -lGLESv1_CM -ldl -llog -L$(LIBFREETYPE_PATH) -lfreetype
#include $(BUILD_SHARED_LIBRARY)
include $(BUILD_STATIC_LIBRARY)
