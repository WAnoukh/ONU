#ifndef RENDERING_H
#define RENDERING_H
#include <cglm/ivec2.h>

struct Entity;
struct TextureAtlas;
struct Camera;

void initialize_renderer(struct Camera *new_main_camera);

unsigned int shaders_use_default();

unsigned int shaders_use_sprite(unsigned int texture);

unsigned int shaders_use_atlas(struct TextureAtlas atlas, int x, int y);

void draw_transformed_quad(unsigned int program, mat3 transform, vec3 color, float alpha);

void draw_entity(struct Entity e);

void render_triangle();

#endif //RENDERING_H
