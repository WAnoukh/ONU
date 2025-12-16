#include "console.h"
#include <time.h>

struct Console *console_current = NULL;

void console_set_current(struct Console *console)
{
    console_current = console;
}

struct Console *console_get_current()
{
    return console_current;
}

int console_get_log_count(struct Console *console)
{
    return console->log_count;
}

int console_get_log_head(struct Console *console)
{
    return console->log_head;
}

struct Log *console_get_log_ring_buffer(struct Console *console)
{
    return console->log;
}

void console_insert_log(struct Console *console, struct Log log)
{
    console->log[console->log_head++] = log;
    if(console->log_head >= CONSOLE_MAX_LOG) console->log_head = 0;
    if(console->log_count < CONSOLE_MAX_LOG) ++console->log_count;
}

struct Log log_create_dated()
{
    struct Log log;
    time_t now = time(NULL);
    struct tm *t = localtime(&now);

    log.hour = (uint8_t)t->tm_hour;
    log.min  = (uint8_t)t->tm_min;
    log.sec  = (uint8_t)t->tm_sec;
    return log;
}

void log_safe_text_copy(struct Log *log, const char *msg)
{
    size_t cur_char = 0;
    while(cur_char < CONSOLE_LOG_LENGTH && msg[cur_char] != '\0')
    {
        log->text[cur_char] = msg[cur_char];
        ++cur_char;
    }
    log->text[cur_char] = '\0';
}

void console_log(const char *msg, enum LogLevel level)
{
    struct Console *console = console_get_current();

    struct Log log = log_create_dated(); 
    log_safe_text_copy(&log, msg);
    log.level = level;  
    console_insert_log(console, log);
}
