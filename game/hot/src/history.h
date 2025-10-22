#ifndef HISTORY_H
#define HISTORY_H

#include "gamestate.h"

struct History
{
    struct GameState *history;
    int size;
    int capacity;
};

struct History history_init();

void history_deinit(struct History *history);

void history_append(struct History *history, struct GameState gamestate);

int history_is_empty(struct History *history);

struct GameState history_pop(struct History *history);

void history_drop_last(struct History *history);

void history_clear(struct History *history);

#endif // HISTORY_H

