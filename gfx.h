/* gfx.h : header for gfx.c, a graphics abstraction library - public domain */
#ifndef GFX_H
#define GFX_H
/*******************************************************************************
 * Types and data-structures
 ******************************************************************************/
typedef void engine_audio_callback_t(void *extra, uint8_t *stream, int len); // TODO: rename this

/*******************************************************************************
 * Interface
 ******************************************************************************/
void gfx_quit(void);
int gfx_init(void);
int gfx_loop(void);
int gfx_step(void);
extern const char *app_title; // TODO: remove this global
extern int gfx_done; // TODO: remove this global - make it a setter function

/*******************************************************************************
 * External functions called by this module
 ******************************************************************************/
/* calling applications need to define these */
extern int do_setup_gl(void); // TODO: make gfx_init() take this as callback
extern void do_update(double seconds); // TODO: make gfx_init() take this as callback
extern int do_paint(void); // TODO: make gfx_init() take this as callback

#endif
