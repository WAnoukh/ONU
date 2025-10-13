#ifndef EDITOR_H
#define EDITOR_H

typedef struct GLFWwindow GLFWwindow;
struct Game;
struct EditorCtx;

int editor_initialize();

void editor_new_frame();

void editor_destroy();

void editor_update(struct EditorCtx *ectx);

void editor_render(struct EditorCtx *ectx);

#endif // EDITOR_H
