#ifndef LEVEL_H
#define LEVEL_H

#include "tilemap.h"

enum EntityType
{
    ENTITY_NONE,
    ENTITY_WALL,
    ENTITY_PLAYER,
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
    enum Solidity Solidity;
    ivec2 position;
    void* data;
};

struct Level 
{
   TileMap tilemap;
   struct Entity entities[100];
   int entity_count;
   int width;
   int height;
};

struct Level get_default_level();

void render_level(struct Level level);

void print_level(struct Level level);

int get_player_position_in_level(struct Level level, ivec2 out_position);

int compute_index_from_position(struct Level level, ivec2 position);

struct Entity *get_player(struct Level *level);
    
void move_entity(struct Entity *entity, ivec2 new_position);
#endif // LEVEL_H
