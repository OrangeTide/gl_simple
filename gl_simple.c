/* gl_simple.c : simple OpenGL application. public domain */
/*******************************************************************************
 * simple GL example program
 ******************************************************************************/
#include <stdio.h>
#include <stdlib.h>

#include <SDL2/SDL.h>

#include "compiler.h"
#include "logging.h"
#include "args.h"
#include "gl.h"
#include "glhelper.h"
#include "gfx.h"
#include "config.h"
#include "debugconsole.h"

/*******************************************************************************
 * Globals
 ******************************************************************************/

/* configured from command-line arguments */
static int graphics_enabled = !APP_COMMAND_LINE;
static int debug_enabled = 0;
const char *app_title;
static unsigned short bmp_width = APP_WIDTH, bmp_height = APP_HEIGHT;
static const char *vertex_shader_filename = "simple.vert";
static const char *fragment_shader_filename = "simple.frag";

/* application state */
static uint32_t *bmp_pixels;
static struct {
	GLubyte r, g, b; /* for the background color */
	double world_time;
	double last_update; // TODO: make tick rate configurable
	double world_speed; /* time in seconds for 1 tick */
} app_state;

/*******************************************************************************
 * World State - the "Model" part of Model-View-Controller
 ******************************************************************************/

void
world_init()
{
	app_state.world_speed = 0.500; /* 500 ms for a tick */

	app_state.r = 127;
	app_state.g = 255;
	app_state.b = 63;

	app_state.world_time = 0.0;
//	app_state.last_update = -app_state.world_speed;
}

/*******************************************************************************
 * App
 ******************************************************************************/

/* dummy function */
int
app_do_nothing(void)
{
	return 0;
}

/* initializes things before drawing */
int
app_pre_init(void)
{
	world_init();

	return 0;
}

/* called whenever it needs to be determine if program should continue */
int
app_keep_going(void)
{
	return !gfx_done;
}

/*******************************************************************************
 * Graphics callbacks
******************************************************************************/

void
do_cleanup_gl(void)
{
#if 0
	// TODO: free shader program
	vposition_loc = 0;
	texcoord_loc = 0;
	vnormal_loc = 0;
	texsampler_loc = 0;
	modelview_loc = 0;
	projection_loc = 0;

	/* free video texture */
	glBindTexture(GL_TEXTURE_2D, 0);
	if (video_tex)
		glDeleteTextures(1, &video_tex);
	video_tex = 0;
#endif
}

int
do_setup_gl(void)
{
#if 0
	/* load and link shaders into a program */
	char *vert, *frag;

	vert = load_file(vertex_shader_filename, NULL);
	frag = load_file(fragment_shader_filename, NULL);
	if (!vert || !frag)
		goto failure;

	my_program = shader_load(vert, frag);
	if (!my_program)
		goto failure;

	free(frag);
	free(vert);

	GL_CHECK_ERROR();

	/* setup locations for binding */
	glUseProgram(my_program);

	vposition_loc = glGetAttribLocation(my_program, "vPosition");
	texcoord_loc = glGetAttribLocation(my_program, "vTexCoord");
	vnormal_loc = glGetAttribLocation(my_program, "vNormal");
	texsampler_loc = glGetUniformLocation(my_program, "texsampler");
	modelview_loc = glGetUniformLocation(my_program, "modelview");
	projection_loc = glGetUniformLocation(my_program, "projection");

	GL_CHECK_ERROR();

	// HACK: fill in bmp_pixels[] with some data
	int y;
	for (y = 0; y < bmp_height; y++) {
		memset(bmp_pixels + y * bmp_width, (y * 91L) & 255, bmp_width * sizeof(*bmp_pixels));
	}

	/* setup vbo for the quad representing the screen */
	float screen_vertices[] = {
		-0.5, -0.5, 0,
		0.5, -0.5, 0,
		-0.5, 0.5, 0,
		0.5, 0.5, 0,
		-0.5, 0.5, 0,
		0.5, -0.5, 0,
	};

	assert(screen_vbo == 0); /* cannot already be initialized */
	glGenBuffers(1, &screen_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, screen_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(screen_vertices), screen_vertices,
		GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0); /* unbind VBO */

	assert(screen_vao == 0); /* cannot already be initialized */
	glGenVertexArrays(1, &screen_vao);
	glBindVertexArray(screen_vao);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, screen_vbo);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glBindBuffer(GL_ARRAY_BUFFER, 0); /* unbind VBO */
	glDisableVertexAttribArray(0);
	glBindVertexArray(0); /* unbind VAO */

	GL_CHECK_ERROR();

	/* create textures */
	assert(video_tex == 0); /* cannot already be initialized */
	glGenTextures(1, &video_tex);
	glBindTexture(GL_TEXTURE_2D, video_tex);

	glTexParameteri(GL_TEXTURE_2D,
		GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D,
		GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D,
		GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D,
		GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB,
		bmp_width, bmp_height, 0,
		GL_RGBA8, GL_UNSIGNED_BYTE, bmp_pixels);

	GL_CHECK_ERROR();

	return 0;

failure:
	ERR_LOG("Unabled to setup graphics parameters");
	free(frag);
	free(vert);
	return -1;
#endif
	return 0;
}

void
do_update(double seconds)
{

	app_state.world_time += seconds;

	/* update world state if a tick has elapsed */
	if (app_state.world_time > app_state.last_update + app_state.world_speed) {
		app_state.last_update = app_state.world_time;
		app_state.r = rand() % 256;
		app_state.g = rand() % 256;
		app_state.b = rand() % 256;
	}

	// TODO: glBufferSubData() to update the dirty parts of the bitmap texture
}

int
do_paint()
{
	// glClearColor(0.0, 1.0, 0.0, 1.0);

	glClearColor(app_state.r / 255., app_state.g / 255., app_state.b / 255., 1.0);
	glClear(GL_COLOR_BUFFER_BIT);
#if 0
	glUseProgram(my_program);

	// glEnable(GL_BLEND);
	// glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	GL_CHECK_ERROR();

	glActiveTexture(GL_TEXTURE0);
	glUniform1i(texsampler_loc, 0); // GL_TEXTURE0
	glBindTexture(GL_TEXTURE_2D, video_tex);

	GL_CHECK_ERROR();

	// TODO: load matrix to work in virtual screen coordinates 256x192

	/* select the vertex and draw it */
	glBindVertexArray(screen_vao);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, screen_vbo);

	GL_CHECK_ERROR();

	glDrawArrays(GL_TRIANGLES, 0, 2 * 3);

	GL_CHECK_ERROR();

	/* clean up state */
	glBindBuffer(GL_ARRAY_BUFFER, 0); /* unbind VBO */
	glDisableVertexAttribArray(0);
	glBindVertexArray(0); /* unbind VAO */
	glBindTexture(GL_TEXTURE_2D, 0);

	GL_CHECK_ERROR();
#endif

	GL_CHECK_ERROR();

	return 1; /* screen was modified - caller needs to do swap buffers */
}

/*******************************************************************************
 * Init & Clean-up
 ******************************************************************************/

static void
cleanup(void)
{
	// TODO: clean up and free all allocated data

	gfx_quit();

	free(bmp_pixels);
	bmp_pixels = NULL;
}

static int
init(void)
{
	if (graphics_enabled) {
		bmp_pixels = calloc(sizeof(*bmp_pixels) * bmp_width, bmp_height);

		if (gfx_init())
			return -1; // TODO: clean-up
	}

	return 0;
}

static void
loop(void)
{
	while (APP_KEEP_GOING) {

		APP_STEP;

		gfx_step();
	}
}

static int
process_args(int argc, char **argv)
{
	const struct arginfo ai[] = {
		{ "-title", 1, args_cbset_charptrptr, &app_title, "Set the window title." },
		{ "-graphics", 0, args_cbset_intptr_true, &graphics_enabled, "Enable graphics display window." },
		{ "-nographics", 0, args_cbset_intptr_false, &graphics_enabled, "Disable graphics display window." },
		{ "-frag", 1, args_cbset_charptrptr, &fragment_shader_filename, "Load alternative fragment shader." },
		{ "-vert", 1, args_cbset_charptrptr, &vertex_shader_filename, "Load alternative vertex shader." },
		{ "-debug", 0, args_cbset_intptr_true, &debug_enabled, "Enable debug console window." },
		{ "-nodebug", 0, args_cbset_intptr_false, &debug_enabled, "Disable debug console window." },
		// TODO: provide a way to application to support additional arguments
	};

	return args_parser(ai, sizeof(ai) / sizeof(*ai), NULL, NULL, argc, argv);
}

/*******************************************************************************
 * Main
 ******************************************************************************/

int
main(int argc, char **argv)
{
	if (!process_args(argc, argv))
		return 1;

	if (debug_enabled) {
		redirect_console();

		fprintf(stdout, "Test output to stdout\n");
		fprintf(stderr, "Test output to stderr\n");
	}

	if (APP_PRE_INIT)
		return 1;

	if (init())
		return 1;

	if (APP_POST_INIT) {
		cleanup();
		return 1;
	}

	loop();

	cleanup();

	return 0;
}
