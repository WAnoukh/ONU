#ifndef RENDERING_H
#define RENDERING_H
#include <cglm/ivec2.h>

typedef unsigned int GLuint;
struct Entity;
struct TextureAtlas;
struct Camera;

typedef unsigned int Shader;

struct Rendering
{
    struct Camera *main_camera;

    mat3 screen_space_view;

    Shader shader_default, shader_sprite, shader_atlas;

    unsigned int cube_vbo, cube_vao;
    unsigned int quad_vbo, quad_vao;
};

#define GLCheckError() GLCheckErrorImpl(__FILE__, __LINE__)

void GLCheckErrorImpl(const char* file, int line);

void rendering_set_current(struct Rendering *info);

struct Rendering *rendering_get_current();

void rendering_set_camera(struct Camera *new_main_camera);

void rendering_defaults(struct Rendering *rendering);

void rendering_start(struct Rendering *rendering);

void load_default_shaders();

unsigned int shaders_use_default();

unsigned int shaders_use_sprite(unsigned int texture);

unsigned int shaders_use_atlas(struct TextureAtlas atlas, int x, int y);

void load_cube_mesh();

void load_quad_mesh();

void draw_quad();

//TODO: erase that
void initialize_renderer();

//TODO: erase that
void draw_transformed_quad(unsigned int program, mat3 transform, vec3 color, float alpha);

//TODO: erase that
void draw_transformed_quad_screen_space(unsigned int program, mat3 transform, vec3 color, float alpha);

//TODO: adapt that
void r_set_main_camera(struct Camera *camera) ;

#endif //RENDERING_H
