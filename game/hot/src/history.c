#include <stdio.h>
#include <stdlib.h>

#include "history.h"

struct History history_init()
{
    struct History history;
    history.capacity = 10;
    history.size = 0;
    history.history = malloc(sizeof(struct GameState)*history.capacity);
    return history;
}

void history_deinit(struct History *history)
{
    free(history->history);
}

void history_append(struct History *history, struct GameState gamestate)
{
    if(history->capacity <= history->size) 
    {
        history->capacity *= 2;
        void* new_ptr = realloc(history->history, sizeof(struct GameState) * history->capacity);
        if(!new_ptr)
        {
            printf("Error while resizing history.\n");
            exit(0);
        }
        history->history = new_ptr;
    }
    history->history[history->size++] = gamestate;
}

int history_is_empty(struct History *history)
{
    return !history->size;
}

struct GameState history_pop(struct History *history)
{
   return history->history[--history->size];
}

void history_drop_last(struct History *history)
{
    --history->size;
}

void history_clear(struct History *history)
{
    history->size = 0;
}
