#include "Rendering/Rendering.h"

#include "Entity.h"
#include "glad/glad.h"

void render_entities(struct entity *entities, int entities_number)
{
    for (int i = 0; i < entities_number; i++)
    {
        //TODO
    }
}

void render_triangle()
{
    float vertices[] = {
        -0.5f, -0.5f, 0.0f,
         0.5f, -0.5f, 0.0f,
         0.0f,  0.5f, 0.0f
    };
    unsigned int VBO;
    glGenBuffers(1, &VBO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
}