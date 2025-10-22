#include <cglm/vec2.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "input_info.h"

struct InputInfo ii;

void i_initialize()
{
    ii = inputinfo_default();
}

struct InputInfo i_get_info()
{
    return ii;
}

void i_clear_pressed()
{
    for(int i = 0; i < KEY_COUNT; ++i )
    {
        ii.keys_pressed[i] = 0;
        ii.keys_released[i] = 0;
    }
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
{}

void register_mouse_move_state(double x, double y)
{}

void register_mouse_state(int button, int action)
{}
