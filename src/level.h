#ifndef LEVEL_H
#define LEVEL_H

#include "gamestate.h"
#include "tilemap.h"


struct Level 
{
    struct TileMap tilemap;
    struct GameState gamestate;
    /*struct Entity entities[100];
    struct KeyBlockData key_block_data[50];
    struct SlotData slot_data[50];
    int entity_count;
    int key_block_data_count;
    int slot_data_count;
    int is_door_opened;
    int is_door_reached;*/
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

void get_default_level(struct Level *level);

void resize_level(struct Level *level, int new_width, int new_height);

void level_shift(struct Level *level, ivec2 offset);

void render_level(struct Level *level, int layer_mask);
#endif // LEVEL_H
