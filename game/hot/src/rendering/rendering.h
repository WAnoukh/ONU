#ifndef RENDERING_H
#define RENDERING_H
#include <cglm/ivec2.h>

typedef unsigned int GLuint;
struct Entity;
struct TextureAtlas;
struct Camera;

struct RenderingInfo
{
    struct Camera *main_camera;

    GLuint shader_program_default, shader_program_sprite, shader_program_atlas;

    unsigned int triangle_VBO;
    unsigned int triangle_VAO;

    mat3 screen_space_view;

    GLuint quad_EBO, quad_VBO, quad_VAO;
};

void render_set_info(struct RenderingInfo *info);

void r_set_main_camera(struct Camera *new_main_camera);

void initialize_renderer();

unsigned int shaders_use_default();

unsigned int shaders_use_sprite(unsigned int texture);

unsigned int shaders_use_atlas(struct TextureAtlas atlas, int x, int y);

void draw_transformed_quad(unsigned int program, mat3 transform, vec3 color, float alpha);

void draw_transformed_quad_screen_space(unsigned int program, mat3 transform, vec3 color, float alpha);

void draw_entity(struct Entity e);

void render_triangle();

#endif //RENDERING_H
