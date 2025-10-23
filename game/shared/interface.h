#ifndef INTERFACE_H
#define INTERFACE_H

#include "memory/arena.h"
#define KEY_COUNT 256

struct EditorCtx;
typedef struct GLFWwindow GLFWwindow;
typedef struct ImGuiContext ImGuiContext;

struct WindowInfo
{
    int width;
    int height;
    float ratio;
    int is_framebuffer_resized; 
    double time;
};

struct InputInfo
{
    float delta_time;
    int keys_pressed[KEY_COUNT];
    int keys_released[KEY_COUNT];
    int keys_down[KEY_COUNT];
};

struct EditorMemory
{
    struct Arena level;
    struct Arena frame;
    struct Arena editor;
};

#endif // INTERFACE_H
