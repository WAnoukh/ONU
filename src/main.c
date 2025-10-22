#include "editor/editor_context.h"
#define EDITOR

#include "game.h"
#include "serialization.h"
#include "texture.h"
#include "window/input.h"
#include "window/window.h"
#include "rendering/rendering.h"
#ifdef EDITOR
#include "editor/editor.h"
#endif

#include <stdio.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

int main()
{
    if (!initGl())
    {
        printf("Failed to initialize OpenGL context\n");
        return 1;
    }

#ifdef EDITOR
    struct EditorCtx ectx = ectx_default(); 
    if(!editor_init(&ectx))
    {
        printf("Editor Error : failed to initialize the editor.\n");
        exit(1);
    }
    initialize_renderer(&ectx.camera);
    ectx.is_playing = 0;
    ectx.game = game_init();

    load_default_images(); 

    while (!glfwWindowShouldClose(w_get_window_ctx()))
    {

        GLenum err;
        while ((err = glGetError()) != GL_NO_ERROR) {
            printf("OpenGL error: 0x%X\n", err);
        }


        editor_update(&ectx);

        glfwSwapBuffers(w_get_window_ctx());
        i_clear_pressed();
        glfwPollEvents();
    }
    editor_deinit(&ectx);
    glfwTerminate();
#endif

    return 0;
}
