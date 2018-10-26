/* gfx.c - graphics engine abstraction. public domain */
/*******************************************************************************
 * Graphics & Audio abstraction layer
 ******************************************************************************/

#include <SDL2/SDL.h>

#include "gl.h"
#include "logging.h"
#include "config.h"

#include "gfx.h"

/*******************************************************************************
 * Globals
 ******************************************************************************/
static SDL_AudioDeviceID audio_device;
static int gfx_initialized = 0;
static int fullscreen = 0;
static SDL_Window *main_win;
static SDL_GLContext main_ctx;
int gfx_done = 0; // TODO: wrap this up in a function

/*******************************************************************************
 * graphics init and clean-up
 ******************************************************************************/

void
gfx_quit(void)
{
	if (main_ctx)
		SDL_GL_DeleteContext(main_ctx);
	main_ctx = NULL;

	if (main_win)
		SDL_DestroyWindow(main_win);
	main_win = NULL;

	if (gfx_initialized)
		SDL_Quit();
	gfx_initialized = 0;
}

int
gfx_init(void)
{
	SDL_LogSetAllPriority(SDL_LOG_PRIORITY_INFO);

	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS | SDL_INIT_AUDIO)) {
		DBG_LOG("Failed to initialize SDL: %s", SDL_GetError());
		return -1;
	}

	gfx_initialized = 1;

	SDL_LogSetAllPriority(SDL_LOG_PRIORITY_INFO);

	fullscreen = 0;

	/* require OpenGL 3.3 */
#if 1 /* default is desktop PC */

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

#elif MOBILE // TODO: need some way to configure this

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);

#endif

	main_win = SDL_CreateWindow(app_title ? app_title : APP_TITLE,
		SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
		APP_OUT_WIDTH, APP_OUT_HEIGHT, SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL);
	if (!main_win) {
		DBG_LOG("Failed to create window: %s", SDL_GetError());
		goto failure;
	}

	main_ctx = SDL_GL_CreateContext(main_win);
	if (!main_ctx) {
		DBG_LOG("Failed to initialize GL context: %s", SDL_GetError());
		goto failure;
	}

	SDL_GL_MakeCurrent(main_win, main_ctx);

	SDL_GL_LoadLibrary(NULL);

	/* TODO: register published key events */

	if (do_setup_gl())
		goto failure;

	return 0;
failure:
	gfx_quit();
	return -1;
}

/* return 0 if outer loop should quit, or 1 to continue */
static int
handle_event(SDL_Event *e)
{
	// TODO: support more than just main_win

	switch (e->type) {
	case SDL_QUIT:
		gfx_done = 1;
		return 0; /* quit! */
	case SDL_KEYDOWN:
	case SDL_KEYUP:
		switch (e->key.keysym.sym) {
		case SDLK_ESCAPE:
			// TODO: prompt before exiting
			if (e->type == SDL_KEYDOWN) {
				gfx_done = 1;
				return 0; /* quit! */
			}
			break;
		case SDLK_F11: /* toggle fullscreen */
			if (e->type == SDL_KEYDOWN) {
				fullscreen = !fullscreen;
				if (fullscreen)
					SDL_SetWindowFullscreen(main_win,
							SDL_WINDOW_FULLSCREEN_DESKTOP);
				else
					SDL_SetWindowFullscreen(main_win, 0);
			}
			break;
#if 0 // TODO: implement all other keys
		case SDLK_LEFT: // TODO: support remapping
			keystate_send(engine_key_state.left, e.type == SDL_KEYDOWN);
			break;
		case SDLK_RIGHT: // TODO: support remapping
			keystate_send(engine_key_state.right, e.type == SDL_KEYDOWN);
			break;
		case SDLK_UP: // TODO: support remapping
			keystate_send(engine_key_state.up, e.type == SDL_KEYDOWN);
			break;
		case SDLK_DOWN: // TODO: support remapping
			keystate_send(engine_key_state.down, e.type == SDL_KEYDOWN);
			break;
		case SDLK_z: // TODO: support remapping
			keystate_send(engine_key_state.button_b, e.type == SDL_KEYDOWN);
			break;
		case SDLK_x: // TODO: support remapping
			keystate_send(engine_key_state.button_a, e.type == SDL_KEYDOWN);
			break;
		case SDLK_a: // TODO: support remapping
			keystate_send(engine_key_state.button_y, e.type == SDL_KEYDOWN);
			break;
		case SDLK_s: // TODO: support remapping
			keystate_send(engine_key_state.button_x, e.type == SDL_KEYDOWN);
			break;
		case SDLK_RETURN: // TODO: support remapping
			keystate_send(engine_key_state.start, e.type == SDL_KEYDOWN);
			break;
		case SDLK_LSHIFT: // TODO: support remapping
		case SDLK_RSHIFT: // TODO: support remapping
			keystate_send(engine_key_state.select, e.type == SDL_KEYDOWN);
			break;
#endif
		}
		break;
	}

	return 1;
}

/* collects events and calls update and paint procedures.
 * return 1 to keep going, 0 to quit, -1 on error.
 */
int
gfx_step(void)
{
	static Uint64 prev = 0;
	static Uint64 freq = 0;
	Uint64 frame_deadline; /* measured in performance counter ticks */
	Uint64 now;
	Uint32 timeout; /* milliseconds */
	SDL_Event e;

	DBG_LOG("%s():Loop start... (done=%d)", __func__, gfx_done);

	if (!prev)
		prev = SDL_GetPerformanceCounter();
	if (!freq)
		freq = SDL_GetPerformanceFrequency();

	now = SDL_GetPerformanceCounter();
	frame_deadline = now + (freq / APP_FRAMERATE);
	do_update((double)(now - prev) / freq);
	prev = now;

//		DBG_LOG("%s():update complete", __func__);

	if (do_paint())
		SDL_GL_SwapWindow(main_win);

//		DBG_LOG("%s():paint complete", __func__);


	while (SDL_PollEvent(&e)) {
		if (!handle_event(&e))
			return 0;
	}

	/* calculates the reamining time in the frame */
	now = SDL_GetPerformanceCounter();
	if (now < frame_deadline)
		timeout = (frame_deadline - now) / freq * 1000;
	else
		timeout = 0;

	if (timeout && SDL_WaitEventTimeout(&e, timeout)) {
		if (!handle_event(&e))
			return 0;
	}

	return 1;
}

/* collects events in a loop and calls update and paint procedures.
 * return 0 on quit, negative value on error.
 */
int
gfx_loop(void)
{
	int e;

	while (!gfx_done) {
		e = gfx_step();
		if (e <= 0)
			return e;
	}
	return 0;
}

// TODO: rename this
int
engine_audio_start(engine_audio_callback_t *cb, void *extra)
{
	SDL_AudioSpec desired, obtained;

	memset(&desired, 0, sizeof(desired));
	desired.freq = 44100;
	desired.format = AUDIO_S16;
	desired.channels = 2;
	desired.samples = 256;
	desired.callback = cb; // TODO: assign cb to a channel and channel_mixer to wrap.
	desired.userdata = extra;

	audio_device = SDL_OpenAudioDevice(NULL, 0, &desired, &obtained, 0);
	if (!audio_device) {
		DBG_LOG("Failed to open audio: %s", SDL_GetError());
		return -1;
	}

	SDL_PauseAudioDevice(audio_device, 0); /* start playing */

	return 0;
}

// TODO: rename this
/* pause=1, pause audio playback.
 * pause=0, resume audio playback
 */
void
engine_audio_pause(int pause)
{
	SDL_PauseAudioDevice(audio_device, pause);
}

// TODO: rename this
void
engine_audio_stop(void)
{
	SDL_CloseAudioDevice(audio_device);
}
