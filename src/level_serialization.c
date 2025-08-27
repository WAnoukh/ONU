#include "level_serialization.h"
#include "level.h"
#include <stdio.h>

#define PUT_INT(value, file) fputc(((char)value)+'0', file)
#define PUT_COMMA(file) fputc(',',file)

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
        PUT_INT(data->is_pressed, file);
        PUT_COMMA(file);
        PUT_INT(data->key, file);
        PUT_COMMA(file);
        fputc('}', file);
    }

    fclose(file);
    return 1;
}

