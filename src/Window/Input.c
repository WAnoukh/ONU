#include <cglm/vec2.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "window/input.h"

#define PRESSED_KEYS_MAX 10
#define PRESSED_BUTTONS_MAX 3

GLFWwindow *window;

float mouse_first_move = 1;
float mouse_last_x, mouse_last_y;
float mouse_offset_x=0, mouse_offset_y=0;
float scroll_x, scroll_y;

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

//Scroll Handling
void register_scroll_state(double x_offset, double y_offset)
{
    scroll_x += (float)x_offset;
    scroll_y += (float)y_offset;
}

void scroll_clear()
{
    scroll_x = 0;
    scroll_y = 0;
}

float i_get_scroll_y()
{
    return scroll_y;
}

//Mouse move Handling
void register_mouse_move_state(double pos_x, double pos_y)
{
    float fpos_x = (float)pos_x, fpos_y = (float)pos_y;
    if(mouse_first_move)
    {
        mouse_first_move = 0;
        mouse_last_x = fpos_x;
        mouse_last_y = fpos_y;
        return;
    }
    mouse_offset_x += fpos_x - mouse_last_x;
    mouse_offset_y += fpos_y - mouse_last_y;
    mouse_last_x = fpos_x;
    mouse_last_y = fpos_y;
}

void mouse_move_clear()
{
    mouse_offset_x = 0;
    mouse_offset_y = 0;
}

void i_get_mouse_move(float *x, float *y)
{
    *x = mouse_offset_x;
    *y = mouse_offset_y;
}

void i_initialize(GLFWwindow *in_window)
{
    window = in_window;
}

void i_get_mouse_pos(float *out_x, float *out_y)
{
    double x_pos, y_pos;
    glfwGetCursorPos(window, &x_pos, &y_pos);
    *out_x = (float)x_pos;
    *out_y = (float)y_pos;
}

void i_process(GLFWwindow *window)
{
    if (i_key_down(GLFW_KEY_ESCAPE))
        glfwSetWindowShouldClose(window, 1);
}

void i_clear_pressed()
{
    pressed_keys_clear();
    pressed_buttons_clear();
    scroll_clear();
    mouse_move_clear();
}
