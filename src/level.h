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
#endif // LEVEL_H
