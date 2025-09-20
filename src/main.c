#define EDITOR

#include "game.h"
#include "gamestate.h"
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

ivec2 directions[] = {{0, -1},{0, 1},{-1, 0},{1, 0}};

int process_targeted_action(struct Game *game, int entity_index, enum ActionType action_type)
{
    if(action_type != ACTION_UP && action_type != ACTION_DOWN && action_type != ACTION_LEFT && action_type != ACTION_RIGHT)
    {
        return 0;
    }
    int dir_index = (int)(action_type - ACTION_UP);
    struct GameState *gamestate = get_current_gamestate(game);
    struct TileMap *tilemap = get_current_tilemap(game);
    struct Entity *ent = gamestate->entities+entity_index;

    if(ent->type == ENTITY_REPEATER)
    {
        int result = 0;
        for(int i = 0; i < gamestate->entity_count; ++i)
        {
            struct Entity *other = gamestate->entities+i;
            if(other->position[0] == ent->position[0] || other->position[1] == ent->position[1])
            {
                result |= push_entity(gamestate, tilemap, other, directions[dir_index]);
            }
        }
        return result;
    }

    return push_entity(gamestate, tilemap, ent, directions[dir_index]);
}

int request_new_turn(struct Game *game, struct Action action)
{
    struct GameState *gamestate = get_current_gamestate(game);
    if(action.type == ACTION_UNDO)
    {
        history_drop_last(game);
        if(!history_is_empty(game))
        {
            load_gamestate(game, history_pop(game));
        }
        return 1;
    }
    if(action.type == ACTION_DOOR_OPEN)
    {
        if(!gamestate->is_door_opened)
        {
            gamestate->is_door_opened = 1;
            return 1;
        }
        else
        {
            return 0;
        }
    }
    if(action.type == ACTION_DOOR_CLOSE)
    {
        if(gamestate->is_door_opened)
        {
            gamestate->is_door_opened = 0;
            return 1;
        }
        else
        {
            return 0;
        }
    }

    return process_targeted_action(game, action.target_entity, action.type);
}

void update_key_blocks(struct Game *game)
{
    struct GameState *gamestate = get_current_gamestate(game);
    int has_revelant_action_happended = 0;
    int first_action = 0;
    int any_non_universal_key_pressed = 0;
    int any_non_universal_key_down = 0;
    for(int i =0; i < gamestate->key_block_data_count; ++i)
    {
        struct KeyBlockData *key_data = gamestate->key_block_data+i;

        if(i_key_pressed(key_data->key))
        {
            any_non_universal_key_down = 1;
            any_non_universal_key_pressed =1;
            break;
        }
        any_non_universal_key_down |= i_key_down(key_data->key);
    }

    for(int i = 0; i < gamestate->entity_count; ++i)
    {
        struct Entity *ent = gamestate->entities+i;
        if(ent->type != ENTITY_KEY) continue;

        struct KeyBlockData *key_data = gamestate->key_block_data+ent->data_index;
        int key_pressed;
        if(key_data->key == GLFW_KEY_PERIOD)
        {
            key_data->is_pressed = any_non_universal_key_down; 
            key_pressed = any_non_universal_key_pressed; 
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
            struct Entity *slot = get_slot_at(gamestate, ent->position);
            if(slot != NULL)
            {
                struct SlotData *slot_data = gamestate->slot_data+slot->data_index;
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
        load_level(&game, loaded_level);
    }

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

        struct GameState *gamestate = get_current_gamestate(&game);
        if(gamestate->is_door_reached)
        {
            if(game.gamemode == GM_SEQUENCE)
            {
                game.sequence_index++;
                if(game.sequence_index >= game.sequence.levels_count)
                {
                    game.sequence_index = 0;
                }
            }
            load_level(&game, *get_current_level(&game));
        }

        glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        render_level(get_current_level(&game), &game.gamestate_current, game.tilemap_layer_mask);

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
