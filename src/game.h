#ifndef GAME_H
#define GAME_H

#include "level.h"
#include "rendering/rendering.h"

#define HISTORY_MAX_SIZE 100

struct Camera
{
    mat3 view;
    vec2 pan;
    float zoom;
};

struct Game
{
    struct Camera camera;
    struct Level level;
    struct Level level_start;
    struct Level history[HISTORY_MAX_SIZE];
    int history_size;
    double last_time;
    double new_time;
    float delta_time;
};

int history_register(struct Game *game);

int history_empty(struct Game *game);

struct Level history_pop(struct Game *game);

void load_level(struct Game *game, struct Level level);
#endif // GAME_H
