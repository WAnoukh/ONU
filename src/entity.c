#include "entity.h"

#include "Rendering/rendering.h"
#include "Window/Input.h"
#include <stdio.h>

struct Entity *entities;
int entity_count;

void initialize_entities()
{
    entity_count = 2;
    entities = (struct Entity*)calloc(entity_count, sizeof(struct Entity));
    struct Entity *player = entities;
    player->type = PLAYER;
    player->color[0] = 1;
    player->width = 1;
    player->height = 1.2f;
    struct Entity *guard = entities+1;
    guard->type = GUARD;
    guard->color[1] = 1;
    guard->width = 1;
    guard->height = 1;
}

void entity_compute_transform(struct Entity *entity)
{
    if (entity == NULL) return;
    vec2 size;
    size[0] = entity->width;
    size[1] = entity->height;
    compute_transform(entity->transform, entity->pos, size);
}

void render_entities()
{
    for (int i = 0; i < entity_count; i++)
    {
        struct Entity e = entities[i];
        entity_compute_transform(entities + i);
        draw_entity(e);
    }
}

void update_player(const float delta_time)
{
    const float player_speed = 4;
    struct Entity *player = entities;
    vec2 input;
    get_player_input(input);
    glm_vec2_scale(input, player_speed * delta_time, input);
    glm_vec2_add(player->pos, input, player->pos);
}

void update_entities(float delta_time)
{
    update_player(delta_time);
    entities[1].pos[0] = sinf((float)get_time());
}
