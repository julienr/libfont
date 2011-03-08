#!/bin/sh
adb shell kill `adb shell ps | awk '/net.fhtagn.libfont.android_demo/ {print $2}'`
