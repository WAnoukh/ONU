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

int main()
{
    GLFWwindow* window;
    if (!initGl(&window) || window == NULL)
    {
        perror("Failed to initialize OpenGL context\n");
        return 1;
    }

    initialize_renderer();

    entity_count = 2;
    entities = (struct entity*)calloc(entity_count, sizeof(struct entity));
    struct entity *player = entities;
    player->type = Player;
    struct entity *guard = entities+1;
    struct entity ent = *guard;
    guard->type = Guard;
    guard->y = 10;

    while (!glfwWindowShouldClose(window))
    {
        processInput(window);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        //render_triangle();
        render_entities(entities, entity_count);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}