#include "rendering/rendering.h"
#include <string.h>

#ifdef _WIN32
    #include <io.h>
    #define my_itoa _itoa
#else
    #include <stdio.h>
    #include <unistd.h>
    char* my_itoa(int value, char* str, int base) {
        if (base != 10) return NULL; // simple example, only base 10 supported
        sprintf(str, "%d", value);
        return str;
    }
#endif
#include <stdio.h>
#include <dirent.h>

#include "editor.h"
#include "editor_context.h"
#include "interface.h"
#include <glad/glad.h>
#include "GLFW/glfw3.h"
#include "cglm/types.h"
#include "cglm/vec2.h"
#include "level.h"
#include "serialization.h"
#include "texture.h"
#include "transform.h"
#define CIMGUI_DEFINE_ENUMS_AND_STRUCTS
#define CIMGUI_USE_OPENGL3
#define CIMGUI_USE_GLFW
#include "cimgui.h"
#include "cimgui_impl.h"

#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))

const char *window_create = "Create entity";
const char *window_move = "Move entity";
const char *window_move_selection = "Move selection";
const char *window_deletion = "Comfirm deletion";
const char *window_deletion_selection = "Comfirm selection deletion";
const char *window_edition = "Edit";
const char *window_saving = "Save";
const char *window_opening = "Opening";
const char *window_sequence_opening = "Sequence Opening";
const char *floating_editor = "World Editor";

char resources_path[] = "resources/level/";
char file_suffix[] = ".level";
char sequence_suffix[] = ".seq";

void editor_imgui_init(struct EditorCtx *ectx, GLFWwindow *window)
{
    glfwMakeContextCurrent(window);
    ectx->ctx = igCreateContext(NULL);
    if(ectx->ctx)
    {
        printf("ImGui context created !\n");
    }
    igSetCurrentContext(ectx->ctx);
    ImGui_ImplGlfw_InitForOpenGL(glfwGetCurrentContext(), true);

    ImGui_ImplOpenGL3_Init("#version 130");
}

void editor_init(struct EditorCtx *ectx, GLFWwindow *window)
{
    //ImGuiIO* io = igGetIO_ContextPtr(ctx);
    ImGuiStyle* style = igGetStyle();

    ImGuiStyle_ScaleAllSizes(style, ectx->ui_scale);
    style->FontScaleMain *= ectx->ui_scale;

    igGetMousePos(&ectx->mouse_pos);

    initialize_renderer();
    ectx->is_playing = 0;
    ectx->game = game_init();
}

void editor_new_frame()
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    igNewFrame();
}

void editor_render(struct EditorCtx *ectx)
{
    igRender();
    ImGui_ImplOpenGL3_RenderDrawData(igGetDrawData());
}

void editor_deinit(struct EditorCtx *ectx)
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    igDestroyContext(ectx->ctx);
}

void edit_entity_key(struct KeyBlockData *key)
{
    char letter[] = {(char)key->key, '\0'};
    igText("Key binding:");
    if(igInputText("##keybinding", letter, 2, ImGuiInputTextFlags_CharsUppercase, NULL, NULL))
    {
        if(letter[0] >= 'a' && letter[0] <= 'z')
        {
            letter[0] = (char)(letter[0] + 'A' - 'a');
        }
        if(letter[0] >= GLFW_KEY_A || letter[0] <= GLFW_KEY_Z || letter[0] == '.')
        {
            key->key = (int)letter[0];
        }
    }
    igText("Is global ?");
    igCheckbox("##global", (bool *)&key->is_global);
}

void edit_entity_slot(struct GameState *gamestate, int *target, enum ActionType *type)
{
    igText("Target:");
    if(igInputInt("##target", target, 1, 1, 0))
    {
        if(*target < -1) *target = -1;
        if(*target >= gamestate->entity_count) *target = gamestate->entity_count-1;
    } 
    igText("Action:");
    igCombo_Str_arr("##action", (int*)type, get_action_names(), ACTION_COUNT, ACTION_COUNT);
}

void edit_entity_door(struct GameState *gamestate, int index)
{
    struct DoorData *door = gamestate->door_data+index;
    igText("Is opened:");
    igCheckbox("##opened", (bool *)(&door->is_opened));
}

int ig_position_input(const char *label, ivec2 pos)
{
    int value_changed = 0;
    ImGuiStyle *style = igGetStyle();

    igBeginGroup();
    igPushID_Str(label);
    igPushMultiItemsWidths(2, igCalcItemWidth());

    igPushID_Str("x");
    value_changed |= igInputInt("", pos, 1 ,1, 0);
    igPopID();

    igSameLine(0, style->ItemInnerSpacing.x);

    igPushID_Str("y");
    value_changed |= igInputInt("", pos+1, 1 ,1, 0);
    igPopID();

    igPopItemWidth();
    igPopItemWidth();
    igPopID();
    igEndGroup();
    return value_changed;
}

void menu_bar(struct EditorCtx *ectx)
{
    if (igBeginMainMenuBar()) {
        if (igBeginMenu("File", true)) {
            /*
            if (igMenuItem_Bool("New", NULL, false, true)) {
                game_setup_default_level(game);
            }
            */
            if (igMenuItem_Bool("Open", NULL, false, true)) {
                ectx->opening = 1;
                ectx->opening_failed = 0;
            }
            if (igMenuItem_Bool("Save", NULL, false, true)) {
                ectx->saving = 1;             
            }
            igSeparator();
            if (igMenuItem_Bool("Open Sequence", NULL, false, true)) {
                ectx->sequence_opening = 1;
                ectx->opening_failed = 0;
            }
            igEndMenu();
        }

        if (igBeginMenu("Level", true)) {
            if(!ectx->level_menu_opened)
            {
                ectx->level_menu_opened = 1;
                ectx->level_temp_size[0] = level_get_width(&ectx->level);
                ectx->level_temp_size[1] = level_get_height(&ectx->level);
                ectx->level_temp_size_changed = 0;
                ectx->level_temp_shift[0] = 0;
                ectx->level_temp_shift[1] = 0;
                ectx->level_temp_shift_changed = 0;
            }
            igCheckbox("Show World Editor", &ectx->floating_editor_show);

            igSeparator();
            igText("Shift Level:");
            ectx->level_temp_shift_changed |= ig_position_input("shift", ectx->level_temp_shift);
            if(ectx->level_temp_shift_changed && igButton("Comfirm", (struct ImVec2){0,0}))
            {
                level_shift(&ectx->level, ectx->level_temp_shift);
                ectx->level_temp_shift[0] = 0;
                ectx->level_temp_shift[1] = 0;
                ectx->level_temp_shift_changed = 0;
            }

            igSeparator();
            igText("Level Size:");
            ectx->level_temp_size_changed |= ig_position_input("levelSize", ectx->level_temp_size);
            if(ectx->level_temp_size_changed && igButton("Comfirm", (struct ImVec2){0,0}))
            {
                resize_level(&ectx->level, ectx->level_temp_size[0], ectx->level_temp_size[1]);
                ectx->level_temp_size_changed = 0;
            }

            igSeparator();
            igText("Views sizes:");
            ivec2 views_size = {ectx->level.view_width, ectx->level.view_height};
            if(ig_position_input("viewsSize", views_size))
            {
                ectx->level.view_width = views_size[0];
                ectx->level.view_height = views_size[1];
            }

            igEndMenu();
        }
        else
        {
            ectx->level_menu_opened = 0;
        }

        if(igBeginMenu("Options", true))
        {
            ImGuiStyle* style = igGetStyle();
            float old_scale = style->FontScaleMain;
            if(igInputFloat("uiscale", &style->FontScaleMain, 1, 1, NULL, 0))
            {
                if(style->FontScaleMain < 1) style->FontScaleMain = 1;
                if(style->FontScaleMain > 4) style->FontScaleMain = 4;
                ImGuiStyle_ScaleAllSizes(style, style->FontScaleMain/old_scale);
                ectx->ui_scale = style->FontScaleMain;
            }
            igEndMenu();
        }

        if(igBeginMenu("Run", true))
        {
            if (igMenuItem_Bool("Run level", NULL, false, true)) 
            {
                ectx_start_level(ectx, ectx->level);
            }
            igEndMenu();
        }

        igEndMainMenuBar();
    }
}

void ig_tilemap_layer(struct EditorCtx *ectx, struct Level *level, char *title, int layer)
{

    igSetNextItemAllowOverlap();
    if (igSelectable_Bool("##selectable", ectx->layer_selected == layer, ImGuiSelectableFlags_SpanAvailWidth, (struct ImVec2){0,0}))
    {
        ectx->layer_selected = layer;
    }

    igSameLine(0, 0);

    //// If you want visibility button
    /*
    ImVec2 spacing = igGetStyle()->ItemSpacing;
    int visible = layer_get_visibility(game, layer);
    if (igSmallButton(visible ? "O" : "_"))
    {
        layer_set_visibility(game, layer, !visible);
    }
    igSameLine(0, spacing.x);
    */
    
    //// If you want the possibility of having an unmaskable layer
    /* 
    ImVec2 spacing = igGetStyle()->ItemSpacing;
    {
        layer_set_visibility(game, layer, layer_selected == layer);

        char* single_char = "0";
        ImVec2 text_size; igCalcTextSize(&text_size, single_char, single_char+1, 0, 0);
        float small_button_size =  text_size.x + igGetStyle()->FramePadding.x * 2.0f + spacing.x;
        igSameLine(0, small_button_size);
    }
    */

    igText(title);
}

void tilemap_ig_selection(struct EditorCtx *ectx, struct ImVec2 pos, float size)
{
    ImDrawList* draw_list = igGetWindowDrawList();
    struct ImVec2 rect_min = { pos.x + (float)ectx->tile_position.x * size,
        pos.y + (float)ectx->tile_position.y * size };
    struct ImVec2 rect_max = { rect_min.x + size,
        rect_min.y + size };

    ImU32 color = igGetColorU32_Vec4((struct ImVec4){255, 0, 0, 20}); // red, semi-transparent
    ImDrawList_AddRectFilled(draw_list, rect_min, rect_max, color, 0.0f, 0);
}

void ig_tilemap_tile_selector(struct EditorCtx *ectx, struct TextureAtlas atlas, int *out_index, struct ImVec2i *out_pos)
{
    unsigned int texture_id = atlas.texture_id;
    struct ImTextureRef *ref =ImTextureRef_ImTextureRef_TextureID(texture_id);
    struct ImVec2 image_size = {(float)(32*atlas.width), (float)(32*atlas.height)};

    struct ImVec2 window_size;
    igGetWindowSize(&window_size);

    struct ImVec2 pos; 
    igGetCursorScreenPos(&pos);

    igImage(*ref, image_size,(struct ImVec2){0,1},(struct ImVec2){1,0});

    tilemap_ig_selection(ectx, pos, 32);

    struct ImVec2 selectable_size;
    selectable_size.x = MIN(window_size.x, image_size.x + pos.x);
    selectable_size.y = MIN(window_size.y, image_size.y + pos.y);

    int tile_x, tile_y;
    struct ImVec2 mouse;
    igGetMousePos(&mouse);
    if (mouse.x >= pos.x && mouse.y >= pos.y &&
            mouse.x <= selectable_size.x &&
            mouse.y <= selectable_size.y &&
            igIsKeyPressed_Bool(ImGuiKey_MouseLeft, false)) 
    {
        float u = (mouse.x - pos.x) / image_size.x;
        float v = (mouse.y - pos.y) / image_size.y;

        tile_x = (int)(u * (float)atlas.width);
        tile_y = (int)(v * (float)atlas.height);
        *out_index = tile_y * atlas.width + tile_x;
        printf("Selected tile n %d\n", *out_index);
    }
    else
    {
        tile_y = *out_index / atlas.width;
        tile_x = *out_index - (tile_y * atlas.width);
    }
    ImTextureRef_destroy(ref);
    *out_pos = (struct ImVec2i){tile_x, tile_y};
}

void selection_draw(struct EditorCtx *ectx, vec2 mouse_pos)
{
    unsigned int program = shaders_use_default();
    mat3 transform;
    vec2 size;
    glm_vec2_sub(ectx->selection_pos_start, mouse_pos, size);
    vec2 pos;
    glm_vec2_add(ectx->selection_pos_start, mouse_pos, pos);
    glm_vec2_scale(pos, 0.5f, pos);


    compute_transform(transform, pos, size);
    draw_transformed_quad_screen_space(program, transform, (vec3){0.f, 1.f, 0.2f}, 0.2f);
}

int ig_save_path_input_popup(struct EditorCtx *ectx, const char *popup_id, char *input_buffer, char *out_path, const char *base_path, const char *suffix)
{
    int result = 0;
    if(igBeginPopupModal(popup_id, NULL, ImGuiWindowFlags_AlwaysAutoResize))
    {
        igText("File name:");
        igInputText("##input", input_buffer, 100, 0, NULL, NULL);

        char path[150];
        strcpy(path, base_path);
        strcat(path, input_buffer);
        unsigned long long path_len = strlen(path);
        unsigned long long suffix_len = strlen(suffix);
        if(strcmp(path + path_len - suffix_len, suffix) != 0) 

        {
            strcat(path, suffix);
        }
        if(access(path, F_OK)==0)  
        {
            igText("/!\\/!\\ This file already exist, it will be overwrited !");
        }
        if(igButton("Comfirm", (struct ImVec2){0,0}))
        {
            strcpy(out_path, path);
            igCloseCurrentPopup();
            result = 1;
        }
        igSameLine(0,-1);
        if(igButton("Cancel", (struct ImVec2){0,0}))
        {
            ectx->saving = 0;
            igCloseCurrentPopup();
            result = -1;
        }

        igEndPopup();
    }
    return result;
}

int ig_open_path_input_popup(struct EditorCtx *ectx, const char *popup_id, char *input_buffer, char *out_path, const char *base_path, const char *suffix)
{
    int result = 0;
    if(igBeginPopupModal(popup_id, NULL, ImGuiWindowFlags_AlwaysAutoResize))
    {
        igText("File name:");
        igInputText("##input2", input_buffer, 100, 0, NULL, NULL);

        char path[150];
        strcpy(path, base_path);
        strcat(path, input_buffer);
        unsigned long long path_len = strlen(path);
        unsigned long long suffix_len = strlen(suffix);
        if(strcmp(path + path_len - suffix_len, suffix) != 0) 
        {
            strcat(path, suffix);
        }
        if(ectx->opening_failed)
        {
            igText("/!\\/!\\ This file don't exist !");
        }
        if(igButton("Comfirm", (struct ImVec2){0,0}))
        {
            if(access(path, F_OK)!=0)  
            {
                ectx->opening_failed = 1;
            }
            else 
            {
                strcpy(out_path, path);
                result = 1;
                igCloseCurrentPopup();
            }
        }
        igSameLine(0,-1);
        if(igButton("Cancel", (struct ImVec2){0,0}))
        {
            result = -1;
            igCloseCurrentPopup();
        }
        igEndPopup();
    }
    return result;
}

void handle_editor_window(struct EditorCtx *ectx, struct Level *level)
{
    struct TileMap *tilemap = &level->tilemap;
    int level_width = tilemap->width, level_height = tilemap->height;

    igPushID_Str("Layers");
    igBegin(floating_editor, NULL, 0);
    igSeparatorText("Layers:");
    for (int i = 0; i < tilemap->layer_count; i++)
    {
        igPushID_Int(i);
        char selectable_title [10] = "Layer ";
        char number[3];
        my_itoa(i, number, 10);
        strcat(selectable_title, number);
        ig_tilemap_layer(ectx, level, selectable_title, i);
        igPopID();
    }
    igSeparatorText("Tiles:");
    ig_tilemap_tile_selector(ectx, get_atlas_tilemap(), &ectx->tile_index, &ectx->tile_position);
    igEnd();
    igPopID();

    //Tilemap edition
    int edition = 0;
    if(igIsMouseDown_Nil(ImGuiMouseButton_Left)) edition = 1;
    if(igIsMouseDown_Nil(ImGuiMouseButton_Right)) edition = -1;
    int alt = igIsKeyDown_Nil(ImGuiKey_LeftAlt);

    ivec2 cursor_grid_ipos;
    cursor_grid_ipos[0] = (int)roundf(ectx->mouse_world_pos[0]-0.5f);
    cursor_grid_ipos[1] = (int)roundf(ectx->mouse_world_pos[1]-0.5f);
    int is_inside = cursor_grid_ipos[0] >= 0 && cursor_grid_ipos[0] < level_width
        && cursor_grid_ipos[1] >= 0 && cursor_grid_ipos[1] < level_height;

    if(edition)
    {
        if(is_inside)
        {
            Tile *layer = tilemap_get_layer_by_index(tilemap, ectx->layer_selected);
            Tile *tile_to_draw = layer + cursor_grid_ipos[0] + cursor_grid_ipos[1] * level_width;
            if(edition > 0)
            {
                if(alt)
                {
                    ectx->tile_index = *tile_to_draw - 1; 
                }
                else
                {
                    *tile_to_draw = ectx->tile_index + 1; 
                }
            }
            else
            {
                *tile_to_draw = 0;
            }
        }
    }
    unsigned int program = shaders_use_default();
    mat3 transform;
    vec2 size = {0.8f, 0.8f};
    vec2 cursor_grid_pos;
    cursor_grid_pos[0] = roundf(ectx->mouse_world_pos[0]-((float)level_width)/2+0.5f)+(float)(level_width)/2-0.5f;
    cursor_grid_pos[1] = roundf(ectx->mouse_world_pos[1]-((float)level_height)/2+0.5f)+(float)(level_height)/2-0.5f;

    compute_transform(transform, cursor_grid_pos, size);
    draw_transformed_quad(program, transform, (vec3){1.f, 0.f, 1.f}, 0.8f);
}

void handle_entity_edition(struct EditorCtx *ectx, vec2 mouse_pos)
{
    struct Level *level = &ectx->level;
    struct TileMap *tilemap = &level->tilemap;
    struct GameState *gamestate = &level->gamestate;
    int level_width = tilemap->width, level_height = tilemap->height;

    // Entity edition
    if(igIsMouseClicked_Bool(ImGuiMouseButton_Right, false))
    {
        ivec2 cursor_grid_pos;
        cursor_grid_pos[0] = (int)roundf(ectx->mouse_world_pos[0]-0.5f);
        cursor_grid_pos[1] = (int)roundf(ectx->mouse_world_pos[1]-0.5f);
        if(cursor_grid_pos[0] >= 0 && cursor_grid_pos[0] < level_width
                && cursor_grid_pos[1] >= 0 && cursor_grid_pos[1] < level_height)
        {
            igOpenPopup_Str("Menu", 0);
            glm_ivec2_copy(cursor_grid_pos, ectx->popup_last_clicked_pos);
        }
    }
    ImGuiIO *io = igGetIO_Nil();
    if(igIsMouseDown_Nil(ImGuiMouseButton_Left) && !io->WantCaptureMouse)
    {
        if(!ectx->selection_started)
        {
            glm_vec2_copy(mouse_pos, ectx->selection_pos_start);
            ectx->selection_started = 1;
        }
    }
    else
    {
        ectx->selection_started = 0;
    }
    if(ectx->selection_started)
    {
        selection_draw(ectx, mouse_pos);
        ectx->selection_ents_count = 0;
        vec2 world_pos_start;
        vec2 world_mouse_pos;
        camera_screen_to_world(&ectx->camera, ectx->selection_pos_start, world_pos_start);
        camera_screen_to_world(&ectx->camera, mouse_pos, world_mouse_pos);
        world_pos_start[0] = world_pos_start[0]-0.5f;
        world_pos_start[1] = world_pos_start[1]-0.5f;
        world_mouse_pos[0] = world_mouse_pos[0]-0.5f;
        world_mouse_pos[1] = world_mouse_pos[1]-0.5f;

        float bound_min_x = MIN(world_pos_start[0], world_mouse_pos[0]);
        float bound_max_x = MAX(world_pos_start[0], world_mouse_pos[0]);
        float bound_min_y = MIN(world_pos_start[1], world_mouse_pos[1]);
        float bound_max_y = MAX(world_pos_start[1], world_mouse_pos[1]);
        for(int i = 0; i < gamestate->entity_count; ++i)
        {
            struct Entity *ent = gamestate->entities+i;
            int inside_x = bound_min_x < (float)ent->position[0] && (float)ent->position[0] < bound_max_x;
            int inside_y = bound_min_y < (float)ent->position[1] && (float)ent->position[1] < bound_max_y;
            if(inside_x && inside_y)
            {
                ectx->selection_ents[ectx->selection_ents_count++] = i; 
            }
        }
    }
    unsigned int program = shaders_use_default();
    for(int i = 0; i < ectx->selection_ents_count; ++i)
    {
        struct Entity *ent = gamestate->entities+(ectx->selection_ents[i]);
        mat3 transform;
        vec2 size = {1.1f, 1.1f};
        vec2 pos;
        pos[0] = (float)ent->position[0]+0.5f;
        pos[1] = (float)ent->position[1]+0.5f;

        compute_transform(transform, pos, size);
        draw_transformed_quad(program, transform, (vec3){1.f, 0.f, 1.f}, 0.2f);
    }
}

int editor_update_internal(struct EditorMemory *mem, struct EditorCtx *ectx, struct InputInfo inputinfo)
{
    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    int window_width, window_height;
    editor_get_window_size(ectx, &window_width, &window_height);

    ImVec2 new_mouse_pos;
    igGetMousePos(&new_mouse_pos);
    ectx->mouse_offset.x = new_mouse_pos.x-ectx->mouse_pos.x;
    ectx->mouse_offset.y = new_mouse_pos.y-ectx->mouse_pos.y;
    ectx->mouse_pos = new_mouse_pos; 

    editor_new_frame();

    if (igIsKeyPressed_Bool(ImGuiKey_Escape, false))
    {
        if(ectx->is_playing)
        {
            ectx->is_playing = 0;
            r_set_main_camera(&ectx->camera);
        }
        else
        {
            return 0;
        }
    }

    if(ectx->is_playing)
    {
        r_set_main_camera(&ectx->game.camera);
        game_update(&ectx->game, ectx->window_info, inputinfo); 
        editor_render(ectx);
        return 1;
    }
    else
    {
        r_set_main_camera(&ectx->camera);
    }

    camera_compute_view(&ectx->camera, editor_get_window_ratio(ectx));
    tilemap_render_background(&ectx->level.tilemap, (vec2){0, 0}, 1);
    render_level_simple(&ectx->level, &ectx->level.gamestate);
    render_level_views(&ectx->level);

    struct GameState *gamestate = &ectx->level.gamestate;

    int camera_view_changed = 0;
    
    struct ImGuiIO *io = igGetIO_Nil();
    float scroll = io->MouseWheel;
    if(scroll != 0.f)
    {
        camera_zoom(&ectx->camera, scroll * 0.2f);
        camera_view_changed = 1;
    }

    if(igIsMouseDown_Nil(ImGuiMouseButton_Middle))
    {
        float mouse_delta_x = ectx->mouse_offset.x, mouse_delta_y = ectx->mouse_offset.y;
        if (fabsf(mouse_delta_x) > 0.00001f || fabsf(mouse_delta_y) > 0.00001f)
        {
            float pan_x, pan_y;
            pan_x = -mouse_delta_x*2/ectx->camera.zoom*editor_get_window_ratio(ectx)/(float)window_width;
            pan_y = mouse_delta_y*2/ectx->camera.zoom/(float)window_height;
            camera_pan(&ectx->camera, pan_x, -pan_y);
            camera_view_changed = 1;
        }
    }

    if(igIsKeyPressed_Bool(ImGuiKey_Tab, false))
    {
        ectx->floating_editor_show = !ectx->floating_editor_show;
    }

    vec2 mouse_pos_normalied;
    mouse_pos_normalied[0] = new_mouse_pos.x / (float)window_width;
    mouse_pos_normalied[1] = 1 - new_mouse_pos.y / (float)window_height;
    camera_screen_to_world(&ectx->camera, mouse_pos_normalied, ectx->mouse_world_pos);

    if(camera_view_changed)
    {
        camera_compute_view(&ectx->camera, editor_get_window_ratio(ectx));
    }

    if(ectx->floating_editor_show)
    {
        handle_editor_window(ectx, &ectx->level);
    }
    else
    {
        handle_entity_edition(ectx, mouse_pos_normalied);
    }

    menu_bar(ectx);

    ImGuiPopupFlags flags = ImGuiPopupFlags_None;
    if(igBeginPopupContextItem("Menu", flags))
    {
        int found = 0;
        if(ectx->selection_ents_count)
        {
            const int number_max_char = 3;
            char str_name[30] = "Selection (";
            char str_number[number_max_char];
            my_itoa(ectx->selection_ents_count, str_number, 10);
            strcat(str_name, str_number);
            strcat(str_name, " ents)");
            igPushID_Str("Selection");
            if(igBeginMenu(str_name, true))
            {
                if(igSelectable_Bool("Move", false, 0, (struct ImVec2){0,0}))
                {
                    ectx->reposition_selection = 1;
                    glm_ivec2_zero(ectx->reposition_selection_delta);
                }
                if(igSelectable_Bool("Remove", false, 0, (struct ImVec2){0,0}))
                {
                    ectx->deletion_selection = 1;
                }
                igEndMenu();
            }
            igPopID();
        }
        for(int entity_index = 0; entity_index < gamestate->entity_count; ++entity_index)
        {
            struct Entity *ent = gamestate->entities+entity_index;
            if(glm_ivec2_eqv(ectx->popup_last_clicked_pos, ent->position))
            {
                ++found;
                const int number_max_char = 3;
                char str_name[30];
                char str_number[number_max_char];
                strcpy(str_name, get_entity_name(ent->type));
                my_itoa(get_entity_index(gamestate, ent), str_number, 10);
                strcat(str_name, " (");
                strcat(str_name, str_number);
                strcat(str_name, ")");
                if(igBeginMenu(str_name, true))
                {
                    if(ent->type == ENTITY_SLOT || ent->type == ENTITY_KEY || ent->type == ENTITY_DOOR)
                    {
                        if(igSelectable_Bool("Edit", false, 0, (struct ImVec2){0,0}))
                        {
                            ectx->edition_entity = ent;
                        }
                    }
                    if(igSelectable_Bool("Move", false, 0, (struct ImVec2){0,0}))
                    {
                        ectx->reposition_entity = ent;
                    }
                    if(igSelectable_Bool("Remove", false, 0, (struct ImVec2){0,0}))
                    {
                        ectx->deletion_index = get_entity_index(gamestate, ent);
                    }
                    igEndMenu();
                }
            }
        }
        if(!found && !ectx->selection_ents_count)
        {
            igText("No entity here.");
        }
        igSeparator();
        if(igBeginMenu("Create entity", true))
        {
            for(enum EntityType i = 1; i < ENTITY_COUNT; ++i)
            {
                if(igSelectable_Bool(get_entity_name(i), false, 0, (struct ImVec2){0,0}))       
                {
                    ectx->creation_type = i;
                    glm_ivec2_copy(ectx->popup_last_clicked_pos, ectx->creation_position);
                    ectx->creation_action_target = -1;
                }
            }
            igEndMenu();
        }
        igEndPopup();
    }
    if(ectx->reposition_entity)
    {
        igOpenPopup_Str(window_move, 0);
    }
    else if(ectx->creation_type)
    {
        igOpenPopup_Str(window_create, 0);
    }
    else if(ectx->deletion_index >= 0)
    {
        igOpenPopup_Str(window_deletion, 0);
    }
    else if(ectx->edition_entity)
    {
        igOpenPopup_Str(window_edition, 0);
    }
    else if(ectx->saving)
    {
        igOpenPopup_Str(window_saving, 0);
    }
    else if(ectx->opening)
    {
        igOpenPopup_Str(window_opening, 0);
    }
    else if(ectx->sequence_opening)
    {
        igOpenPopup_Str(window_sequence_opening, 0);
    }
    else if(ectx->reposition_selection)
    {
        igOpenPopup_Str(window_move_selection, 0);
    }
    else if(ectx->deletion_selection)
    {
        igOpenPopup_Str(window_deletion_selection, 0);
    }
    struct ImVec2 center;
    ImGuiViewport_GetCenter(&center, igGetMainViewport());
    igSetNextWindowPos(center, ImGuiCond_Appearing, (struct ImVec2){0.5f,0.5f});

    //////////////////////
    //  SELECTION MOVE  //
    //////////////////////
    if(igBeginPopupModal(window_move_selection, NULL, ImGuiWindowFlags_AlwaysAutoResize))
    {
        igText("Entity position:");

        ivec2 old_delta;
        glm_ivec2_copy(ectx->reposition_selection_delta, old_delta);

        if(ig_position_input("entPos", ectx->reposition_selection_delta))
        {
            ivec2 deltas_delta;
            glm_ivec2_sub(ectx->reposition_selection_delta, old_delta, deltas_delta);
            for(int i = 0; i < ectx->selection_ents_count; ++i)
            {
                struct Entity *ent = gamestate->entities+(ectx->selection_ents[i]);
                glm_ivec2_add(ent->position, deltas_delta, ent->position);
            }
        }
        if(igButton("Comfirm", (struct ImVec2){0,0}))
        {
            ectx->reposition_selection = 0;
            igCloseCurrentPopup();
        }
        igEndPopup();
    }
    
    //////////////////////////
    //  SELECTION DELETION  //
    //////////////////////////
    if(igBeginPopupModal(window_deletion_selection, NULL, ImGuiWindowFlags_AlwaysAutoResize))
    {
        igText("Are you sure you want to delete the selected entities?");
        igSeparator();
        if(igButton("Yes", (struct ImVec2){0,0}))
        {
            for(int i = ectx->selection_ents_count-1; i>=0; --i)
            {
                remove_entity(gamestate, ectx->selection_ents[i]);
            }
            ectx->selection_ents_count = 0;
            ectx->deletion_selection = 0;
            igCloseCurrentPopup();
        }
        igSameLine(0,-1);
        if(igButton("No", (struct ImVec2){0,0}))
        {
            ectx->deletion_selection = 0;
            igCloseCurrentPopup();
        }

        igEndPopup();
    }
    
    ///////////////////
    //  ENTITY MOVE  //
    ///////////////////
    if(igBeginPopupModal(window_move, NULL, ImGuiWindowFlags_AlwaysAutoResize))
    {
        igText("Entity position:");

        ig_position_input("entPos", ectx->reposition_entity->position);
        if(igButton("Comfirm", (struct ImVec2){0,0}))
        {
            ectx->reposition_entity = NULL; 
            igCloseCurrentPopup();
        }
        igEndPopup();
    }

    ///////////////////////
    //  ENTITY CREATION  //
    ///////////////////////
    if(igBeginPopupModal(window_create, NULL, ImGuiWindowFlags_AlwaysAutoResize))
    {
        igText(get_entity_name(ectx->creation_type));
        igSeparator();
        igText("Entity position:");
        igInputInt2("##position", ectx->creation_position, ImGuiInputTextFlags_None);
        if(ectx->creation_type == ENTITY_KEY)
        {
            edit_entity_key(&ectx->creation_key);
        }
        if(ectx->creation_type == ENTITY_SLOT)
        {
            edit_entity_slot(gamestate, &ectx->creation_action_target, &ectx->creation_action);
        }
        if(igButton("Comfirm", (struct ImVec2){0,0}))
        {
            switch(ectx->creation_type)
            {
                case ENTITY_NONE:
                    break;
                case ENTITY_KEY:
                    create_key_block_at(gamestate, ectx->creation_position[0], ectx->creation_position[1], ectx->creation_key.key, ectx->creation_key.is_global);
                    break;
                case ENTITY_SLOT:
                    create_slot_at(gamestate, ectx->creation_position[0], ectx->creation_position[1], ectx->creation_action, ectx->creation_action_target);
                    break;
                case ENTITY_DOOR:
                    create_door_at(gamestate, ectx->creation_position[0], ectx->creation_position[1]);
                    break;
                case ENTITY_BUTTON:
                    create_button_at(gamestate, ectx->creation_position[0], ectx->creation_position[1]);
                    break;
                case ENTITY_ANTIBUTTON:
                    create_antibutton_at(gamestate, ectx->creation_position[0], ectx->creation_position[1]);
                    break;
                default:
                    create_movable_at(gamestate, ectx->creation_position[0], ectx->creation_position[1], ectx->creation_type);
                    break;
            }
            ectx->creation_type = 0;
            igCloseCurrentPopup();
        }
        igEndPopup();
    }
    
    ///////////////////////
    //  ENTITY DELETION  //
    ///////////////////////
    if(igBeginPopupModal(window_deletion, NULL, ImGuiWindowFlags_AlwaysAutoResize))
    {
        igText("Are you sure you want to delete that entity?");
        igSeparator();
        if(igButton("Yes", (struct ImVec2){0,0}))
        {
            remove_entity(gamestate, ectx->deletion_index);
            ectx->deletion_index = -1;
            igCloseCurrentPopup();
        }
        igSameLine(0,-1);
        if(igButton("No", (struct ImVec2){0,0}))
        {
            ectx->deletion_index = -1;
            igCloseCurrentPopup();
        }

        igEndPopup();
    }
    
    //////////////////////
    //  ENTITY EDITION  //
    //////////////////////
    if(igBeginPopupModal(window_edition, NULL, ImGuiWindowFlags_AlwaysAutoResize))
    {
        switch(ectx->edition_entity->type)
        {
            case ENTITY_KEY:
                {
                    struct KeyBlockData *key = gamestate->key_block_data+ectx->edition_entity->data_index;
                    edit_entity_key(key);
                }
                break;
            case ENTITY_SLOT:
                {
                    struct SlotData *slot = gamestate->slot_data+ectx->edition_entity->data_index;
                    edit_entity_slot(gamestate, &slot->action.target_entity, &slot->action.type);
                }
                break;
            case ENTITY_DOOR:
                    edit_entity_door(gamestate, ectx->edition_entity->data_index);
                break;
            default:
                igCloseCurrentPopup();
                break;
        }

        if(igButton("Comfirm", (struct ImVec2){0,0}))
        {
            ectx->edition_entity = NULL;
            igCloseCurrentPopup();
        }
        igEndPopup();
    }
    
    ///////////////
    //  SAVING   //
    ///////////////
    char save_path[150];
    int save_result = ig_save_path_input_popup(ectx, window_saving, ectx->file_current, save_path, resources_path, file_suffix);
    if(save_result > 0)
    {
        struct Level level_to_save = ectx->level; 
        serialize_level(&level_to_save, save_path);
        ectx->saving = 0;
    }
    else if(save_result < 0)
    {
        ectx->saving = 0;
    }

    ////////////////
    //  OPENING   //
    ////////////////
    char opening_path[150];
    int opening_result = ig_open_path_input_popup(ectx, window_opening, ectx->file_current, opening_path, resources_path, file_suffix);
    if(opening_result > 0)
    {
        ectx->opening = 0;
        //deserialize_level_into_game(game, opening_path);
        struct Level loaded_level;
        arena_reset(&mem->level);
        if(deserialize_level(&mem->level, &loaded_level, opening_path))
        {
            ectx->level = loaded_level;
        }
    }
    else if(opening_result < 0)
    {
        ectx->opening = 0;
    }

    /////////////////////////
    //  OPENING SEQUENCE   //
    /////////////////////////
    char opening_sequence_path[150];
    int opening_sequence_result = ig_open_path_input_popup(ectx, window_sequence_opening, ectx->sequence_current, opening_sequence_path, resources_path, sequence_suffix);
    if(opening_sequence_result > 0)
    {
        //deserialize_sequence_into_game(game, opening_sequence_path);
        ectx->sequence_opening = 0;
    }
    else if(opening_sequence_result < 0)
    {
        ectx->sequence_opening = 0;
    }

    editor_render(ectx);
    return 1;
}

float editor_get_window_ratio(struct EditorCtx *ectx)
{
    return ectx->window_info.ratio;
}

void editor_get_window_size(struct EditorCtx *ectx, int *w, int *h)
{
    *w = ectx->window_info.width;
    *h = ectx->window_info.height;
}
