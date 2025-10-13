#include "game.h"
#include "GLFW/glfw3.h"
#include "cglm/mat3.h"
#include "window/input.h"
#include "window/window.h"

ivec2 directions[] = {{0, -1},{0, 1},{-1, 0},{1, 0}};

struct Game game_init()
{
    struct Game game;
    game.history = history_init();
    return game;
}

void game_deinit(struct Game *game)
{
    history_deinit(&game->history);
}

void game_history_register(struct Game *game)
{
    history_append(&game->history, *get_current_gamestate(game));
}

int game_history_is_empty(struct Game *game)
{
    return history_is_empty(&game->history);
}

struct GameState game_history_pop(struct Game *game)
{
    return history_pop(&game->history);
}

void game_history_drop_last(struct Game *game)
{
    history_drop_last(&game->history);
}

void game_history_clear(struct Game *game)
{
    history_clear(&game->history);
}

void load_level(struct Game *game, struct Level level)
{
    game->level = level;
    game->gamestate_current = level.gamestate;
    game_history_clear(game);
}

void game_setup_default_level(struct Game *game)
{
    game->gamemode = GM_LEVEL;
    get_default_level(&game->level);
    load_gamestate(game, game->level.gamestate);
    game_history_clear(game);
}

void game_set_sequence(struct Game *game, struct Sequence sequence)
{
    game->gamemode = GM_SEQUENCE;
    game->sequence_index = 0;
    game->sequence = sequence;
}

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
        game_history_drop_last(game);
        if(!game_history_is_empty(game))
        {
            load_gamestate(game, game_history_pop(game));
        }
        return 1;
    }
    if(action.type == ACTION_DOOR_OPEN)
    {
        if(action.target_entity < 0) return 0;
        struct Entity *targeted_door = gamestate->entities+action.target_entity;
        struct DoorData *data = gamestate->door_data+targeted_door->data_index;
        if(!data->is_opened)
        {
            data->is_opened = 1;
            return 1;
        }
        else
        {
            return 0;
        }
    }
    if(action.type == ACTION_DOOR_CLOSE)
    {
        if(action.target_entity < 0) return 0;
        struct Entity *targeted_door = gamestate->entities+action.target_entity;
        struct DoorData *data = gamestate->door_data+targeted_door->data_index;
        if(data->is_opened)
        {
            data->is_opened = 0;
            return 1;
        }
        else
        {
            return 0;
        }
    }

    return process_targeted_action(game, action.target_entity, action.type);
}

void process_buttons(struct GameState *gamestate)
{
    int are_button_in_scene = 0;
    int are_all_button_correct = 1;
    for(int i = 0; i < gamestate->entity_count; ++i)
    {
        struct Entity *ent = gamestate->entities+i;
        if(ent->type != ENTITY_BUTTON && ent->type != ENTITY_ANTIBUTTON) continue;
        are_button_in_scene = 1;
        
        // Searching boxes at the same location
        int box_found = 0;
        for(int j = 0; j < gamestate->entity_count; ++j)
        {
            struct Entity *other_ent = gamestate->entities+j;
            if(other_ent->type != ENTITY_BOX) continue;

            if(glm_ivec2_eqv(ent->position, other_ent->position))
            {
                box_found = 1;
                break;
            }
        }
        if((ent->type == ENTITY_BUTTON && !box_found) || (ent->type == ENTITY_ANTIBUTTON && box_found))
        {
            are_all_button_correct = 0;
            break;
        }
    }

    if(are_button_in_scene)
    {
        for(int i = 0; i < gamestate->door_data_count; ++i)
        {
            gamestate->door_data[i].is_opened = are_all_button_correct;
        }
    }
}

void check_if_player_reached_end(struct GameState *gamestate)
{
    int found = 0;
    int player_reached = 0;
    ivec2 other_pos;
    for(int i = 0; i < gamestate->entity_count; ++i)
    {
        struct Entity *ent = gamestate->entities+i;
        if(ent->type == ENTITY_PLAYER)
        {
            if(!found)
            {
                glm_ivec2_copy(ent->position, other_pos);
                found = 1;
            }
            else if(found < 0 && glm_ivec2_eqv(other_pos, ent->position))
            {
               player_reached = 1;
               break;
            }
        }
        else if(ent->type == ENTITY_END) 
        {
            if(!found)
            {
                glm_ivec2_copy(ent->position, other_pos);
                found = -1;
            }
            else if(found > 0 && glm_ivec2_eqv(other_pos, ent->position))
            {
                player_reached = 1;
                break;
            }
        }
    }
    gamestate->is_end_reached = player_reached;
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
                game_history_register(game);
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
        game_history_drop_last(game);
    }
    if(first_action && has_revelant_action_happended)
    {
        process_buttons(gamestate);
        check_if_player_reached_end(gamestate);
    }
}

void game_update(struct Game *game)
{
    game->new_time = get_time();
    game->delta_time = (float)(game->new_time - game->last_time);
    game->last_time = game->new_time;
    
    if(is_framebuffer_resized())
    {
        camera_compute_view(&game->camera);
        clear_framebuffer_resized();
    }

    i_process(w_get_window_ctx());
    update_key_blocks(game);

    //Checkin if player reached the end door
    struct GameState *gamestate = get_current_gamestate(game);
    if(gamestate->is_end_reached)
    {
        if(game->gamemode == GM_SEQUENCE)
        {
            game->sequence_index++;
            if(game->sequence_index >= game->sequence.levels_count)
            {
                game->sequence_index = 0;
            }
        }
        load_level(game, *get_current_level(game));
    }

    render_level(get_current_level(game), &game->gamestate_current, game->tilemap_layer_mask);
}
