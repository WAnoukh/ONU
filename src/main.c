#include <stdio.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "Window/Input.h"
#include "Window/Window.h"

int main()
{
    GLFWwindow* window;
    if (!initGl(&window) || window == NULL)
    {
        perror("Failed to initialize OpenGL context");
        return 1;
    }

    while (!glfwWindowShouldClose(window))
    {
        processInput(window);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}