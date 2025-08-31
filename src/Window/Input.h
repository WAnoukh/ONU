#ifndef INPUT_H
#define INPUT_H
#include <cglm/ivec2.h>

#include "window/window.h"

struct Game;

void i_process(GLFWwindow *window);

void i_initialize(GLFWwindow *in_window);

void register_key_state(int key, int action);

void register_mouse_state(int button, int action);

void register_scroll_state(double x_offset, double y_offset);

void register_mouse_move_state(double pos_x, double pos_y);

int i_key_pressed(int key);

int i_key_down(int key);

int i_button_down(int button);

int i_button_pressed(int button);

float i_get_scroll_y();

void i_get_mouse_move(float *x, float *y);

void i_get_mouse_pos(float *out_x, float *out_y);

void i_clear_pressed();

#endif //INPUT_H
