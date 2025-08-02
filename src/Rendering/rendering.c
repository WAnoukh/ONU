#include "Rendering/rendering.h"

#include <stdlib.h>
#include <math.h>
#include <cglm/cglm.h>

#include "entity.h"
#include "glad/glad.h"
#include "Rendering/shader.h"
#include "Window/window.h"

GLuint default_shader_program;

unsigned int triangle_VBO = 0;
unsigned int triangle_VAO = 0;

float triangle_vertices[] = {
    -0.5f, -0.5f, 0.0f,
    0.5f, -0.5f, 0.0f,
    0.0f, 0.5f, 0.0f
};

float quad_vertices[] = {
    0.5f, 0.5f, 0.0f, // top right
    0.5f, -0.5f, 0.0f, // bottom right
    -0.5f, -0.5f, 0.0f, // bottom left
    -0.5f, 0.5f, 0.0f // top left
};

unsigned int quad_indices[] = {
    // note that we start from 0!
    0, 1, 3, // first triangle
    1, 2, 3 // second triangle
};

GLuint quad_EBO = 0, quad_VBO = 0, quad_VAO = 0;

mat3 view;

float zoom = 0.2;

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
    default_shader_program = create_shader_program("../shader/generic/default.vert", "../shader/generic/default.frag");
    if (!default_shader_program)
    {
        exit(1);
    }
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

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *) 0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindVertexArray(0);
}

void compute_camera_view()
{
    float sx = zoom / window_get_screen_ratio();
    float sy = zoom;
    float tx = 0;
    float ty = 0;
    glm_mat3_identity(view);
    view[0][0] = sx;
    view[1][1] = sy;
    view[2][0] = tx;
    view[2][1] = ty;
}

void set_camera_zoom(float in_zoom)
{
    const float max = 1.5;
    const float min = 0.1;
    zoom = in_zoom;
    zoom = glm_clamp(zoom, min, max);
    compute_camera_view();
}

float get_camera_zoom()
{
    return zoom;
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

void draw_entity(struct entity e)
{
    glUseProgram(default_shader_program);
    mat3 result;
    glm_mat3_mul(view, e.transform, result);
    shader_set_mat3(default_shader_program, "view", result);
    shader_set_vec3(default_shader_program, "color", e.color);
    draw_quad(e.transform);
}

void render_triangle()
{
    if (triangle_VBO == 0)
    {
        initialize_triangle();
    }

    if (default_shader_program == 0)
    {
        initialize_default_shader_program();
    }

    glUseProgram(default_shader_program);
    glBindVertexArray(triangle_VAO);
    glDrawArrays(GL_TRIANGLES, 0, 3);
}
