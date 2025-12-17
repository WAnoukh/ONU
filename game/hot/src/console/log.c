#include "log.h"
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

void log_formated(enum LogLevel level, const char *fmt, ...)
{
    char buffer[CONSOLE_LINE_LENGTH];

    va_list args;
    va_start(args, fmt);

    int written = vsnprintf(buffer, sizeof(buffer), fmt, args);

    va_end(args);

    if(written < 0)
        return;

    buffer[sizeof(buffer) - 1] = '\0';

    StringView view = { .data = buffer, .length = strlen(buffer) };
    console_log(view, level);
}
