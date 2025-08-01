#include "Window/Input.h"

#include <stdio.h>
#include <cglm/vec2.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "Rendering/Rendering.h"

GLFWwindow *window;

void processInput(GLFWwindow *window)
{
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, 1);
}

void initialize_input(GLFWwindow *in_window)
{
    window = in_window;
}

int key_pressed(int key)
{
    return glfwGetKey(window, key) == GLFW_PRESS;
}

void get_player_input(vec2 out_input)
{
    out_input[0] = (float)(key_pressed(GLFW_KEY_D) - key_pressed(GLFW_KEY_A));
    out_input[1] = (float)(key_pressed(GLFW_KEY_W) - key_pressed(GLFW_KEY_S));
    glm_vec2_normalize(out_input);
}

void scroll(float x_offset, float y_offset)
{
    float zoom = get_camera_zoom();
    zoom += 0.1f * zoom * y_offset;
    set_camera_zoom(zoom);
}
