#define EDITOR

#include "interface.h"
#include "window/input.h"
#include "window/window.h"

#include <stdio.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <windows.h>

typedef void (*editor_start_fn)(GLFWwindow *window);
typedef int (*editor_update_fn)(struct WindowInfo windowinfo, struct InputInfo inputinfo);
typedef void (*editor_stop_fn)(void);

struct WindowInfo window_info = {
    0,
    0,
    1,
    0
};

int main()
{
    if (!initGl())
    {
        printf("Failed to initialize OpenGL context\n");
        return 1;
    }
#ifdef EDITOR
    HMODULE dll = LoadLibraryA("libeditor_hot.dll");
    if (!dll) 
    {
        printf("Dll loading failed !\n");
        return -1;
    }

    editor_update_fn editor_update = (editor_update_fn)GetProcAddress(dll, "editor_update");
    if(!editor_update)
    {
        printf("Failed to get dll function: editor_update");
        return -1;
    }
    editor_start_fn editor_start = (editor_start_fn)GetProcAddress(dll, "editor_start");
    if(!editor_start)
    {
        printf("Failed to get dll function: editor_start");
        return -1;
    }
    editor_stop_fn editor_stop = (editor_stop_fn)GetProcAddress(dll, "editor_stop");
    if(!editor_stop)
    {
        printf("Failed to get dll function: editor_stop");
        return -1;
    }

    glfwMakeContextCurrent(NULL);
    editor_start(w_get_window_ctx());

    while (!glfwWindowShouldClose(w_get_window_ctx()))
    {
        if(is_framebuffer_resized())
        {
            window_get_size(&window_info.width, &window_info.height);
            window_info.is_framebuffer_resized = 1;
            window_info.ratio = (float)window_info.width/(float)window_info.height;
            clear_framebuffer_resized();
        }
        window_info.time = get_time();
        GLenum err;
        while ((err = glGetError()) != GL_NO_ERROR) {
            printf("OpenGL error: 0x%X\n", err);
        }

        if(!editor_update(window_info, i_get_info()))
        {
            glfwSetWindowShouldClose(w_get_window_ctx(), 1);
        }

        glfwSwapBuffers(w_get_window_ctx());
        i_clear_pressed();
        glfwPollEvents();
        window_info.is_framebuffer_resized = 0;
    }
    
    editor_stop();

    glfwTerminate();
#endif

    return 0;
}
