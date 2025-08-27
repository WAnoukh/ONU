#ifndef LEVEL_H
#define LEVEL_H

#include "tilemap.h"
#include "window/input.h"

enum EntityType
{
    ENTITY_NONE,
    ENTITY_WALL,
    ENTITY_PLAYER,
    ENTITY_BOX,
    ENTITY_KEY,
    ENTITY_SLOT,
    ENTITY_DOOR,
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
    int data_index;
};

struct KeyBlockData
{
    int key;
    int is_pressed;
};

struct SlotData
{
    enum PlayerAction action;
};

struct DoorData
{
    int is_opened;
};

struct Level 
{
    TileMap tilemap;
    struct Entity entities[100];
    struct KeyBlockData key_block_data[10];
    struct SlotData slot_data[10];
    int entity_count;
    int key_block_data_count;
    int slot_data_count;
    int is_door_opened;
    int is_door_reached;
    int width;
    int height;
};

void get_default_level(struct Level *level);

void render_level(struct Level *level);

void print_level(struct Level *level);

int get_player_position_in_level(struct Level *level, ivec2 out_position);

struct Entity *get_slot_at(struct Level *level, ivec2 at);

int compute_index_from_position(struct Level *level, ivec2 position);

int is_tilemap_solid_at(struct Level *level, ivec2 position);

struct Entity *get_player(struct Level *level);
    
void push_entity(struct Level *level, struct Entity *entity, ivec2 offset);

static inline void create_movable_at(struct Level *level, int x, int y, enum EntityType type)
{
    level->entities[level->entity_count++] = (struct Entity){
        type,
        SOLIDITY_MOVABLE,
        {x, y},
        -1,
    };
}

static inline void create_key_block_at(struct Level *level, int x, int y, int key)
{
    level->entities[level->entity_count++] = (struct Entity){
        ENTITY_KEY,
        SOLIDITY_MOVABLE,
        {x, y},
        level->key_block_data_count,
    };
    level->key_block_data[level->key_block_data_count++] = (struct KeyBlockData){
        key,
        0,
    };
}

static inline void create_slot_at(struct Level *level, int x, int y, enum PlayerAction action)
{
    level->entities[level->entity_count++] = (struct Entity){
        ENTITY_SLOT,
        SOLIDITY_NONE,
        {x, y},
        level->slot_data_count,
    };
    level->slot_data[level->slot_data_count++] = (struct SlotData){
        action,
    };
}

static inline void create_door_at(struct Level *level, int x, int y)
{
    level->entities[level->entity_count++] = (struct Entity){
        ENTITY_DOOR,
        SOLIDITY_NONE,
        {x, y},
        -1,
    };
}
#endif // LEVEL_H
