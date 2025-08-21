#ifndef TILEMAP_H
#define TILEMAP_H
#include <cglm/ivec2.h>

enum Solidity
{
   TILE_NONE,
   TILE_SOLID,
};

struct Tile
{
   enum Solidity solidity;
};

typedef struct Tile* TileMap;

void render_tilemap(TileMap tilemap, int tm_width, int tm_height, vec2 pos, float size);
#endif
