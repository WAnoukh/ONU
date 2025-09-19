#include <string.h>

#include <io.h>
#include <stdio.h>
#include <dirent.h>

#include "editor.h"
#include "GLFW/glfw3.h"
#include "cglm/types.h"
#include "cglm/vec2.h"
#include "game.h"
#include "level.h"
#include "serialization.h"
#include "texture.h"
#include "transform.h"
#include "window/input.h"
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

ImGuiContext* ctx;
vec2 cursor_pos;
ivec2 popup_last_clicked_pos;
_Bool floating_editor_show = 0;

struct Entity *reposition_entity = NULL;
int reposition_selection = 0;
ivec2 reposition_selection_delta;

enum EntityType creation_type = ENTITY_NONE;
ivec2 creation_position;
int creation_key = GLFW_KEY_A;
enum ActionType creation_action = ACTION_NONE;
int creation_action_target = 0;

int deletion_index = -1;
int deletion_selection = 0;

int layer_selected = 1;
int layer_last_selected = 0;

int selection_started = 0;
vec2 selection_pos_start;
#define selection_ents_max 30
int selection_ents[selection_ents_max];
int selection_ents_count = 0;

struct Entity *edition_entity = NULL;

char resources_path[] = "resources/level/";

char file_suffix[] = ".level";
char file_current[100] = "NewFile";
int saving = 0;
int opening = 0;
int opening_failed = 0;

char sequence_suffix[] = ".seq";
char sequence_current[100] = "demo";
int sequence_opening = 0;
int sequence_opening_failed = 0;

int tile_index;
struct ImVec2i tile_position;

float ui_scale = 2.f;

int level_menu_opened = 0;
ivec2 level_temp_size;
int level_temp_size_changed = 0;
ivec2 level_temp_shift;
int level_temp_shift_changed = 0;

int editor_initialize(GLFWwindow *window)
{

    ctx = igCreateContext(NULL);
    if (ctx) {
        printf("Dear ImGui context created!\n");
    }
    else
    {
        return 0;
    }
    ImGuiIO* io = igGetIO_ContextPtr(ctx);
    ImGuiStyle* style = igGetStyle();

    ImGuiStyle_ScaleAllSizes(style, ui_scale);
    style->FontScaleMain *= ui_scale;

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 130");

    return 1;
}

void editor_new_frame()
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    igNewFrame();
}

void editor_render()
{
    igRender();
    ImGui_ImplOpenGL3_RenderDrawData(igGetDrawData());
}

void editor_destroy()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    igDestroyContext(ctx);
}

void edit_entity_key(int *key)
{
    char letter[] = {(char)*key, '\0'};
    igText("Key binding:");
    if(igInputText("##keybinding", letter, 2, ImGuiInputTextFlags_CharsUppercase, NULL, NULL))
    {
        if(letter[0] >= 'a' && letter[0] <= 'z')
        {
            letter[0] = (char)(letter[0] + 'A' - 'a');
        }
        if(letter[0] >= GLFW_KEY_A || letter[0] <= GLFW_KEY_Z || letter[0] == '.')
        {
            *key = (int)letter[0];
        }
    }
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

void menu_bar(struct Game *game)
{
    struct GameState *gamestate = get_current_gamestate(game);
    if (igBeginMainMenuBar()) {
        if (igBeginMenu("File", true)) {
            if (igMenuItem_Bool("New", NULL, false, true)) {
                game_setup_default_level(game);
            }
            if (igMenuItem_Bool("Open", NULL, false, true)) {
                opening = 1;
                opening_failed = 0;
            }
            if (igMenuItem_Bool("Save", NULL, false, true)) {
                saving = 1;             
            }
            igSeparator();
            if (igMenuItem_Bool("Open Sequence", NULL, false, true)) {
                sequence_opening = 1;
                opening_failed = 0;
            }
            igEndMenu();
        }

        if (igBeginMenu("Level", true)) {
            if(!level_menu_opened)
            {
                level_menu_opened = 1;
                level_temp_size[0] = level_get_width(&game->level);
                level_temp_size[1] = level_get_height(&game->level);
                level_temp_size_changed = 0;
                level_temp_shift[0] = 0;
                level_temp_shift[1] = 0;
                level_temp_shift_changed = 0;
            }
            igCheckbox("Show World Editor", &floating_editor_show);
            igCheckbox("Door open", (_Bool *)(&gamestate->is_door_opened));

            igSeparator();
            igText("Shift Level:");
            level_temp_shift_changed |= ig_position_input("shift", level_temp_shift);
            if(level_temp_shift_changed && igButton("Comfirm", (struct ImVec2){0,0}))
            {
                level_shift(&game->level, level_temp_shift);
                level_temp_shift[0] = 0;
                level_temp_shift[1] = 0;
                level_temp_shift_changed = 0;
            }

            igSeparator();
            igText("Level Size:");
            level_temp_size_changed |= ig_position_input("levelSize", level_temp_size);
            if(level_temp_size_changed && igButton("Comfirm", (struct ImVec2){0,0}))
            {
                resize_level(&game->level, level_temp_size[0], level_temp_size[1]);
                level_temp_size_changed = 0;
            }

            igEndMenu();
        }
        else
        {
            level_menu_opened = 0;
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
                ui_scale = style->FontScaleMain;
            }
            igEndMenu();
        }

        igEndMainMenuBar();
    }
}

void tilemap_ig_layer(struct Game *game, char *title, int layer)
{
    igSetNextItemAllowOverlap();
    if (igSelectable_Bool("##selectable", layer_selected == layer, ImGuiSelectableFlags_SpanAvailWidth, (struct ImVec2){0,0}))
    {
        layer_selected = layer;
    }

    igSameLine(0, 0);

    int visible = layer_get_visibility(game, layer);
    if (igSmallButton(visible ? "O" : "_"))
    {
        layer_set_visibility(game, layer, !visible);
    }
    ImVec2 spacing = igGetStyle()->ItemSpacing;

    igSameLine(0, spacing.x);

    igText(title);
}

void tilemap_ig_selection(struct ImVec2 pos, float size)
{
    ImDrawList* draw_list = igGetWindowDrawList();
    struct ImVec2 rect_min = { pos.x + (float)tile_position.x * size,
        pos.y + (float)tile_position.y * size };
    struct ImVec2 rect_max = { rect_min.x + size,
        rect_min.y + size };

    ImU32 color = igGetColorU32_Vec4((struct ImVec4){255, 0, 0, 20}); // red, semi-transparent
    ImDrawList_AddRectFilled(draw_list, rect_min, rect_max, color, 0.0f, 0);
}

int tilemap_ig_tile_selector(struct TextureAtlas atlas, int *out_index, struct ImVec2i *out_pos)
{
    unsigned int texture_id = atlas.texture_id;
    struct ImTextureRef *ref =ImTextureRef_ImTextureRef_TextureID(texture_id);
    struct ImVec2 image_size = {(float)(32*atlas.width), (float)(32*atlas.height)};

    struct ImVec2 window_size;
    igGetWindowSize(&window_size);

    struct ImVec2 pos; 
    igGetCursorScreenPos(&pos);

    igImage(*ref, image_size,(struct ImVec2){0,1},(struct ImVec2){1,0});

    tilemap_ig_selection(pos, 32);

    struct ImVec2 selectable_size;
    selectable_size.x = MIN(window_size.x, image_size.x + pos.x);
    selectable_size.y = MIN(window_size.y, image_size.y + pos.y);

    struct ImVec2 mouse;
    igGetMousePos(&mouse);
    if (mouse.x >= pos.x && mouse.y >= pos.y &&
            mouse.x <= selectable_size.x &&
            mouse.y <= selectable_size.y &&
            igIsKeyPressed_Bool(ImGuiKey_MouseLeft, false)) 
    {
        float u = (mouse.x - pos.x) / image_size.x;
        float v = (mouse.y - pos.y) / image_size.y;

        int tile_x = (int)(u * (float)atlas.width);
        int tile_y = (int)(v * (float)atlas.height);

        *out_index = tile_y * atlas.width + tile_x;
        *out_pos = (struct ImVec2i){tile_x, tile_y};
        return 1;
    }
    ImTextureRef_destroy(ref);
    return 0;
}

void selection_draw(vec2 mouse_pos)
{
    unsigned int program = shaders_use_default();
    mat3 transform;
    vec2 size;
    glm_vec2_sub(selection_pos_start, mouse_pos, size);
    vec2 pos;
    glm_vec2_add(selection_pos_start, mouse_pos, pos);
    glm_vec2_scale(pos, 0.5f, pos);


    compute_transform(transform, pos, size);
    draw_transformed_quad_screen_space(program, transform, (vec3){0.f, 1.f, 0.2f}, 0.2f);
}

int ig_save_path_input_popup(const char *popup_id, char *input_buffer, char *out_path, const char *base_path, const char *suffix)
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
            saving = 0;
            igCloseCurrentPopup();
            result = -1;
        }

        igEndPopup();
    }
    return result;
}

int ig_open_path_input_popup(const char *popup_id, char *input_buffer, char *out_path, const char *base_path, const char *suffix)
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
        if(opening_failed)
        {
            igText("/!\\/!\\ This file don't exist !");
        }
        if(igButton("Comfirm", (struct ImVec2){0,0}))
        {
            if(access(path, F_OK)!=0)  
            {
                opening_failed = 1;
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

void editor_update(struct Game *game, GLFWwindow *window)
{
    ImGuiIO *io = igGetIO_Nil();
    struct GameState *gamestate = get_current_gamestate(game);
    struct TileMap *tilemap = get_current_tilemap(game);

    int level_width = tilemap->width;
    int level_height = tilemap->height;

    int camera_view_changed = 0;
    float scroll = i_get_scroll_y();
    if(scroll != 0.f)
    {
        camera_zoom(&game->camera, scroll * 0.2f);
        camera_view_changed = 1;
    }

    if (i_button_down(GLFW_MOUSE_BUTTON_3))
    {
        float mouse_delta_x, mouse_delta_y;
        i_get_mouse_move(&mouse_delta_x, &mouse_delta_y);
        if (fabsf(mouse_delta_x) > 0.00001f || fabsf(mouse_delta_y) > 0.00001f)
        {
            int width, height;
            glfwGetFramebufferSize(window, &width, &height);
            camera_pan(&game->camera, mouse_delta_x/(float)width, -mouse_delta_y/(float)height);
            camera_view_changed = 1;
        }
    }

    if(igIsKeyPressed_Bool(ImGuiKey_Tab, false))
    {
        floating_editor_show = !floating_editor_show;
        if(floating_editor_show)
        {
            layer_selected = layer_last_selected;
        }
    }

    if(!floating_editor_show)
    {
        layer_selected = 1;
    }
    else
    {
        layer_last_selected = layer_selected;
    }

    if(camera_view_changed)
    {
        camera_compute_view(&game->camera);
    }

    vec2 mouse_pos;
    i_get_mouse_pos_normalize(mouse_pos, mouse_pos+1);
    camera_screen_to_world(&game->camera, mouse_pos, cursor_pos);

    if(layer_selected != 1)
    {
        int edition = 0;
        if(i_button_down(GLFW_MOUSE_BUTTON_1)) edition = 1;
        if(i_button_down(GLFW_MOUSE_BUTTON_2)) edition = -1;

        ivec2 cursor_grid_ipos;
        cursor_grid_ipos[0] = (int)roundf(cursor_pos[0]+((float)level_width)/2-0.5f);
        cursor_grid_ipos[1] = (int)roundf(-cursor_pos[1]+((float)level_height)/2-0.5f);
        int is_inside = cursor_grid_ipos[0] >= 0 && cursor_grid_ipos[0] < level_width
                    && cursor_grid_ipos[1] >= 0 && cursor_grid_ipos[1] < level_height;

        if(edition && layer_selected == 0)
        {
            if(is_inside)
            {
                int index =cursor_grid_ipos[0]+cursor_grid_ipos[1]*level_width;
                game->level.tilemap.solidity[index] = edition > 0 ? STILE_SOLID : STILE_EMPTY;
            }
        }
        if(edition && layer_selected >= 2)
        {
            if(is_inside)
            {
                Tile *layer = tilemap_get_layer_by_index(tilemap, layer_selected - 2);
                Tile *tile_to_draw = layer + cursor_grid_ipos[0] + cursor_grid_ipos[1] * level_width;
                if(edition > 0)
                {
                    *tile_to_draw = tile_index + 1; 
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
        cursor_grid_pos[0] = roundf(cursor_pos[0]-((float)level_width)/2+0.5f)+(float)(level_width)/2-0.5f;
        cursor_grid_pos[1] = roundf(cursor_pos[1]-((float)level_height)/2+0.5f)+(float)(level_height)/2-0.5f;

        compute_transform(transform, cursor_grid_pos, size);
        draw_transformed_quad(program, transform, (vec3){1.f, 0.f, 1.f}, 0.8f);
    }

    if(floating_editor_show)
    {
        igPushID_Str("Layers");
        igBegin(floating_editor, NULL, 0);
        igSeparatorText("Layers:");
        igPushID_Int(0);
        tilemap_ig_layer(game, "Collisions", 0);
        igPopID();
        igPushID_Int(1);
        tilemap_ig_layer(game, "Entities", 1);
        igPopID();
        for (int i = 2; i < tilemap->layer_count+2; i++)
        {
            igPushID_Int(i);
            char selectable_title [10] = "Layer ";
            char number[3];
            itoa(i-2, number, 10);
            strcat(selectable_title, number);
            tilemap_ig_layer(game, selectable_title, i);
            igPopID();
        }
        if(layer_selected >= 2)
        {
            igSeparatorText("Tiles:");
            tilemap_ig_tile_selector(get_atlas_tilemap(), &tile_index, &tile_position);
        }
        igEnd();
        igPopID();
    }

    menu_bar(game);

    if(layer_selected == 1)
    {
        if(igIsMouseClicked_Bool(ImGuiMouseButton_Right, false))
        {
            ivec2 cursor_grid_pos;
            cursor_grid_pos[0] = (int)roundf(cursor_pos[0]+((float)level_width)/2-0.5f);
            cursor_grid_pos[1] = (int)roundf(-cursor_pos[1]+((float)level_height)/2-0.5f);
            if(cursor_grid_pos[0] >= 0 && cursor_grid_pos[0] < level_width
                    && cursor_grid_pos[1] >= 0 && cursor_grid_pos[0] < level_height)
            {
                igOpenPopup_Str("Menu", 0);
                glm_ivec2_copy(cursor_grid_pos, popup_last_clicked_pos);
            }
        }
        ImGuiIO *io = igGetIO_Nil();
        if(igIsMouseDown_Nil(ImGuiMouseButton_Left) && !io->WantCaptureMouse)
        {
            if(!selection_started)
            {
                glm_vec2_copy(mouse_pos, selection_pos_start);
                selection_started = 1;
            }
        }
        else
        {
            selection_started = 0;
        }
        if(selection_started)
        {
            selection_draw(mouse_pos);
            selection_ents_count = 0;
            vec2 world_pos_start;
            vec2 world_mouse_pos;
            camera_screen_to_world(&game->camera, selection_pos_start, world_pos_start);
            camera_screen_to_world(&game->camera, mouse_pos, world_mouse_pos);
            world_pos_start[0] = world_pos_start[0]+(float)tilemap->width/2-0.5f;
            world_pos_start[1] = -world_pos_start[1]+(float)tilemap->height/2-0.5f;
            world_mouse_pos[0] = world_mouse_pos[0]+(float)tilemap->width/2-0.5f;
            world_mouse_pos[1] = -world_mouse_pos[1]+(float)tilemap->height/2-0.5f;

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
                    selection_ents[selection_ents_count++] = i; 
                }
            }
        }
        unsigned int program = shaders_use_default();
        for(int i = 0; i < selection_ents_count; ++i)
        {
            struct Entity *ent = gamestate->entities+(selection_ents[i]);
            mat3 transform;
            vec2 size = {1.1f, 1.1f};
            vec2 pos;
            pos[0] = (float)ent->position[0]-(float)tilemap->width/2+0.5f;
            pos[1] = (float)tilemap->height/2-(float)ent->position[1]-0.5f;

            compute_transform(transform, pos, size);
            draw_transformed_quad(program, transform, (vec3){1.f, 0.f, 1.f}, 0.2f);
        }
    }

    ImGuiPopupFlags flags = ImGuiPopupFlags_None;
    if(igBeginPopupContextItem("Menu", flags))
    {
        int found = 0;
        for(int entity_index = 0; entity_index < gamestate->entity_count; ++entity_index)
        {
            struct Entity *ent = gamestate->entities+entity_index;
            if(glm_ivec2_eqv(popup_last_clicked_pos, ent->position))
            {
                ++found;
                const int number_max_char = 3;
                char str_name[30];
                char str_number[number_max_char];
                strcpy(str_name, get_entity_name(ent->type));
                itoa(get_entity_index(gamestate, ent), str_number, 10);
                strcat(str_name, " (");
                strcat(str_name, str_number);
                strcat(str_name, ")");
                if(igBeginMenu(str_name, true))
                {
                    if(ent->type == ENTITY_SLOT || ent->type == ENTITY_KEY)
                    {
                        if(igSelectable_Bool("Edit", false, 0, (struct ImVec2){0,0}))
                        {
                            edition_entity = ent;
                        }
                    }
                    if(igSelectable_Bool("Move", false, 0, (struct ImVec2){0,0}))
                    {
                        reposition_entity = ent;
                    }
                    if(igSelectable_Bool("Remove", false, 0, (struct ImVec2){0,0}))
                    {
                        deletion_index = get_entity_index(gamestate, ent);
                    }
                    igEndMenu();
                }
            }
        }
        if(selection_ents_count)
        {
            const int number_max_char = 3;
            char str_name[30] = "Selection (";
            char str_number[number_max_char];
            itoa(selection_ents_count, str_number, 10);
            strcat(str_name, str_number);
            strcat(str_name, " ents)");
            igPushID_Str("Selection");
            if(igBeginMenu(str_name, true))
            {
                if(igSelectable_Bool("Move", false, 0, (struct ImVec2){0,0}))
                {
                    reposition_selection = 1;
                    glm_ivec2_zero(reposition_selection_delta);
                }
                if(igSelectable_Bool("Remove", false, 0, (struct ImVec2){0,0}))
                {
                    deletion_selection = 1;
                }
                igEndMenu();
            }
            igPopID();
        }
        if(!found && !selection_ents_count)
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
                    creation_type = i;
                    glm_ivec2_copy(popup_last_clicked_pos, creation_position);
                    creation_action_target = -1;
                }
            }
            igEndMenu();
        }
        igEndPopup();
    }
    if(reposition_entity)
    {
        igOpenPopup_Str(window_move, 0);
    }
    else if(creation_type)
    {
        igOpenPopup_Str(window_create, 0);
    }
    else if(deletion_index >= 0)
    {
        igOpenPopup_Str(window_deletion, 0);
    }
    else if(edition_entity)
    {
        igOpenPopup_Str(window_edition, 0);
    }
    else if(saving)
    {
        igOpenPopup_Str(window_saving, 0);
    }
    else if(opening)
    {
        igOpenPopup_Str(window_opening, 0);
    }
    else if(sequence_opening)
    {
        igOpenPopup_Str(window_sequence_opening, 0);
    }
    else if(reposition_selection)
    {
        igOpenPopup_Str(window_move_selection, 0);
    }
    else if(deletion_selection)
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
        glm_ivec2_copy(reposition_selection_delta, old_delta);

        if(ig_position_input("entPos", reposition_selection_delta))
        {
            ivec2 deltas_delta;
            glm_ivec2_sub(reposition_selection_delta, old_delta, deltas_delta);
            for(int i = 0; i < selection_ents_count; ++i)
            {
                struct Entity *ent = gamestate->entities+(selection_ents[i]);
                glm_ivec2_add(ent->position, deltas_delta, ent->position);
            }
        }
        if(igButton("Comfirm", (struct ImVec2){0,0}))
        {
            reposition_selection = 0;
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
            for(int i = selection_ents_count-1; i>=0; --i)
            {
                remove_entity(gamestate, selection_ents[i]);
            }
            selection_ents_count = 0;
            deletion_selection = 0;
            igCloseCurrentPopup();
        }
        igSameLine(0,-1);
        if(igButton("No", (struct ImVec2){0,0}))
        {
            deletion_selection = 0;
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

        ig_position_input("entPos", reposition_entity->position);
        if(igButton("Comfirm", (struct ImVec2){0,0}))
        {
            reposition_entity = NULL; 
            igCloseCurrentPopup();
        }
        igEndPopup();
    }

    ///////////////////////
    //  ENTITY CREATION  //
    ///////////////////////
    if(igBeginPopupModal(window_create, NULL, ImGuiWindowFlags_AlwaysAutoResize))
    {
        igText(get_entity_name(creation_type));
        igSeparator();
        igText("Entity position:");
        igInputInt2("##position", creation_position, ImGuiInputTextFlags_None);
        if(creation_type == ENTITY_KEY)
        {
            edit_entity_key(&creation_key);
        }
        if(creation_type == ENTITY_SLOT)
        {
            edit_entity_slot(gamestate, &creation_action_target, &creation_action);
        }
        if(igButton("Comfirm", (struct ImVec2){0,0}))
        {
            switch(creation_type)
            {
                case ENTITY_NONE:
                    break;
                case ENTITY_KEY:
                    create_key_block_at(gamestate, creation_position[0], creation_position[1], creation_key);
                    break;
                case ENTITY_SLOT:
                    create_slot_at(gamestate, creation_position[0], creation_position[1], creation_action, creation_action_target);
                    break;
                case ENTITY_DOOR:
                    create_door_at(gamestate, creation_position[0], creation_position[1]);
                    break;
                default:
                    create_movable_at(gamestate, creation_position[0], creation_position[1], creation_type);
                    break;
            }
            creation_type = 0;
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
            remove_entity(gamestate, deletion_index);
            deletion_index = -1;
            igCloseCurrentPopup();
        }
        igSameLine(0,-1);
        if(igButton("No", (struct ImVec2){0,0}))
        {
            deletion_index = -1;
            igCloseCurrentPopup();
        }

        igEndPopup();
    }
    
    //////////////////////
    //  ENTITY EDITION  //
    //////////////////////
    if(igBeginPopupModal(window_edition, NULL, ImGuiWindowFlags_AlwaysAutoResize))
    {
        switch(edition_entity->type)
        {
            case ENTITY_KEY:
                {
                    struct KeyBlockData *key = gamestate->key_block_data+edition_entity->data_index;
                    edit_entity_key(&key->key);
                }
                break;
            case ENTITY_SLOT:
                {
                    struct SlotData *slot = gamestate->slot_data+edition_entity->data_index;
                    edit_entity_slot(gamestate, &slot->action.target_entity, &slot->action.type);
                }
                break;
            default:
                igCloseCurrentPopup();
                break;
        }

        if(igButton("Comfirm", (struct ImVec2){0,0}))
        {
            edition_entity = NULL;
            igCloseCurrentPopup();
        }
        igEndPopup();
    }
    
    ///////////////
    //  SAVING   //
    ///////////////
    char save_path[150];
    int save_result = ig_save_path_input_popup(window_saving, file_current, save_path, resources_path, file_suffix);
    if(save_result > 0)
    {
        struct Level level_to_save = *get_current_level(game);
        level_to_save.gamestate = *get_current_gamestate(game);
        serialize_level(&level_to_save, save_path);
        saving = 0;
    }
    else if(save_result < 0)
    {
        saving = 0;
    }

    ////////////////
    //  OPENING   //
    ////////////////
    char opening_path[150];
    int opening_result = ig_open_path_input_popup(window_opening, file_current, opening_path, resources_path, file_suffix);
    if(opening_result > 0)
    {
        opening = 0;
        deserialize_level_into_game(game, opening_path);
    }
    else if(opening_result < 0)
    {
        opening = 0;
    }

    /////////////////////////
    //  OPENING SEQUENCE   //
    /////////////////////////
    char opening_sequence_path[150];
    int opening_sequence_result = ig_open_path_input_popup(window_sequence_opening, sequence_current, opening_sequence_path, resources_path, sequence_suffix);
    if(opening_sequence_result > 0)
    {
         
        deserialize_sequence_into_game(game, opening_sequence_path);
        sequence_opening = 0;
    }
    else if(opening_sequence_result < 0)
    {
        sequence_opening = 0;
    }
}
