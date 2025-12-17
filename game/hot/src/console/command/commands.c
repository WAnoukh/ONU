#include "commands.h"
#include "console/console.h"
#include "editor/editor.h"
#include "editor/editor_context.h"
#include "string/StringView.h"
#include <stdio.h>

//
// MISC
//

void command_echo(struct CommandContext *cctx, StringView args, BString *out)
{
    if(args.length == 0)
    {
        bstr_cat_cstr(out, "Argument expected.");
        return;
    }
    bstr_cat_view(out, args);
}

//
// LEVEL
//

void command_load_level(struct CommandContext *cctx, StringView args, BString *out)
{
    char path_buffer[200];
    snprintf(path_buffer, 200, "resources/level/%.*s.level", (int)args.length, args.data);
    editor_load_level_from_file(cctx->ectx, path_buffer);
}

void command_run_game(struct CommandContext *cctx, StringView args, BString *out)
{
    ectx_start_level(cctx->ectx, cctx->ectx->level);
}

////////////////////////////////////////

void command_register_base()
{
    console_register_command(VIEW_FROM_CONST_STR("echo"), command_echo);
    console_register_command(VIEW_FROM_CONST_STR("load"), command_load_level);
    console_register_command(VIEW_FROM_CONST_STR("run" ), command_run_game);
}
