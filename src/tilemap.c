#include <cglm/vec3.h>

#include "tilemap.h"
#include "rendering/rendering.h"
#include "transform.h"


vec3 tile_color[2] = {
    {1.f,1.f,1.f},
    {0.1f,0.5f,0.3f},
};

void render_tilemap(TileMap tilemap, int tm_width, int tm_height, vec2 pos, float size)
{
    int x = 0;
    int y = (int)tm_height;
    const float tm_width_2 = (float)tm_width * size/2;
    const float tm_height_2 = (float)tm_height * size/2;
    for (int i = 0; i < tm_height * tm_width; ++i)
    {
        const enum TileType type = tilemap[i].type;
        vec3 *color =   (vec3*)tile_color[type];

        mat3 transform;
        vec2 size_vec = {size, size};
        vec2 pos_offset;
        pos_offset[0] = (float)x * size - tm_width_2;
        pos_offset[1] = (float)y * size - tm_height_2;
        glm_vec2_add(pos, pos_offset, pos_offset);
        compute_transform(transform, pos_offset, size_vec);
        draw_transformed_quad(transform, *color);
        ++x;
        if (x>=tm_width)
        {
            x=0;
            --y;
        }
    }
}
