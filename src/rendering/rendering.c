#include <stdlib.h>
#include <cglm/cglm.h>

#include "glad/glad.h"
#include "rendering/camera.h"
#include "rendering/shader.h"
#include "texture.h"
#include "rendering/rendering.h"

#define GLCheckError() GLCheckErrorImpl(__FILE__, __LINE__)

struct Camera *main_camera;

static void GLCheckErrorImpl(const char* file, int line) {
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

GLuint shader_program_default, shader_program_sprite, shader_program_atlas;

unsigned int triangle_VBO = 0;
unsigned int triangle_VAO = 0;

mat3 screen_space_view;

float triangle_vertices[] = {
    -0.5f, -0.5f, 0.0f,
    0.5f, -0.5f, 0.0f,
    0.0f, 0.5f, 0.0f
};

float quad_vertices[] = {
     // positions          //texture coords
     0.5f,  0.5f,  0.0f,   1.0f, 1.0f, 
     0.5f, -0.5f,  0.0f,   1.0f, 0.0f,
    -0.5f, -0.5f,  0.0f,   0.0f, 0.0f,
    -0.5f,  0.5f,  0.0f,   0.0f, 1.0f
};

unsigned int quad_indices[] = {
    // note that we start from 0!
    0, 1, 3, // first triangle
    1, 2, 3 // second triangle
};

GLuint quad_EBO = 0, quad_VBO = 0, quad_VAO = 0;

void initialize_quad();

void initialize_default_shader_program();

void initialize_triangle();

void initialize_screen_space_view()
{
    glm_mat3_zero(screen_space_view);
    screen_space_view[0][0] = 2;
    screen_space_view[1][1] = 2;
    screen_space_view[2][0] = -1;
    screen_space_view[2][1] = -1;
}

void r_set_main_camera(struct Camera *new_main_camera)
{
    main_camera = new_main_camera;
}

void initialize_renderer(struct Camera *new_main_camera)
{
    initialize_default_shader_program();
    initialize_quad();
    initialize_triangle();
    initialize_screen_space_view();
    r_set_main_camera(new_main_camera);
}

void initialize_default_shader_program()
{
    shader_program_default = create_shader_program("resources/shader/generic/default.vert", "resources/shader/generic/default.frag");
    if (!shader_program_default)
    {
        exit(1);
    }
    shader_program_sprite = create_shader_program("resources/shader/generic/sprite.vert","resources/shader/generic/sprite.frag");
    if(!shader_program_sprite)
    {
        exit(1);
    }
    shader_program_atlas = create_shader_program("resources/shader/generic/atlas.vert","resources/shader/generic/atlas.frag");
    if(!shader_program_atlas)
    {
        exit(1);
    }
}

unsigned int shaders_use_default()
{
    glUseProgram(shader_program_default);
    return shader_program_default;
}
 
unsigned int shaders_use_sprite(unsigned int texture)
{
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);
    glUseProgram(shader_program_sprite);
    shader_set_int(shader_program_sprite, "texture", 0);
    return shader_program_sprite;
}

unsigned int shaders_use_atlas(struct TextureAtlas atlas, int x, int y)
{
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, atlas.texture_id);
    glUseProgram(shader_program_atlas);
    GLCheckError();
    shader_set_int(shader_program_atlas, "texture", 0);
    GLCheckError();
    shader_set_vec2(shader_program_atlas, "atlas_pos", (float)x, (float)y);
    GLCheckError();
    shader_set_vec2(shader_program_atlas, "atlas_size", (float)atlas.width, (float)atlas.height);
    GLCheckError();
    return shader_program_atlas;
}

void initialize_quad()
{
    glGenVertexArrays(1, &quad_VAO);
    glGenBuffers(1, &quad_VBO);
    glGenBuffers(1, &quad_EBO);

    glBindVertexArray(quad_VAO);

    glBindBuffer(GL_ARRAY_BUFFER, quad_VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quad_vertices), quad_vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, quad_EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(quad_indices), quad_indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *) 0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindVertexArray(0);
}

void initialize_triangle()
{
    glGenVertexArrays(1, &triangle_VAO);
    glGenBuffers(1, &triangle_VBO);

    glBindVertexArray(triangle_VAO);

    glBindBuffer(GL_ARRAY_BUFFER, triangle_VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(triangle_vertices), triangle_vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *) 0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindVertexArray(0);
}

void draw_quad()
{
    GLCheckError();
    if (quad_EBO == 0)
    {
        initialize_quad();
    }
    glBindVertexArray(quad_VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    GLCheckError();
}

void draw_wire_quad()
{
    GLCheckError();
    if (quad_EBO == 0)
    {
        initialize_quad();
    }
    glBindVertexArray(quad_VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    GLCheckError();
}

void draw_transformed_quad(unsigned int program, mat3 transform, vec3 color, float alpha)
{
    mat3 result;
    glm_mat3_mul(main_camera->view, transform, result);
    //glm_mat3_mul(transform, camera->view, result);
    shader_set_mat3(program, "view", result);
    shader_set_vec3(program, "color", color);
    shader_set_float(program, "alpha", alpha);
    draw_quad();
}

void draw_transformed_quad_screen_space(unsigned int program, mat3 transform, vec3 color, float alpha)
{
    mat3 result;
    glm_mat3_mul(screen_space_view, transform, result);
    //glm_mat3_mul(transform, camera->view, result);
    shader_set_mat3(program, "view", result);
    shader_set_vec3(program, "color", color);
    shader_set_float(program, "alpha", alpha);
    draw_quad();
}

void render_triangle()
{
    if (triangle_VBO == 0)
    {
        initialize_triangle();
    }

    if (shader_program_default == 0)
    {
        initialize_default_shader_program();
    }

    glUseProgram(shader_program_default);
    glBindVertexArray(triangle_VAO);
    glDrawArrays(GL_TRIANGLES, 0, 3);
}
