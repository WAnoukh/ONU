#include "entity_tile.h"
#include "cglm/vec2.h"
#include "rendering/rendering.h"
#include "transform.h"

vec3 entity_color[2] = {
    {0.f,0.f,0.f},
    {0.5f,0.1f,0.3f}
};

void render_entities(EntityMap entitymap, int em_width, int em_height, vec2 pos, float size)
{
    int x = -1;
    int y = (int)em_height;
    const float em_width_2 = (float)em_width * size/2;
    const float em_height_2 = (float)em_height * size/2;
    for (int i = 0; i < em_height * em_width; ++i)
    {
        ++x;
        if (x>=em_width)
        {
            x=0;
            --y;
        }

        enum EntityType entity_type = entitymap[i].entity_type;

        if(entity_type == ENTITY_NONE) continue;

        vec3 *color =   (vec3*)entity_color[entity_type];

        mat3 transform;
        vec2 size_vec = {size, size};
        vec2 pos_offset;
        pos_offset[0] = (float)x * size - em_width_2;
        pos_offset[1] = (float)y * size - em_height_2;
        glm_vec2_add(pos, pos_offset, pos_offset);
        compute_transform(transform, pos_offset, size_vec);
        draw_transformed_quad(transform, *color);
    }
}
