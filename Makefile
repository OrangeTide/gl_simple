#### Makefile ####
## common makefile stuff
# detect Windows host
ifeq ($(OS),Windows_NT)
RM=del
CC=gcc
X=.exe
SDL_LDLIBS = -lmingw32 -Wl,-Bstatic -lSDL2main -lSDL2 -Wl,-Bdynamic -mwindows -Wl,--no-undefined -lm -ldinput8 -ldxguid -ldxerr8 -luser32 -lgdi32 -lwinmm -limm32 -lole32 -loleaut32 -lshell32 -lversion -luuid -static-libgcc
SDL_CFLAGS = -I/usr/local/x86_64-w64-mingw32/include/SDL2 -Dmain=SDL_main
else
SDL_LDLIBS = $(shell sdl2-config --libs) $(shell pkg-config --libs gl)
SDL_CFLAGS = $(shell sdl2-config --cflags) $(shell pkg-config --cflags gl)
endif
# default rules
all ::
clean ::
# include all the sub-makefiles
include $(wildcard *.mk)
