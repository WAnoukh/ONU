#include <cglm/vec2.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "inputs.h"

struct InputInfo ii;

void i_initialize()
{
    ii = inputinfo_default();
}

struct InputInfo i_get_info()
{
    return ii;
}

void i_clear()
{
    for(int i = 0; i < KEY_COUNT; ++i )
    {
        ii.keys_pressed[i] = 0;
        ii.keys_released[i] = 0;
    }
    for(int i = 0; i < 5; ++i)
    {
        ii.mouse_buttons_released[i] = 0;
        ii.mouse_buttons_pressed[i] = 0;
    }
    ii.scroll_x = 0;
    ii.scroll_y = 0;
}

void register_key_state(int key, int action)
{
    if(action == GLFW_PRESS)
    {
        ii.keys_pressed[key] = 1;
        ii.keys_down[key] = 1;
    }
    else if(action == GLFW_RELEASE)
    {
        ii.keys_down[key] = 0;
        ii.keys_released[key] = 1;
    }
}

void register_scroll_state(float x, float y)
{
    ii.scroll_x += x;
    ii.scroll_y += y;
}

void register_mouse_move_state(double x, double y)
{
    ii.mouse_x = x;
    ii.mouse_y = y;
}

void register_mouse_state(int button, int action)
{
    if(action == GLFW_PRESS)
    {
        ii.mouse_buttons_pressed[button] = 1;
        ii.mouse_buttons_down[button] = 1;
    }
    else if(action == GLFW_RELEASE)
    {
        ii.mouse_buttons_released[button] = 1;
        ii.mouse_buttons_down[button] = 0;
    }
}
