#include <cglm/vec3.h>

#include "tilemap.h"
#include "rendering/rendering.h"
#include "texture.h"
#include "transform.h"

const enum TileSolidity DEFAULT_STILE = STILE_SOLID;
const int DEFAULT_TILE = 0;

vec3 tile_color[2] = {
    {1.f,1.f,1.f},
    {0.1f,0.5f,0.3f},
};

int tilemap_get_default_tile()
{
    return DEFAULT_TILE;
}

int tilemap_get_default_tile_solidity()
{
    return DEFAULT_STILE;
}

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
        if(tile)
        {
            mat3 transform;
            vec2 size_vec = {size, size};
            vec2 pos_offset;
            vec3 color;
            pos_offset[0] = ((float)x+0.5f) * size - tm_width_2;
            pos_offset[1] = ((float)y-0.5f) * size - tm_height_2;
            glm_vec2_add(pos, pos_offset, pos_offset);
            compute_transform(transform, pos_offset, size_vec);
            struct TextureAtlas atlas = get_atlas_tilemap();
            int tile_x, tile_y;
            atlas_index_to_coordinates(atlas, tile-1, &tile_x, &tile_y);
            program = shaders_use_atlas(atlas, tile_x, tile_y);
            color[0] = 1; color[1] = 1; color[2] = 1; 
            compute_transform(transform, pos_offset, size_vec);
            draw_transformed_quad(program, transform, color, 1);
        }
        ++x;
        if (x>=width)
        {
            x=0;
            --y;
        }
    }
}

void tilemap_render_solidmap(const enum TileSolidity *solidmap, int tm_width, int tm_height, vec2 pos, float size)
{
    int x = 0;
    int y = (int)tm_height;
    const float tm_width_2 = (float)tm_width * size/2.f;
    const float tm_height_2 = (float)tm_height * size/2.f;
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
            pos_offset[0] = ((float)x+0.5f) * size - tm_width_2;
            pos_offset[1] = ((float)y-0.5f) * size - tm_height_2;
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

void tilemap_render_background(const struct TileMap *tilemap, vec2 pos, float size)
{
    unsigned int program = shaders_use_default();
    vec3 color = {0,0,0};
    mat3 transform;
    vec2 size_vec = {size*(float)tilemap->width, size*(float)tilemap->height};
    compute_transform(transform, pos, size_vec);
    draw_transformed_quad(program, transform, color, 0.6f);
}

void tilemap_shift_right(struct TileMap *tilemap, int amount)
{
    if(amount < 0)
    {
        printf("Error: shifting right with a negative amount"); 
        return;
    }
    // Collisions
    for(int y = 0; y < tilemap->height; ++y)
    {
        
        for(int x = tilemap->width - 1; x >= amount; --x)
        {
            int index = x + y * tilemap->width; 
            tilemap->solidity[index] = tilemap->solidity[index-amount]; 
        }
        
        for(int x = 0; x < amount; ++x)
        {
            int index = x + y * tilemap->width; 
            tilemap->solidity[index] = tilemap_get_default_tile_solidity();
        }
    }
    // Tile Layers
    for(int layer_index = 0; layer_index < tilemap->layer_count; ++layer_index)
    {
        Tile *layer = tilemap_get_layer_by_index(tilemap, layer_index);
        for(int y = 0; y < tilemap->height; ++y)
        {
            for(int x = tilemap->width - 1; x >= amount; --x)
            {
                Tile *tile = layer + x + y * tilemap->width;
                *tile = *(tile - amount);
            }
            for(int x = 0; x < amount; ++x)
            {
                Tile *tile = layer + x + y * tilemap->width;
                *tile = tilemap_get_default_tile();
            }
        }
    }
}

void tilemap_shift_left(struct TileMap *tilemap, int amount)
{
    if(amount < 0)
    {
        printf("Error: shifting left with a negative amount"); 
        return;
    }
    // Collisions
    for(int y = 0; y < tilemap->height; ++y)
    {
        int end = tilemap->width - amount;
        for(int x = 0; x < end; ++x)
        {
            int index = x + y * tilemap->width; 
            tilemap->solidity[index] = tilemap->solidity[index+amount]; 
        }
        for(int x = end; x < tilemap->width; ++x)
        {
            int index = x + y * tilemap->width; 
            tilemap->solidity[index] = tilemap_get_default_tile_solidity();
        }
    }
    // Tile Layers
    for(int layer_index = 0; layer_index < tilemap->layer_count; ++layer_index)
    {
        Tile *layer = tilemap_get_layer_by_index(tilemap, layer_index);
        for(int y = 0; y < tilemap->height; ++y)
        {
            int end = tilemap->width - amount;
            for(int x = 0; x < end; ++x)
            {
                Tile *tile = layer + x + y * tilemap->width;
                *tile = *(tile + amount);
            }
            for(int x = end; x < tilemap->width; ++x)
            {
                Tile *tile = layer + x + y * tilemap->width;
                *tile = tilemap_get_default_tile();
            }
        }
    }
}

void tilemap_shift_down(struct TileMap *tilemap, int amount)
{
    if(amount < 0)
    { 
        printf("Error: shifting up with a negative amount"); 
        return;
    }
    // Collisions
    for(int x = 0; x < tilemap->width; ++x)
    {
        for(int y = tilemap->height-1; y >= amount; --y)
        {
            int index = x + y * tilemap->width; 
            tilemap->solidity[index] = tilemap->solidity[index-(amount*tilemap->width)]; 
        }
        for(int y = 0; y < amount; ++y)
        {
            int index = x + y * tilemap->width; 
            tilemap->solidity[index] = tilemap_get_default_tile_solidity();
        }
    }
    // Tile Layers
    for(int layer_index = 0; layer_index < tilemap->layer_count; ++layer_index)
    {
        for(int x = 0; x < tilemap->width; ++x)
        {
            Tile *layer = tilemap_get_layer_by_index(tilemap, layer_index);
            for(int y = tilemap->height-1; y >= amount; --y)
            {
                Tile *tile = layer + x + y * tilemap->width;
                *tile = *(tile - (amount*tilemap->width));
            }
            for(int y = 0; y < amount; ++y)
            {
                Tile *tile = layer + x + y * tilemap->width;
                *tile = tilemap_get_default_tile();
            }
        }
    }
}

void tilemap_shift_up(struct TileMap *tilemap, int amount)
{
    if(amount < 0)
    { 
        printf("Error: shifting down with a negative amount"); 
        return;
    }
    // Collisions
    for(int x = 0; x < tilemap->width; ++x)
    {
        int end = tilemap->height-amount;
        for(int y = 0; y < end; ++y)
        {
            int index = x + y * tilemap->width; 
            tilemap->solidity[index] = tilemap->solidity[index+(amount*tilemap->width)]; 
        }
        for(int y = end; y < tilemap->height; ++y)
        {
            int index = x + y * tilemap->width; 
            tilemap->solidity[index] = tilemap_get_default_tile_solidity();
        }
    }
    // Tile Layers
    for(int layer_index = 0; layer_index < tilemap->layer_count; ++layer_index)
    {
        for(int x = 0; x < tilemap->width; ++x)
        {
            int end = tilemap->height-amount;
            Tile *layer = tilemap_get_layer_by_index(tilemap, layer_index);
            for(int y = 0; y < end; ++y)
            {
                Tile *tile = layer + x + y * tilemap->width;
                *tile = *(tile + (amount*tilemap->width));
            }
            for(int y = end; y < tilemap->height; ++y)
            {
                Tile *tile = layer + x + y * tilemap->width;
                *tile = tilemap_get_default_tile();
            }
        }
    }
}
