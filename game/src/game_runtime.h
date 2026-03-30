#ifndef GAME_RUNTIME_H
#define GAME_RUNTIME_H

#include "error/error.h"
#include "inputs.h"
#include "memory/arena.h"
#include "window_context.h"
typedef struct GLFWwindow GLFWwindow;
struct InputInfo;

struct EditorMemory
{
    struct Arena      level;
    struct Arena      frame;
    struct Arena      editor;
    struct EditorCtx *editor_root;
};

typedef struct App 
{
    struct EditorMemory *mem;
    struct Engine *engine;
    struct GameArenas *arenas;
    struct TextureInfo *textures;

    struct EditorCtx *ectx;
} App;

struct GameArenas
{
    struct Arena *main;
    struct Arena *frame;
    struct Arena *level;
};

struct FrameContext
{
    struct WindowContext    window;
    struct InputInfo        inputs;
    float                   dt;
    float                   time;
    float                   real_dt;
    float                   real_time;
};

Error app_build(struct App *app, GLFWwindow *window, struct Arena *arena);

Error app_runtime_start(struct App *app);

void app_shutdown(struct App *app);

void app_post_reload(struct App *app);

void app_runtime_update(struct App *app, struct FrameContext frame);

#endif // GAME_RUNTIME_H
