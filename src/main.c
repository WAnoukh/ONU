#include "game.h"
#include "level.h"
#include "level_serialization.h"
#include "texture.h"
#include "window/input.h"
#include "window/window.h"
#include "rendering/rendering.h"
#include "editor/editor.h"

#include <stdio.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

ivec2 directions[] = {{0, -1},{0, 1},{-1, 0},{1, 0}};

int process_targeted_action(struct Game *game, int entity_index, enum ActionType action_type)
{
    if(action_type != ACTION_UP && action_type != ACTION_DOWN && action_type != ACTION_LEFT && action_type != ACTION_RIGHT)
    {
        return 0;
    }
    int dir_index = (int)(action_type - ACTION_UP);
    struct Entity *ent = game->level.entities+entity_index;

    push_entity(&game->level, ent, directions[dir_index]);
    return 1;
}

void request_new_turn(struct Game *game, struct Action action)
{
    if(action.type == ACTION_UNDO)
    {
        if(!history_clear(game))
        {
            game->level = history_pop(game); 
        }
        return;
    }
    if(action.type == ACTION_DOOR_OPEN)
    {
        game->level.is_door_opened = 1;
    }

    history_register(game);

    process_targeted_action(game, action.target_entity, action.type);
}

void update_key_blocks(struct Game *game)
{
    for(int i = 0; i < game->level.entity_count; ++i)
    {
        struct Entity *ent = game->level.entities+i;
        if(ent->type != ENTITY_KEY) continue;

        struct KeyBlockData *key_data = game->level.key_block_data+ent->data_index;
        key_data->is_pressed = i_key_down(key_data->key);
        if(i_key_pressed(key_data->key))
        {
            struct Entity *slot = get_slot_at(&game->level, ent->position);
            if(slot != NULL)
            {
                struct SlotData *slot_data = game->level.slot_data+slot->data_index;
                request_new_turn(game, slot_data->action);
            }
        }
    }
}


struct Game initialize_game()
{
    struct Game game;

    get_default_level(&game.level_start);

    game.last_time = glfwGetTime();
    game.camera.zoom = 0.2f;
    game.camera.pan[0] = 0;
    game.camera.pan[1] = 0;
    camera_compute_view(&game.camera);
    return game;
}

int main()
{
    int do_ser = 0;
    int do_deser = 0;
    int editor = 1;

    GLFWwindow* window;
    if (!initGl(&window) || window == NULL)
    {
        perror("Failed to initialize OpenGL context\n");
        return 1;
    }

    if(!editor_initialize(window))
    {
        printf("Editor Error : failed to initialize the editor.\n");
        exit(1);
    }

    const char* level_path="resources/level/test.level";

    struct Game game = initialize_game();
    initialize_renderer(&game.camera);
    i_initialize(window);
    load_default_images(); 

    struct Level loaded_level;
    if(do_deser && deserialize_level(&loaded_level, level_path))
    {
        //game.level_start = loaded_level;
    }
    load_level(&game, game.level_start);

    while (!glfwWindowShouldClose(window))
    {
        editor_new_frame();
        GLenum err;
        while ((err = glGetError()) != GL_NO_ERROR) {
            printf("OpenGL error: 0x%X\n", err);
        }
        game.new_time = get_time();
        game.delta_time = (float)(game.new_time - game.last_time);
        game.last_time = game.new_time;

        if(is_framebuffer_resized())
        {
            camera_compute_view(&game.camera);
            clear_framebuffer_resized();
        }

        i_process(window);
        update_key_blocks(&game);

        if(game.level.is_door_reached)
        {
            load_level(&game, game.level_start);
        }

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        render_level(&game.level);

        if(editor) editor_update(&game, window);

        editor_render();
        glfwSwapBuffers(window);
        i_clear_pressed();
        glfwPollEvents();
    }

    if(do_ser)
    {
        serialize_level(game.level, level_path);
    }
    editor_destroy();
    glfwTerminate();
    return 0;
}
