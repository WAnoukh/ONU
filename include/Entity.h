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
};

static void entity_compute_transform(struct entity *entity)
{
    if (entity == NULL) return;
    glm_mat3_identity(entity->transform);
    glm_translate2d(entity->transform, entity->pos);
}
#endif //ENTITY_H
