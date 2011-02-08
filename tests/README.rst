=======
libfont
=======

This library aims to provide an easy way to render text on Android OpenGL ES (or any other OpenGL/ES capable device).

Usage
=====
See tests/sdltest.cpp for an demo using the SDL.

Internals
=========
A font is loaded from a .ttf file using freetype and a font atlas (one big texture containing all the font glyphs) is created. The font altas creation is a one-time initialization process.

When a string is rendered, the font atlas texture is bound and tex coords are used to render each glyph. This mean that once the font atlas for a particular font is loaded, rendering of arbitrary string is quite fast (at least _way_ faster than using freetype to render to texture and then use the texture to draw the text).
