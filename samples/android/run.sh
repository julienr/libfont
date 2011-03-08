#!/bin/sh
#arg 1 : path to APK
./kill.sh
adb install -r $@
adb shell am start -a android.intent.action.MAIN -c android.intent.category.LAUNCHER -n net.fhtagn.libfont.android_demo/android.app.NativeActivity
