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

void initialize_entities();

void update_entities(const float delta_time);

void entity_compute_transform(struct entity *entity);

void render_entities();
#endif //ENTITY_H
