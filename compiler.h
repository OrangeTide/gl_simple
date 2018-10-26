/* compiler.h : wrappers for compiler extensions. public domain */
#ifndef COMPILER_H
#define COMPILER_H

/* _unused can be used as a hint when arguments are not used */
#ifndef _unused
#  ifdef __GNUC__
#    define _unused __attribute__((unused))
#  else
#    define _unused /* TODO: define for your compiler */
#  endif
#endif

#endif
