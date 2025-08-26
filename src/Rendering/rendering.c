#include <stdlib.h>
#include <cglm/cglm.h>

#include "glad/glad.h"
#include "rendering/shader.h"
#include "window/window.h"
#include "rendering/rendering.h"
#define GLCheckError() GLCheckErrorImpl(__FILE__, __LINE__)

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
GLuint shader_program_default, shader_program_sprite;

unsigned int triangle_VBO = 0;
unsigned int triangle_VAO = 0;

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

mat3 view;

float zoom = 0.2f;
vec2 pan;

void initialize_quad();

void initialize_default_shader_program();

void initialize_triangle();

void initialize_renderer()
{
    initialize_default_shader_program();
    initialize_quad();
    compute_camera_view();
    initialize_triangle();
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

void compute_camera_view()
{
    float sx = zoom / window_get_screen_ratio();
    float sy = zoom;
    float tx = pan[0];
    float ty = pan[1];
    glm_mat3_identity(view);
    view[0][0] = sx;
    view[1][1] = sy;
    view[2][0] = tx;
    view[2][1] = ty;
}

void set_camera_zoom(float in_zoom)
{
    const float max = 1.5f;
    const float min = 0.1f;
    zoom = in_zoom;
    zoom = glm_clamp(zoom, min, max);
    compute_camera_view();
}

float get_camera_zoom()
{
    return zoom;
}

void pan_camera(float x_offset, float y_offset)
{
    pan[0] += x_offset;
    pan[1] += y_offset;
    compute_camera_view();
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
    if (quad_EBO == 0)
    {
        initialize_quad();
    }
    glBindVertexArray(quad_VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

void draw_transformed_quad(unsigned int program, mat3 transform, vec3 color)
{
    mat3 result;
    glm_mat3_mul(view, transform, result);
    //glm_mat3_mul(transform, view, result);
    shader_set_mat3(program, "view", result);
    shader_set_vec3(program, "color", color);
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
