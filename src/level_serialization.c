#include <stdio.h>

#include "level_serialization.h"
#include "level.h"
#include "tilemap.h"
#include "game.h"

#define S_ERROR(msg) printf("%s, %d, Serialization error: %s", __FILE__, __LINE__, msg)

void serialize_gamestate(const struct GameState *gamestate, FILE *file)
{
    fwrite(&gamestate->entity_count, sizeof(int), 1, file);
    fwrite(gamestate->entities, sizeof(struct Entity)*gamestate->entity_count, 1, file);

    fwrite(&gamestate->key_block_data_count, sizeof(gamestate->key_block_data_count), 1, file);
    fwrite(gamestate->key_block_data, sizeof(struct KeyBlockData)*gamestate->key_block_data_count, 1, file);

    fwrite(&gamestate->slot_data_count, sizeof(gamestate->slot_data_count), 1, file);
    fwrite(gamestate->slot_data, sizeof(struct SlotData)*gamestate->slot_data_count, 1, file);

    fwrite(&gamestate->is_door_opened, sizeof(gamestate->is_door_opened), 1, file);
}

int serialize_level(const struct Level *level, const char* path)
{
    FILE *file=fopen(path, "wb");
    if(file == NULL)
    {
        printf("Error");
        return 0;
    }

    int level_width = level_get_width(level);
    int level_height = level_get_height(level);

    fwrite(&level_width, sizeof(level_width), 1, file);
    fwrite(&level_height, sizeof(level_height), 1, file);

    for(int i = 0; i < level_height * level_width; ++i)
    {
        fwrite(level->tilemap.solidity + i, sizeof(enum TileSolidity), 1, file);
    }

    int layer_count = level->tilemap.layer_count;
    fwrite(&layer_count, sizeof(layer_count), 1, file);
    
    for(int i = 0; i < layer_count * level_width * level_height; ++i)
    {
        fwrite(level->tilemap.tile+i, sizeof(Tile), 1, file);
    }

    serialize_gamestate(&level->gamestate, file);

    fclose(file);
    return 1;
}

int deserialize_level_into_game(struct Game *game, const char *path)
{
    free(game->level.tilemap.solidity);
    free(game->level.tilemap.tile);
    int result = deserialize_level(&game->level, path);
    if(result)
    {
        load_level(game, game->level);
    }
    return result;
}

void deserialize_gamestate(struct GameState *out_gamestate, FILE *file)
{
    out_gamestate->is_door_reached = 0;

    fread(&out_gamestate->entity_count, sizeof(int), 1, file);
    fread(out_gamestate->entities, sizeof(struct Entity)*out_gamestate->entity_count, 1, file);

    fread(&out_gamestate->key_block_data_count, sizeof(out_gamestate->key_block_data_count), 1, file);
    fread(&out_gamestate->key_block_data, sizeof(struct KeyBlockData)* out_gamestate->key_block_data_count, 1, file);
    
    fread(&out_gamestate->slot_data_count, sizeof(out_gamestate->slot_data_count), 1, file);
    fread(out_gamestate->slot_data, sizeof(struct SlotData) * out_gamestate->slot_data_count, 1, file);

    fread(&out_gamestate->is_door_opened, sizeof(out_gamestate->is_door_opened), 1, file);
}

int deserialize_level(struct Level *out_level, const char *path)
{
    struct Level level;  

    FILE *file=fopen(path, "r");
    if(file == NULL)
    {
        S_ERROR("unable to read the file.");
        return 0;
    }


    fread(&level.tilemap.width, sizeof(level.tilemap.width), 1, file);
    fread(&level.tilemap.height, sizeof(level.tilemap.height), 1, file);
    level.tilemap.solidity = malloc(sizeof(enum TileSolidity)*level.tilemap.width*level.tilemap.height);

    fread(level.tilemap.solidity, sizeof(enum TileSolidity)*level.tilemap.height*level.tilemap.width, 1, file);

    fread(&level.tilemap.layer_count, sizeof(level.tilemap.layer_count), 1, file);
    level.tilemap.tile = malloc(sizeof(Tile)*level.tilemap.width*level.tilemap.height*level.tilemap.layer_count);

    fread(level.tilemap.tile, sizeof(Tile)*level.tilemap.height*level.tilemap.width*level.tilemap.layer_count, 1, file);

    deserialize_gamestate(&level.gamestate, file);

    *out_level = level;

    fclose(file);
    return 1;
}
