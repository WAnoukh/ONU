#include <stdio.h>

#include "level.h"
#include "GLFW/glfw3.h"
#include "cglm/io.h"
#include "cglm/ivec2.h"
#include "cglm/vec2.h"
#include "rendering/rendering.h"
#include "tilemap.h"
#include "transform.h"
#include "window/input.h"
#include "texture.h"

#define DEFAULT_LEVEL_SIZE 10
#define DEFAULT_LEVEL_GRID_SIZE DEFAULT_LEVEL_SIZE * DEFAULT_LEVEL_SIZE

struct Tile default_grid[DEFAULT_LEVEL_GRID_SIZE] = 
{
    {TILE_WALL}, {TILE_WALL}, {TILE_WALL}, {TILE_WALL}, {TILE_WALL}, {TILE_WALL}, {TILE_WALL}, {TILE_WALL}, {TILE_WALL}, {TILE_WALL},
    {TILE_WALL}, {TILE_EMPTY}, {TILE_EMPTY}, {TILE_WALL}, {TILE_EMPTY}, {TILE_EMPTY}, {TILE_EMPTY}, {TILE_EMPTY}, {TILE_WALL}, {TILE_WALL},
    {TILE_WALL}, {TILE_EMPTY}, {TILE_EMPTY}, {TILE_EMPTY}, {TILE_EMPTY}, {TILE_EMPTY}, {TILE_EMPTY}, {TILE_EMPTY}, {TILE_EMPTY}, {TILE_WALL},
    {TILE_WALL}, {TILE_EMPTY}, {TILE_EMPTY}, {TILE_EMPTY}, {TILE_EMPTY}, {TILE_EMPTY}, {TILE_EMPTY}, {TILE_EMPTY}, {TILE_EMPTY}, {TILE_WALL},
    {TILE_WALL}, {TILE_EMPTY}, {TILE_EMPTY}, {TILE_WALL}, {TILE_EMPTY}, {TILE_EMPTY}, {TILE_EMPTY}, {TILE_EMPTY}, {TILE_EMPTY}, {TILE_WALL},
    {TILE_WALL}, {TILE_EMPTY}, {TILE_EMPTY}, {TILE_EMPTY}, {TILE_EMPTY}, {TILE_EMPTY}, {TILE_EMPTY}, {TILE_EMPTY}, {TILE_EMPTY}, {TILE_WALL},
    {TILE_WALL}, {TILE_EMPTY}, {TILE_EMPTY}, {TILE_EMPTY}, {TILE_EMPTY}, {TILE_EMPTY}, {TILE_EMPTY}, {TILE_EMPTY}, {TILE_EMPTY}, {TILE_WALL},
    {TILE_WALL}, {TILE_EMPTY}, {TILE_EMPTY}, {TILE_EMPTY}, {TILE_EMPTY}, {TILE_EMPTY}, {TILE_EMPTY}, {TILE_EMPTY}, {TILE_EMPTY}, {TILE_WALL},
    {TILE_WALL}, {TILE_EMPTY}, {TILE_EMPTY}, {TILE_EMPTY}, {TILE_EMPTY}, {TILE_EMPTY}, {TILE_EMPTY}, {TILE_EMPTY}, {TILE_EMPTY}, {TILE_WALL},
    {TILE_WALL}, {TILE_WALL}, {TILE_WALL}, {TILE_WALL}, {TILE_WALL}, {TILE_WALL}, {TILE_WALL}, {TILE_WALL}, {TILE_WALL}, {TILE_WALL},
};

void get_default_level(struct Level *level)
{
    level->tilemap = default_grid;
    level->entities[1] = (struct Entity){
        ENTITY_PLAYER,
        SOLIDITY_MOVABLE,
        {8,8},
        NULL
    };
    level->entities[0] = create_slot_at(2, 5, PA_DOOR_OPEN, level->slot_data);
    level->entities[2] = create_key_block_at(2, 7, GLFW_KEY_F, level->key_block_data);
    level->entities[3] = create_movable_at(7,7,ENTITY_BOX);
    level->entities[4] = create_door_at(2,0);
    level->entity_count = 5;
    level->key_block_data_count = 1;
    level->slot_data_count = 1;
    level->is_door_opened = 0;
    level->height = DEFAULT_LEVEL_SIZE;
    level->width = DEFAULT_LEVEL_SIZE;
}

vec3 color_key_block_activated = {203.f, 214.f, 0.f};
vec3 color_door_open = {0.f,1.f,0.f};
vec3 entities_color[] = {{0.f,0.f,0.f}, {0.f,0.f,0.f},{0.5f,0.1f,0.3f},{0.2f,0.2f,0.2f},{0.1f,0.6f,0.6f},{0.9f,0.9f,0.9f},{0.f,0.f,0.f}};

void render_entities(struct Level *level, vec2 pos, float size)
{
    float width_2 = (float)level->width / 2.f;
    float height_2 = (float)level->height / 2.f;
    for(int i = 0; i < level->entity_count; ++i)
    {
        struct Entity ent = level->entities[i];
        
        vec3 color;
        glm_vec3_copy(entities_color[(int)ent.type], color);
        if(ent.type == ENTITY_KEY)
        {
            struct KeyBlockData *key_block_data = (struct KeyBlockData*)ent.data;
            if(key_block_data == NULL)
            {
                perror("This ENTITY_KEY doesn't have KeyBlockData.");
                exit(1);
            }
            if(key_block_data->is_pressed)
            {
                glm_vec3_copy(color_key_block_activated, color);
            }
        }else if(ent.type == ENTITY_DOOR)
        {
            if(level->is_door_opened)
            {
                glm_vec3_copy(color_door_open, color);
            }
        }
        unsigned int program;
        switch (ent.type)
        {
            case ENTITY_SLOT:
                program = shaders_use_sprite(get_texture_slot());
                break;
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
        pos_offset[0] = (float)(ent.position[0]) * size - width_2;
        pos_offset[1] = (float)(level->height-ent.position[1]) * size - height_2;
        glm_vec2_add(pos, pos_offset, pos_offset);
        compute_transform(transform, pos_offset, size_vec);
        draw_transformed_quad(program, transform, color);
    }
}

void render_level(struct Level *level)
{
    vec2 pos = {0,0};
    float size = 1;
    render_tilemap(level->tilemap, level->width, level->height, pos, size);
    render_entities(level, pos, size);
}

void compute_position_from_index(struct Level *level, int index, ivec2 out_position)
{
    out_position[1] = index / level->height;
    out_position[0] = index - out_position[1]* level->width;
    glm_ivec2_print(out_position, stdout);
}

int compute_index_from_coordinate(struct Level *level, int x, int y)
{
    if(x<0 || x >= level->width || y < 0 || y >+ level->height)
    {
        perror("Coordinate out of bounds");
        exit(1);
    }
    return x + y * level->width;
}

int compute_index_from_position(struct Level *level, ivec2 position)
{
    return compute_index_from_coordinate(level, position[0], position[1]);
}

int is_tilemap_solid_at(struct Level *level, ivec2 position)
{
   return level->tilemap[compute_index_from_position(level, position)].type == TILE_WALL;
}

char tile_print[] = {' ', 'X'}; 
char entity_print[] = {' ', 'P', 'K'};

void print_level(struct Level *level)
{
    //TODO : Reimlement that
    /*
    for(int y = 0; y < level.height; ++y)
    {
        for(int x = 0; x < level.height; ++x)
        {
            int index = compute_index_from_coordinate(level, x, y);
            struct Tile tile = level.tilemap[index];
            struct EntityCell entity = level.entities[index];
            
            if(entity.entity_type != ENTITY_NONE)
            {
                printf("%c", entity_print[entity.entity_type]);
            }
            else
            {
                printf("%c", tile_print[tile.solidity]);
            }
        }
        printf("\n");
    }
    */
}

struct Entity *get_entity_at(struct Level *level, ivec2 at)
{
    for(int i = 0 ; i < level->entity_count ; ++i)
    {
        if(glm_ivec2_eqv(level->entities[i].position, at))
        {
            return level->entities + i;
        }
    }
    return NULL;
}

struct Entity *get_solid_entity_at(struct Level *level, ivec2 at)
{
    for(int i = 0 ; i < level->entity_count ; ++i)
    {
        if(level->entities[i].solidity == SOLIDITY_NONE) continue;
        if(glm_ivec2_eqv(level->entities[i].position, at))
        {
            return level->entities + i;
        }
    }
    return NULL;
}

struct Entity *get_slot_at(struct Level *level, ivec2 at)
{
    for(int i = 0 ; i < level->entity_count ; ++i)
    {
        if(level->entities[i].type != ENTITY_SLOT) continue;
        if(glm_ivec2_eqv(level->entities[i].position, at))
        {
            return level->entities + i;
        }
    }
    return NULL;
}

struct Entity *get_player(struct Level *level)
{
    for(int i = 0 ; i < level->entity_count ; ++i)
    {
        if(level->entities[i].type == ENTITY_PLAYER)
        {
            return level->entities + i;
        }
    }
    return NULL;
}

int get_player_position_in_level(struct Level *level, ivec2 out_position)
{
    for(int i = 0 ; i < level->entity_count ; ++i)
    {
        if(level->entities[i].type == ENTITY_PLAYER)
        {
            glm_ivec2_copy(level->entities[i].position, out_position);
        }
    }
    return 0;
} 

// NOLINTNEXTLINE(misc-no-recursion)
int try_push_entity(struct Level *level, struct Entity *entity, ivec2 offset)
{
    ivec2 target_pos;
    glm_ivec2_add(entity->position, offset, target_pos);

    struct Entity *obstacle = get_entity_at(level, target_pos);
    if(entity->type == ENTITY_PLAYER && obstacle != NULL && obstacle->type == ENTITY_DOOR && level->is_door_opened)
    {
        glm_ivec2_copy(target_pos, entity->position);
        level->is_door_reached = 1;
        return 1;
    }
    if(is_tilemap_solid_at(level, target_pos))
    {
        return 0;
    }
    struct Entity *solid_entity = get_solid_entity_at(level, target_pos);
    if(solid_entity == NULL || solid_entity->solidity == SOLIDITY_NONE)
    {
        glm_ivec2_copy(target_pos, entity->position);
        return 1;
    }
    if(solid_entity->solidity == SOLIDITY_MOVABLE && try_push_entity(level, solid_entity, offset))
    {
        glm_ivec2_copy(target_pos, entity->position);
        return 1;
    }
    return 0;
}

void push_entity(struct Level *level, struct Entity *entity, ivec2 offset)
{
    if(glm_ivec2_eq(offset, 0)) return;
    if(entity->solidity != SOLIDITY_MOVABLE)
    {
        perror("Moving a non movable entity\n");
        return;
    }
    try_push_entity(level, entity, offset);
}
