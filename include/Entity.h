#ifndef ENTITY_H
#define ENTITY_H
;
enum entity_type
{
    Player,
    Guard
};

struct entity
{
    enum entity_type type;
    int x;
    int y;
};

#endif //ENTITY_H
