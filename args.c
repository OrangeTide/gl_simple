/* args.c : parses command-line arguments. public domain. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "compiler.h"
#include "args.h"

/*******************************************************************************
 * Argument parser
 ******************************************************************************/

/* default function that displays the usage */
static void
fallback_usage(const char *program_name, const struct arginfo *ai, size_t nr_args)
{
	/* TODO: process args */
	size_t i;

	fprintf(stderr, "usage: %s\n", program_name);

	for (i = 0; i < nr_args; i++) {
		if(ai[i].name) {
			fprintf(stderr, "  %-13s %s\n", ai[i].name,
				ai[i].description ? ai[i].description : "");
		}
	}
}

/* processes an arginfo list.
 * ai, nr_args - the arginfo list and its length
 * usage - function to override fallback_usage(), NULL to use fallback
 * q - pointer that is passed to all the function callbacks in the arginfo list
 * argc,argv - program arguments
 */
int
args_parser(const struct arginfo *ai, size_t nr_args,
		void (*usage)(const char *program_name, const struct arginfo *ai, size_t nr_args),
		void *q, int argc, char **argv)
{
	int i;
	size_t j;
	const char *opt;
	const char *program_name;

	/* set program_name to something without paths */
	if (!(program_name = strrchr(argv[0], '/')))
		program_name=argv[0];
	else
		program_name++;

	/* use a fallback usage if one was not supplied */
	if (!usage)
		usage=fallback_usage;

	for (i = 1; i < argc; i++) {
		for (j = 0; j < nr_args; j++) {
			if (!strcasecmp(argv[i], ai[j].name)) {
				if (ai[j].has_parameter && (i + 1) >= argc) {
					fprintf(stderr, "Error: option '%s' takes an argument.\n", ai[j].name);
					goto usage;
				}
				opt = ai[j].has_parameter ? argv[++i] : NULL;
				if (ai[j].func_cb) {
					if (!ai[j].func_cb(ai[j].name, opt, ai[j].p, q)) {
						fprintf(stderr, "Error: failed with option '%s'.\n", ai[j].name);
						goto usage;
					}
				}
				goto success; /* we processed a parameter successfully */
			}
		}
		/* display an error message if we got here when the option wasn't -help */
		if (strcasecmp(argv[i], "-help")) {
			fprintf(stderr, "Error: unknown command-line option '%s'.\n", argv[i]);
		}
usage:
		usage(program_name, ai, nr_args);
		exit(EXIT_FAILURE);
success:
		;
	}
	return 1; /* success */
}

/* use the void *p to set an int through (int*) */
int
args_cbset_intptr(const char *name _unused, const char *param, void *p, void *q _unused)
{
	char *endptr;
	int *intp = p;

	if (intp) {
		*intp = strtol(param, &endptr, 0);
		if (endptr != param && !*endptr)
			return 1; /* success */
	}

	return 0; /* failure */
}

/* use the void *p to set a long through (long*) */
int
args_cbset_longptr(const char *name _unused, const char *param, void *p, void *q _unused)
{
	char *endptr;
	long *longp = p;

	if (longp) {
		*longp = strtol(param, &endptr, 0);
		if (endptr != param && !*endptr)
			return 1; /* success */
	}

	return 0; /* failure */
}

/* use the void *p to set a string through (const char**) */
int
args_cbset_charptrptr(const char *name _unused, const char *param, void *p, void *q _unused)
{
	const char **strptr = p;

	if (strptr) {
		*strptr = param;
		return 1; /* success */
	}

	return 0; /* empty */
}

/* use the void *p to set an int to true(1). there should be no param */
int
args_cbset_intptr_true(const char *name _unused, const char *param _unused, void *p, void *q _unused)
{
	int *intp = p;

	if (intp) {
		*intp = 1;
		return 1; /* success */
	}

	return 0; /* failure */
}

/* use the void *p to set an int to true(1). there should be no param */
int
args_cbset_intptr_false(const char *name _unused, const char *param _unused, void *p, void *q _unused)
{
	int *intp = p;

	if (intp) {
		*intp = 1;
		return 1; /* success */
	}

	return 0; /* failure */
}
