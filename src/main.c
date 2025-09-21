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

struct Transaction compute_transaction_from_move(struct GameState *gamestate, struct MoveRequest move)
{
    struct Transaction tr;
    tr.moves[0] = move;
    tr.moves_count = 1;
    int cur_move_index = 0; 
    ivec2 dir;
    glm_ivec2_sub(move.to, move.from, dir);
    while(cur_move_index < tr.moves_count)
    {
        struct MoveRequest cur_move = tr.moves[cur_move_index];
        struct Entity *moving_ent = gamestate->entities+cur_move.entity;
        for(int i = 0; i < gamestate->entity_count; ++i)
        {
            struct Entity *other = gamestate->entities+i; 
            if(!glm_ivec2_eqv(other->position, cur_move.to)) continue;
            if(other->solidity == SOLIDITY_STATIC)
            {
                tr.moves_count = 0;            
                return tr;
            }
            if(other->solidity == SOLIDITY_MOVABLE)
            {
                struct MoveRequest other_move;
                other_move.entity = i;
                glm_ivec2_copy(other->position, other_move.from);
                glm_ivec2_add(other_move.from, dir, other_move.to);
                tr.moves[tr.moves_count++] = other_move;
            }
        }
        ++cur_move_index;
    }
    return tr;
}

void reduce_colliding_transactions(struct TransactionList *tr_ls, struct GameState *gamestate,  struct TileMap *tilemap)
{
    for(int i = tr_ls->count-1; i >= 0 ; --i)
    {
        struct Transaction *tr = tr_ls->transactions+i;
        int valid = 1;
        for(int j = 0; j < tr->moves_count; ++j)
        {
            struct MoveRequest move = tr->moves[j];
            struct Entity *ent = gamestate->entities+move.entity;
            if(ent->type == ENTITY_PLAYER && is_door_at(gamestate, move.to) && gamestate->is_door_opened)
            {
                continue;
            }
            if(is_tilemap_solid_at(tilemap, move.to))
            {
                valid = 0;
                break;
            }
        }
        if(valid) continue;
        tr_ls->transactions[i] = tr_ls->transactions[tr_ls->count--];
    }
}

struct Transaction compute_transaction(struct GameState *gamestate, enum ActionType type, int target_index)
{
    struct Entity *target = gamestate->entities+target_index;
    struct Transaction transaction;
    int dir_index = (int)(type - ACTION_UP);
    if(target->solidity == SOLIDITY_MOVABLE)
    {
        struct MoveRequest move;
        move.entity = target_index;
        glm_ivec2_copy(target->position, move.from);
        glm_ivec2_add(move.from, directions[dir_index], move.to);
        transaction = compute_transaction_from_move(gamestate, move);
    }
    else
    {
        transaction.moves_count = 0;
    }
    return transaction;
}

void update_key_blocks(struct Game *game)
{
    struct GameState *gamestate = get_current_gamestate(game);
    struct TileMap *tilemap = get_current_tilemap(game);
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

    int door_conflict = 0;
    int door_action = 0;
    int undo = 0;
    struct TransactionList transactionlist = transactionlist_init(); 
    
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
                            struct Transaction tr = compute_transaction(gamestate, slot_data->action.type, slot_data->action.target_entity);
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
    reduce_colliding_transactions(&transactionlist, gamestate, tilemap);
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
