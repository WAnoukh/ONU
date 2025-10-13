#include "game.h"
#include "cglm/mat3.h"
#include "window/window.h"


struct Game game_init()
{
    struct Game game;
    game.history = history_init();
    return game;
}

void game_deinit(struct Game *game)
{
    history_deinit(&game->history);
}

void game_history_register(struct Game *game)
{
    history_append(&game->history, *get_current_gamestate(game));
}

int game_history_is_empty(struct Game *game)
{
    return history_is_empty(&game->history);
}

struct GameState game_history_pop(struct Game *game)
{
    return history_pop(&game->history);
}

void game_history_drop_last(struct Game *game)
{
    history_drop_last(&game->history);
}

void game_history_clear(struct Game *game)
{
    history_clear(&game->history);
}

void load_level(struct Game *game, struct Level level)
{
    game->level = level;
    game->gamestate_current = level.gamestate;
    game_history_clear(game);
}

void camera_pan(struct Camera *camera, float x_offset, float y_offset)
{
    camera->pan[0] += x_offset;
    camera->pan[1] += y_offset;
}

void camera_compute_view(struct Camera *camera)
{
    float sx = camera->zoom / window_get_screen_ratio();
    float sy = camera->zoom;
    float tx = camera->pan[0];
    float ty = camera->pan[1];
    glm_mat3_identity(camera->view);
    camera->view[0][0] = sx;
    camera->view[1][1] = sy;
    camera->view[2][0] = tx;
    camera->view[2][1] = ty;
    glm_mat3_inv(camera->view, camera->view_inverse);
}

void camera_zoom(struct Camera *camera, float amount)
{
    float zoom = camera->zoom;
    zoom += zoom * amount;
    const float max = 1.5f;
    const float min = 0.1f;
    camera->zoom = zoom;
    camera->zoom = glm_clamp(camera->zoom, min, max);
}

void camera_screen_to_world(struct Camera *camera, const vec2 screen_pos, vec2 out_world)
{
    vec3 pos;
    vec3 world_pos;
    pos[0] = screen_pos[0]*2-1;
    pos[1] = screen_pos[1]*2-1;
    pos[2] = 1;

    glm_mat3_mulv(camera->view_inverse, pos, world_pos);

    out_world[0] = world_pos[0];
    out_world[1] = world_pos[1];
}

void game_setup_default_level(struct Game *game)
{
    game->gamemode = GM_LEVEL;
    get_default_level(&game->level);
    load_gamestate(game, game->level.gamestate);
    game_history_clear(game);
}

void game_set_sequence(struct Game *game, struct Sequence sequence)
{
    game->gamemode = GM_SEQUENCE;
    game->sequence_index = 0;
    game->sequence = sequence;
}

