#ifndef TILEMAP_H
#define TILEMAP_H
#include <cglm/ivec2.h>
#include <stdio.h>

enum TileSolidity
{
    STILE_EMPTY,
    STILE_SOLID,
    STILE_COUNT,
};

typedef int Tile;

struct TileMap
{
    enum TileSolidity *solidity;
    Tile *tile;
    int width;
    int height;
    int layer_count;
};

static inline Tile *tilemap_get_layer_by_index(struct TileMap *tilemap, int index)
{
    if(index < 0 || index >= tilemap->layer_count)
    {
        printf("Tilemap Error : layer index out of range\n");
        return NULL;
    }
    return tilemap->tile+(index * tilemap->width * tilemap->height);
}

void tilemap_render_layer(struct TileMap *tilemap, int layer, vec2 pos, float size);

void render_solidmap(const enum TileSolidity *solidmap, int tm_width, int tm_height, vec2 pos, float size);
#endif
