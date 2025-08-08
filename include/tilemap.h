#ifndef TILEMAP_H
#define TILEMAP_H
#include <cglm/ivec2.h>

enum Solidity
{
   NONE,
   SOLID,
};

struct Tile
{
   enum Solidity solidity;
};

typedef struct
{
   struct Tile* grid;
   unsigned int height;
   unsigned int width;
} TileMap;

void render_tilemap(TileMap tilemap, vec2 pos, float size);

void render_main_tilemap();
#endif
