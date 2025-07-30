#ifndef INPUT_H
#define INPUT_H
#include <cglm/ivec2.h>

#include "Window/Window.h"

void processInput(GLFWwindow *window);

void initialize_input(GLFWwindow *in_window);

int key_pressed(int key);

void get_player_input(vec2 out_input);
#endif //INPUT_H
