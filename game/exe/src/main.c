#include "reloading.h"
#define EDITOR

#include "interface.h"
#include "window/input.h"
#include "window/window.h"

#include <stdio.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>


struct WindowInfo window_info = {
    0,
    0,
    1,
    0
};

int dllversion = 0;

int main()
{
    if (!initGl())
    {
        printf("Failed to initialize OpenGL context\n");
        return 1;
    }

    FILETIME write_time;

#ifdef EDITOR
    struct HotDll hot;
    char *dll_path = "build/bin/libeditor_hot.dll";
    char *dll_temp_path = "build/bin/libeditor_hot_temp.dll";
    file_get_write_time(dll_path, &write_time);
    if(!load_dll_as_temp(&hot, dll_path, dll_temp_path))
    {
        printf("Initial dll loading failed.\n");
        return -1;
    }

    glfwMakeContextCurrent(NULL);

    struct EditorMemory mem;
    mem.level = arena_init(64 * 1024 * 1024);
    mem.frame = arena_init(32 * 1024 * 1024);
    mem.editor = arena_init(32 * 1024 * 1024);


    hot.editor_start(&mem, w_get_window_ctx());

    while (!glfwWindowShouldClose(w_get_window_ctx()))
    {
        FILETIME new_file_time;
        if(!hot.dll || (file_get_write_time(dll_path, &new_file_time) && file_time_changed(&write_time, &new_file_time)))
        {
            printf("new dll ! trying to load it !");
            if(hot.dll)
            {
                hot.editor_stop(&mem);
            }

            file_get_write_time(dll_path, &write_time);
            if(!load_dll_as_temp(&hot, dll_path, dll_temp_path))
            {
                printf("hot dll loading failed.\n");
                continue;
            }
            else
            {
                write_time = new_file_time;
                printf("dll changed ! v%d\n",dllversion);
                hot.editor_restart(&mem, w_get_window_ctx());
                dllversion++;
            }
        }
        
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

        if(!hot.editor_update(&mem, window_info, i_get_info()))
        {
            glfwSetWindowShouldClose(w_get_window_ctx(), 1);
        }

        glfwSwapBuffers(w_get_window_ctx());
        i_clear_pressed();
        glfwPollEvents();
        window_info.is_framebuffer_resized = 0;
    }
    
    hot.editor_stop(&mem);

    glfwTerminate();
#endif

    return 0;
}
