#ifndef EDITOR_H
#define EDITOR_H

typedef struct GLFWwindow GLFWwindow;
struct Game;

int editor_initialize();

void editor_new_frame();

void editor_render();

void editor_destroy();

void editor_update(struct Game *game);

#endif // EDITOR_H
