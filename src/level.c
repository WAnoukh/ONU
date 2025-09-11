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

const char *entity_names[] = {"None","Player","Box","Key","Slot","Door"};

const char *get_entity_name(enum EntityType type)
{
    return entity_names[type];
}

const char * const*get_entity_names()
{
    return entity_names;
}

const char *action_names[] = {"None","Up","Down","Left","Right","Undo","DoorOpen","DoorClose"};

const char *get_action_name(enum ActionType type)
{
    return action_names[type];
}

const char * const*get_action_names()
{
    return action_names;
}

int get_entity_index(struct Level *level, struct Entity *entity)
{
    return (int)(entity - level->entities);
}

enum TileSolidity default_solidmap[DEFAULT_LEVEL_GRID_SIZE] = 
{
    STILE_SOLID, STILE_SOLID, STILE_SOLID, STILE_SOLID, STILE_SOLID, STILE_SOLID, STILE_SOLID, STILE_SOLID, STILE_SOLID, STILE_SOLID,
    STILE_SOLID, STILE_EMPTY, STILE_EMPTY, STILE_SOLID, STILE_EMPTY, STILE_EMPTY, STILE_EMPTY, STILE_EMPTY, STILE_SOLID, STILE_SOLID,
    STILE_SOLID, STILE_EMPTY, STILE_EMPTY, STILE_EMPTY, STILE_EMPTY, STILE_EMPTY, STILE_EMPTY, STILE_EMPTY, STILE_EMPTY, STILE_SOLID,
    STILE_SOLID, STILE_EMPTY, STILE_EMPTY, STILE_EMPTY, STILE_EMPTY, STILE_EMPTY, STILE_EMPTY, STILE_EMPTY, STILE_EMPTY, STILE_SOLID,
    STILE_SOLID, STILE_EMPTY, STILE_EMPTY, STILE_SOLID, STILE_EMPTY, STILE_EMPTY, STILE_EMPTY, STILE_EMPTY, STILE_EMPTY, STILE_SOLID,
    STILE_SOLID, STILE_EMPTY, STILE_EMPTY, STILE_EMPTY, STILE_EMPTY, STILE_EMPTY, STILE_EMPTY, STILE_EMPTY, STILE_EMPTY, STILE_SOLID,
    STILE_SOLID, STILE_EMPTY, STILE_EMPTY, STILE_EMPTY, STILE_EMPTY, STILE_EMPTY, STILE_EMPTY, STILE_EMPTY, STILE_EMPTY, STILE_SOLID,
    STILE_SOLID, STILE_EMPTY, STILE_EMPTY, STILE_EMPTY, STILE_EMPTY, STILE_EMPTY, STILE_EMPTY, STILE_EMPTY, STILE_EMPTY, STILE_SOLID,
    STILE_SOLID, STILE_EMPTY, STILE_EMPTY, STILE_EMPTY, STILE_EMPTY, STILE_EMPTY, STILE_EMPTY, STILE_EMPTY, STILE_EMPTY, STILE_SOLID,
    STILE_SOLID, STILE_SOLID, STILE_SOLID, STILE_SOLID, STILE_SOLID, STILE_SOLID, STILE_SOLID, STILE_SOLID, STILE_SOLID, STILE_SOLID,
};

void get_default_level(struct Level *level)
{
    //TILEMAP
    //solidity
    int grid_size=sizeof(enum TileSolidity) * DEFAULT_LEVEL_GRID_SIZE;
    level->tilemap.solidity = malloc(grid_size);
    memcpy(level->tilemap.solidity, default_solidmap, grid_size);
    level->tilemap.layer_count = 0;
    //tiles
    level->tilemap.tile = malloc(sizeof(Tile) * grid_size);
    level->tilemap.layer_count = 1;
    for(int i =0; i < grid_size; ++i) level->tilemap.tile[i] = 4+20;
    printf("Tiles init %d \n", level->tilemap.tile[0]);
    level_set_width(level, DEFAULT_LEVEL_SIZE);
    level_set_height(level, DEFAULT_LEVEL_SIZE);

    level->entity_count = 0;
    create_slot_at(level, 2, 5, ACTION_DOOR_OPEN, -1);
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
}

vec3 color_key_block_activated = {203.f, 214.f, 0.f};
vec3 color_door_open = {0.f,1.f,0.f};
vec3 entities_color[] = {{0.f,0.f,0.f}, {0.5f,0.1f,0.3f},{0.2f,0.2f,0.2f},{0.1f,0.6f,0.6f},{0.9f,0.9f,0.9f},{0.f,0.f,0.f}};

void render_entities(struct Level *level, vec2 pos, float size)
{
    float width_2 = (float)level_get_width(level) / 2.f;
    float height_2 = (float)level_get_height(level) / 2.f;
    //Render slot first
    for(int i = 0; i < level->entity_count; ++i)
    {
        struct Entity ent = level->entities[i];
        if(ent.type != ENTITY_SLOT) continue;
        vec3 color;
        glm_vec3_copy(entities_color[(int)ent.type], color);
        unsigned int program;
        program = shaders_use_sprite(get_texture_slot());

        mat3 transform;
        vec2 size_vec = {size, size};
        vec2 pos_offset;
        pos_offset[0] = (float)(ent.position[0]) * size - width_2;
        pos_offset[1] = (float)(level_get_height(level)-ent.position[1]) * size - height_2;
        glm_vec2_add(pos, pos_offset, pos_offset);
        compute_transform(transform, pos_offset, size_vec);
        draw_transformed_quad(program, transform, color, 1);
    }


    //Render the rest
    for(int i = 0; i < level->entity_count; ++i)
    {
        struct Entity ent = level->entities[i];
        if(ent.type == ENTITY_SLOT) continue;
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
        pos_offset[1] = (float)(level_get_height(level)-ent.position[1]) * size - height_2;
        glm_vec2_add(pos, pos_offset, pos_offset);
        compute_transform(transform, pos_offset, size_vec);
        draw_transformed_quad(program, transform, color, 1);
        if(ent.type == ENTITY_KEY)
        {
            int x, y;
            struct TextureAtlas atlas = get_texture_font_atlas();
            struct KeyBlockData *key_block_data = level->key_block_data+ent.data_index;
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

void resize_level(struct Level *level, int new_width, int new_height)
{
    //TODO: Resize the tilemap too
    int new_size = new_height*new_width;
    enum TileSolidity *new_solidmap = malloc(sizeof(enum TileSolidity)*new_size);
    if(!new_solidmap)
    {
        printf("Error while allocating resized level collisions");
        exit(1);
    }
    Tile *new_tiles = malloc(sizeof(Tile)*new_size*level->tilemap.layer_count);
    if(!new_tiles)
    {
        printf("Error while allocating resized level tiles");
        exit(1);
    }
    for(int i = 0; i < new_size; ++i)
    {
        new_solidmap[i] = STILE_SOLID;
    }
    for(int i = 0; i < new_size*level->tilemap.layer_count; ++i)
    {
        new_tiles[i] = 0;
    }
    //memset(new_tilemap, TILE_WALL, sizeof(struct Tile)*new_size);
    int copy_width = level_get_width(level), copy_height = level_get_height(level);
    if(copy_width > new_width) copy_width = new_width;
    if(copy_height > new_height) copy_height = new_height;
    for(int h = 0; h < copy_height; ++h)
    {
        int original_index = h * level_get_width(level);
        int new_index = h * new_width;
        memcpy(new_solidmap+new_index, level->tilemap.solidity+original_index, sizeof(enum TileSolidity)*copy_width);
        for(int layer =0; layer<level->tilemap.layer_count; ++layer)
        {
            memcpy(new_tiles+(layer*new_size)+new_index, level->tilemap.tile+original_index+(layer*level_get_width(level)*level_get_height(level)), sizeof(Tile)*copy_width);
        }
    }
    free(level->tilemap.solidity);
    level->tilemap.solidity = new_solidmap;
    free(level->tilemap.tile);
    level->tilemap.tile = new_tiles;
    level_set_width(level, new_width);
    level_set_height(level, new_height);
}

void render_level(struct Level *level, int layer_mask)
{
    vec2 pos = {0,0};
    float size = 1;
    if(level->tilemap.layer_count > 0 && (layer_mask & 0b100))
    {
        tilemap_render_layer(&level->tilemap, 0, pos, size);
    }
    if(layer_mask & 0b1)
    {
        render_solidmap(level->tilemap.solidity, level_get_width(level), level_get_height(level), pos, size);
    }
    if(layer_mask & 0b10)
    {
        render_entities(level, pos, size);
    }
}

void compute_position_from_index(struct Level *level, int index, ivec2 out_position)
{
    out_position[1] = index / level_get_height(level);
    out_position[0] = index - out_position[1]* level_get_width(level);
    glm_ivec2_print(out_position, stdout);
}

int compute_index_from_coordinate(struct Level *level, int x, int y)
{
    if(x<0 || x >= level_get_width(level) || y < 0 || y >+ level_get_height(level))
    {
        perror("Coordinate out of bounds");
        exit(1);
    }
    return x + y * level_get_width(level);
}

int compute_index_from_position(struct Level *level, ivec2 position)
{
    return compute_index_from_coordinate(level, position[0], position[1]);
}

int is_tilemap_solid_at(struct Level *level, ivec2 position)
{
   return level->tilemap.solidity[compute_index_from_position(level, position)] == STILE_SOLID;
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

void remove_entity(struct Level *level, int index)
{
    for(int i = 0; i < level->slot_data_count; ++i)
    {
        struct SlotData *slot = level->slot_data+i;
        if(slot->action.target_entity == level->entity_count)
        {
            slot->action.target_entity = index;
        }
        else if(slot->action.target_entity == index)
        {
            slot->action.target_entity = -1;
        }
    }
    level->entities[index] = level->entities[--level->entity_count];
}
