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

struct Level 
{
   TileMap tilemap;
   struct Entity entities[100];
   int entity_count;
   int width;
   int height;
};

struct Level get_default_level();

void render_level(struct Level *level);

void print_level(struct Level *level);

int get_player_position_in_level(struct Level *level, ivec2 out_position);

int compute_index_from_position(struct Level *level, ivec2 position);

int is_tilemap_solid_at(struct Level *level, ivec2 position);

struct Entity *get_player(struct Level *level);
    
void push_entity(struct Level *level, struct Entity *entity, ivec2 offset);

static inline struct Entity create_box_at(int x, int y)
{
    return (struct Entity){
        ENTITY_BOX,
        SOLIDITY_MOVABLE,
        {x, y},
        NULL
    };
}
#endif // LEVEL_H
