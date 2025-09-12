#ifndef GAME_H
#define GAME_H

#include "level.h"
#include "rendering/rendering.h"

#define HISTORY_MAX_SIZE 100

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
    struct Level level_start;
    struct Level history[HISTORY_MAX_SIZE];
    int history_size;
    double last_time;
    double new_time;
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

int history_register(struct Game *game);

int history_clear(struct Game *game);

struct Level history_pop(struct Game *game);

void history_drop_last(struct Game *game);

void load_level(struct Game *game, struct Level level);

void camera_pan(struct Camera *camera, float x_offset, float y_offset);

void camera_zoom(struct Camera *camera, float amount);

void camera_compute_view(struct Camera *camera);

void camera_screen_to_world(struct Camera *camera, const vec2 screen_pos, vec2 out_world);

#endif // GAME_H
