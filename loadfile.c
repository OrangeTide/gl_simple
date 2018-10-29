/* loadfile.c : loads a raw file into a dynamically allocated buffer. public domain */
#include <stdlib.h>
#include <stdio.h>

#include "logging.h"
#include "loadfile.h"

/* allocates a string containing the contents of a file.
 * you must free() the string. */
char *
load_file(const char *filename, size_t *len)
{
	FILE *f = fopen(filename, "rb");
	if (!f)
		return perror(filename), NULL;
	long p;
	char *s = NULL;
	size_t cnt;

	if (fseek(f, 0L, SEEK_END))
		goto failed;
	p = ftell(f);
	if (p < 0)
		goto failed;
	rewind(f);

	s = malloc(p + 1);
	if (!s)
		goto failed;
	cnt = fread(s, 1, p, f);
	if ((long)cnt != p) { /* requested length doesn't match */
		ERR_LOG("%s:truncated result", filename);
		free(s);
		goto failed;
	}
	s[cnt] = 0; /* null terminate the string */
	fclose(f);

	if (len)
		*len = cnt;

	return s;
failed:
	perror(filename);
	fclose(f);
	return NULL;
}
