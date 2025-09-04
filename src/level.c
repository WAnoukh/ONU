#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "level.h"
#include "GLFW/glfw3.h"
#include "cglm/io.h"
#include "cglm/ivec2.h"
#include "cglm/vec2.h"
#include "rendering/rendering.h"
#include "tilemap.h"
#include "transform.h"
#include "texture.h"
#include "level.h"

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
    int grid_size=sizeof(struct Tile) * DEFAULT_LEVEL_GRID_SIZE;
    level->tilemap = malloc(grid_size);
    level->tilemap[0].type = TILE_WALL;
    void *result = memcpy(level->tilemap, default_grid, grid_size);
    level->entity_count = 0;
    create_slot_at(level, 2, 5, ACTION_DOOR_OPEN, 0);
    create_slot_at(level, 7, 1, ACTION_UP, 6);
    create_slot_at(level, 7, 2, ACTION_DOWN, 6);
    create_slot_at(level, 6, 2, ACTION_LEFT, 6);
    create_slot_at(level, 8, 2, ACTION_RIGHT, 6);
    create_slot_at(level, 4, 1, ACTION_UNDO, 6);
    level->entities[level->entity_count++] = (struct Entity){
        ENTITY_PLAYER,
        SOLIDITY_MOVABLE,
        {8,8},
        -1,
    };
    create_key_block_at(level, 2, 7, GLFW_KEY_F);
    create_key_block_at(level, 7, 1, GLFW_KEY_W);
    create_key_block_at(level, 7, 2, GLFW_KEY_S);
    create_key_block_at(level, 6, 2, GLFW_KEY_A);
    create_key_block_at(level, 8, 2, GLFW_KEY_D);
    create_key_block_at(level, 4, 1, GLFW_KEY_R);
    create_movable_at(level, 7, 7, ENTITY_BOX);
    create_door_at(level, 2, 0);
    level->is_door_opened = 0;
    level->height = DEFAULT_LEVEL_SIZE;
    level->width = DEFAULT_LEVEL_SIZE;
    printf("Resizing !! \n");
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
            struct KeyBlockData *key_block_data = level->key_block_data+ent.data_index;
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
        if(ent.type == ENTITY_KEY)
        {
            int x, y;
            struct TextureAtlas atlas = get_texture_font_atlas();
            struct KeyBlockData *key_block_data = level->key_block_data+ent.data_index;
            atlas_index_to_coordinates(atlas, key_block_data->key - 'A', &x, &y);
            program = shaders_use_atlas(atlas, x, y);
            color[0] = 0; color[1] = 0; color[2] = 0; 
            size_vec[0] *= 0.3f;
            size_vec[1] *= 0.6f;
            compute_transform(transform, pos_offset, size_vec);
            draw_transformed_quad(program, transform, color);
        }
    }
}

void resize_level(struct Level *level, int new_width, int new_height)
{
    int new_size = new_height*new_width;
    TileMap new_tilemap = malloc(sizeof(struct Tile)*new_size);
    if(!new_tilemap)
    {
        printf("Error while allocating resized level");
        exit(1);
    }
    for(int i = 0; i < new_size; ++i)
    {
        new_tilemap[i].type = TILE_WALL;
    }
    //memset(new_tilemap, TILE_WALL, sizeof(struct Tile)*new_size);
    int copy_width = level->width, copy_height = level->height;
    if(copy_width > new_width) copy_width = new_width;
    if(copy_height > new_height) copy_height = new_height;
    for(int h = 0; h < copy_height; ++h)
    {
        int original_index = h * level->width;
        int new_index = h * new_width;
        memcpy(new_tilemap+new_index, level->tilemap+original_index, sizeof(struct Tile)*copy_width);
    }
    free(level->tilemap);
    level->tilemap = new_tilemap;
    level->width = new_width;
    level->height = new_height;
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
