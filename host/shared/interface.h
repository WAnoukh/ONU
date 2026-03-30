#ifndef INTERFACE_H
#define INTERFACE_H

#include "inputs.h"
#include "memory/arena.h"
#include "window_context.h"

typedef struct GLFWwindow GLFWwindow;

struct HostArenas
{
    struct Arena main_arena;
    struct Arena frame_arena;
    struct Arena level_arena;
};

struct HostContext
{
    struct HostArenas       arenas;
    struct InputInfo        inputinfo;
    struct WindowContext    window;
    GLFWwindow *            window_ctx; 
    void *                  root;
    float                   time;
    float                   delta_time;
};

typedef int (*HotInitFunction)(struct HostContext *);
typedef void (*HotDeinitFunction)(struct HostContext *);
typedef int (*HotUpdateFunction)(struct HostContext *);

struct HotFunctions
{
    HotInitFunction     init;
    HotDeinitFunction   deinit;
    HotUpdateFunction   update;
};

__declspec(dllexport) int init(struct HostContext *host);

__declspec(dllexport) void deinit(struct HostContext *host);

__declspec(dllexport) void update(struct HostContext *host);

#endif // INTERFACE_H
