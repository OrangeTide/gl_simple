/* args.h : header file for args.c, argument parsing library - public domain */
#ifndef ARGS_H
#define ARGS_H

struct arginfo {
	char *name;
	int has_parameter;
	int (*func_cb)(const char *name, const char *param, void *p, void *q);
	void *p; /* extra pointer, used with args_cbset_xxx() functions */
	const char *description;
};

/* processes an arginfo list.
 * ai, nr_args - the arginfo list and its length
 * usage - function to override fallback_usage(), NULL to use fallback
 * q - pointer that is passed to all the function callbacks in the arginfo list
 * argc,argv - program arguments
 */
int args_parser(const struct arginfo *ai, size_t nr_args, void (*usage )(const char *program_name,const struct arginfo *ai,size_t nr_args ), void *q, int argc, char **argv);

/* use the void *p to set an int through (int*) */
int args_cbset_intptr(const char *name, const char *param, void *p, void *q);

/* use the void *p to set a long through (long*) */
int args_cbset_longptr(const char *name, const char *param, void *p, void *q);

/* use the void *p to set a string through (const char**) */
int args_cbset_charptrptr(const char *name, const char *param, void *p, void *q);

int args_cbset_intptr_true(const char *name, const char *param, void *p, void *q);

int args_cbset_intptr_false(const char *name, const char *param, void *p, void *q);

#endif
