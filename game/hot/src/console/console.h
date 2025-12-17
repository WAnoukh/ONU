#ifndef CONSOLE_H
#define CONSOLE_H

#include "console/command/command_registry.h"
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
    struct ConsoleLine     lines[CONSOLE_MAX_LOG];
    struct CommandRegistry registry;
    int                    lines_count;
    int                    lines_head;
};

struct Console console_init();

void console_register_base_commands();

void console_set_current(struct Console *console);

struct Console *console_current();

int console_lines_count(struct Console *console);

int console_lines_head(struct Console *console);

struct ConsoleLine *console_lines_ring_buffer(struct Console *console);

void console_log(StringView msg, enum LogLevel level);

void console_call_command(StringView input);

void console_register_command(StringView command_name, CommandFn fn);

#endif // CONSOLE_H
