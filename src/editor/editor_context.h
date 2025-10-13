#ifndef EDITOR_CONTEXT_H
#define EDITOR_CONTEXT_H

#include "level.h"
#include "rendering/camera.h"

struct EditorCtx
{
    struct Level level;
    struct Camera camera;
};

struct EditorCtx ectx_default();

#endif // EDITOR_CONTEXT_H
