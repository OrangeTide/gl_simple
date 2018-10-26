/* glhelper.h : header file for glhelper.c - public domain */
#ifndef GLHELPER_H
#define GLHELPER_H
#define GL_CHECK_ERROR() do { \
	GLenum e = glGetError(); \
	if (e) { \
		DBG_LOG("GL ERROR 0x%08lx @ %s():%d", (unsigned long)e, \
			__func__, __LINE__); \
	} \
	} while (0)

char *load_file(const char *filename, size_t *len);
GLuint shader_load(const char *vertex_source, const char *fragment_source);
#endif
