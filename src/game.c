#include "game.h"
#include "cglm/mat3.h"
#include "window/window.h"

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
