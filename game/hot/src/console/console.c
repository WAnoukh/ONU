#include "console.h"
#include "console/command/console_input_parser.h"
#include "string/BString.h"
#include "string/StringView.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

struct Console *console_current_ptr = NULL;

void console_set_current(struct Console *console)
{
    console_current_ptr = console;
}

struct Console *console_current()
{
    return console_current_ptr;
}

struct Console console_init()
{
    struct Console console;
    console.lines_count = 0;
    console.lines_head = 0;
    console.registry.entries_count = 0;
    console.command_ctx.ectx = NULL;
    return console;
}

void console_set_command_context(struct EditorCtx *ectx)
{
    struct Console *console = console_current();
    assert(console);

    console->command_ctx.ectx = ectx;
}

int console_lines_count(struct Console *console)
{
    return console->lines_count;
}

int console_lines_head(struct Console *console)
{
    return console->lines_head;
}

struct ConsoleLine *console_lines_ring_buffer(struct Console *console)
{
    return console->lines;
}

void console_insert_line(struct Console *console, struct ConsoleLine log)
{
    console->lines[console->lines_head++] = log;
    if(console->lines_head >= CONSOLE_MAX_LOG) console->lines_head = 0;
    if(console->lines_count < CONSOLE_MAX_LOG) ++console->lines_count;
}

struct ConsoleLine console_line_create_timestamped()
{
    struct ConsoleLine line;
    time_t now = time(NULL);
    struct tm *t = localtime(&now);

    line.hour = (uint8_t)t->tm_hour;
    line.min  = (uint8_t)t->tm_min;
    line.sec  = (uint8_t)t->tm_sec;
    return line;
}

void console_line_copy_from_view(struct ConsoleLine *line, StringView view)
{
    const char *view_data = view_raw(view);

    size_t i;
    for(i = 0; i < CONSOLE_LINE_LENGTH - 1 && i < view.length; ++i)
    {
        line->text[i] = view_data[i];     
    }
    line->text[i] = '\0';
}

void console_log(StringView msg, enum LogLevel level)
{
    struct Console *console = console_current();
    assert(console);

    struct ConsoleLine log = console_line_create_timestamped(); 
    console_line_copy_from_view(&log, msg);
    log.level = level;  
    log.type = CLINE_LOG;
    console_insert_line(console, log);
}

void console_call_command(StringView input)
{
    struct Console *console = console_current();
    assert(console);

    struct ConsoleLine cmd = console_line_create_timestamped(); 
    BString out = (BString){.data = 
        cmd.text, 
        .capacity=CONSOLE_LINE_LENGTH, 
        .length=0
    };
    bstr_cat_view(&out, input);
    cmd.type = CLINE_COMMAND;

    struct ConsoleParsedCommand parsed_cmd = console_parse_input(input);

    CommandFn func = console_get_command_func_ptr(&console->registry, parsed_cmd.command_name);

    bstr_cat_cstr(&out, "\n");

    if(!func)
    {
        bstr_cat_cstr(&out, "Command unknown.");
    }
    else func(&console->command_ctx, parsed_cmd.command_args, &out);
    
    bstr_null_terminate(&out);
    console_insert_line(console, cmd);
}

void console_register_command(StringView command_name, CommandFn fn)
{
    struct Console *console = console_current();
    assert(console);

    console_register_command_internal(&console->registry, command_name, fn);
}
