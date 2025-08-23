#ifndef LEVEL_H
#define LEVEL_H

#include "entity_tile.h"
#include "tilemap.h"

struct Level 
{
   TileMap tilemap;
   EntityMap entities;
   int width;
   int height;
};

struct Level get_default_level();

void render_level(struct Level level);

void print_level(struct Level level);

int get_player_position_in_level(struct Level level, ivec2 out_position);

void move_entity(struct Level level, ivec2 from, ivec2 to);
#endif // LEVEL_H
