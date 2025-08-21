#ifndef ENTITY_H
#define ENTITY_H

#include <cglm/cglm.h>

enum EntityType
{
    PLAYER,
    GUARD
};

struct Entity
{
    enum EntityType type;
    mat3 transform;
    vec3 color;
    vec2 pos;
    float width;
    float height;
};

void initialize_entities();

void update_entities(float delta_time);

void entity_compute_transform(struct Entity *entity);

void render_entities();

static void compute_transform(mat3 out_transform, vec2 pos, vec2 size)
{
    glm_mat3_identity(out_transform);
    glm_translate2d(out_transform, pos);
    mat3 scale;
    glm_mat3_identity(scale);
    out_transform[0][0] = size[0];
    out_transform[1][1] = size[1];
    //glm_mat3_mul(scale,out_transform,  out_transform);
}
#endif //ENTITY_H
