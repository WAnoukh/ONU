#ifndef ENTITY_TILE_H
#define ENTITY_TILE_H

#include "cglm/types.h"

enum EntityType
{
    ENTITY_NONE,
    ENTITY_PLAYER,
    ENTITY_KEY
};

struct EntityCell
{
    enum EntityType entity_type;
};

typedef struct EntityCell* EntityMap;
 
void render_entities(EntityMap entitymap, int em_width, int em_height, vec2 pos, float size);
#endif
