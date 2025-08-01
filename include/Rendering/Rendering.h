#ifndef RENDERING_H
#define RENDERING_H

struct entity;

void initialize_renderer();

void set_camera_zoom(float zoom);

float get_camera_zoom();

void compute_camera_view();

void render_entities(struct entity *entities, int entities_number);

void render_triangle();

#endif //RENDERING_H
