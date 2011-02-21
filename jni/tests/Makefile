all:
	$(MAKE) -C ..
	g++ -I../ -o sdltest sdltest.cpp -I/usr/include/freetype2/ -lfreetype -lSDL -lSDL_ttf -lGL -g -L ../ -lfont -DPLATFORM_SDL

clean:
	rm sdltest
	$(MAKE) -C .. clean
