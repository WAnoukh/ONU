#include "commands.h"
#include "console/console.h"
#include "string/StringView.h"

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

////////////////////////////////////////

void command_register_base()
{
    console_register_command(VIEW_FROM_CONST_STR("echo"), command_echo);
}
