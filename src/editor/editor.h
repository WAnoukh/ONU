#ifndef EDITOR_H
#define EDITOR_H

typedef struct GLFWwindow GLFWwindow;
struct Game;
struct EditorCtx;

int editor_init(struct EditorCtx *ectx);

void editor_new_frame();

void editor_deinit(struct EditorCtx *ectx);

void editor_update(struct EditorCtx *ectx);

void editor_render(struct EditorCtx *ectx);

#endif // EDITOR_H
