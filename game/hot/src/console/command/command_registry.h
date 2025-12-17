#ifndef COMMAND_REGISTRY_H
#define COMMAND_REGISTRY_H

#include "string/BString.h"
#include "string/StringView.h"

#define COMMAND_NAME_LENGTH 128
#define COMMAND_MAX_COUNT   64

struct CommandContext;

typedef void (*CommandFn)(struct CommandContext *cctx, StringView args, BString *out);

struct CommandEntry
{
    char      name[COMMAND_NAME_LENGTH];
    CommandFn fn;
};

struct CommandRegistry
{
    struct CommandEntry entries[COMMAND_MAX_COUNT];
    size_t              entries_count;
};

void console_register_command_internal(struct CommandRegistry *registry, StringView name, CommandFn fn);

CommandFn console_get_command_func_ptr(struct CommandRegistry *registry, StringView name);

#endif // COMMAND_REGISTRY_H
