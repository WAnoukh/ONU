#include "editor_interface.h"
#include "editor/editor.h"
#include "editor/editor_context.h"
#include "interface.h"
#include "texture.h"
#include <glad/glad.h>
#include "GLFW/glfw3.h"


void editor_start(struct EditorMemory *mem, GLFWwindow *window)
{
    struct EditorCtx *ectx = arena_allocate(&mem->editor, sizeof(struct EditorCtx));
    *ectx = ectx_default(mem);
    if(!editor_init(ectx, window))
    {
        printf("Editor Error : failed to initialize the editor.\n");
        exit(1);
    } 
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
    initialize_renderer(&ectx->camera);
    ectx->is_playing = 0;
    ectx->game = game_init();

    load_default_images(); 
}

void editor_stop(struct EditorMemory *mem)
{
    struct EditorCtx *ectx = (struct EditorCtx *)mem->editor.start;
    editor_deinit(ectx);
}

int editor_update(struct EditorMemory *mem, struct WindowInfo windowinfo, struct InputInfo inputinfo)
{
    struct EditorCtx *ectx = (struct EditorCtx *)mem->editor.start;
    ectx->window_info = windowinfo;
    return editor_update_internal(mem, ectx, inputinfo);
}
