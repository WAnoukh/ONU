#ifndef WINDOW_H
#define WINDOW_H

extern const int SCR_WIDTH;
extern const int SCR_HEIGHT;

typedef struct GLFWwindow GLFWwindow;

int initGl(GLFWwindow **window);

float window_get_screen_ratio();

#endif //WINDOW_H
