all :: gl_simple
gl_simple : gl_simple.c debugconsole.c args.c gfx.c glhelper.c gl.c
gl_simple : CFLAGS += -Wall -W -Os -std=gnu99 $(SDL_CFLAGS)
gl_simple : LDLIBS += $(SDL_LDLIBS) -lm
gl_simple : CPPFLAGS +=
clean :: ; $(RM) gl_simple$X
