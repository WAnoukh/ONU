#include "camera.h"
#include "cglm/mat3.h"
#include <stdio.h>

struct Camera camera_get_default()
{
    struct Camera camera;
    camera.zoom = 1.0f;
    camera.pos[0] = 0;
    camera.pos[1] = 0;

    return camera;
}

void camera_pan(struct Camera *camera, float x_offset, float y_offset)
{
    camera->pos[0] += x_offset;
    camera->pos[1] += y_offset;
}

void camera_compute_view(struct Camera *camera, float window_ratio)
{
    float sx = camera->zoom / window_ratio; 
    float sy = camera->zoom;
    float tx = -camera->pos[0];
    float ty = camera->pos[1];
    glm_mat3_identity(camera->view);
    camera->view[0][0] = sx;
    camera->view[1][1] = -sy;
    camera->view[2][0] = tx*sx;
    camera->view[2][1] = ty*sy;
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

