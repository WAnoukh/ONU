#include "engine_commands.h"
#include "console/command/commands.h"
#include "rendering/texture.h"
#include <stdlib.h>
#include "cimgui.h"
#include "string/BString.h"
#include "string/StringView.h"
#include "console/console.h"

void command_uiscale(struct CommandContext *cctx, StringView args, BString *out)
{
    char* endptr;
    float scale = strtof(args.data, &endptr);

    if(endptr == args.data){
        bstr_cat_cstr(out, "Invalid arguments: uiscale need a numerical value");
        return;

    }
    else if (scale < 1) scale = 1;

    ImGuiStyle *style = igGetStyle();
    style->FontScaleMain = scale;
}

void command_reload_art(struct CommandContext *cctx, StringView args, BString *out)
{
    load_default_images();
}

void engine_register_commands()
{
    console_clear_commands();
    command_register_base(); 
    console_register_command(VIEW_FROM_CONST_STR("uiscale"), command_uiscale);
    console_register_command(VIEW_FROM_CONST_STR("rel_art"), command_reload_art);
}
