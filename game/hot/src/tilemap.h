#ifndef TILEMAP_H
#define TILEMAP_H
#include <cglm/ivec2.h>
#include <stdio.h>

typedef int Tile;

struct TileMap
{
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

int tilemap_get_default_tile();

int tilemap_get_default_tile_solidity();

void tilemap_render_layer_fow(struct TileMap *tilemap, int layer, vec2 fow_center, vec2 pos, float size);

void tilemap_render_layer(struct TileMap *tilemap, int layer, vec2 pos, float size);

void tilemap_render_solidmap(struct TileMap *tilemap, int tm_width, int tm_height, vec2 pos, float size);

void tilemap_render_background(const struct TileMap *tilemap, vec2 pos, float size);

void tilemap_shift_right(struct TileMap *tilemap, int amount);

void tilemap_shift_left(struct TileMap *tilemap, int amount);

void tilemap_shift_up(struct TileMap *tilemap, int amount);

void tilemap_shift_down(struct TileMap *tilemap, int amount);

int is_tilemap_solid_at(struct TileMap *tilemap, ivec2 position);

#endif
