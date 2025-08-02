#include "entity.h"

#include "Rendering/rendering.h"
#include "Window/Input.h"

struct entity *entities;
int entity_count;

void initialize_entities()
{
    entity_count = 2;
    entities = (struct entity*)calloc(entity_count, sizeof(struct entity));
    struct entity *player = entities;
    player->type = Player;
    player->color[0] = 1;
    player->width = 1;
    player->height = 1.2f;
    struct entity *guard = entities+1;
    guard->type = Guard;
    guard->color[1] = 1;
    guard->width = 1;
    guard->height = 1;
}

void entity_compute_transform(struct entity *entity)
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

void render_entities()
{
    for (int i = 0; i < entity_count; i++)
    {
        struct entity e = entities[i];
        entity_compute_transform(entities + i);
        draw_entity(e);
    }
}

void update_player(const float delta_time)
{
    const float player_speed = 4;
    struct entity *player = entities;
    vec2 input;
    get_player_input(input);
    glm_vec2_scale(input, player_speed * delta_time, input);
    glm_vec2_add(player->pos, input, player->pos);
}

void update_entities(const float delta_time)
{
    update_player(delta_time);
    entities[1].pos[0] = sinf((float)get_time());
}
