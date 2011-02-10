CC=g++
CPPFLAGS=-c -Wall -I. -I/usr/include/freetype2/
LDFLAGS=
SOURCES=font.cpp ftlib.cpp
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
