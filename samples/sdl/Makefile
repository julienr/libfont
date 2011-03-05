all:
	$(MAKE) -C ../../jni/
	g++ -I../../jni/ -o sdltest sdltest.cpp -I/usr/include/freetype2/ -lfreetype -lSDL -lSDL_ttf -lGL -g -L ../../jni -lfont -DPLATFORM_SDL

clean:
	rm sdltest
	$(MAKE) -C ../../jni/ clean
