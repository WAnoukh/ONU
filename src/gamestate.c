#include "gamestate.h"
#include "cglm/vec2.h"
#include "cglm/vec3.h"
#include "rendering/rendering.h"
#include "texture.h"
#include "tilemap.h"
#include "transform.h"

const char *entity_names[] = {"None","Player","Box","Key","Slot","Door","Repeater","Button", "Anti-button"};
const char *action_names[] = {"None","Up","Down","Left","Right","Undo","DoorOpen","DoorClose"};

vec3 color_key_block_activated = {203.f, 214.f, 0.f};
vec3 color_door_open = {0.f,1.f,0.f};
vec3 entities_color[] = {
    {0.f,0.f,0.f},          
    {0.5f,0.1f,0.3f},       //Player
    {0.82f,0.56f,0.16f},    //Box
    {0.1f,0.6f,0.6f},       //Key
    {0.9f,0.9f,0.9f},       //Slot
    {0.f,0.f,0.f},          //Door
    {0.9f,0.3f,0.4f},        //Repeater
    {0.1f,0.9f,0.1f},        //Button
    {0.9f,0.1f,0.1f},        //Button
};

const char *get_entity_name(enum EntityType type)
{
    return entity_names[type];
}

const char * const*get_entity_names()
{
    return entity_names;
}

const char *get_action_name(enum ActionType type)
{
    return action_names[type];
}

const char * const*get_action_names()
{
    return action_names;
}

int get_entity_index(struct GameState *gamestate, struct Entity *entity)
{
    return (int)(entity - gamestate->entities);
}

void render_repeater_range_tile(unsigned int program, const ivec2 pos, vec2 offset, float size)
{
    mat3 transform;
    vec2 size_vec = {size, size};
    vec2 pos_offseted;
    pos_offseted[0] = ((float)pos[0]+0.5f) * size;
    pos_offseted[1] = (-(float)pos[1]-0.5f) * size;
    glm_vec2_add(offset, pos_offseted, pos_offseted);
    compute_transform(transform, pos_offseted, size_vec);
    draw_transformed_quad(program, transform, entities_color[ENTITY_REPEATER], 0.2f);
}

void render_repeaters_range(struct GameState *gamestate, struct TileMap *tilemap, vec2 pos, float size)
{
    for(int i = 0; i < gamestate->entity_count; ++i)
    {
        struct Entity ent = gamestate->entities[i];
        if(ent.type != ENTITY_REPEATER) continue;
        {
            for(int x = 0; x < tilemap->width; ++x)
            {
                ivec2 tile_pos = {x, ent.position[1]};
                if(is_tilemap_solid_at(tilemap, tile_pos))
                {
                    continue;
                }
                unsigned int program = shaders_use_default();
                render_repeater_range_tile(program, tile_pos, pos, size);
            }
            for(int y = 0; y < tilemap->width; ++y)
            {
                ivec2 tile_pos = {ent.position[0], y};
                if(is_tilemap_solid_at(tilemap, tile_pos))
                {
                    continue;
                }
                unsigned int program = shaders_use_default();
                render_repeater_range_tile(program, tile_pos, pos, size);
            }
        }
    }
}

void render_entities(struct GameState *gamestate, vec2 pos, float size)
{
    //Render slot/buttons first
    for(int i = 0; i < gamestate->entity_count; ++i)
    {
        struct Entity ent = gamestate->entities[i];
        if(ent.type != ENTITY_SLOT && ent.type != ENTITY_BUTTON && ent.type != ENTITY_ANTIBUTTON) continue;
        vec3 color;
        glm_vec3_copy(entities_color[(int)ent.type], color);
        unsigned int program;
        program = shaders_use_sprite(get_texture_slot());

        mat3 transform;
        vec2 size_vec = {size, size};
        vec2 pos_offset;
        pos_offset[0] = ((float)ent.position[0]+0.5f) * size;
        pos_offset[1] = (-(float)ent.position[1]-0.5f) * size;
        glm_vec2_add(pos, pos_offset, pos_offset);
        compute_transform(transform, pos_offset, size_vec);
        draw_transformed_quad(program, transform, color, 1);
    }

    //Render the rest
    for(int i = 0; i < gamestate->entity_count; ++i)
    {
        struct Entity ent = gamestate->entities[i];
        if(ent.type == ENTITY_SLOT || ent.type == ENTITY_BUTTON || ent.type == ENTITY_ANTIBUTTON) continue;
        vec3 color;
        glm_vec3_copy(entities_color[(int)ent.type], color);
        if(ent.type == ENTITY_KEY)
        {
            struct KeyBlockData *key_block_data = gamestate->key_block_data+ent.data_index;
            if(key_block_data->is_pressed)
            {
                glm_vec3_copy(color_key_block_activated, color);
            }
        }else if(ent.type == ENTITY_DOOR)
        {
            if(gamestate->is_door_opened)
            {
                glm_vec3_copy(color_door_open, color);
            }
        }
        unsigned int program;
        switch (ent.type)
        {
            case ENTITY_KEY:
                program = shaders_use_sprite(get_texture_key());
                break;
            default:
                program = shaders_use_default();
                break;
        }

        mat3 transform;
        vec2 size_vec = {size, size};
        vec2 pos_offset;
        pos_offset[0] = ((float)ent.position[0]+0.5f) * size;
        pos_offset[1] = ((float)-ent.position[1]-0.5f) * size;
        glm_vec2_add(pos, pos_offset, pos_offset);
        compute_transform(transform, pos_offset, size_vec);
        draw_transformed_quad(program, transform, color, 1);
        if(ent.type == ENTITY_KEY)
        {
            int x, y;
            struct TextureAtlas atlas = get_texture_font_atlas();
            struct KeyBlockData *key_block_data = gamestate->key_block_data+ent.data_index;
            int index;
            if(key_block_data->key != '.') 
            {
                index = key_block_data->key - 'A';
            }
            else
            {
                index = 27;
            }
            atlas_index_to_coordinates(atlas, index, &x, &y);
            program = shaders_use_atlas(atlas, x, y);
            color[0] = 0; color[1] = 0; color[2] = 0; 
            size_vec[0] *= 0.3f;
            size_vec[1] *= 0.6f;
            compute_transform(transform, pos_offset, size_vec);
            draw_transformed_quad(program, transform, color, 1);
        }
    }
}

struct Entity *get_entity_at(struct GameState *gamestate, ivec2 at)
{
    for(int i = 0 ; i < gamestate->entity_count ; ++i)
    {
        if(glm_ivec2_eqv(gamestate->entities[i].position, at))
        {
            return gamestate->entities + i;
        }
    }
    return NULL;
}

struct Entity *get_solid_entity_at(struct GameState *gamestate, ivec2 at)
{
    for(int i = 0 ; i < gamestate->entity_count ; ++i)
    {
        if(gamestate->entities[i].solidity == SOLIDITY_NONE) continue;
        if(glm_ivec2_eqv(gamestate->entities[i].position, at))
        {
            return gamestate->entities + i;
        }
    }
    return NULL;
}

struct Entity *get_slot_at(struct GameState *gamestate, ivec2 at)
{
    for(int i = 0 ; i < gamestate->entity_count ; ++i)
    {
        if(gamestate->entities[i].type != ENTITY_SLOT) continue;
        if(glm_ivec2_eqv(gamestate->entities[i].position, at))
        {
            return gamestate->entities + i;
        }
    }
    return NULL;
}

struct Entity *get_player(struct GameState *gamestate)
{
    for(int i = 0 ; i < gamestate->entity_count ; ++i)
    {
        if(gamestate->entities[i].type == ENTITY_PLAYER)
        {
            return gamestate->entities + i;
        }
    }
    return NULL;
}

int get_player_position_in_gs(struct GameState *gamestate, ivec2 out_position)
{
    for(int i = 0 ; i < gamestate->entity_count ; ++i)
    {
        if(gamestate->entities[i].type == ENTITY_PLAYER)
        {
            glm_ivec2_copy(gamestate->entities[i].position, out_position);
        }
    }
    return 0;
} 

// NOLINTNEXTLINE(misc-no-recursion)
int try_push_entity(struct GameState *gamestate, struct TileMap *tilemap, struct Entity *entity, ivec2 offset)
{
    ivec2 target_pos;
    glm_ivec2_add(entity->position, offset, target_pos);

    struct Entity *obstacle = get_entity_at(gamestate, target_pos);
    if(entity->type == ENTITY_PLAYER && obstacle != NULL && obstacle->type == ENTITY_DOOR && gamestate->is_door_opened)
    {
        glm_ivec2_copy(target_pos, entity->position);
        gamestate->is_door_reached = 1;
        return 1;
    }
    if(is_tilemap_solid_at(tilemap, target_pos))
    {
        return 0;
    }
    struct Entity *solid_entity = get_solid_entity_at(gamestate, target_pos);
    if(solid_entity == NULL || solid_entity->solidity == SOLIDITY_NONE)
    {
        glm_ivec2_copy(target_pos, entity->position);
        return 1;
    }
    if(solid_entity->solidity == SOLIDITY_MOVABLE && try_push_entity(gamestate, tilemap, solid_entity, offset))
    {
        glm_ivec2_copy(target_pos, entity->position);
        return 1;
    }
    return 0;
}

int push_entity(struct GameState *gamestate, struct TileMap *tilemap, struct Entity *entity, ivec2 offset)
{
    if(glm_ivec2_eq(offset, 0)) return 0;
    if(entity->solidity != SOLIDITY_MOVABLE)
    {
        perror("Moving a non movable entity\n");
        return 0;
    }
    return try_push_entity(gamestate, tilemap, entity, offset);
}

void remove_entity(struct GameState *gamestate, int index)
{
    for(int i = 0; i < gamestate->slot_data_count; ++i)
    {
        struct SlotData *slot = gamestate->slot_data+i;
        if(slot->action.target_entity == gamestate->entity_count)
        {
            slot->action.target_entity = index;
        }
        else if(slot->action.target_entity == index)
        {
            slot->action.target_entity = -1;
        }
    }
    gamestate->entities[index] = gamestate->entities[--gamestate->entity_count];
}
