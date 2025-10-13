#ifndef CAMERA_H
#define CAMERA_H

#include "cglm/types.h"

struct Camera
{
    mat3 view;
    mat3 view_inverse;
    vec2 pan;
    float zoom;
};

struct Camera camera_get_default();

void camera_pan(struct Camera *camera, float x_offset, float y_offset);

void camera_zoom(struct Camera *camera, float amount);

void camera_compute_view(struct Camera *camera);

void camera_screen_to_world(struct Camera *camera, const vec2 screen_pos, vec2 out_world);

#endif // CAMERA_H
