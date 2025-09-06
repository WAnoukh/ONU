#include "editor.h"
#include "GLFW/glfw3.h"
#include "cglm/types.h"
#include "game.h"
#include "level.h"
#include "transform.h"
#include "window/input.h"
#define CIMGUI_DEFINE_ENUMS_AND_STRUCTS
#define CIMGUI_USE_OPENGL3
#define CIMGUI_USE_GLFW
#include "cimgui.h"
#include "cimgui_impl.h"


bool editing_tilemap = false;
ivec2 popup_last_clicked_pos;
struct Entity *reposition_entity = NULL;
enum EntityType creation_type = ENTITY_NONE;
ivec2 creation_position;
vec2 cursor_pos;

ImGuiContext* ctx;

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
    //ImGuiIO* io = igGetIO_ContextPtr(ctx);
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

void editor_update(struct Game *game, GLFWwindow *window)
{
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

    if(camera_view_changed)
    {
        camera_compute_view(&game->camera);
    }

    vec2 mouse_pos;
    i_get_mouse_pos_normalize(mouse_pos, mouse_pos+1);
    camera_screen_to_world(&game->camera, mouse_pos, cursor_pos);

    struct Level *level = &game->level;

    if(editing_tilemap)
    {
        int edition = 0;
        if(i_button_down(GLFW_MOUSE_BUTTON_1)) edition = 1;
        if(i_button_down(GLFW_MOUSE_BUTTON_2)) edition = -1;

        if(edition)
        {
            ivec2 cursor_grid_pos;
            cursor_grid_pos[0] = (int)roundf(cursor_pos[0]+((float)level->width)/2);
            cursor_grid_pos[1] = (int)roundf(-cursor_pos[1]+((float)level->height)/2);
            if(cursor_grid_pos[0] >= 0 && cursor_grid_pos[0] < level->width
                    && cursor_grid_pos[1] >= 0 && cursor_grid_pos[0] < level->height)
            {
                int index =cursor_grid_pos[0]+cursor_grid_pos[1]*level->width;
                game->level.tilemap[index].type = edition > 0 ? TILE_WALL : TILE_EMPTY;
            }
        }
        unsigned int program = shaders_use_default();
        mat3 transform;
        vec2 size = {0.2f, 0.2f};
        vec2 cursor_grid_pos;
        cursor_grid_pos[0] = roundf(cursor_pos[0]-((float)level->width)/2)+(float)(level->width)/2;
        cursor_grid_pos[1] = roundf(cursor_pos[1]-((float)level->height)/2)+(float)(level->height)/2;

        compute_transform(transform, cursor_grid_pos, size);
        draw_transformed_quad(program, transform, (vec3){1.f, 0.f, 1.f});
    }

    igBegin("LevelEditor", NULL, 0);
    igCheckbox("Edit Tilemap", &editing_tilemap);
    ivec2 tilemapSize;
    tilemapSize[0] = game->level.width;
    tilemapSize[1] = game->level.height;
    if(igInputInt2("Tilemap size: ", tilemapSize, ImGuiInputTextFlags_None) && igIsKeyPressed_Bool(ImGuiKey_Enter, false))
    {
        resize_level(&game->level, tilemapSize[0], tilemapSize[1]);
    }
    igEnd();
    
    if(!editing_tilemap && igIsMouseClicked_Bool(ImGuiMouseButton_Right, false))
    {
        ivec2 cursor_grid_pos;
        cursor_grid_pos[0] = (int)roundf(cursor_pos[0]+((float)level->width)/2);
        cursor_grid_pos[1] = (int)roundf(-cursor_pos[1]+((float)level->height)/2);
        if(cursor_grid_pos[0] >= 0 && cursor_grid_pos[0] < level->width
                && cursor_grid_pos[1] >= 0 && cursor_grid_pos[0] < level->height)
        {
            igOpenPopup_Str("Menu", 0);
            glm_ivec2_copy(cursor_grid_pos, popup_last_clicked_pos);
        }
    }
    ImGuiPopupFlags flags = ImGuiPopupFlags_None;
    if(igBeginPopupContextItem("Menu", flags))
    {
        int found = 0;
        for(int entity_index = 0; entity_index < level->entity_count; ++entity_index)
        {
            struct Entity *ent = level->entities+entity_index;
            if(glm_ivec2_eqv(popup_last_clicked_pos, ent->position))
            {
                ++found;
                if(igBeginMenu(get_entity_name(ent->type), true))
                {
                    if(igSelectable_Bool("change position", false, 0, (struct ImVec2){0,0}))
                    {
                        reposition_entity = ent;
                    }
                    igEndMenu();
                }
            }
        }
        if(!found)
        {
            igText("No entity here.");
        }
        if(igBeginMenu("Create entity", true))
        {
            for(enum EntityType i = 1; i < ENTITY_COUNT; ++i)
            {
                if(igSelectable_Bool(get_entity_name(i), false, 0, (struct ImVec2){0,0}))       
                {
                    creation_type = i;
                    creation_position[0] = 0;
                    creation_position[1] = 0;
                }
            }
            igEndMenu();
        }
        igEndPopup();
    }
    if(reposition_entity)
    {
        igOpenPopup_Str("reposition", 0);
    }
    else if(creation_type)
    {
        igOpenPopup_Str("create", 0);
    }
    struct ImVec2 center;
    ImGuiViewport_GetCenter(&center, igGetMainViewport());
    igSetNextWindowPos(center, ImGuiCond_Appearing, (struct ImVec2){0.5f,0.5f});
    if(igBeginPopupModal("reposition", NULL, ImGuiWindowFlags_AlwaysAutoResize))
    {
        igText("Entity position:");
        igInputInt2("Tilemap size: ", reposition_entity->position, ImGuiInputTextFlags_None);
        if(igButton("Comfirm", (struct ImVec2){0,0}))
        {
            reposition_entity = NULL; 
            igCloseCurrentPopup();
        }
        igEndPopup();
    }
    if(igBeginPopupModal("create", NULL, ImGuiWindowFlags_AlwaysAutoResize))
    {
        igText("Entity position:");
        igInputInt2("Tilemap size: ", creation_position, ImGuiInputTextFlags_None);
        if(igButton("Comfirm", (struct ImVec2){0,0}))
        {
            creation_type = 0;
            igCloseCurrentPopup();
        }
        igEndPopup();
    }
}
