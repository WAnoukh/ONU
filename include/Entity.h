#ifndef ENTITY_H
#define ENTITY_H

#include <cglm/cglm.h>

enum entity_type
{
    Player,
    Guard
};

struct entity
{
    enum entity_type type;
    mat3 transform;
    vec3 color;
    vec2 pos;
    float width;
    float height;
};

static void entity_compute_transform(struct entity *entity)
{
    if (entity == NULL) return;
    glm_mat3_identity(entity->transform);
    glm_translate2d(entity->transform, entity->pos);
    mat3 scale;
    glm_mat3_identity(scale);
    scale[0][0] = entity->width;
    scale[1][1] = entity->height;
    glm_mat3_mul(scale, entity->transform, entity->transform);
}
#endif //ENTITY_H
