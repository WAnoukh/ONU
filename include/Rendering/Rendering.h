#ifndef RENDERING_H
#define RENDERING_H

struct entity;

void initialize_renderer();

void initialize_camera();

void render_entities(struct entity *entities, int entities_number);

void render_triangle();

#endif //RENDERING_H
