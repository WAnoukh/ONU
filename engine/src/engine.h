#ifndef ENGINE_H
#define ENGINE_H 

#include "console/console.h"
#include "error/error.h"
typedef struct GLFWwindow GLFWwindow;
struct HostContext;
struct InputInfo;
struct Arena;

struct Engine 
{
    struct ImGuiContext *imgui_ctx;
    GLFWwindow          *window_ctx;
    struct Rendering    *rendering;
    struct Console      *console;
    int                  show_console;
};

struct Engine *engine_get_current();

void engine_set_current(struct Engine *engine);

Error engine_build(struct Engine *engine, GLFWwindow *window, struct Arena *arena);

void engine_setup_imgui(struct Engine *engine);

void engine_setup_globals(struct Engine *engine);

void engine_shutdown(struct Engine *engine);

void engine_imgui_new_frame();

void engine_imgui_render(struct Engine *engine);

#endif // ENGINE_H 
