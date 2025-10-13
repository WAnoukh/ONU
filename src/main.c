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


struct Game initialize_game()
{
    struct Game game;

    game = game_init();

    game_setup_default_level(&game);
    game.last_time = glfwGetTime();
    game.tilemap_layer_mask = -1;
    game.camera = camera_get_default();
    camera_compute_view(&game.camera);
    return game;
}

int main()
{
    if (!initGl())
    {
        printf("Failed to initialize OpenGL context\n");
        return 1;
    }

#ifdef EDITOR
    if(!editor_initialize())
    {
        printf("Editor Error : failed to initialize the editor.\n");
        exit(1);
    }
    struct EditorCtx ectx = ectx_default(); 
    initialize_renderer(&ectx.camera);
    ectx.is_playing = 0;
    ectx.game = game_init();
    ectx.game.camera = camera_get_default();
    ectx.game.camera.zoom = 0.2f;
    camera_compute_view(&ectx.game.camera);
    int editor = 1;
#endif

    struct Game game = initialize_game();
#ifndef EDITOR
    initialize_renderer(&game.camera);
#endif

    load_default_images(); 

    while (!glfwWindowShouldClose(w_get_window_ctx()))
    {

        GLenum err;
        while ((err = glGetError()) != GL_NO_ERROR) {
            printf("OpenGL error: 0x%X\n", err);
        }

        glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

#ifdef EDITOR
        if(editor) editor_update(&ectx);
        else game_update(&game);
#else
        game_update(&game);
#endif

        glfwSwapBuffers(w_get_window_ctx());
        i_clear_pressed();
        glfwPollEvents();
    }

    game_deinit(&game);

#ifdef EDITOR
    editor_destroy();
#endif
    glfwTerminate();
    return 0;
}
