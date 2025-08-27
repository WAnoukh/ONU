#include "game.h"

int history_register(struct Game *game)
{
    if(game->history_size >= HISTORY_MAX_SIZE) {
        perror("History full");
        return 0;
    }
    game->history[game->history_size++] = game->level;
    return 1;
}

int history_empty(struct Game *game)
{
    return game->history_size <= 0;
}

struct Level history_pop(struct Game *game)
{
   if(game->history_size <= 0) 
   {
        perror("Trying to pop an empty history\n");
        exit(1);
   }
   return game->history[--game->history_size];
}

void load_level(struct Game *game, struct Level level)
{
    game->level = level;
    game->history_size = 0;
}

