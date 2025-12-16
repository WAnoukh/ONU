#ifndef CONSOLE_H
#define CONSOLE_H

#include <stddef.h>
#include <stdint.h>

#define CLOG( msg ) console_log(msg, sizeof(msg)) 

#define CONSOLE_MAX_LOG 256
#define CONSOLE_LOG_LENGTH 256

enum LogLevel
{
    LOGL_ERROR,
    LOGL_WARNING,
    LOGL_INFO,
    LOGL_VERBOSE,
    LOGL_COUNT,
};

struct Log 
{
    uint8_t       hour;
    uint8_t       min;
    uint8_t       sec;
    enum LogLevel level;
    char          text[CONSOLE_LOG_LENGTH];
};

struct Console
{
    struct Log log[CONSOLE_MAX_LOG];
    int        log_count;
    int        log_head;
};

static inline struct Console console_init()
{
    struct Console console;
    console.log_count = 0;
    console.log_head = 0;
    return console;
}

void console_set_current(struct Console *console);

struct Console *console_get_current();

int console_get_log_count(struct Console *console);

int console_get_log_head(struct Console *console);

struct Log *console_get_log_ring_buffer(struct Console *console);

void console_log(const char *msg, enum LogLevel level);

#endif // CONSOLE_H
