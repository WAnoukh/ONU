#ifndef EDITOR_CONTEXT_H
#define EDITOR_CONTEXT_H

#include "game.h"
#include "level.h"
#include "rendering/camera.h"

struct EditorCtx
{
    struct Game game;
    struct Level level;
    struct Camera camera;
    int is_playing;
};

struct EditorCtx ectx_default();

void ectx_start_level(struct EditorCtx *ectx, struct Level level);

#endif // EDITOR_CONTEXT_H
