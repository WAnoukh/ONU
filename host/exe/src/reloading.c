#include "reloading.h"
#include "interface.h"
#include <stdio.h>
#include <windows.h>

struct LoadedModule *ctx;

struct LoadedModule hr_initialize(const char *path, const char *copy_path)
{
    struct LoadedModule ctx = (struct LoadedModule)
    {
        {}, 
        path,
        copy_path,
        NULL,
    };
    return ctx;
}

void hr_set_ctx(struct LoadedModule *new_ctx)
{
    ctx = new_ctx;
}

int file_get_write_time(const char *path, FILETIME *out)
{
    WIN32_FILE_ATTRIBUTE_DATA data;
    if (!GetFileAttributesExA(path, GetFileExInfoStandard, &data))
        return 0;

    *out = data.ftLastWriteTime;
    return 1;
}

int load_internal()
{
    if(ctx->dll)
    {
        FreeLibrary(ctx->dll);
    }

    if(!CopyFile(ctx->path, ctx->copy_path, 0))
    {
        printf("Error: %lu\n", GetLastError());
        return 0;
    }

    ctx->dll = LoadLibrary(ctx->copy_path);
    if(!ctx->dll)
    {
        printf("Error: dll load failed!\n");
        return 0;
    }

    ctx->fuctions.init = (HotInitFunction)GetProcAddress(ctx->dll, "init");
    if(!ctx->fuctions.init)
    {
        printf("Error: init function not found!\n");
        return 0;
    }

    ctx->fuctions.deinit = (HotDeinitFunction)GetProcAddress(ctx->dll, "deinit");
    if(!ctx->fuctions.deinit)
    {
        printf("Error: restart function not found!\n");
        return 0;
    }

    ctx->fuctions.update = (HotUpdateFunction)GetProcAddress(ctx->dll, "update");
    if(!ctx->fuctions.update)
    {
        printf("Error: update function not found!\n");
        return 0;
    }
    return 1;
}

int hr_load()
{
    file_get_write_time(ctx->path, &ctx->loaded_filetime);
    return load_internal();
}

int hr_newer_available()
{
    FILETIME current_filetime; 
    file_get_write_time(ctx->path, &current_filetime);
    return CompareFileTime(&current_filetime, &ctx->loaded_filetime) != 0;
}
