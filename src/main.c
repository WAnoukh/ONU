#include "game.h"
#include "level_serialization.h"
#include "texture.h"
#include "transform.h"
#include "window/input.h"
#include "window/window.h"
#include "rendering/rendering.h"
#define CIMGUI_DEFINE_ENUMS_AND_STRUCTS
#define CIMGUI_USE_OPENGL3
#define CIMGUI_USE_GLFW
#include "cimgui.h"
#include "cimgui_impl.h"

#include <stdio.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

ivec2 directions[] = {{0, -1},{0, 1},{-1, 0},{1, 0}};
vec2 cursor_pos;

int process_targeted_action(struct Game *game, int entity_index, enum ActionType action_type)
{
    if(action_type != ACTION_UP && action_type != ACTION_DOWN && action_type != ACTION_LEFT && action_type != ACTION_RIGHT)
    {
        return 0;
    }
    int dir_index = (int)(action_type - ACTION_UP);
    struct Entity *ent = game->level.entities+entity_index;

    push_entity(&game->level, ent, directions[dir_index]);
    return 1;
}

void request_new_turn(struct Game *game, struct Action action)
{
    if(action.type == ACTION_UNDO)
    {
        if(!history_empty(game))
        {
            game->level = history_pop(game); 
        }
        return;
    }
    if(action.type == ACTION_DOOR_OPEN)
    {
        game->level.is_door_opened = 1;
    }

    history_register(game);

    process_targeted_action(game, action.target_entity, action.type);
}

void update_key_blocks(struct Game *game)
{
    for(int i = 0; i < game->level.entity_count; ++i)
    {
        struct Entity *ent = game->level.entities+i;
        if(ent->type != ENTITY_KEY) continue;

        struct KeyBlockData *key_data = game->level.key_block_data+ent->data_index;
        key_data->is_pressed = i_key_down(key_data->key);
        if(i_key_pressed(key_data->key))
        {
            struct Entity *slot = get_slot_at(&game->level, ent->position);
            if(slot != NULL)
            {
                struct SlotData *slot_data = game->level.slot_data+slot->data_index;
                request_new_turn(game, slot_data->action);
            }
        }
    }
}

bool editing_tilemap = false;

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

    if(editing_tilemap)
    {
        int edition = 0;
        if(i_button_down(GLFW_MOUSE_BUTTON_1)) edition = 1;
        if(i_button_down(GLFW_MOUSE_BUTTON_2)) edition = -1;

        struct Level *level = &game->level;
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

}

struct Game initialize_game()
{
    struct Game game;

    get_default_level(&game.level_start);

    game.last_time = glfwGetTime();
    game.camera.zoom = 0.2f;
    game.camera.pan[0] = 0;
    game.camera.pan[1] = 0;
    camera_compute_view(&game.camera);
    return game;
}

int main()
{
    int do_ser = 0;
    int do_deser = 0;
    int editor = 1;

    GLFWwindow* window;
    if (!initGl(&window) || window == NULL)
    {
        perror("Failed to initialize OpenGL context\n");
        return 1;
    }
    ImGuiContext* ctx = igCreateContext(NULL);
    if (ctx) {
        printf("Dear ImGui context created!\n");
    }
    else
    {
        return 1;
    }
    ImGuiIO* io = igGetIO_ContextPtr(ctx);
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 130");

    const char* level_path="resources/level/test.level";

    struct Game game = initialize_game();
    initialize_renderer(&game.camera);
    i_initialize(window);
    load_default_images(); 

    struct Level loaded_level;
    if(do_deser && deserialize_level(&loaded_level, level_path))
    {
        //game.level_start = loaded_level;
    }
    load_level(&game, game.level_start);

    while (!glfwWindowShouldClose(window))
    {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        igNewFrame();

        GLenum err;
        while ((err = glGetError()) != GL_NO_ERROR) {
            printf("OpenGL error: 0x%X\n", err);
        }
        game.new_time = get_time();
        game.delta_time = (float)(game.new_time - game.last_time);
        game.last_time = game.new_time;

        if(is_framebuffer_resized())
        {
            camera_compute_view(&game.camera);
            clear_framebuffer_resized();
        }

        i_process(window);
        update_key_blocks(&game);

        if(game.level.is_door_reached)
        {
            load_level(&game, game.level_start);
        }

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        render_level(&game.level);

        if(editor) editor_update(&game, window);

        igRender();
        ImGui_ImplOpenGL3_RenderDrawData(igGetDrawData());
        glfwSwapBuffers(window);
        i_clear_pressed();
        glfwPollEvents();
    }

    if(do_ser)
    {
        serialize_level(game.level, level_path);
    }
    igDestroyContext(ctx);
    glfwTerminate();
    return 0;
}
