#include "command_registry.h"
#include "console/log.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>

struct CommandEntry entry_create(StringView name, CommandFn fn)
{
    struct CommandEntry entry = (struct CommandEntry)
    {
        .fn = fn
    };

    size_t view_len = view_length(name);
    assert(view_len < COMMAND_NAME_LENGTH - 1/* Command name is too large. */);

    view_copy_to_cstr(name, entry.name);

    return entry;
}

void console_register_command_internal(struct CommandRegistry *registry, StringView name, CommandFn fn)
{
    if(registry->entries_count >= COMMAND_MAX_COUNT - 1)
    {
        LOG_ERROR("Failed to register command \"%.*s\": entries full.", (int)name.length, name.data);
        return;
    }
    
    struct CommandEntry entry = entry_create(name, fn);

    for(int i = 0; i < registry->entries_count; ++i)
    {
        if(!strcmp(entry.name, registry->entries[i].name))    
        {
        LOG_ERROR("Failed to register command \"%.*s\": this command already exists.", (int)name.length, name.data);
            return;
        }
    }

    registry->entries[registry->entries_count++] = entry;

    LOG_VERBOSE("Command \"%.*s\" succefully registered.", (int)name.length, name.data);
}

CommandFn console_get_command_func_ptr(struct CommandRegistry *registry, StringView name)
{
    char command_name[COMMAND_NAME_LENGTH];
    view_copy_to_cstr(name, command_name);

    for(int i = 0; i < registry->entries_count; ++i)
    {
        struct CommandEntry *entry = registry->entries+i;
        if(!strcmp(command_name, entry->name))    
        {
            return entry->fn;
        }
    }
    return NULL;
}
