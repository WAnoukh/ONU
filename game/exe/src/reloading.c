#include "reloading.h"
#include <stdio.h>


int load_dll(struct HotDll *hot, char *path)
{
    hot->dll = LoadLibraryA(path);
    if (!hot->dll) 
    {
        printf("Dll loading failed !\n");
        return 0;
    }

    hot->editor_update = (editor_update_fn)GetProcAddress(hot->dll, "editor_update");
    if(!hot->editor_update)
    {
        printf("Failed to get dll function: editor_update");
        return 0;
    }
    hot->editor_start = (editor_start_fn)GetProcAddress(hot->dll, "editor_start");
    if(!hot->editor_start)
    {
        printf("Failed to get dll function: editor_start");
        return 0;
    }
    hot->editor_restart = (editor_start_fn)GetProcAddress(hot->dll, "editor_restart");
    if(!hot->editor_restart)
    {
        printf("Failed to get dll function: editor_restart");
        return 0;
    }
    hot->editor_stop = (editor_stop_fn)GetProcAddress(hot->dll, "editor_stop");
    if(!hot->editor_stop)
    {
        printf("Failed to get dll function: editor_stop");
        return 0;
    }
    return 1;
}

int load_dll_as_temp(struct HotDll *hot, char *dll_path, char *dll_temp_path)
{
    if(hot->dll)
    {
        FreeLibrary(hot->dll);
        hot->dll = NULL;
    }

    CopyFileA(dll_path, dll_temp_path, FALSE);
    if(!load_dll(hot, dll_temp_path))
    {
        return 0;
    }
    return 1;
}

int file_get_write_time(const char *path, FILETIME *out)
{
    WIN32_FILE_ATTRIBUTE_DATA data;
    if (!GetFileAttributesExA(path, GetFileExInfoStandard, &data))
        return 0;

    *out = data.ftLastWriteTime;
    return 1;
}

int file_time_changed(const FILETIME *a, const FILETIME *b)
{
    return CompareFileTime(a, b) != 0;
}

