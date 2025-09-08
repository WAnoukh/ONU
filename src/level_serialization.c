#include <stdio.h>

#include "level_serialization.h"
#include "level.h"
#include "tilemap.h"
#include "game.h"

#define S_ERROR(msg) printf("%s, %d, Serialization error: %s", __FILE__, __LINE__, msg)

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

    fwrite(&level->entity_count, sizeof(int), 1, file);
    fwrite(level->entities, sizeof(struct Entity)*level->entity_count, 1, file);

    fwrite(&level->key_block_data_count, sizeof(level->key_block_data_count), 1, file);
    fwrite(level->key_block_data, sizeof(struct KeyBlockData)*level->key_block_data_count, 1, file);

    fwrite(&level->slot_data_count, sizeof(level->slot_data_count), 1, file);
    fwrite(level->slot_data, sizeof(struct SlotData)*level->slot_data_count, 1, file);

    fclose(file);
    return 1;
}

int deserialize_level_into_game(struct Game *game, const char *path)
{
    //TODO : Adapt to new tilemap
    free(game->level.tilemap.solidity);
    int result = deserialize_level(&game->level_start, path);
    if(result)
    {
        load_level(game, game->level_start);
    }
    return result;
}

int deserialize_level(struct Level *out_level, const char *path)
{
    struct Level level;  
    level.is_door_reached = 0;

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

    fread(&level.entity_count, sizeof(int), 1, file);
    fread(level.entities, sizeof(struct Entity)*level.entity_count, 1, file);

    fread(&level.key_block_data_count, sizeof(level.key_block_data_count), 1, file);
    fread(&level.key_block_data, sizeof(struct KeyBlockData)* level.key_block_data_count, 1, file);
    
    fread(&level.slot_data_count, sizeof(level.slot_data_count), 1, file);
    fread(level.slot_data, sizeof(struct SlotData) * level.slot_data_count, 1, file);

    *out_level = level;

    fclose(file);
    return 1;
}
