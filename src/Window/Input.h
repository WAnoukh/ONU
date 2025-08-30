#ifndef INPUT_H
#define INPUT_H
#include <cglm/ivec2.h>

#include "window/window.h"

void i_process(GLFWwindow *window);

void i_initialize(GLFWwindow *in_window);

int i_key_pressed(int key);

int i_key_down(int key);

void register_key_state(int key, int action);

void scroll(float x_offset, float y_offset);

void mouse_move(float x, float y);

void i_clear_pressed();

#endif //INPUT_H
