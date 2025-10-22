#ifndef EDITOR_CONTEXT_H
#define EDITOR_CONTEXT_H

#include "game.h"
#include "interface.h"
#include "level.h"
#include "rendering/camera.h"

#define CIMGUI_DEFINE_ENUMS_AND_STRUCTS
#define CIMGUI_USE_OPENGL3
#define CIMGUI_USE_GLFW
#include "cimgui.h"
#include "cimgui_impl.h"

struct ImGuiContext;

#define selection_ents_max 30

struct EditorCtx
{
    struct WindowInfo window_info;
    struct ImGuiContext* ctx;

    struct Game game;
    struct Level level;
    struct Camera camera;
    int is_playing;

    ImVec2 mouse_pos;
    ImVec2 mouse_offset;
    vec2 mouse_world_pos;

    ivec2 popup_last_clicked_pos;
    _Bool floating_editor_show;

    struct Entity *reposition_entity;
    int reposition_selection;
    ivec2 reposition_selection_delta;

    enum EntityType creation_type;
    ivec2 creation_position;
    struct KeyBlockData creation_key;
    enum ActionType creation_action;
    int creation_action_target;

    int deletion_index;
    int deletion_selection;

    int layer_selected;

    int selection_started;
    vec2 selection_pos_start;
    int selection_ents[selection_ents_max];
    int selection_ents_count;

    struct Entity *edition_entity;


    char file_current[100];
    int saving;
    int opening;
    int opening_failed;

    char sequence_current[100];
    int sequence_opening;
    int sequence_opening_failed;

    int tile_index;
    struct ImVec2i tile_position;

    float ui_scale;

    int level_menu_opened;
    ivec2 level_temp_size;
    int level_temp_size_changed;
    ivec2 level_temp_shift;
    int level_temp_shift_changed;
};

struct EditorCtx ectx_default();

void ectx_start_level(struct EditorCtx *ectx, struct Level level);

#endif // EDITOR_CONTEXT_H
