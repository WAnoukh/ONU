#include <stdio.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "entity.h"
#include "tilemap.h"
#include "window/input.h"
#include "window/window.h"
#include "rendering/rendering.h"

double last_time;
double new_time;
float delta_time;

int main()
{
    GLFWwindow* window;
    if (!initGl(&window) || window == NULL)
    {
        perror("Failed to initialize OpenGL context\n");
        return 1;
    }

    initialize_renderer();
    initialize_entities();
    initialize_input(window);

    last_time = glfwGetTime();

    while (!glfwWindowShouldClose(window))
    {
        process_input(window);
        new_time = get_time();
        delta_time = (float)(new_time - last_time);
        last_time = new_time;

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        update_entities(delta_time);
        render_main_tilemap();
        render_entities();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
