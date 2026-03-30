#ifndef EDITOR_H
#define EDITOR_H

typedef struct GLFWwindow GLFWwindow;
struct Game;
struct EditorCtx;
struct InputInfo;
struct FrameContext;
struct EditorMemory;

void editor_new_frame();

void editor_imgui_init(struct EditorCtx *ectx, GLFWwindow *window);

void editor_init(struct EditorCtx *ectx, GLFWwindow *window);

void editor_deinit(struct EditorCtx *ectx);

void editor_render(struct EditorCtx *ectx);

int editor_update_internal(struct EditorMemory *mem, struct EditorCtx *ectx, struct FrameContext *frame);

float editor_get_window_ratio(struct FrameContext *frame);

void editor_get_window_size(struct FrameContext *frame, int *w, int *h);

void editor_load_level_from_file(struct EditorCtx *ectx, const char *path);

#endif // EDITOR_H
