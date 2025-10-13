#define EDITOR

#include "game.h"
#include "level.h"
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
    game.camera.zoom = 0.2f;
    game.camera.pan[0] = 0;
    game.camera.pan[1] = 0;
    game.tilemap_layer_mask = -1;
    camera_compute_view(&game.camera);
    return game;
}

int main()
{
    int do_ser = 0;
    int do_deser = 0;

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
    int editor = 1;
#endif

    const char* level_path="resources/level/test.level";

    struct Game game = initialize_game();
    initialize_renderer(&game.camera);
    load_default_images(); 

    struct Level loaded_level;
    if(do_deser && deserialize_level(&loaded_level, level_path))
    {
        load_level(&game, loaded_level);
    }

    while (!glfwWindowShouldClose(w_get_window_ctx()))
    {
        
#ifdef EDITOR
        editor_new_frame();
#endif

        GLenum err;
        while ((err = glGetError()) != GL_NO_ERROR) {
            printf("OpenGL error: 0x%X\n", err);
        }

        //game_update(&game);

        glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        render_level(get_current_level(&game), &game.gamestate_current, game.tilemap_layer_mask);

#ifdef EDITOR
        if(editor) editor_update(&game);
        editor_render();
#endif

        glfwSwapBuffers(w_get_window_ctx());
        i_clear_pressed();
        glfwPollEvents();
    }

    game_deinit(&game);

    if(do_ser)
    {
        serialize_level(&game.level, level_path);
    }
#ifdef EDITOR
    editor_destroy();
#endif
    glfwTerminate();
    return 0;
}
