#include <stdio.h>
#include <stdlib.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "Window/Input.h"
#include "Window/Window.h"
#include "Entity.h"
#include "Rendering/Rendering.h"

struct entity *entities;
int entity_count;

double last_time;
double new_time;
float delta_time;

void initialize_entities()
{
    entity_count = 2;
    entities = (struct entity*)calloc(entity_count, sizeof(struct entity));
    struct entity *player = entities;
    player->type = Player;
    player->color[0] = 1;
    struct entity *guard = entities+1;
    struct entity ent = *guard;
    guard->type = Guard;
    guard->color[1] = 1;
}

void update_entities()
{
    entities[0].pos[0] = sinf((float)new_time);
}

int main()
{
    GLFWwindow* window;
    if (!initGl(&window) || window == NULL)
    {
        perror("Failed to initialize OpenGL context\n");
        return 1;
    }

    initialize_renderer();
    last_time = glfwGetTime();

    initialize_entities();

    int acc = 0;

    while (!glfwWindowShouldClose(window))
    {
        processInput(window);
        new_time = glfwGetTime();
        delta_time = (float)(new_time - last_time);
        last_time = new_time;

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        update_entities();
        //render_triangle();
        render_entities(entities, entity_count);

        acc++;
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}