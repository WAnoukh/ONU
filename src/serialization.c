#include <string.h>
#include <stdio.h>

#include "serialization.h"
#include "level.h"
#include "level_sequence.h"
#include "tilemap.h"
#include "game.h"

#define S_ERROR(msg) printf("%s, %d, Serialization error: %s", __FILE__, __LINE__, msg)
#define MAJOR 0
#define MINOR 4
#define PATCH 1

const struct Version version = 
{
    MAJOR,
    MINOR,
    PATCH,
};

void serialize_gamestate(const struct GameState *gamestate, FILE *file)
{
    fwrite(&gamestate->entity_count, sizeof(int), 1, file);
    fwrite(gamestate->entities, sizeof(struct Entity)*gamestate->entity_count, 1, file);

    fwrite(&gamestate->key_block_data_count, sizeof(gamestate->key_block_data_count), 1, file);
    fwrite(gamestate->key_block_data, sizeof(struct KeyBlockData)*gamestate->key_block_data_count, 1, file);

    fwrite(&gamestate->slot_data_count, sizeof(gamestate->slot_data_count), 1, file);
    fwrite(gamestate->slot_data, sizeof(struct SlotData)*gamestate->slot_data_count, 1, file);

    fwrite(&gamestate->slot_data_count, sizeof(gamestate->door_data_count), 1, file);
    fwrite(gamestate->door_data, sizeof(struct DoorData)*gamestate->door_data_count, 1, file);
}

void deserialize_gamestate(struct GameState *out_gamestate, FILE *file, struct Version file_version)
{
    out_gamestate->is_end_reached = 0;

    fread(&out_gamestate->entity_count, sizeof(int), 1, file);
    fread(out_gamestate->entities, sizeof(struct Entity)*out_gamestate->entity_count, 1, file);

    fread(&out_gamestate->key_block_data_count, sizeof(out_gamestate->key_block_data_count), 1, file);
    fread(&out_gamestate->key_block_data, sizeof(struct KeyBlockData)* out_gamestate->key_block_data_count, 1, file);
    
    fread(&out_gamestate->slot_data_count, sizeof(out_gamestate->slot_data_count), 1, file);
    fread(out_gamestate->slot_data, sizeof(struct SlotData) * out_gamestate->slot_data_count, 1, file);

    if(compare_version_value(file_version, 0, 3, 0) > 0)
    {
        fread(&out_gamestate->door_data_count, sizeof(out_gamestate->door_data_count), 1, file);
        fread(out_gamestate->door_data, sizeof(struct DoorData) * out_gamestate->door_data_count, 1, file);
    }
    else
    {
        printf("GameState deserialization: Adapting doors for pre v0.3.0\n");
        int is_main_door_opened;
        fread(&is_main_door_opened, sizeof(is_main_door_opened), 1, file);

        out_gamestate->door_data_count = 1;
        out_gamestate->door_data[0].is_opened = is_main_door_opened;
        int door_index = -1;
        for(int i = 0; i < out_gamestate->entity_count; ++i)
        {
            struct Entity *ent = out_gamestate->entities+i;
            if(ent->type == ENTITY_DOOR)
            {
                ent->data_index = 0;
                door_index = i;
                create_end_at(out_gamestate, ent->position[0], ent->position[1]);
            }
        }
        if(door_index >= 0)
        {
            for(int i = 0; i < out_gamestate->entity_count; ++i)
            {
                struct Entity *ent = out_gamestate->entities+i;
                if (ent->type == ENTITY_SLOT)
                {
                    struct SlotData *data = out_gamestate->slot_data+ent->data_index;
                    if(data->action.type == ACTION_DOOR_OPEN || data->action.type == ACTION_DOOR_CLOSE)
                    {
                        data->action.target_entity = door_index;
                    }
                }
            }
        }
    }
}

int serialize_level(const struct Level *level, const char* path)
{
    FILE *file=fopen(path, "wb");
    if(file == NULL)
    {
        printf("Error");
        return 0;
    }

    fwrite(&version.major, sizeof(version.major), 1, file);
    fwrite(&version.minor, sizeof(version.minor), 1, file);
    fwrite(&version.patch, sizeof(version.patch), 1, file);

    int level_width = level_get_width(level);
    int level_height = level_get_height(level);

    fwrite(&level_width, sizeof(level_width), 1, file);
    fwrite(&level_height, sizeof(level_height), 1, file);

    int layer_count = level->tilemap.layer_count;
    fwrite(&layer_count, sizeof(layer_count), 1, file);
    
    for(int i = 0; i < layer_count * level_width * level_height; ++i)
    {
        fwrite(level->tilemap.tile+i, sizeof(Tile), 1, file);
    }

    serialize_gamestate(&level->gamestate, file);

    fwrite(&level->view_width, sizeof(level->view_width), 1, file);
    fwrite(&level->view_height, sizeof(level->view_height), 1, file);

    fclose(file);
    return 1;
}

int deserialize_level(struct Level *out_level, const char *path)
{
    struct Level level;  

    FILE *file=fopen(path, "r");
    if(file == NULL)
    {
        S_ERROR("unable to read the file.\n");
        return 0;
    }

    int major, minor, patch; 
    fread(&major, sizeof(major), 1, file);
    fread(&minor, sizeof(minor), 1, file);
    fread(&patch, sizeof(patch), 1, file);
    struct Version file_version = (struct Version){
        major,
        minor,
        patch
    };

    if(compare_version(file_version, version) < 0)
    {
        printf("Reading a old version !\n");
    }

    fread(&level.tilemap.width, sizeof(level.tilemap.width), 1, file);
    fread(&level.tilemap.height, sizeof(level.tilemap.height), 1, file);

    fread(&level.tilemap.layer_count, sizeof(level.tilemap.layer_count), 1, file);
    level.tilemap.tile = malloc(sizeof(Tile)*level.tilemap.width*level.tilemap.height*level.tilemap.layer_count);

    fread(level.tilemap.tile, sizeof(Tile)*level.tilemap.height*level.tilemap.width*level.tilemap.layer_count, 1, file);

    deserialize_gamestate(&level.gamestate, file, file_version);

    if(compare_version_value(file_version, 0, 4, 1) < 0)
    {
        printf("Zeroing views width\n");
        level.view_width = 0;
        level.view_height = 0;
    }
    else
    {
        fread(&level.view_width, sizeof(level.view_width), 1, file);
        fread(&level.view_height, sizeof(level.view_height), 1, file);
    }

    *out_level = level;

    fclose(file);
    return 1;
}

int deserialize_level_into_game(struct Game *game, const char *path)
{
    game->gamemode = GM_LEVEL;
    struct Level level;
    int result = deserialize_level(&level, path);
    if(result)
    {
        level_deinit(&game->level);
        load_level(game, level);
    }
    return result;
}

int serialize_path_sequence(struct PathSequence path_seq, const char *path)
{
    FILE *file=fopen(path, "wb");
    if(file == NULL)
    {
        printf("Error");
        return 0;
    }
    for(int i = 0; i < path_seq.pathes_count; ++i)
    {
        char *seq_path = path_seq.pathes[i];
        unsigned long long seq_path_size = strlen(seq_path);
        fwrite(seq_path, sizeof(char)*seq_path_size, 1, file);
        if(i < path_seq.pathes_count - 1)
        {
            putc('\n', file);
        }
    }
    return 1;
}

int deserialize_path_sequence(struct PathSequence *out_path_seq, const char *path)
{
    struct PathSequence path_seq;  

    FILE *file=fopen(path, "r");
    if(file == NULL)
    {
        S_ERROR("unable to read the file.\n");
        return 0;
    }

    char cur_char = 'A';
    char buffer[260];
    int buffer_index = 0;
    char *pathes[100];
    int pathes_index = 0;
    while(cur_char != EOF)
    {
        cur_char = (char)fgetc(file);
        if((cur_char == '\n' || cur_char == EOF) && buffer_index)
        {
            buffer[buffer_index] = '\0';
            pathes[pathes_index] = malloc(sizeof(char)*buffer_index);
            strcpy(pathes[pathes_index], buffer);
            ++pathes_index;
            buffer_index = 0;
        }
        else
        {
            buffer[buffer_index++] = cur_char;
        }
    }

    path_seq.pathes = malloc(sizeof(char*) * pathes_index);
    path_seq.pathes_count = pathes_index;

    for(int i = 0; i < pathes_index; ++i)
    {
        path_seq.pathes[i] = pathes[i];
    }
    *out_path_seq = path_seq;
    return 1;
}

int deserialize_sequence_into_game(struct Game *game, char *path)
{
    struct PathSequence path_seq;
    if(!deserialize_path_sequence(&path_seq, path))
    {   
        return 0;
    }
    struct Sequence sequence;
    if(!sequence_load_path_sequence(path_seq, &sequence))
    {
        return 0;
    }
    path_sequence_deinit(&path_seq);
    game_set_sequence(game, sequence);
    load_level(game, *get_current_level(game));
    return 1;
}
