#include "game_commands.h"
#include "console/console.h"
#include "game.h"
#include "string/StringView.h"
#include "rendering/rendering.h"
#include "console/log.h"

void command_reload_shaders(struct CommandContext *cctx, StringView args, BString *out)
{
    load_default_shaders(); 
    LOG_INFO("Shaders reloaded!");
}

void game_register_commands()
{
    console_register_command(VIEW_FROM_CONST_STR("relshad"), command_reload_shaders);
}
