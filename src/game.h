#ifndef GAME_H
#define GAME_H

#include "level.h"

#define HISTORY_MAX_SIZE 100

struct Game
{
    struct Level level;
    struct Level level_start;
    struct Level history[HISTORY_MAX_SIZE];
    int history_size;
};

int history_register(struct Game *game);

int history_empty(struct Game *game);

struct Level history_pop(struct Game *game);

void load_level(struct Game *game, struct Level level);
#endif // GAME_H
