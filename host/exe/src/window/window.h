#ifndef WINDOW_H
#define WINDOW_H

extern const int SCR_WIDTH;
extern const int SCR_HEIGHT;

typedef struct GLFWwindow GLFWwindow;

int initGl();

double get_time();

float window_get_screen_ratio();

void window_get_size(int *width, int *height);

int is_framebuffer_resized();

void clear_framebuffer_resized();

GLFWwindow *w_get_window_ctx();

int window_should_close();

#endif //WINDOW_H
