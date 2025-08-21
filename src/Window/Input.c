#include <cglm/vec2.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "window/input.h"
#include "rendering/rendering.h"

GLFWwindow *window;

int first_mouse_input = 1;
float last_mouse_x;
float last_mouse_y;
float mouse_delta_x;
float mouse_delta_y;

void process_mouse_move()
{
    double dx, dy;
    glfwGetCursorPos(window,&dx, &dy);
    float x = (float)dx, y = (float)dy;
    if (first_mouse_input)
    {
        last_mouse_x = x;
        last_mouse_y = y;
        first_mouse_input = 0;
        return;
    }
    mouse_delta_x = x - last_mouse_x;
    mouse_delta_y = y - last_mouse_y;

    last_mouse_x = x;
    last_mouse_y = y;
}

void process_input(GLFWwindow *window)
{
    process_mouse_move();
    if (key_pressed(GLFW_KEY_ESCAPE))
        glfwSetWindowShouldClose(window, 1);
    if (mouse_pressed(GLFW_MOUSE_BUTTON_1))
    {
        if (fabsf(mouse_delta_x) > 0.00001f || fabsf(mouse_delta_y) > 0.00001f)
        {
            int width, height;
            glfwGetFramebufferSize(window, &width, &height);
            pan_camera(mouse_delta_x/(float)width, -mouse_delta_y/(float)height);
        }
    }
}

void initialize_input(GLFWwindow *in_window)
{
    window = in_window;
}

int key_pressed(int key)
{
    return glfwGetKey(window, key) == GLFW_PRESS;
}

int mouse_pressed(int button)
{
    return glfwGetMouseButton(window, button) == GLFW_PRESS;
}

void get_player_input(vec2 out_input)
{
    out_input[0] = (float) (key_pressed(GLFW_KEY_D) - key_pressed(GLFW_KEY_A));
    out_input[1] = (float) (key_pressed(GLFW_KEY_W) - key_pressed(GLFW_KEY_S));
    glm_vec2_normalize(out_input);
}

void scroll(float x_offset, float y_offset)
{
    float zoom = get_camera_zoom();
    zoom += 0.2f * zoom * y_offset;
    set_camera_zoom(zoom);
}
