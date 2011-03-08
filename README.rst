=======
libfont
=======

This library aims to provide an easy way to render text on Android OpenGL ES (or any other OpenGL/ES capable device).

Usage
=====
The *samples/* directory contains sample code.
It currently has sample for SDL and Android.

Android
-------
Note that the sample uses NDK's Native Activity. Therefore, it requires a device (or the emulator) running Android 2.3. 
But the code can be ported to work with the standard Java/JNI glue that was required for earlier android versions


This is how the android demo should look :

.. image:: http://fhtagn.net/images/libfont/android-screen.png

Internals
=========
A font is loaded from a .ttf file using freetype and a font atlas (one big texture containing all the font glyphs) is created. The font altas creation is a one-time initialization process.

When a string is rendered, the font atlas texture is bound and tex coords are used to render each glyph. This mean that once the font atlas for a particular font is loaded, rendering of arbitrary string is quite fast (at least _way_ faster than using freetype to render to texture and then use the texture to draw the text).

LICENSE
=======
The code is under a BSD license, see the LICENSE file.
The font provided with the sample comes from the "Liberation font" project[1]. It is licensed under the GPLv2 license with some exceptions (see FONT-LICENSE.txt)
[1] https://fedorahosted.org/liberation-fonts/


