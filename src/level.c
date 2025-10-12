#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "level.h"
#include "GLFW/glfw3.h"
#include "cglm/ivec2.h"
#include "cglm/vec2.h"
#include "rendering/rendering.h"
#include "tilemap.h"
#include "level.h"
#include "transform.h"

#define DEFAULT_LEVEL_SIZE 10
#define DEFAULT_LEVEL_GRID_SIZE DEFAULT_LEVEL_SIZE * DEFAULT_LEVEL_SIZE



void level_deinit(struct Level *level)
{
    free(level->tilemap.tile);
}

void get_default_level(struct Level *level)
{
    //TILEMAP
    int grid_size = DEFAULT_LEVEL_GRID_SIZE;
    //tiles
    level->tilemap.tile = malloc(sizeof(Tile) * grid_size);
    level->tilemap.layer_count = 1;
    for(int i =0; i < grid_size; ++i) level->tilemap.tile[i] = 4+20;
    printf("Tiles init %d \n", level->tilemap.tile[0]);
    level_set_width(level, DEFAULT_LEVEL_SIZE);
    level_set_height(level, DEFAULT_LEVEL_SIZE);
    //views
    level->views_width = 0;
    level->views_height = 0;

    struct GameState *gamestate = &level->gamestate;
    gamestate->entity_count = 0;
    create_slot_at(gamestate, 2, 5, ACTION_DOOR_OPEN, -1);
    create_slot_at(gamestate, 7, 1, ACTION_UP, 6);
    create_slot_at(gamestate, 7, 2, ACTION_DOWN, 6);
    create_slot_at(gamestate, 6, 2, ACTION_LEFT, 6);
    create_slot_at(gamestate, 8, 2, ACTION_RIGHT, 6);
    create_slot_at(gamestate, 4, 1, ACTION_UNDO, 6);
    gamestate->entities[gamestate->entity_count++] = (struct Entity){
        ENTITY_PLAYER,
        SOLIDITY_MOVABLE,
        {8,8},
        -1,
    };
    create_key_block_at(gamestate, 2, 7, GLFW_KEY_F);
    create_key_block_at(gamestate, 7, 1, GLFW_KEY_W);
    create_key_block_at(gamestate, 7, 2, GLFW_KEY_S);
    create_key_block_at(gamestate, 6, 2, GLFW_KEY_A);
    create_key_block_at(gamestate, 8, 2, GLFW_KEY_D);
    create_key_block_at(gamestate, 4, 1, GLFW_KEY_R);
    create_movable_at(gamestate, 7, 7, ENTITY_BOX);
    create_door_at(gamestate, 2, 0);
    gamestate->is_door_opened = 0;
}

void draw_view_borders(struct Level *level, vec2 pos, float size)
{
    glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
    int level_width = level_get_width(level);
    int level_height = level_get_height(level);
    unsigned int program;
    program = shaders_use_default(); 
    vec3 color = {0.2f, 1, 0.2f};

    for(int x = 0; x < level_width/level->views_width; x++)
    {
        for(int y = 0; y < level_height/level->views_height; y++)
        {
            mat3 transform;
            vec2 pos_offset;
            vec2 size_vec = {size*(float)level->views_width, size*(float)level->views_height};

            pos_offset[0] = ((float)x + 0.5f) * size_vec[0];
            pos_offset[1] = (-(float)y - 0.5f) * size_vec[1];
            glm_vec2_add(pos, pos_offset, pos_offset);
            compute_transform(transform, pos_offset, size_vec);
            draw_transformed_quad(program, transform, color, 1);
        }
    }
    glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
}

void render_level(struct Level *level, struct GameState *gamestate, int layer_mask)
{
    vec2 pos = {0,0};
    float size = 1;

    tilemap_render_background(&level->tilemap, pos, size);

    if(level->tilemap.layer_count > 0 && (layer_mask & 0b1))
    {
        tilemap_render_layer(&level->tilemap, 0, pos, size);
    }
    
    //tilemap_render_solidmap(&level->tilemap, level_get_width(level), level_get_height(level), pos, size);
    
    vec2 ent_offset = {-0.5f*size*(float)level->tilemap.width, 0.5f*size*(float)level->tilemap.height};
    vec2 ents_pos;
    glm_vec2_add(pos, ent_offset, ents_pos);

    render_repeaters_range(gamestate, &level->tilemap, ents_pos, size);
    render_entities(gamestate, ents_pos, size);

    if(level->views_height > 0 && level->views_width > 0)
    {
       draw_view_borders(level, ents_pos, size);
    }
}

void resize_level(struct Level *level, int new_width, int new_height)
{
    int new_size = new_height*new_width;
    Tile *new_tiles = malloc(sizeof(Tile)*new_size*level->tilemap.layer_count);
    if(!new_tiles)
    {
        printf("Error while allocating resized level tiles");
        exit(1);
    }
    for(int i = 0; i < new_size*level->tilemap.layer_count; ++i)
    {
        new_tiles[i] = tilemap_get_default_tile();
    }

    int copy_width = level_get_width(level), copy_height = level_get_height(level);
    if(copy_width > new_width) copy_width = new_width;
    if(copy_height > new_height) copy_height = new_height;
    for(int h = 0; h < copy_height; ++h)
    {
        int original_index = h * level_get_width(level);
        int new_index = h * new_width;
        for(int layer =0; layer<level->tilemap.layer_count; ++layer)
        {
            memcpy(new_tiles+(layer*new_size)+new_index, level->tilemap.tile+original_index+(layer*level_get_width(level)*level_get_height(level)), sizeof(Tile)*copy_width);
        }
    }
    free(level->tilemap.tile);
    level->tilemap.tile = new_tiles;
    level_set_width(level, new_width);
    level_set_height(level, new_height);
}

void level_shift(struct Level *level, ivec2 offset)
{
    int x = offset[0];
    if(x < 0) 
    {
        tilemap_shift_left(&level->tilemap, -x);
    }else if(x > 0)
    {
        tilemap_shift_right(&level->tilemap, x);
    }
    int y = offset[1];
    if(y < 0) 
    {
        tilemap_shift_up(&level->tilemap, -y);
    }else if(y > 0)
    {
        tilemap_shift_down(&level->tilemap, y);
    }
    for(int i = 0; i < level->gamestate.entity_count; ++i)
    {
        ivec2 *pos = &level->gamestate.entities[i].position;
        glm_ivec2_add(*pos, offset, *pos);
    }
}
