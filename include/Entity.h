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
    CGLM_ALIGN_MAT mat4 model;
    vec2 pos;
};

static void entity_compute_model(struct entity *entity)
{
    if (entity == NULL) return;
    glm_mat4_identity(entity->model);
    glm_translate2d(entity->model, entity->pos);
}
#endif //ENTITY_H
