CC=g++
CPPFLAGS=-c -Wall -I. -I/usr/include/freetype2/ -DPLATFORM_SDL
LDFLAGS=
SOURCES=Font.cpp FTLib.cpp
OBJECTS=$(SOURCES:.cpp=.o)
LIBRARY=libfont.a


all: $(SOURCES) $(LIBRARY)
	
$(LIBRARY): $(OBJECTS) 
	#gcc -shared -o $(LIBRARY) $(OBJECTS)
	ar rcs $(LIBRARY) $(OBJECTS)

.c.o:
	$(CC) $(CPPFLAGS) $< -o $@

clean:
	rm -f $(LIBRARY) 
	rm -f *.o
