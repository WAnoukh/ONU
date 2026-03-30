#ifndef FATAL_H
#define FATAL_H

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

#define fatal(...) fatal_impl(__FILE__, __LINE__, __VA_ARGS__)

static void fatal_impl(const char* file, int line, const char* fmt, ...)
{
    fprintf(stderr, "[FATAL] %s:%d: ", file, line);

    va_list args;
    va_start(args, fmt);
    vfprintf(stderr, fmt, args);
    va_end(args);

    fprintf(stderr, "\n");

    fflush(stderr);
    fflush(stdout);

    abort(); 
}

#endif // FATAL_H
