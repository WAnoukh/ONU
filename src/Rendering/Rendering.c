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

void render_entities(struct entity *entities, int entities_number)
{
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

void initialize_default_shader_program()
{
    default_shader_program = create_shader_program("../shader/generic/default.vert", "../shader/generic/default.frag");
    if (!default_shader_program)
    {
        exit(1);
    }
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