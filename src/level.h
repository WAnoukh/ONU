#ifndef LEVEL_H
#define LEVEL_H

#include "tilemap.h"

enum EntityType
{
    ENTITY_NONE,
    ENTITY_WALL,
    ENTITY_PLAYER,
    ENTITY_BOX,
    ENTITY_KEY,
    ENTITY_SLOT,
};

enum Solidity 
{
    SOLIDITY_NONE,
    SOLIDITY_STATIC,
    SOLIDITY_MOVABLE,
};

struct Entity
{
    enum EntityType type;
    enum Solidity solidity;
    ivec2 position;
    void* data;
};

struct KeyBlockData
{
    int key;
    int is_pressed;
};

struct Level 
{
    TileMap tilemap;
    struct Entity entities[100];
    struct KeyBlockData key_block_data[10];
    int entity_count;
    int key_block_data_count;
    int width;
    int height;
};

void get_default_level(struct Level *level);

void render_level(struct Level *level);

void print_level(struct Level *level);

int get_player_position_in_level(struct Level *level, ivec2 out_position);

int compute_index_from_position(struct Level *level, ivec2 position);

int is_tilemap_solid_at(struct Level *level, ivec2 position);

struct Entity *get_player(struct Level *level);
    
void push_entity(struct Level *level, struct Entity *entity, ivec2 offset);

static inline struct Entity create_movable_at(int x, int y, enum EntityType type)
{
    return (struct Entity){
        type,
        SOLIDITY_MOVABLE,
        {x, y},
        NULL
    };
}

static inline struct Entity create_key_block_at(int x, int y, int key, struct KeyBlockData *out_data)
{
    *out_data = (struct KeyBlockData){
        key,
        0,
    };
    return (struct Entity){
        ENTITY_KEY,
        SOLIDITY_MOVABLE,
        {x, y},
        (void*)out_data,
    };
}
#endif // LEVEL_H
