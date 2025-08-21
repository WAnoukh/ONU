#include "window/window.h"
#include <stdio.h>
#include <glad/glad.h>
#include <glfw/glfw3.h>
#include "rendering/rendering.h"
#include "window/input.h"

const int SCR_WIDTH = 800;
const int SCR_HEIGHT = 600;

float framebuffer_ratio = (float)SCR_WIDTH/(float)SCR_HEIGHT;

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
    framebuffer_ratio = (float)width/(float)height;
    compute_camera_view();
}

void scroll_callback(GLFWwindow* window, double x_offset, double y_offset)
{
    scroll((float)x_offset, (float)y_offset);
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
    glfwSetScrollCallback(*window, scroll_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        perror("Failed to initialize GLAD\n");
        return 0;
    }
    return 1;
}

double get_time()
{
   return glfwGetTime();
}

float window_get_screen_ratio()
{
    return framebuffer_ratio;
}
