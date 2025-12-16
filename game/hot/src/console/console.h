#ifndef CONSOLE_H
#define CONSOLE_H

#include "string/StringView.h"
#include <stddef.h>
#include <stdint.h>

#define CLOG( msg ) console_log(msg, sizeof(msg)) 

#define CONSOLE_MAX_LOG 256
#define CONSOLE_LINE_LENGTH 256

enum ConsoleLineType
{
    CLINE_LOG,
    CLINE_COMMAND,
};

enum LogLevel
{
    LOGL_ERROR,
    LOGL_WARNING,
    LOGL_INFO,
    LOGL_VERBOSE,
    LOGL_COUNT,
};

struct ConsoleLine 
{
    char                 text[CONSOLE_LINE_LENGTH];
    enum ConsoleLineType type;
    enum LogLevel        level;
    uint8_t              hour;
    uint8_t              min;
    uint8_t              sec;
};

struct Console
{
    struct ConsoleLine lines[CONSOLE_MAX_LOG];
    int        lines_count;
    int        lines_head;
};

static inline struct Console console_init()
{
    struct Console console;
    console.lines_count = 0;
    console.lines_head = 0;
    return console;
}

void console_set_current(struct Console *console);

struct Console *console_current();

int console_lines_count(struct Console *console);

int console_lines_head(struct Console *console);

struct ConsoleLine *console_lines_ring_buffer(struct Console *console);

void console_log(StringView view, enum LogLevel level);

void console_command(StringView view);

#endif // CONSOLE_H
