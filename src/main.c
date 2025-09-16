#define EDITOR

#include "game.h"
#include "level.h"
#include "level_serialization.h"
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

ivec2 directions[] = {{0, -1},{0, 1},{-1, 0},{1, 0}};

int process_targeted_action(struct Game *game, int entity_index, enum ActionType action_type)
{
    if(action_type != ACTION_UP && action_type != ACTION_DOWN && action_type != ACTION_LEFT && action_type != ACTION_RIGHT)
    {
        return 0;
    }
    int dir_index = (int)(action_type - ACTION_UP);
    struct Entity *ent = game->level.entities+entity_index;

    return push_entity(&game->level, ent, directions[dir_index]);
}

int request_new_turn(struct Game *game, struct Action action)
{
    if(action.type == ACTION_UNDO)
    {
        history_drop_last(game);
        if(!history_clear(game))
        {
            game->level = history_pop(game); 
        }
        return 1;
    }
    if(action.type == ACTION_DOOR_OPEN)
    {
        game->level.is_door_opened = 1;
        return 1;
    }
    if(action.type == ACTION_DOOR_CLOSE)
    {
        game->level.is_door_opened = 0;
        return 1;
    }

    return process_targeted_action(game, action.target_entity, action.type);
}

void update_key_blocks(struct Game *game)
{
    int has_revelant_action_happended = 0;
    int first_action = 0;
    for(int i = 0; i < game->level.entity_count; ++i)
    {
        struct Entity *ent = game->level.entities+i;
        if(ent->type != ENTITY_KEY) continue;

        struct KeyBlockData *key_data = game->level.key_block_data+ent->data_index;
        int key_pressed;
        if(key_data->key == GLFW_KEY_PERIOD)
        {
            key_data->is_pressed = i_any_letter_down();
            key_pressed = i_any_letter_pressed();
        }
        else
        {
            key_data->is_pressed = i_key_down(key_data->key);
            key_pressed = i_key_pressed(key_data->key);
        }
        if(key_pressed)
        {
            if(!first_action)
            {
                history_register(game);
                first_action = 1;
            }
            struct Entity *slot = get_slot_at(&game->level, ent->position);
            if(slot != NULL)
            {
                struct SlotData *slot_data = game->level.slot_data+slot->data_index;
                has_revelant_action_happended |= request_new_turn(game, slot_data->action);
                if(slot_data->action.type == ACTION_UNDO)
                {
                    break;
                }
            }
        }
    }
    if(first_action && !has_revelant_action_happended)
    {
        history_drop_last(game);
    }
    if(first_action)
    {
        printf(" revelant action \n");
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
    game.tilemap_layer_mask = -1;
    camera_compute_view(&game.camera);
    return game;
}

int main()
{
    int do_ser = 0;
    int do_deser = 0;

    GLFWwindow* window;
    if (!initGl(&window) || window == NULL)
    {
        perror("Failed to initialize OpenGL context\n");
        return 1;
    }

#ifdef EDITOR
    if(!editor_initialize(window))
    {
        printf("Editor Error : failed to initialize the editor.\n");
        exit(1);
    }
    int editor = 1;
#endif

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
#ifdef EDITOR
        editor_new_frame();
#endif
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

        glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        render_level(&game.level, game.tilemap_layer_mask);

#ifdef EDITOR
        if(editor) editor_update(&game, window);
        editor_render();
#endif

        glfwSwapBuffers(window);
        i_clear_pressed();
        glfwPollEvents();
    }

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
