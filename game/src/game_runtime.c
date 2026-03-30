#include "game_runtime.h"
#include "editor/editor.h"
#include "editor/editor_context.h"
#include "editor/editor_interface.h"
#include "game.h"
#include "GLFW/glfw3.h"
#include "engine.h"
#include "engine_commands.h"
#include "game_commands.h"
#include "memory/allocation.h"
#include "memory/arena.h"
#include "rendering/rendering.h"
#include "rendering/texture.h"
#include <assert.h>
#include "cimgui.h"
#include "cimgui_impl.h"


void app_start(App *app)
{
    editor_start(app->mem, app->engine->window_ctx);
}
 
void app_update(App *app, struct FrameContext *frame)
{
    editor_update(app->mem, frame);
}

void do_engine_setup(struct App *app)
{
    //TODO decoment that
    //engine_setup_imgui(app->engine);
    engine_setup_globals(app->engine);
    engine_register_commands();
    game_register_commands();
}

Error app_build(struct App *app, GLFWwindow *window, struct Arena *arena)
{
    Error error = ERR_OK;
    ArenaOffset arena_offset = arena_get_offset(arena);
    *app = (struct App){0};

    struct GameArenas *arenas;
    TRY_ALLOC(arenas, arena_allocate(arena, sizeof(struct GameArenas)),
            ERR_OOM, fail);
    arenas->main = arena;

    TRY_ALLOC(arenas->frame, arena_allocate(arena, sizeof(struct Arena)),
            ERR_OOM, fail);
    TRY_ALLOC(arenas->level, arena_allocate(arena, sizeof(struct Arena)),
            ERR_OOM, fail);

    if(
        !arena_create_patch(arena, MB(1), arenas->level) ||
        !arena_create_patch(arena, MB(1), arenas->frame)
      ) 
    { error = ERR_OOM; goto fail; }

    struct Engine *engine;
    TRY_ALLOC(engine, arena_allocate(arena, sizeof(struct Engine)),
            ERR_OOM, fail);

    TRY(engine_build(engine, window, arena), fail);

    struct TextureInfo *texture_info;
    TRY_ALLOC(texture_info, arena_allocate(arena, sizeof(struct TextureInfo)),
            ERR_OOM, fail);
    texture_set_info(texture_info);

    //TODO change that plzzzz
    struct EditorCtx *ectx = arena_allocate(arena, sizeof(struct EditorCtx));
    struct EditorMemory *mem = arena_allocate(arena, sizeof(struct EditorMemory));
    *mem = (struct EditorMemory){
        .editor_root = ectx,
        .frame  = *arenas->frame,
            .editor = *arenas->main,
            .level = *arenas->level,
    };

    app->engine    = engine;
    app->arenas    = arenas;
    app->textures  = texture_info;
    *ectx = ectx_default(mem);
    app->mem = mem;

    return error;
fail:
    arena_set_offset(arena, arena_offset);
    return error;
}

Error app_runtime_start(struct App *app)
{
    do_engine_setup(app);
    rendering_start(app->engine->rendering);
    load_default_images();
    console_set_command_context(app);

    app_start(app);
    return ERR_OK;
    assert(app);
    assert(app->engine);
    assert(app->engine->rendering);
    Error error = ERR_OK;

    glEnable(GL_DEPTH_TEST);
    glfwSwapInterval( 0 );


    return error;
}

void app_post_reload(struct App *app)
{
    do_engine_setup(app);
    texture_set_info(app->textures);
    load_default_images();
}

void app_shutdown(struct App *app)
{
    engine_shutdown(app->engine);
}

void app_runtime_update(struct App *app, struct FrameContext frame)
{
    assert(app);
    assert(app->engine);
    struct Engine *engine = app->engine;

    //TODO decoment that
    //engine_imgui_new_frame();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);

    app_update(app, &frame);

    engine_imgui_render(engine);

    glfwSwapBuffers(engine->window_ctx);

    arena_reset(app->arenas->frame);
}

