#include <stdio.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "cglm/mat3.h"
#include "game.h"
#include "level_serialization.h"
#include "texture.h"
#include "window/input.h"
#include "window/window.h"
#include "rendering/rendering.h"

ivec2 directions[] = {{0, -1},{0, 1},{-1, 0},{1, 0}};

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

void camera_zoom(struct Camera *camera, float amount)
{
    float zoom = camera->zoom;
    zoom += zoom * amount;
    const float max = 1.5f;
    const float min = 0.1f;
    camera->zoom = zoom;
    camera->zoom = glm_clamp(camera->zoom, min, max);
}

void camera_pan(struct Camera *camera, float x_offset, float y_offset)
{
    camera->pan[0] += x_offset;
    camera->pan[1] += y_offset;
}

void camera_compute_view(struct Camera *camera)
{
    float sx = camera->zoom / window_get_screen_ratio();
    float sy = camera->zoom;
    float tx = camera->pan[0];
    float ty = camera->pan[1];
    glm_mat3_identity(camera->view);
    camera->view[0][0] = sx;
    camera->view[1][1] = sy;
    camera->view[2][0] = tx;
    camera->view[2][1] = ty;
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

    if (i_button_down(GLFW_MOUSE_BUTTON_2))
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

    if(i_button_down(GLFW_MOUSE_BUTTON_1))
    {
        float mouse_x, mouse_y;
        i_get_mouse_pos(&mouse_x, &mouse_y);

    }
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

    const char* level_path="resources/level/test.level";

    struct Game game = initialize_game();
    initialize_renderer(&game.camera);
    i_initialize(window);
    load_default_images(); 

    struct Level loaded_level;
    if(do_deser && deserialize_level(&loaded_level, level_path))
    {
        game.level_start = loaded_level;
    }
    load_level(&game, game.level_start);

    while (!glfwWindowShouldClose(window))
    {
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
        if(editor) editor_update(&game, window);
        update_key_blocks(&game);

        if(game.level.is_door_reached)
        {
            load_level(&game, game.level_start);
        }

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        render_level(&game.level);

        glfwSwapBuffers(window);
        i_clear_pressed();
        glfwPollEvents();
    }

    if(do_ser)
    {
        serialize_level(game.level, level_path);
    }

    glfwTerminate();
    return 0;
}
