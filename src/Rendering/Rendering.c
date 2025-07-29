#include "Rendering/Rendering.h"

#include <stdlib.h>

#include "Entity.h"
#include "glad/glad.h"
#include "Rendering/Shader.h"

GLuint default_shader_program;

unsigned int triangle_VBO = 0;
unsigned int triangle_VAO = 0;

float triangle_vertices[] = {
    -0.5f, -0.5f, 0.0f,
     0.5f, -0.5f, 0.0f,
     0.0f,  0.5f, 0.0f
};

float quad_vertices[] = {
    0.5f,  0.5f, 0.0f,  // top right
    0.5f, -0.5f, 0.0f,  // bottom right
   -0.5f, -0.5f, 0.0f,  // bottom left
   -0.5f,  0.5f, 0.0f   // top left
};

unsigned int quad_indices[] = {  // note that we start from 0!
    0, 1, 3,   // first triangle
    1, 2, 3    // second triangle
};

GLuint quad_EBO = 0, quad_VBO = 0, quad_VAO = 0;

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

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
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
    if (default_shader_program == 0)
    {
        initialize_default_shader_program();
    }

    glUseProgram(default_shader_program);
    glBindVertexArray(quad_VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

void render_entities(struct entity *entities, int entities_number)
{
    draw_quad();
    for (int i = 0; i < entities_number; i++)
    {
        //TODO
    }
}

void initialize_triangle_VBO()
{
    glGenVertexArrays(1, &triangle_VAO);
    glGenBuffers(1, &triangle_VBO);

    glBindVertexArray(triangle_VAO);

    glBindBuffer(GL_ARRAY_BUFFER, triangle_VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(triangle_vertices), triangle_vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindVertexArray(0);
}

void render_triangle()
{
    if (triangle_VBO == 0)
    {
        initialize_triangle_VBO();
    }

    if (default_shader_program == 0)
    {
        initialize_default_shader_program();
    }

    glUseProgram(default_shader_program);
    glBindVertexArray(triangle_VAO);
    glDrawArrays(GL_TRIANGLES, 0, 3);
}