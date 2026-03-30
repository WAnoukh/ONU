#include "engine.h"

#include <assert.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "console/imgui_console.h"
#include "error/error.h"
#include "memory/allocation.h"
#include "memory/arena.h"
#include "rendering/rendering.h"
#include "cimgui.h"
#include "cimgui_impl.h"

struct Engine *current_engine;

struct Engine *engine_get_current()
{
    assert(current_engine);
    return current_engine;
}

void engine_set_current(struct Engine *engine)
{
    assert(engine);
    current_engine = engine;
}

void engine_setup_contexts(struct Engine *engine, GLFWwindow *window_ctx)
{
    engine->window_ctx = window_ctx;
}

void engine_setup_imgui(struct Engine *engine)
{
    struct ImGuiContext *ig_ctx = igCreateContext(NULL);
    engine->imgui_ctx = ig_ctx;
    igSetCurrentContext(ig_ctx);
    ImGui_ImplGlfw_InitForOpenGL(glfwGetCurrentContext(), true);
    ImGui_ImplOpenGL3_Init("#version 130");

    
    ImGuiStyle *style = igGetStyle();
    style->FontScaleMain = 2;

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        perror("Failed to initialize GLAD\n");
        return;
    }
}

void engine_setup_globals(struct Engine *engine)
{
    assert(engine);
    engine_set_current(engine);

    assert(engine->console);
    console_set_current(engine->console);

    assert(engine->rendering);
    rendering_set_current(engine->rendering);
}

Error engine_build(struct Engine *engine, GLFWwindow *window, struct Arena *arena)
{
    Error error = ERR_OK;
    ArenaOffset arena_offset = arena_get_offset(arena);

    *engine = (struct Engine){0};

    // GLFW
    glfwMakeContextCurrent(window);

    // CONSOLE
    struct Console *console;
    TRY_ALLOC(console, arena_allocate(arena, sizeof(struct Console)),
            ERR_OOM, fail);
    console_defaults(console);

    // Rendering
    struct Rendering *rendering;
    TRY_ALLOC(rendering, arena_allocate(arena, sizeof(struct Rendering)),
            ERR_OOM, fail);
    rendering_defaults(rendering);

    engine->window_ctx = window;
    engine->console = console;
    engine->rendering = rendering;

    return error;
fail:
    arena_set_offset(arena, arena_offset);
    return error;
}

void engine_shutdown(struct Engine *engine)
{
    assert(engine->imgui_ctx);

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    igDestroyContext(engine->imgui_ctx);
}

void engine_imgui_new_frame()
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();

    igNewFrame();
}

void engine_imgui_render(struct Engine *engine)
{
    if (engine->show_console) igConsole();
    igRender();
    ImGui_ImplOpenGL3_RenderDrawData(igGetDrawData());
}

