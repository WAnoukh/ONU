#ifndef LOG_H
#define LOG_H

#include "console/console.h"

#define LOG_ERROR(fmt, ...)   log_formated(LOGL_ERROR,   fmt, ##__VA_ARGS__)
#define LOG_WARNING(fmt, ...) log_formated(LOGL_WARNING, fmt, ##__VA_ARGS__)
#define LOG_INFO(fmt, ...)    log_formated(LOGL_INFO,    fmt, ##__VA_ARGS__)
#define LOG_VERBOSE(fmt, ...) log_formated(LOGL_VERBOSE, fmt, ##__VA_ARGS__)

__attribute__((format(printf, 2, 3)))
void log_formated(enum LogLevel level, const char *fmt, ...);

#endif // LOG_H
