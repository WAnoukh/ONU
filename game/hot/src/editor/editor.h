#ifndef EDITOR_H
#define EDITOR_H

typedef struct GLFWwindow GLFWwindow;
struct Game;
struct EditorCtx;
struct WindowInfo;
struct InputInfo;
struct EditorMemory;

void editor_new_frame();

int editor_init(struct EditorCtx *ectx, GLFWwindow *window);

void editor_deinit(struct EditorCtx *ectx);

void editor_render(struct EditorCtx *ectx);

int editor_update_internal(struct EditorMemory *mem, struct EditorCtx *ectx, struct InputInfo inputinfo);

float editor_get_window_ratio(struct EditorCtx *ectx);

void editor_get_window_size(struct EditorCtx *ectx, int *w, int *h);

#endif // EDITOR_H
