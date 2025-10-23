#ifndef RELOADING_H
#define RELOADING_H

#include <windows.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "interface.h"

typedef int (*editor_update_fn)(struct EditorMemory *mem, struct WindowInfo windowinfo, struct InputInfo inputinfo);
typedef void (*editor_start_fn)(struct EditorMemory *mem, GLFWwindow *window);
typedef void (*editor_restart_fn)(struct EditorMemory *mem, GLFWwindow *window);
typedef void (*editor_stop_fn)(struct EditorMemory *mem);

struct HotDll
{
    HMODULE dll;
    editor_update_fn editor_update;
    editor_start_fn editor_start;
    editor_start_fn editor_restart;
    editor_stop_fn editor_stop;
};

int load_dll(struct HotDll *hot, char *path);

int load_dll_as_temp(struct HotDll *hot, char *dll_path, char *dll_temp_path);

int file_get_write_time(const char *path, FILETIME *out);

int file_time_changed(const FILETIME *a, const FILETIME *b);

#endif // RELOADING_H
