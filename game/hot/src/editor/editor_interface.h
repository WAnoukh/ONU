#ifndef EDITOR_INTERFACE_H
#define EDITOR_INTERFACE_H

#ifdef _WIN32
#define HOT_API __declspec(dllexport) 
#else
#define HOT_API __attribute__((visibility("default")))
#endif

struct EditorMemory;
struct WindowInfo;
struct InputInfo;
typedef struct GLFWwindow GLFWwindow;

HOT_API void editor_restart(struct EditorMemory *mem, GLFWwindow *window);

HOT_API void editor_start(struct EditorMemory *mem, GLFWwindow *window);

HOT_API void editor_cold_start(struct EditorMemory *mem, GLFWwindow *window);

HOT_API void editor_stop(struct EditorMemory *mem);

HOT_API int editor_update(struct EditorMemory *mem, struct WindowInfo windowinfo, struct InputInfo inputinfo);

#endif // EDITOR_INTERFACE_H
