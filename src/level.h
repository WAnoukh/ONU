#ifndef LEVEL_H
#define LEVEL_H

#include "gamestate.h"
#include "tilemap.h"

struct Game;

struct Level 
{
    struct TileMap tilemap;
    struct GameState gamestate;
    int view_width;
    int view_height;
};

static inline int level_get_width(const struct Level *level)
{
    return level->tilemap.width;
}

static inline int level_get_height(const struct Level *level)
{
    return level->tilemap.height;
}

static inline void level_set_width(struct Level *level, int width)
{
    level->tilemap.width = width;
}

static inline void level_set_height(struct Level *level, int height)
{
    level->tilemap.height = height;
}

void level_deinit(struct Level *level);

void get_default_level(struct Level *level);

void resize_level(struct Level *level, int new_width, int new_height);

void level_shift(struct Level *level, ivec2 offset);

void render_level(struct Game *game);

void render_level_simple(struct Level *level, struct GameState *gamestate);

void render_level_views(struct Level *level);

void level_get_view_coord_from_coord(struct Level *level, int x, int y, int *out_view_x, int *out_view_y);

#endif // LEVEL_H
