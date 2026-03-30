#include <assert.h>
#include <cglm/cglm.h>
#include <stdio.h>

#include "glad/glad.h"
#include "rendering/camera.h"
#include "rendering/shader.h"
#include "rendering/texture.h"
#include "rendering/rendering.h"


struct Rendering *current_rendering = NULL;

void GLCheckErrorImpl(const char* file, int line) {
    GLenum err;
    while ((err = glGetError()) != GL_NO_ERROR) {
        const char* error;
        switch (err) {
            case GL_INVALID_ENUM:                  error = "GL_INVALID_ENUM"; break;
            case GL_INVALID_VALUE:                 error = "GL_INVALID_VALUE"; break;
            case GL_INVALID_OPERATION:             error = "GL_INVALID_OPERATION"; break;
            case GL_STACK_OVERFLOW:                error = "GL_STACK_OVERFLOW"; break;
            case GL_STACK_UNDERFLOW:               error = "GL_STACK_UNDERFLOW"; break;
            case GL_OUT_OF_MEMORY:                 error = "GL_OUT_OF_MEMORY"; break;
            case GL_INVALID_FRAMEBUFFER_OPERATION: error = "GL_INVALID_FRAMEBUFFER_OPERATION"; break;
            default:                               error = "Unknown Error"; break;
        }
        fprintf(stderr, "OpenGL error (%s) in %s:%d\n", error, file, line);
    }
}

void initialize_renderer()
{
    rendering_defaults(current_rendering);
    load_default_shaders();
    load_cube_mesh();
    load_quad_mesh();
}

void rendering_set_current(struct Rendering *info)
{
    current_rendering = info;
}

struct Rendering *rendering_get_current()
{
    return current_rendering;
}

void load_default_shaders()
{
    assert(current_rendering);

    current_rendering->shader_default = create_shader_program(
            "resources/shader/2D/default.vert", 
            "resources/shader/2D/default.frag"
            );
    if (!current_rendering->shader_default)
    {
        exit(1);
    }

    current_rendering->shader_atlas = create_shader_program(
            "resources/shader/2D/atlas.vert", 
            "resources/shader/2D/atlas.frag"
            );
    if (!current_rendering->shader_atlas)
    {
        exit(1);
    }

    current_rendering->shader_sprite = create_shader_program(
            "resources/shader/2D/sprite.vert", 
            "resources/shader/2D/sprite.frag"
            );
    if (!current_rendering->shader_sprite)
    {
        exit(1);
    }
}

unsigned int shaders_use_default()
{
    assert(current_rendering);
    assert(current_rendering->shader_default);
    glUseProgram(current_rendering->shader_default);
    return current_rendering->shader_default;
}

unsigned int shaders_use_sprite(unsigned int texture)
{
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);
    glUseProgram(current_rendering->shader_sprite);
    shader_set_int(current_rendering->shader_sprite, "texture", 0);
    return current_rendering->shader_sprite;
}

unsigned int shaders_use_atlas(struct TextureAtlas atlas, int x, int y)
{
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, atlas.texture_id);
    glUseProgram(current_rendering->shader_atlas);
    GLCheckError();
    shader_set_int(current_rendering->shader_atlas, "texture", 0);
    GLCheckError();
    shader_set_vec2(current_rendering->shader_atlas, "atlas_pos", (float)x, (float)y);
    GLCheckError();
    shader_set_vec2(current_rendering->shader_atlas, "atlas_size", (float)atlas.width, (float)atlas.height);
    GLCheckError();
    return current_rendering->shader_atlas;
}

void create_screen_space_view(mat3 out_view)
{
    glm_mat3_zero(out_view);
    out_view[0][0] = 2;
    out_view[1][1] = 2;
    out_view[2][0] = -1;
    out_view[2][1] = -1;
}

void rendering_set_camera(struct Camera *new_main_camera)
{
    assert(current_rendering);
    assert(new_main_camera);
    current_rendering->main_camera = new_main_camera;
}

void rendering_defaults(struct Rendering *rendering)
{
    *rendering = (struct Rendering){0};
    create_screen_space_view(rendering->screen_space_view);
}

void rendering_start(struct Rendering *rendering)
{
    load_default_shaders();
    load_cube_mesh();
    load_cube_mesh();
}

const float cube_vertices[] = {

    // -Z (back): T=+X, B=+Y
    -0.5f,-0.5f,-0.5f,  0,0,-1,   1,0,0,   0,1,0,   0,0,
     0.5f,-0.5f,-0.5f,  0,0,-1,   1,0,0,   0,1,0,   1,0,
     0.5f, 0.5f,-0.5f,  0,0,-1,   1,0,0,   0,1,0,   1,1,
     0.5f, 0.5f,-0.5f,  0,0,-1,   1,0,0,   0,1,0,   1,1,
    -0.5f, 0.5f,-0.5f,  0,0,-1,   1,0,0,   0,1,0,   0,1,
    -0.5f,-0.5f,-0.5f,  0,0,-1,   1,0,0,   0,1,0,   0,0,

    // +Z (front): T=+X, B=+Y
    -0.5f,-0.5f, 0.5f,  0,0, 1,   1,0,0,   0,1,0,   0,0,
     0.5f,-0.5f, 0.5f,  0,0, 1,   1,0,0,   0,1,0,   1,0,
     0.5f, 0.5f, 0.5f,  0,0, 1,   1,0,0,   0,1,0,   1,1,
     0.5f, 0.5f, 0.5f,  0,0, 1,   1,0,0,   0,1,0,   1,1,
    -0.5f, 0.5f, 0.5f,  0,0, 1,   1,0,0,   0,1,0,   0,1,
    -0.5f,-0.5f, 0.5f,  0,0, 1,   1,0,0,   0,1,0,   0,0,

    // -X (left):  T=+Y, B=-Z   (THIS is the fix)
    -0.5f, 0.5f, 0.5f, -1,0,0,   0,1,0,   0,0,-1,  1,0,
    -0.5f, 0.5f,-0.5f, -1,0,0,   0,1,0,   0,0,-1,  1,1,
    -0.5f,-0.5f,-0.5f, -1,0,0,   0,1,0,   0,0,-1,  0,1,
    -0.5f,-0.5f,-0.5f, -1,0,0,   0,1,0,   0,0,-1,  0,1,
    -0.5f,-0.5f, 0.5f, -1,0,0,   0,1,0,   0,0,-1,  0,0,
    -0.5f, 0.5f, 0.5f, -1,0,0,   0,1,0,   0,0,-1,  1,0,

    // +X (right): T=+Y, B=-Z   (matches your UVs; handedness differs vs N, but this is correct for UV)
     0.5f, 0.5f, 0.5f,  1,0,0,   0,1,0,   0,0,-1,  1,0,
     0.5f, 0.5f,-0.5f,  1,0,0,   0,1,0,   0,0,-1,  1,1,
     0.5f,-0.5f,-0.5f,  1,0,0,   0,1,0,   0,0,-1,  0,1,
     0.5f,-0.5f,-0.5f,  1,0,0,   0,1,0,   0,0,-1,  0,1,
     0.5f,-0.5f, 0.5f,  1,0,0,   0,1,0,   0,0,-1,  0,0,
     0.5f, 0.5f, 0.5f,  1,0,0,   0,1,0,   0,0,-1,  1,0,

    // -Y (bottom): T=+X, B=-Z
    -0.5f,-0.5f,-0.5f,  0,-1,0,   1,0,0,   0,0,-1,  0,1,
     0.5f,-0.5f,-0.5f,  0,-1,0,   1,0,0,   0,0,-1,  1,1,
     0.5f,-0.5f, 0.5f,  0,-1,0,   1,0,0,   0,0,-1,  1,0,
     0.5f,-0.5f, 0.5f,  0,-1,0,   1,0,0,   0,0,-1,  1,0,
    -0.5f,-0.5f, 0.5f,  0,-1,0,   1,0,0,   0,0,-1,  0,0,
    -0.5f,-0.5f,-0.5f,  0,-1,0,   1,0,0,   0,0,-1,  0,1,

    // +Y (top): T=+X, B=-Z
    -0.5f, 0.5f,-0.5f,  0, 1,0,   1,0,0,   0,0,-1,  0,1,
     0.5f, 0.5f,-0.5f,  0, 1,0,   1,0,0,   0,0,-1,  1,1,
     0.5f, 0.5f, 0.5f,  0, 1,0,   1,0,0,   0,0,-1,  1,0,
     0.5f, 0.5f, 0.5f,  0, 1,0,   1,0,0,   0,0,-1,  1,0,
    -0.5f, 0.5f, 0.5f,  0, 1,0,   1,0,0,   0,0,-1,  0,0,
    -0.5f, 0.5f,-0.5f,  0, 1,0,   1,0,0,   0,0,-1,  0,1
};

const float quad_vertices[] = {
        -0.5f, -0.5f, 0,        0, 0, 
        -0.5f,  0.5f, 0,        0, 1,
         0.5f,  0.5f, 0,        1, 1,

         0.5f,  0.5f, 0,        1, 1,
        -0.5f, -0.5f, 0,        0, 0, 
         0.5f, -0.5f, 0,        1, 0,
};

void load_cube_mesh()
{
    assert(current_rendering);

    glGenVertexArrays(1, &current_rendering->cube_vao);
    glGenBuffers(1, &current_rendering->cube_vbo);

    glBindVertexArray(current_rendering->cube_vao);

    glBindBuffer(GL_ARRAY_BUFFER, current_rendering->cube_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cube_vertices), cube_vertices, GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // normal attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    // tangent attribute
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);
    // bitangent attribute
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(9 * sizeof(float)));
    glEnableVertexAttribArray(3);
    // texture coord attribute
    glVertexAttribPointer(4, 2, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(12 * sizeof(float)));
    glEnableVertexAttribArray(4);
    GLCheckError();
}

void load_quad_mesh()
{
    assert(current_rendering);

    glGenVertexArrays(1, &current_rendering->quad_vao);
    glGenBuffers(1, &current_rendering->quad_vbo);

    glBindVertexArray(current_rendering->quad_vao); 

    glBindBuffer(GL_ARRAY_BUFFER, current_rendering->quad_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quad_vertices), quad_vertices, GL_STATIC_DRAW);

    
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    GLCheckError();
}

void draw_quad()
{
    assert(current_rendering);
    glBindVertexArray(current_rendering->quad_vao);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    GLCheckError();
}

void draw_transformed_quad(unsigned int program, mat3 transform, vec3 color, float alpha)
{
    mat3 result;
    glm_mat3_mul(current_rendering->main_camera->view, transform, result);
    //glm_mat3_mul(transform, camera->view, result);
    shader_set_mat3(program, "view", result);
    shader_set_vec3(program, "color", color);
    shader_set_float(program, "alpha", alpha);
    draw_quad();
}

void draw_transformed_quad_screen_space(unsigned int program, mat3 transform, vec3 color, float alpha)
{
    assert(current_rendering);
    mat3 result;
    glm_mat3_mul(current_rendering->screen_space_view, transform, result);
    //glm_mat3_mul(transform, camera->view, result);
    shader_set_mat3(program, "view", result);
    shader_set_vec3(program, "color", color);
    shader_set_float(program, "alpha", alpha);
    draw_quad();
}

void r_set_main_camera(struct Camera *camera)
{
    assert(current_rendering);
    current_rendering->main_camera = camera;
}
