#ifndef GAMESTATE_H
#define GAMESTATE_H

#include "cglm/types.h"

struct TileMap;

enum ActionType
{
    ACTION_NONE,
    ACTION_UP,
    ACTION_DOWN,
    ACTION_LEFT,
    ACTION_RIGHT,
    ACTION_UNDO,
    ACTION_DOOR_OPEN,
    ACTION_DOOR_CLOSE,
    ACTION_COUNT,
};

struct Action
{
    enum ActionType type;    
    int target_entity;
};

struct KeyBlockData
{
    int key;
    int is_pressed;
};

struct SlotData
{
    struct Action action;
};

struct DoorData
{
    int is_opened;
};

enum EntityType
{
    ENTITY_NONE,
    ENTITY_PLAYER,
    ENTITY_BOX,
    ENTITY_KEY,
    ENTITY_SLOT,
    ENTITY_DOOR,
    ENTITY_REPEATER,
    ENTITY_COUNT,
};

enum Solidity 
{
    SOLIDITY_NONE,
    SOLIDITY_STATIC,
    SOLIDITY_MOVABLE,
};

struct Entity
{
    enum EntityType type;
    enum Solidity solidity;
    ivec2 position;
    int data_index;
};

struct GameState
{
    struct Entity entities[100];
    struct KeyBlockData key_block_data[50];
    struct SlotData slot_data[50];
    int entity_count;
    int key_block_data_count;
    int slot_data_count;
    int is_door_opened;
    int is_door_reached;
};

const char *get_entity_name(enum EntityType type);

const char * const*get_entity_names();

const char *get_action_name(enum ActionType type);

const char * const*get_action_names();

int get_entity_index(struct GameState *gamestate, struct Entity *entity);

int push_entity(struct GameState *gamestate, struct TileMap *tilemap, struct Entity *entity, ivec2 offset);

void render_repeaters_range(struct GameState *gamestate, struct TileMap *tilemap, vec2 pos, float size);

void render_entities(struct GameState *gamestate, vec2 pos, float size);

void remove_entity(struct GameState *gamestate, int index);

struct Entity *get_slot_at(struct GameState *gamestate, ivec2 at);

static inline void create_movable_at(struct GameState *gs, int x, int y, enum EntityType type)
{
    gs->entities[gs->entity_count++] = (struct Entity){
        type,
        SOLIDITY_MOVABLE,
        {x, y},
        -1,
    };
}

static inline void create_key_block_at(struct GameState *gs, int x, int y, int key)
{
    gs->entities[gs->entity_count++] = (struct Entity){
        ENTITY_KEY,
        SOLIDITY_MOVABLE,
        {x, y},
        gs->key_block_data_count,
    };
    gs->key_block_data[gs->key_block_data_count++] = (struct KeyBlockData){
        key,
        0,
    };
}

static inline void create_slot_at(struct GameState *gs, int x, int y, enum ActionType action, int target_index)
{
    gs->entities[gs->entity_count++] = (struct Entity){
        ENTITY_SLOT,
        SOLIDITY_NONE,
        {x, y},
        gs->slot_data_count,
    };
    gs->slot_data[gs->slot_data_count++] = (struct SlotData){
        { action, target_index },
    };
}

static inline void create_door_at(struct GameState *gs, int x, int y)
{
    gs->entities[gs->entity_count++] = (struct Entity){
        ENTITY_DOOR,
        SOLIDITY_NONE,
        {x, y},
        -1,
    };
}
#endif // GAMESTATE_H
