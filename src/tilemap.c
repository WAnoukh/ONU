#include <cglm/vec3.h>

#include "entity.h"
#include "Tilemap.h"
#include "Rendering/rendering.h"

#define TILEMAP_SIZE 10

struct Tile default_grid[TILEMAP_SIZE*TILEMAP_SIZE] = {
    SOLID, SOLID, SOLID, SOLID, SOLID, SOLID, SOLID, SOLID, SOLID, SOLID,
    SOLID, NONE, NONE, SOLID, NONE, NONE, NONE, NONE, SOLID, SOLID,
    SOLID, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, SOLID,
    SOLID, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, SOLID,
    SOLID, NONE, NONE, SOLID, NONE, NONE, NONE, NONE, NONE, SOLID,
    SOLID, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, SOLID,
    SOLID, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, SOLID,
    SOLID, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, SOLID,
    SOLID, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, SOLID,
    SOLID, SOLID, SOLID, SOLID, SOLID, SOLID, SOLID, SOLID, SOLID, SOLID,
};

vec3 tile_color[2] = {
    {1.f,1.f,1.f},
    {0.1f,0.5f,0.3f}
};

TileMap get_default_tilemap()
{
    TileMap tilemap;
    tilemap.grid = default_grid;
    tilemap.height = TILEMAP_SIZE;
    tilemap.width = TILEMAP_SIZE;
    return tilemap;
}

void render_tilemap(const TileMap tilemap, vec2 pos, const float size)
{
    int x = 0;
    int y = (int)tilemap.height;
    const float tilemap_width_2 = (float)tilemap.width * size/2;
    const float tilemap_height_2 = (float)tilemap.height * size/2;
    for (int i = 0; i < tilemap.height * tilemap.width; ++i)
    {
        const enum Solidity solidity= tilemap.grid[i].solidity;
        vec3 *color =   (vec3*)tile_color[solidity];

        mat3 transform;
        vec2 size_vec = {size, size};
        vec2 pos_offset;
        pos_offset[0] = (float)x * size - tilemap_width_2;
        pos_offset[1] = (float)y * size - tilemap_height_2;
        glm_vec2_add(pos, pos_offset, pos_offset);
        compute_transform(transform, pos_offset, size_vec);
        draw_transformed_quad(transform, *color);
        ++x;
        if (x>=tilemap.width)
        {
            x=0;
            --y;
        }
    }
}

void render_main_tilemap()
{
    vec2 pos = {1.f,1.f};
    render_tilemap(get_default_tilemap(), pos, 1.f);
}
