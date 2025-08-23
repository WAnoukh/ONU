#include "level.h"
#include "cglm/io.h"
#include "entity_tile.h"
#include "tilemap.h"
#include <stdio.h>

#define DEFAULT_LEVEL_SIZE 10
#define DEFAULT_LEVEL_GRID_SIZE DEFAULT_LEVEL_SIZE * DEFAULT_LEVEL_SIZE

struct Tile default_grid[DEFAULT_LEVEL_GRID_SIZE] = 
{
    TILE_SOLID, TILE_SOLID, TILE_SOLID, TILE_SOLID, TILE_SOLID, TILE_SOLID, TILE_SOLID, TILE_SOLID, TILE_SOLID, TILE_SOLID,
    TILE_SOLID, TILE_NONE, TILE_NONE, TILE_SOLID, TILE_NONE, TILE_NONE, TILE_NONE, TILE_NONE, TILE_SOLID, TILE_SOLID,
    TILE_SOLID, TILE_NONE, TILE_NONE, TILE_NONE, TILE_NONE, TILE_NONE, TILE_NONE, TILE_NONE, TILE_NONE, TILE_SOLID,
    TILE_SOLID, TILE_NONE, TILE_NONE, TILE_NONE, TILE_NONE, TILE_NONE, TILE_NONE, TILE_NONE, TILE_NONE, TILE_SOLID,
    TILE_SOLID, TILE_NONE, TILE_NONE, TILE_SOLID, TILE_NONE, TILE_NONE, TILE_NONE, TILE_NONE, TILE_NONE, TILE_SOLID,
    TILE_SOLID, TILE_NONE, TILE_NONE, TILE_NONE, TILE_NONE, TILE_NONE, TILE_NONE, TILE_NONE, TILE_NONE, TILE_SOLID,
    TILE_SOLID, TILE_NONE, TILE_NONE, TILE_NONE, TILE_NONE, TILE_NONE, TILE_NONE, TILE_NONE, TILE_NONE, TILE_SOLID,
    TILE_SOLID, TILE_NONE, TILE_NONE, TILE_NONE, TILE_NONE, TILE_NONE, TILE_NONE, TILE_NONE, TILE_NONE, TILE_SOLID,
    TILE_SOLID, TILE_NONE, TILE_NONE, TILE_NONE, TILE_NONE, TILE_NONE, TILE_NONE, TILE_NONE, TILE_NONE, TILE_SOLID,
    TILE_SOLID, TILE_SOLID, TILE_SOLID, TILE_SOLID, TILE_SOLID, TILE_SOLID, TILE_SOLID, TILE_SOLID, TILE_SOLID, TILE_SOLID,
};

struct EntityCell default_entity_grid[DEFAULT_LEVEL_GRID_SIZE] =
{
       ENTITY_NONE, ENTITY_NONE, ENTITY_NONE, ENTITY_NONE, ENTITY_NONE, ENTITY_NONE, ENTITY_NONE, ENTITY_NONE, ENTITY_NONE, ENTITY_NONE, 
       ENTITY_NONE, ENTITY_NONE, ENTITY_NONE, ENTITY_NONE, ENTITY_NONE, ENTITY_NONE, ENTITY_NONE, ENTITY_NONE, ENTITY_NONE, ENTITY_NONE, 
       ENTITY_NONE, ENTITY_NONE, ENTITY_NONE, ENTITY_NONE, ENTITY_NONE, ENTITY_NONE, ENTITY_NONE, ENTITY_NONE, ENTITY_NONE, ENTITY_NONE, 
       ENTITY_NONE, ENTITY_NONE, ENTITY_NONE, ENTITY_NONE, ENTITY_NONE, ENTITY_NONE, ENTITY_NONE, ENTITY_NONE, ENTITY_NONE, ENTITY_NONE, 
       ENTITY_NONE, ENTITY_NONE, ENTITY_NONE, ENTITY_NONE, ENTITY_NONE, ENTITY_NONE, ENTITY_NONE, ENTITY_NONE, ENTITY_NONE, ENTITY_NONE, 
       ENTITY_NONE, ENTITY_NONE, ENTITY_NONE, ENTITY_NONE, ENTITY_NONE, ENTITY_NONE, ENTITY_NONE, ENTITY_NONE, ENTITY_NONE, ENTITY_NONE, 
       ENTITY_NONE, ENTITY_NONE, ENTITY_NONE, ENTITY_NONE, ENTITY_NONE, ENTITY_NONE, ENTITY_NONE, ENTITY_NONE, ENTITY_NONE, ENTITY_NONE, 
       ENTITY_NONE, ENTITY_NONE, ENTITY_KEY, ENTITY_NONE, ENTITY_NONE, ENTITY_NONE, ENTITY_NONE, ENTITY_NONE, ENTITY_NONE, ENTITY_NONE, 
       ENTITY_NONE, ENTITY_PLAYER, ENTITY_NONE, ENTITY_NONE, ENTITY_NONE, ENTITY_NONE, ENTITY_NONE, ENTITY_NONE, ENTITY_NONE, ENTITY_NONE, 
       ENTITY_NONE, ENTITY_NONE, ENTITY_NONE, ENTITY_NONE, ENTITY_NONE, ENTITY_NONE, ENTITY_NONE, ENTITY_NONE, ENTITY_NONE, ENTITY_NONE, 
};

struct Level get_default_level()
{
    struct Level level;
    level.tilemap = default_grid;
    level.entities = default_entity_grid;
    level.height = DEFAULT_LEVEL_SIZE;
    level.width = DEFAULT_LEVEL_SIZE;
    return level;
}

void render_level(struct Level level)
{
    vec2 pos = {0,0};
    float size = 1;
    render_tilemap(level.tilemap, level.width, level.height, pos, size);
    render_entities(level.entities, level.width, level.height, pos, size);
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
}

int get_player_position_in_level(struct Level level, ivec2 out_position)
{
    int total = level.width * level.height;
    for(int i = 0 ; i < total ; ++i)
    {
        if(level.entities[i].entity_type == ENTITY_PLAYER)
        {
            compute_position_from_index(level, i, out_position);
            glm_ivec2_print(out_position, stdout);
            return 1;
        }
    }
    return 0;
} 

void move_entity(struct Level level, ivec2 from, ivec2 to)
{
    glm_ivec2_print(from, stdout);
    int from_index = compute_index_from_position(level, from);
    int to_index = compute_index_from_position(level, to);
    level.entities[to_index] = level.entities[from_index];
    level.entities[from_index] = (struct EntityCell){ENTITY_NONE};
}
