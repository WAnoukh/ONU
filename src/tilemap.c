#include <cglm/vec3.h>

#include "tilemap.h"
#include "rendering/rendering.h"
#include "texture.h"
#include "transform.h"


vec3 tile_color[2] = {
    {1.f,1.f,1.f},
    {0.1f,0.5f,0.3f},
};

void tilemap_render_layer(struct TileMap *tilemap, int layer, vec2 pos, float size)
{
    Tile *tiles = tilemap_get_layer_by_index(tilemap, layer);
    tiles = tilemap->tile;
    int width = tilemap->width;
    int height = tilemap->height;
    int x = 0;
    int y = (int)height;
    const float tm_width_2 = (float)width * size/2;
    const float tm_height_2 = (float)height * size/2;
    unsigned int program = shaders_use_default();
    for (int i = 0; i < height * width; ++i)
    {
        Tile tile = tiles[i];
        mat3 transform;
        vec2 size_vec = {size, size};
        vec2 pos_offset;
        vec3 color;
        pos_offset[0] = (float)x * size - tm_width_2;
        pos_offset[1] = (float)y * size - tm_height_2;
        glm_vec2_add(pos, pos_offset, pos_offset);
        compute_transform(transform, pos_offset, size_vec);
        struct TextureAtlas atlas = get_atlas_tilemap();
        int tile_x, tile_y;
        atlas_index_to_coordinates(atlas, tile, &tile_x, &tile_y);
        program = shaders_use_atlas(atlas, tile_x, tile_y);
        color[0] = 1; color[1] = 1; color[2] = 1; 
        compute_transform(transform, pos_offset, size_vec);
        draw_transformed_quad(program, transform, color, 1);
        ++x;
        if (x>=width)
        {
            x=0;
            --y;
        }
    }
}

void render_solidmap(const enum TileSolidity *solidmap, int tm_width, int tm_height, vec2 pos, float size)
{
    int x = 0;
    int y = (int)tm_height;
    const float tm_width_2 = (float)tm_width * size/2;
    const float tm_height_2 = (float)tm_height * size/2;
    unsigned int program = shaders_use_default();
    for (int i = 0; i < tm_height * tm_width; ++i)
    {
        const enum TileSolidity solidity = solidmap[i];
        if(solidity == STILE_SOLID) 
        {
            vec3 *color =   (vec3*)tile_color[solidity];

            mat3 transform;
            vec2 size_vec = {size, size};
            vec2 pos_offset;
            pos_offset[0] = (float)x * size - tm_width_2;
            pos_offset[1] = (float)y * size - tm_height_2;
            glm_vec2_add(pos, pos_offset, pos_offset);
            compute_transform(transform, pos_offset, size_vec);
            draw_transformed_quad(program, transform, *color, 0.6f);
        }
        ++x;
        if (x>=tm_width)
        {
            x=0;
            --y;
        }
    }
}
