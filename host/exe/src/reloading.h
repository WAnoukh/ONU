#ifndef RELOADING_H
#define RELOADING_H

#include <minwindef.h>
#include "interface.h"

struct LoadedModule 
{
    struct HotFunctions fuctions;
    const char *path;
    const char *copy_path;
    HINSTANCE dll;
    FILETIME loaded_filetime;
};

struct LoadedModule hr_initialize(const char *path, const char *copy_path);

void hr_set_ctx(struct LoadedModule *new_ctx);

int hr_load();

int hr_newer_available();

#endif // RELOADING_H
