
#
# Generic Makefile to make one OpenGL based executable.
# Use GNU make .
#

SHELL	= /bin/sh

SOURCES = main.cpp calendar.cpp picking.cpp calendar_primitives.cpp effect.cpp \
	  effect1.cpp effect2.cpp effect3.cpp effect4.cpp effect5.cpp effect6.cpp \
	  effect7.cpp effect8.cpp effect9.cpp effect10.cpp effect11.cpp effect12.cpp \
	  effect13.cpp effect14.cpp effect15.cpp effect16.cpp effect17.cpp effect18.cpp \
	  effect19.cpp effect20.cpp effect21.cpp effect22.cpp effect23.cpp effect24.cpp \
	  texture.cpp messages.cpp pause.cpp \
	  stars.cpp vectormath.cpp util.cpp quit.cpp readtex.cpp
TARGET  = calendar
OBJECTS = $(SOURCES:.cpp=.o)

CC	= g++
SDL_CFLAGS := $(shell sdl2-config --cflags)
SDL_LDFLAGS := $(shell sdl2-config --libs)
CFLAGS	= -O2 -g -Wall $(SDL_CFLAGS)
LDFLAGS	= -L/usr/X11R6/lib -lGLU -lGL -lXt -lXi -lXmu -lXext -lX11 -lm $(SDL_LDFLAGS) -lpng


all: $(TARGET)
.PHONY: all


$(OBJECTS): %.o: %.cpp
	$(CC) -c $(CFLAGS) $^ -o $@

$(TARGET): $(OBJECTS)
	$(CC) $^ $(LDFLAGS) -o $@

.PHONY: clean
clean:
	-rm -f core \#*\# *~ *.o $(OBJECTS) $(TARGET)
