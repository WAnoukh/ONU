#include "Window/Window.h"
#include <stdio.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "Rendering/Rendering.h"

const int SCR_WIDTH = 800;
const int SCR_HEIGHT = 600;

float framebuffer_ratio = (float)SCR_WIDTH/(float)SCR_HEIGHT;

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
    framebuffer_ratio = (float)width/(float)height;
    initialize_camera();
}

int initGl(GLFWwindow **window)
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);


    *window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (*window == NULL)
    {
        perror("Failed to create GLFW window\n");
        glfwTerminate();
        return 0;
    }
    glfwMakeContextCurrent(*window);
    glfwSetFramebufferSizeCallback(*window, framebuffer_size_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        perror("Failed to initialize GLAD\n");
        return 0;
    }
    return 1;
}

float window_get_screen_ratio()
{
    return framebuffer_ratio;
}
