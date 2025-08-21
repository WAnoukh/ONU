#ifndef INPUT_H
#define INPUT_H
#include <cglm/ivec2.h>

#include "window/window.h"

enum PlayerAction
{
    PA_NONE,
    PA_UP,
    PA_DOWN,
    PA_LEFT,
    PA_RIGHT,
    PA_UNDO,
    PA_COUNT,
};

void i_process(GLFWwindow *window);

void i_initialize(GLFWwindow *in_window);

int i_key_pressed(int key);

int i_key_down(int key);

void register_key_state(int key, int action);

enum PlayerAction get_player_input();

void scroll(float x_offset, float y_offset);

void mouse_move(float x, float y);

void i_clear_pressed();

enum PlayerAction i_get_current_player_action();

int i_player_action_just_changed();

#endif //INPUT_H
