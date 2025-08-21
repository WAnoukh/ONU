#include <cglm/vec2.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stdio.h>

#include "window/input.h"
#include "rendering/rendering.h"

#define PRESSED_KEYS_MAX 10

GLFWwindow *window;

int first_mouse_input = 1;
float last_mouse_x;
float last_mouse_y;
float mouse_delta_x;
float mouse_delta_y;

enum PlayerAction player_action_last = PA_NONE;
int player_action_changed = 0;

int down_keys[PRESSED_KEYS_MAX];
int down_keys_count = 0;

int pressed_keys[PRESSED_KEYS_MAX];
int pressed_keys_count = 0;

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

enum PlayerAction i_get_current_player_action()
{
    return player_action_last;
}

int i_player_action_just_changed()
{
    return player_action_changed;
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

int i_key_down(int key)
{
    return down_keys_contain(key);
}

int i_key_pressed(int key)
{
    return pressed_keys_contain(key);
}

int mouse_down(int button)
{
    return glfwGetMouseButton(window, button) == GLFW_PRESS;
}

void scroll(float x_offset, float y_offset)
{
    float zoom = get_camera_zoom();
    zoom += 0.2f * zoom * y_offset;
    set_camera_zoom(zoom);
}

int player_action_binds[] = 
{
    GLFW_KEY_UNKNOWN, GLFW_KEY_W, GLFW_KEY_S, GLFW_KEY_A, GLFW_KEY_D, GLFW_KEY_U, GLFW_KEY_UNKNOWN 
};

void process_player_action()
{
    for(int i = 0; i < PA_COUNT; ++i)
    {
        int player_action_key = player_action_binds[i];
        if (player_action_key == GLFW_KEY_UNKNOWN)
          continue;
        if(pressed_keys_contain(player_action_key))
        {
            player_action_changed = true;
            player_action_last = (enum PlayerAction)i;
            return;
        }
    }

    if(player_action_last == PA_NONE) return;

    if(!down_keys_contain(player_action_binds[player_action_last]))
    {
        player_action_last = PA_NONE;
    }
}

void i_process(GLFWwindow *window)
{
    process_mouse_move();
    if (i_key_down(GLFW_KEY_ESCAPE))
        glfwSetWindowShouldClose(window, 1);
    if (mouse_down(GLFW_MOUSE_BUTTON_1))
    {
        if (fabsf(mouse_delta_x) > 0.00001f || fabsf(mouse_delta_y) > 0.00001f)
        {
            int width, height;
            glfwGetFramebufferSize(window, &width, &height);
            pan_camera(mouse_delta_x/(float)width, -mouse_delta_y/(float)height);
        }
    }

    process_player_action();
}

void i_clear_pressed()
{
    player_action_changed = false;
    pressed_keys_clear();
}
