#ifndef GAME_H
#define GAME_H

#include "level.h"
#include "level_sequence.h"
#include "rendering/rendering.h"

#define HISTORY_MAX_SIZE 100

enum GameMode
{
    GM_LEVEL,
    GM_SEQUENCE,
};

struct Camera
{
    mat3 view;
    mat3 view_inverse;
    vec2 pan;
    float zoom;
};

struct Game
{
    struct Camera camera;
    struct Level level;
    struct Sequence sequence;
    struct GameState gamestate_current;
    struct GameState history[HISTORY_MAX_SIZE];
    enum GameMode gamemode;
    double last_time;
    double new_time;
    int sequence_index;
    int history_size;
    float delta_time;
    int tilemap_layer_mask;
};

static inline void layer_set_visibility(struct Game *game, int layer, int visibility)
{
    if(visibility)
    {
        game->tilemap_layer_mask |= 0b1 << layer;
    }
    else
    {
        game->tilemap_layer_mask &= ~(0b1 << layer);
    }
}

static inline int layer_get_visibility(struct Game *game, int layer)
{
    return game->tilemap_layer_mask & 0b1 << layer;
}

static inline struct GameState *get_current_gamestate(struct Game *game)
{
    return &game->gamestate_current;
}

static inline void load_gamestate(struct Game *game, struct GameState gamestate)
{
    game->gamestate_current = gamestate;
}

static inline struct Level *get_current_level(struct Game *game)
{
    switch(game->gamemode)
    {
        case GM_LEVEL:
            return &game->level;
        case GM_SEQUENCE:
            return game->sequence.levels+game->sequence_index;
        default:
            return NULL;
    }
}

static inline struct TileMap *get_current_tilemap(struct Game *game)
{
    return &get_current_level(game)->tilemap;
}

int history_register(struct Game *game);

int history_is_empty(struct Game *game);

struct GameState history_pop(struct Game *game);

void history_drop_last(struct Game *game);

void history_clear(struct Game *game);

void load_level(struct Game *game, struct Level level);

void camera_pan(struct Camera *camera, float x_offset, float y_offset);

void camera_zoom(struct Camera *camera, float amount);

void camera_compute_view(struct Camera *camera);

void camera_screen_to_world(struct Camera *camera, const vec2 screen_pos, vec2 out_world);

void game_setup_default_level(struct Game *game);

int game_load_default_sequence(struct Game *game);
#endif // GAME_H
