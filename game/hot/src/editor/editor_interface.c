#include "editor_interface.h"
#include "console/log.h"
#include "editor/editor.h"
#include "editor/editor_context.h"
#include "interface.h"
#include "texture.h"
#include <glad/glad.h>
#include "GLFW/glfw3.h"


void editor_restart(struct EditorMemory *mem, GLFWwindow *window)
{
    struct EditorCtx *ectx = mem->editor_root;
    editor_imgui_init(ectx, window);
    render_set_info(ectx->renderinginfo);
    texture_set_info(ectx->textureinfo);
    console_set_current(&ectx->console);
    LOG_INFO("DLL's has been reloaded.");

    ImGuiStyle* style = igGetStyle();

    ImGuiStyle_ScaleAllSizes(style, ectx->ui_scale);
    style->FontScaleMain *= ectx->ui_scale;

    //load_default_images(); 
    //initialize_renderer();
}

void editor_start(struct EditorMemory *mem, GLFWwindow *window)
{
    struct EditorCtx *ectx = arena_allocate(&mem->editor, sizeof(struct EditorCtx));
    mem->editor_root = ectx;
    *ectx = ectx_default(mem);
    ectx->renderinginfo = arena_allocate(&mem->editor, sizeof(struct RenderingInfo));
    ectx->textureinfo = arena_allocate(&mem->editor, sizeof(struct TextureInfo));
    editor_restart(mem, window);
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
    load_default_images(); 
    initialize_renderer();
    editor_init(ectx, window);
}

void editor_stop(struct EditorMemory *mem)
{
    struct EditorCtx *ectx = (struct EditorCtx *)mem->editor.start;
    editor_deinit(ectx);
}

int editor_update(struct EditorMemory *mem, struct WindowInfo windowinfo, struct InputInfo inputinfo)
{
    struct EditorCtx *ectx = mem->editor_root;
    ectx->window_info = windowinfo;
    return editor_update_internal(mem, ectx, inputinfo);
}
