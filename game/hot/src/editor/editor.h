#ifndef EDITOR_H
#define EDITOR_H

#ifdef _WIN32
#define HOT_API __declspec(dllexport) 
#else
#define HOT_API __attribute__((visibility("default")))
#endif

typedef struct GLFWwindow GLFWwindow;
struct Game;
struct EditorCtx;
struct WindowInfo;
struct InputInfo;

void editor_new_frame();

void editor_deinit(struct EditorCtx *ectx);

void editor_render(struct EditorCtx *ectx);

float editor_get_window_ratio(struct EditorCtx *ectx);

void editor_get_window_size(struct EditorCtx *ectx, int *w, int *h);

HOT_API void editor_start(GLFWwindow *window);

HOT_API void editor_stop();

HOT_API int editor_update(struct WindowInfo windowinfo, struct InputInfo inputinfo);

#endif // EDITOR_H
