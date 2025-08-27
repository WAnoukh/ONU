#include <stdio.h>

#include "level_serialization.h"
#include "level.h"
#include "tilemap.h"

#define PUT_INT(value, file) fputc(((char)value)+'0', file)
#define PUT_COMMA(file) fputc(',',file)
#define S_ERROR(msg) printf("%s, %d, Serialization error: %s", __FILE__, __LINE__, msg)

int serialize_level(struct Level level, const char* path)
{
    FILE *file=fopen(path, "w");
    if(file == NULL)
    {
        printf("Error");
        return 0;
    }

    for(int y = 0; y < level.width; ++y)
    {
        for(int x = 0; x < level.width; ++x)
        {
            int index = y*level.width + x;
            char tile = (char)level.tilemap[index].type;
            tile+='0';
            fputc(tile, file);
        }
        fputc('\n', file);
    }
    
    for(int i = 0; i<level.entity_count; ++i)
    {
        if(i > 0)
        {
            PUT_COMMA(file);
        }
        struct Entity *ent = level.entities + i;
        fputc('{', file);
        PUT_INT(ent->type, file);
        PUT_COMMA(file);
        PUT_INT(ent->solidity, file);
        PUT_COMMA(file);
        PUT_INT(ent->position[0], file);
        PUT_COMMA(file);
        PUT_INT(ent->position[1], file);
        PUT_COMMA(file);
        PUT_INT(ent->data_index, file);
        fputc('}', file);
    }

    fputc('\n', file);

    for(int i =0; i<level.key_block_data_count; ++i)
    {
        if(i > 0)
        {
            PUT_COMMA(file);
        }
        struct KeyBlockData *data = level.key_block_data+i;
        fputc('{', file);
        fputc(data->key, file);
        fputc('}', file);
    }

    fputc('\n', file);

    for(int i = 0; i<level.slot_data_count; ++i)
    {
        if(i > 0)
        {
            PUT_COMMA(file);
        }
        struct SlotData *data = level.slot_data+i;
        fputc('{', file);
        PUT_INT(data->action, file);
        fputc('}', file);
    }

    fputc('\n', file);

    PUT_INT(level.is_door_opened, file);

    fclose(file);
    return 1;
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
    
    level.tilemap = malloc(sizeof(struct Tile) * 500);
    level.height = 0;
    level.width = 0;
    int tile_index = 0;
    int width_counter;
      
    char cur_char = (char)fgetc(file);

    while(cur_char != '{')
    {
        if(cur_char == EOF)
        {
            S_ERROR("unexpected EOF.");
            return 0;
        }

        if(cur_char == '\n')
        {
            if(level.height == 0)
            {
                level.width = width_counter;
            }
            else if(level.width != width_counter)
            {
                S_ERROR("all the tile width doesn't matches.");
                return 0;
            }
            width_counter = 0;
            level.height++;
            do
            {
                cur_char = (char)fgetc(file);
            }while(cur_char == '\n');
            continue;
        }

        enum TileType tile = cur_char - '0';
        if(tile < 0 || tile >= TILE_COUNT)
        {
            S_ERROR("tile doesn't exsit.");
            return 0;
        }
        level.tilemap[tile_index++].type = tile;
        width_counter++;
        cur_char = (char)fgetc(file);
    }
    

    *out_level = level;
    return 1;
}
