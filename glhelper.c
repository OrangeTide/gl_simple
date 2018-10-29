/* glhelper.c : utilities useful for GL applications. public domain */
#include "gl.h"
#include "glhelper.h"
#include "logging.h"

/*******************************************************************************
 * GL helper utilities
 ******************************************************************************/

static void
glerr(const char *reason)
{
	GLenum code = glGetError();

	if (code == GL_NO_ERROR)
		return;

	if (!reason)
		ERR_LOG("GL error 0x%04x", (unsigned)code);
	else
		ERR_LOG("%s:GL error 0x%04x", reason, (unsigned)code);
}

static void
print_shader_error(GLuint shader, const char *reason)
{
	GLint info_len = 0;
	glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &info_len);
	if (info_len > 255)
		info_len = 255;
	char info[info_len + 1];
	glGetShaderInfoLog(shader, info_len, NULL, info);
	ERR_LOG("%s:%s", reason, info);
}

static GLuint
load_shader_from_string(GLenum type, const char *s)
{
	GLuint shader;
	GLint compile_status;
	const GLchar *source[] = { (const GLchar*)s };

	shader = glCreateShader(type);
	if (!shader)
		return 0;

	glShaderSource(shader, 1, source, NULL);
	glCompileShader(shader);
	glGetShaderiv(shader, GL_COMPILE_STATUS, &compile_status);
	if (!compile_status) {
		print_shader_error(shader, "shader compile failed");
		glDeleteShader(shader);
		return 0;
	}

	return shader;
}

GLuint
shader_load(const char *vertex_source, const char *fragment_source)
{
	GLuint vertex_shader;
	GLuint fragment_shader;
	GLuint program;
	GLint link_status;

	vertex_shader = load_shader_from_string(GL_VERTEX_SHADER, vertex_source);
	fragment_shader = load_shader_from_string(GL_FRAGMENT_SHADER, fragment_source);
	if (!vertex_shader || !fragment_shader)
		goto err;
	program = glCreateProgram();
	if (!program) {
		glerr("glCreateProgram()");
		goto err_free_shaders;
	}
	glAttachShader(program, vertex_shader);
	glAttachShader(program, fragment_shader);
// #if USE_GLES2
//	glBindAttribLocation(program, 0, "vPosition");
//	glBindAttribLocation(program, 1, "vColor");
// #endif
	glLinkProgram(program);
	glGetProgramiv(program, GL_LINK_STATUS, &link_status);
	if (!link_status) {
		print_shader_error(program, "shader linking failed");
		goto err_free_program;
	}
	glDeleteShader(vertex_shader);
	glDeleteShader(fragment_shader);
	return program;
err_free_program:
	glDeleteProgram(program);
err_free_shaders:
	glDeleteShader(vertex_shader);
	glDeleteShader(fragment_shader);
err:
	return 0;
}
