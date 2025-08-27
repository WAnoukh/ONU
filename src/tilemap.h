#ifndef TILEMAP_H
#define TILEMAP_H
#include <cglm/ivec2.h>

enum TileType
{
    TILE_EMPTY,
    TILE_WALL,
    TILE_COUNT,
};

struct Tile
{
   enum TileType type;
};

typedef struct Tile* TileMap;

void render_tilemap(TileMap tilemap, int tm_width, int tm_height, vec2 pos, float size);
#endif
