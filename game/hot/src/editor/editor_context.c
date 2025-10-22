#include "editor_context.h"
#include "GLFW/glfw3.h"
#include <string.h>

#define DEFAULT_LEVEL_SIZE 10
#define DEFAULT_LEVEL_GRID_SIZE DEFAULT_LEVEL_SIZE * DEFAULT_LEVEL_SIZE

void get_default_level(struct Level *level)
{
    int grid_size = DEFAULT_LEVEL_GRID_SIZE;

    level->tilemap.tile = malloc(sizeof(Tile) * grid_size);
    level->tilemap.layer_count = 1;
    for(int i =0; i < grid_size; ++i) level->tilemap.tile[i] = 4+20;
    level_set_width(level, DEFAULT_LEVEL_SIZE);
    level_set_height(level, DEFAULT_LEVEL_SIZE);

    level->view_width = 0;
    level->view_height = 0;

    struct GameState *gamestate = &level->gamestate;
    gamestate->entity_count = 0;
    gamestate->key_block_data_count = 0;
    gamestate->slot_data_count = 0;
    gamestate->door_data_count = 0;
    create_slot_at(gamestate, 2, 5, ACTION_DOOR_OPEN, -1);
    create_slot_at(gamestate, 7, 1, ACTION_UP, 6);
    create_slot_at(gamestate, 7, 2, ACTION_DOWN, 6);
    create_slot_at(gamestate, 6, 2, ACTION_LEFT, 6);
    create_slot_at(gamestate, 8, 2, ACTION_RIGHT, 6);
    create_slot_at(gamestate, 4, 1, ACTION_UNDO, 6);
    gamestate->entities[gamestate->entity_count++] = (struct Entity){
        ENTITY_PLAYER,
        SOLIDITY_MOVABLE,
        {8,8},
        -1,
    };
    create_key_block_at(gamestate, 2, 7, GLFW_KEY_F, 0);
    create_key_block_at(gamestate, 7, 1, GLFW_KEY_W, 0);
    create_key_block_at(gamestate, 7, 2, GLFW_KEY_S, 0);
    create_key_block_at(gamestate, 6, 2, GLFW_KEY_A, 0);
    create_key_block_at(gamestate, 8, 2, GLFW_KEY_D, 0);
    create_key_block_at(gamestate, 4, 1, GLFW_KEY_R, 0);
    create_movable_at(gamestate, 7, 7, ENTITY_BOX);
    create_door_at(gamestate, 2, 0);
    gamestate->is_end_reached = 0;
}

struct EditorCtx ectx_default()
{
    struct EditorCtx ctx;

    get_default_level(&ctx.level);

    ctx.camera.zoom = 0.2f;
    ctx.camera.pos[0] = 0;
    ctx.camera.pos[1] = 0;

    ctx.floating_editor_show = 0;

    ctx.reposition_entity = NULL;
    ctx.reposition_selection = 0;

    ctx.creation_type = ENTITY_NONE;
    ctx.creation_key = (struct KeyBlockData){GLFW_KEY_A, 0, 0};
    ctx.creation_action = ACTION_NONE;
    ctx.creation_action_target = 0;

    ctx.deletion_index = -1;
    ctx.deletion_selection = 0;

    ctx.layer_selected = 0;

    ctx.selection_started = 0;

    ctx.selection_ents_count = 0; 

    ctx.edition_entity = NULL;

    strcpy(ctx.file_current, "NewFile");
    ctx.saving = 0;
    ctx.opening = 0;
    ctx.opening_failed = 0;

    strcpy(ctx.sequence_current, "demo");
    ctx.sequence_opening = 0;
    ctx.sequence_opening_failed = 0;

    ctx.ui_scale = 2.f;

    ctx.level_menu_opened = 0;
    ctx.level_temp_size_changed = 0;
    ctx.level_temp_shift_changed = 0;
    return ctx;
}

void ectx_start_level(struct EditorCtx *ectx, struct Level level)
{
    ectx->game.gamemode = GM_LEVEL;
    load_level(&ectx->game, level);
    load_gamestate(&ectx->game, level.gamestate);
    game_start(&ectx->game);
    ectx->is_playing = 1;
}
