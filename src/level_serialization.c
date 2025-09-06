#include <stdio.h>

#include "level_serialization.h"
#include "level.h"
#include "tilemap.h"
#include "game.h"

#define S_ERROR(msg) printf("%s, %d, Serialization error: %s", __FILE__, __LINE__, msg)

int serialize_level(struct Level level, const char* path)
{
    FILE *file=fopen(path, "wb");
    if(file == NULL)
    {
        printf("Error");
        return 0;
    }

    fwrite(&level.width, sizeof(level.width), 1, file);
    fwrite(&level.height, sizeof(level.height), 1, file);

    for(int i = 0; i < level.height * level.width; ++i)
    {
        fwrite(level.tilemap + i, sizeof(struct Tile), 1, file);
    }
    
    fwrite(&level.entity_count, sizeof(int), 1, file);
    fwrite(level.entities, sizeof(struct Entity)*level.entity_count, 1, file);

    fwrite(&level.key_block_data_count, sizeof(level.key_block_data_count), 1, file);
    fwrite(level.key_block_data, sizeof(struct KeyBlockData)*level.key_block_data_count, 1, file);

    fwrite(&level.slot_data_count, sizeof(level.slot_data_count), 1, file);
    fwrite(level.slot_data, sizeof(struct SlotData)*level.slot_data_count, 1, file);

    fclose(file);
    return 1;
}

int deserialize_level_into_game(struct Game *game, const char *path)
{
    free(game->level.tilemap);
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

    level.tilemap = malloc(sizeof(struct Tile)*500);

    fread(&level.width, sizeof(level.width), 1, file);
    fread(&level.height, sizeof(level.height), 1, file);

    fread(level.tilemap, sizeof(struct Tile)*level.height*level.width, 1, file);

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
