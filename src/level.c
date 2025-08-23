#include "level.h"
#include "cglm/io.h"
#include "cglm/ivec2.h"
#include "cglm/vec2.h"
#include "rendering/rendering.h"
#include "tilemap.h"
#include "transform.h"
#include <stdio.h>

#define DEFAULT_LEVEL_SIZE 10
#define DEFAULT_LEVEL_GRID_SIZE DEFAULT_LEVEL_SIZE * DEFAULT_LEVEL_SIZE

struct Tile default_grid[DEFAULT_LEVEL_GRID_SIZE] = 
{
    TILE_WALL, TILE_WALL, TILE_WALL, TILE_WALL, TILE_WALL, TILE_WALL, TILE_WALL, TILE_WALL, TILE_WALL, TILE_WALL,
    TILE_WALL, TILE_EMPTY, TILE_EMPTY, TILE_WALL, TILE_EMPTY, TILE_EMPTY, TILE_EMPTY, TILE_EMPTY, TILE_WALL, TILE_WALL,
    TILE_WALL, TILE_EMPTY, TILE_EMPTY, TILE_EMPTY, TILE_EMPTY, TILE_EMPTY, TILE_EMPTY, TILE_EMPTY, TILE_EMPTY, TILE_WALL,
    TILE_WALL, TILE_EMPTY, TILE_EMPTY, TILE_EMPTY, TILE_EMPTY, TILE_EMPTY, TILE_EMPTY, TILE_EMPTY, TILE_EMPTY, TILE_WALL,
    TILE_WALL, TILE_EMPTY, TILE_EMPTY, TILE_WALL, TILE_EMPTY, TILE_EMPTY, TILE_EMPTY, TILE_EMPTY, TILE_EMPTY, TILE_WALL,
    TILE_WALL, TILE_EMPTY, TILE_EMPTY, TILE_EMPTY, TILE_EMPTY, TILE_EMPTY, TILE_EMPTY, TILE_EMPTY, TILE_EMPTY, TILE_WALL,
    TILE_WALL, TILE_EMPTY, TILE_EMPTY, TILE_EMPTY, TILE_EMPTY, TILE_EMPTY, TILE_EMPTY, TILE_EMPTY, TILE_EMPTY, TILE_WALL,
    TILE_WALL, TILE_EMPTY, TILE_EMPTY, TILE_EMPTY, TILE_EMPTY, TILE_EMPTY, TILE_EMPTY, TILE_EMPTY, TILE_EMPTY, TILE_WALL,
    TILE_WALL, TILE_EMPTY, TILE_EMPTY, TILE_EMPTY, TILE_EMPTY, TILE_EMPTY, TILE_EMPTY, TILE_EMPTY, TILE_EMPTY, TILE_WALL,
    TILE_WALL, TILE_WALL, TILE_WALL, TILE_WALL, TILE_WALL, TILE_WALL, TILE_WALL, TILE_WALL, TILE_WALL, TILE_WALL,
};

struct Level get_default_level()
{
    struct Level level;
    level.tilemap = default_grid;
    level.entities[0] = (struct Entity){
        ENTITY_PLAYER,
        SOLIDITY_MOVABLE,
        {8,8},
        NULL
    };
    level.entity_count = 1;
    level.height = DEFAULT_LEVEL_SIZE;
    level.width = DEFAULT_LEVEL_SIZE;
    return level;
}

vec3 player_color = {0.5f,0.1f,0.3f};

void render_entities(struct Level level, vec2 pos, float size)
{
    //TODO FIX THIS
    float width_2 = (float)level.width / 2.f;
    float height_2 = (float)level.height / 2.f;
    for(int i = 0; i < level.entity_count; ++i)
    {
        struct Entity ent = level.entities[i];
        mat3 transform;
        vec2 size_vec = {size, size};
        vec2 pos_offset;
        pos_offset[0] = (float)(ent.position[0]) * size - width_2;
        pos_offset[1] = (float)(level.height-ent.position[1]) * size - height_2;
        glm_vec2_add(pos, pos_offset, pos_offset);
        compute_transform(transform, pos_offset, size_vec);
        draw_transformed_quad(transform, player_color);
    }
}

void render_level(struct Level level)
{
    vec2 pos = {0,0};
    float size = 1;
    render_tilemap(level.tilemap, level.width, level.height, pos, size);
    render_entities(level, pos, size);
}

void compute_position_from_index(struct Level level, int index, ivec2 out_position)
{
    out_position[1] = index / level.height;
    out_position[0] = index - out_position[1]* level.width;
    glm_ivec2_print(out_position, stdout);
}

int compute_index_from_coordinate(struct Level level, int x, int y)
{
    return x + y * level.width;
}

int compute_index_from_position(struct Level level, ivec2 position)
{
    return compute_index_from_coordinate(level, position[0], position[1]);
}

char tile_print[] = {' ', 'X'}; 
char entity_print[] = {' ', 'P', 'K'};

void print_level(struct Level level)
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

struct Entity *get_player_at(struct Level *level, ivec2 at)
{
    for(int i = 0 ; i < level->entity_count ; ++i)
    {
        ivec2 ent_pos;
        if(glm_ivec2_eq(level->entities[i].position, *at))
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

int get_player_position_in_level(struct Level level, ivec2 out_position)
{
    int total = level.width * level.height;
    for(int i = 0 ; i < level.entity_count ; ++i)
    {
        if(level.entities[i].type == ENTITY_PLAYER)
        {
            glm_ivec2_copy(level.entities[i].position, out_position);
        }
    }
    return 0;
} 

void move_entity(struct Entity *entity, ivec2 new_position)
{
    glm_ivec2_copy(new_position, entity->position);
}
