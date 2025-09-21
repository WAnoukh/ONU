#define EDITOR

#include "game.h"
#include "gamestate.h"
#include "level.h"
#include "serialization.h"
#include "texture.h"
#include "transaction.h"
#include "window/input.h"
#include "window/window.h"
#include "rendering/rendering.h"
#ifdef EDITOR
#include "editor/editor.h"
#endif

#include <stdio.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

void update_key_blocks(struct Game *game)
{
    struct GameState *gamestate = get_current_gamestate(game);
    struct TileMap *tilemap = get_current_tilemap(game);
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

    int door_conflict = 0;
    int door_action = 0;
    int undo = 0;
    struct TxBatch transactionlist = transactionlist_init(); 
    
    for(int i = 0; i <= gamestate->entity_count; ++i)
    {
        struct Entity *key = gamestate->entities+i;
        if(key->type != ENTITY_KEY) continue;
        struct KeyBlockData *key_data = gamestate->key_block_data+key->data_index;
        key_data->is_pressed = i_key_down(key_data->key)||(key_data->key == '.' && any_non_universal_key_down);
        if(i_key_pressed(key_data->key) || (key_data->key == '.' && any_non_universal_key_pressed))
        {
            for(int j = 0; j <= gamestate->entity_count; ++j)
            {
                struct Entity *slot = gamestate->entities+j;
                if(!glm_ivec2_eqv(slot->position, key->position)) continue;
                if(slot->type != ENTITY_SLOT) continue;
                struct SlotData *slot_data = gamestate->slot_data+slot->data_index;
                switch(slot_data->action.type)
                {
                    case ACTION_DOOR_OPEN:
                        if(door_conflict) continue;
                        if(door_action && door_action != 1) door_conflict = 1;
                        door_action = 1;
                        continue;
                        break;
                    case ACTION_DOOR_CLOSE:
                        if(door_conflict) continue;
                        if(door_action && door_action != -1) door_conflict = -1;
                        door_action = -1;
                        continue;
                        break;
                    case ACTION_UNDO:
                        undo = 1;
                        break;
                    case ACTION_UP:
                    case ACTION_DOWN:
                    case ACTION_LEFT:
                    case ACTION_RIGHT:
                        {
                            struct Transaction tr = create_transaction_from_action(gamestate, slot_data->action.type, slot_data->action.target_entity);
                            transactionlist_append(&transactionlist, tr);
                        }
                        break;
                    default:
                        break;
                }
                if(undo) break;
            }
            if(undo) break;
        }
    }

    if(undo)
    {
        transactionlist_deinit(&transactionlist);
        if(!history_is_empty(game))
        {
            game->gamestate_current = history_pop(game);
        }
        return;
    }
    int history_registered = 0;
    int door_need_modification = door_action && !door_conflict && 
        ((door_action < 0 && gamestate->is_door_opened) || (door_action > 0 && !gamestate->is_door_opened));
    if(door_need_modification)
    {
        history_register(game);
        gamestate->is_door_opened = door_action > 0;
        history_registered = 1;
    }

    batch_remove_tilmap_collision(&transactionlist, gamestate, tilemap);

    if(transactionlist.count && !history_registered)    
    {
        history_register(game);
        history_registered = 1;
    }
    for(int i = 0; i < transactionlist.count; ++i)
    {
        struct Transaction tr = transactionlist.transactions[i];
        for(int j = 0; j < tr.moves_count; ++j)
        {
            struct MoveRequest move = tr.moves[j];
            struct Entity *target = gamestate->entities+move.entity;
            glm_ivec2_copy(move.to, target->position);
        }
    }
    if(transactionlist.count)
    {
        printf("%d transactions applied.\n", transactionlist.count);
    }
    transactionlist_deinit(&transactionlist);
    gamestate->is_door_reached = is_player_on_door(gamestate);
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
