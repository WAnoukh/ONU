#include "console.h"
#include "string/StringView.h"
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

void console_log(StringView view, enum LogLevel level)
{
    struct Console *console = console_current();

    struct ConsoleLine log = console_line_create_timestamped(); 
    console_line_copy_from_view(&log, view);
    log.level = level;  
    log.type = CLINE_LOG;
    console_insert_line(console, log);
}

void console_command(StringView view)
{
    struct Console *console = console_current();

    struct ConsoleLine cmd = console_line_create_timestamped(); 
    console_line_copy_from_view(&cmd, view);
    cmd.type = CLINE_COMMAND;
    console_insert_line(console, cmd);
}
