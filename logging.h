/* logging.h : logging macros. public domain */
#ifndef LOGGING_H
#define LOGGING_H

#include <stdio.h>

// #if 1 /* disabled logging */
#ifdef NDEBUG
#  define DBG_LOG(...) /* disabled */
#  define TRACE_LOG(f, ...) /* disabled */
#else /* uses ANSI/ISO C stdio routines */
#  define DBG_LOG(f, ...) fprintf(stderr, "DEBUG:" f "\n", ## __VA_ARGS__)
#  define TRACE_LOG(f, ...) fprintf(stderr, "TRACE:" f "\n", ## __VA_ARGS__)
#endif

/* uses ANSI/ISO C stdio routines */
#define ERR_LOG(f, ...) fprintf(stderr, "ERROR:" f "\n", ## __VA_ARGS__)

#endif
