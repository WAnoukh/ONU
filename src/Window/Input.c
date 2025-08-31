#include <cglm/vec2.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "window/input.h"
#include "rendering/rendering.h"

#define PRESSED_KEYS_MAX 10
#define PRESSED_BUTTONS_MAX 3

GLFWwindow *window;

int first_mouse_input = 1;
float last_mouse_x;
float last_mouse_y;
float mouse_delta_x;
float mouse_delta_y;

int down_keys[PRESSED_KEYS_MAX];
int down_keys_count = 0;

int pressed_keys[PRESSED_KEYS_MAX];
int pressed_keys_count = 0;

int down_buttons[PRESSED_BUTTONS_MAX];
int down_buttons_count = 0;

int pressed_buttons[PRESSED_BUTTONS_MAX];
int pressed_buttons_count = 0;

//KEYBOARD KEY HANDLING
void down_keys_add(int key)
{
   if(down_keys_count >= PRESSED_KEYS_MAX) return;
   down_keys[down_keys_count++] = key; 
}
 
void down_keys_remove(int key)
{
    int i = 0;
    while(i < down_keys_count && down_keys[i] != key) { ++i; }
    if(i >= down_keys_count) return;
    down_keys[i] = down_keys[--down_keys_count];
}

int down_keys_contain(int key)
{
    int i = 0;
    while(i < down_keys_count && down_keys[i] != key) { ++i; }
    return i < down_keys_count;
}

void pressed_keys_add(int key)
{
    if(pressed_keys_count >= PRESSED_KEYS_MAX) return;
    pressed_keys[pressed_keys_count++] = key;
}

void pressed_keys_clear()
{
    pressed_keys_count = 0;
}

int pressed_keys_contain(int key)
{
    int i = 0;
    while(i < pressed_keys_count && pressed_keys[i] != key) { ++i; }
    return i < pressed_keys_count;
}

void register_key_state(int key, int action)
{
    if(action == GLFW_PRESS)
    {
        if(!down_keys_contain(key))
        {
            pressed_keys_add(key);
            down_keys_add(key);
        }
    }
    else if(action == GLFW_RELEASE)
    {
        down_keys_remove(key);
    }
}

int i_key_down(int key)
{
    return down_keys_contain(key);
}

int i_key_pressed(int key)
{
    return pressed_keys_contain(key);
}

//MOUSE BUTTON HANDLING
void down_buttons_add(int button)
{
   if(down_buttons_count >= PRESSED_BUTTONS_MAX) return;
   down_buttons[down_buttons_count++] = button; 
}
 
void down_buttons_remove(int button)
{
    int i = 0;
    while(i < down_buttons_count && down_buttons[i] != button) { ++i; }
    if(i >= down_buttons_count) return;
    down_buttons[i] = down_buttons[--down_buttons_count];
}

int down_buttons_contain(int button)
{
    int i = 0;
    while(i < down_buttons_count && down_buttons[i] != button) { ++i; }
    return i < down_buttons_count;
}

void pressed_buttons_add(int button)
{
    if(pressed_buttons_count >= PRESSED_BUTTONS_MAX) return;
    pressed_buttons[pressed_buttons_count++] = button;
}

void pressed_buttons_clear()
{
    pressed_buttons_count = 0;
}

int pressed_buttons_contain(int button)
{
    int i = 0;
    while(i < pressed_buttons_count && pressed_buttons[i] != button) { ++i; }
    return i < pressed_buttons_count;
}

void register_mouse_state(int button, int action)
{
    if(action == GLFW_PRESS)
    {
        if(!down_buttons_contain(button))
        {
            pressed_buttons_add(button);
            down_buttons_add(button);
        }
    }
    else if(action == GLFW_RELEASE)
    {
        down_buttons_remove(button);
    }
}

int i_button_down(int button)
{
    return down_buttons_contain(button);
}

int i_button_pressed(int button)
{
    return pressed_buttons_contain(button);
}

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

void i_initialize(GLFWwindow *in_window)
{
    window = in_window;
}

void scroll(float x_offset, float y_offset)
{
    float zoom = get_camera_zoom();
    zoom += 0.2f * zoom * y_offset;
    set_camera_zoom(zoom);
}

void i_process(GLFWwindow *window)
{
    process_mouse_move();
    if (i_key_down(GLFW_KEY_ESCAPE))
        glfwSetWindowShouldClose(window, 1);
    if (i_button_down(GLFW_MOUSE_BUTTON_1))
    {
        if (fabsf(mouse_delta_x) > 0.00001f || fabsf(mouse_delta_y) > 0.00001f)
        {
            int width, height;
            glfwGetFramebufferSize(window, &width, &height);
            pan_camera(mouse_delta_x/(float)width, -mouse_delta_y/(float)height);
        }
    }
}

void i_clear_pressed()
{
    pressed_keys_clear();
    pressed_buttons_clear();
}
