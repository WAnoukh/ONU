#ifndef RENDERING_H
#define RENDERING_H
#include <cglm/ivec2.h>

struct Entity;

void initialize_renderer();

void set_camera_zoom(float zoom);

float get_camera_zoom();

void pan_camera(float x_offset, float y_offset);

void compute_camera_view();

void draw_transformed_quad(mat3 transform, vec3 color);

void draw_entity(struct Entity e);

void render_triangle();

#endif //RENDERING_H
