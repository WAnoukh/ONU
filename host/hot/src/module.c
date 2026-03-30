#include "module.h"
#include "game.h"
#include "game_runtime.h"
#include "interface.h"
#include "console/log.h"
#include <assert.h>


int init(struct HostContext *host)
{
    int reloaded = 1;

    if(!host->root)
    {
        //TODO log that
        struct Arena *main_arena = &host->arenas.main_arena;
        host->root = arena_allocate(main_arena, sizeof(struct Game));
        if(!host->root) return 0;

        if(app_build(host->root, host->window_ctx, &host->arenas.main_arena) != ERR_OK) return 0;
        if(app_runtime_start(host->root) != ERR_OK) return 0;

        reloaded = 0;
    }

    if(reloaded)
    {
        app_post_reload(host->root);
        LOG_INFO("DLL's has been reloaded.");
    }
    else
    {
        LOG_INFO("Engine initialized.");
    }

    return 1;
}

void deinit(struct HostContext *host)
{
    assert(host->root);
    app_shutdown(host->root);
}

void update(struct HostContext *host)
{
    assert(host->root);

    struct FrameContext frame = (struct FrameContext)
    {
        .window    = host->window, 
        .inputs    = host->inputinfo,
        .real_dt   = host->delta_time,
        .real_time = host->time,
        .dt   = 0,
        .time = 0,
    };

    app_runtime_update(host->root, frame);
}

