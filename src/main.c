#include <stdio.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "cglm/ivec2.h"
#include "game.h"
#include "level_serialization.h"
#include "texture.h"
#include "window/input.h"
#include "window/window.h"
#include "rendering/rendering.h"


double last_time;
double new_time;
float delta_time;

float time_between_input = 0.3f;
float last_action_time = 0.f;


void request_new_turn(struct Game *game, enum PlayerAction action)
{
    print_level(&game->level);
    struct Entity *player =get_player(&game->level);
    if(player == NULL)
    {
        perror("Player not found in that level.");
        return;
    }

    if(action == PA_UNDO)
    {
        if(!history_empty(game))
        {
           game->level = history_pop(game); 
        }
        return;
    }
    if(action == PA_DOOR_OPEN)
    {
        game->level.is_door_opened = 1;
    }

    history_register(game);

    ivec2 player_movement;
    player_movement[0] = 0;
    player_movement[1] = 0;
    if(action == PA_UP)
    {
       player_movement[1] = -1;
    }
    else if (action == PA_DOWN)
    {
        player_movement[1] = 1;
    }
    else if (action == PA_LEFT)
    {
        player_movement[0] = -1;
    }
    else if (action == PA_RIGHT)
    {
        player_movement[0] = 1;
    }

    ivec2 new_pos;
    glm_ivec2_add(player->position, player_movement, new_pos);

    push_entity(&game->level, player, player_movement);
}

void request_new_turn_if_needed(struct Game *game)
{
    enum PlayerAction player_action = i_get_current_player_action();
    if(player_action == PA_NONE) return;

    float new_time_f = (float)new_time;

    if(i_player_action_just_changed())
    {
        last_action_time = new_time_f;
        request_new_turn(game, player_action);
        return;
    }

    if(new_time_f < last_action_time + time_between_input)
    {
        return; 
    }

    last_action_time = new_time_f;
    request_new_turn(game, player_action);
}

void update_key_blocks(struct Game *game)
{
    for(int i = 0; i < game->level.entity_count; ++i)
    {
        struct Entity *ent = game->level.entities+i;
        if(ent->type != ENTITY_KEY) continue;

        struct KeyBlockData *key_data = ent->data;
        key_data->is_pressed = i_key_down(key_data->key);
        if(i_key_pressed(key_data->key))
        {
            struct Entity *slot = get_slot_at(&game->level, ent->position);
            if(slot != NULL)
            {
                struct SlotData *slot_data = slot->data;
                request_new_turn(game, slot_data->action);
            }
        }
    }
}

int main()
{
    GLFWwindow* window;
    if (!initGl(&window) || window == NULL)
    {
        perror("Failed to initialize OpenGL context\n");
        return 1;
    }


    initialize_renderer();
    i_initialize(window);
    load_default_images(); 
 
    struct Game game;

    get_default_level(&game.level_start);
    load_level(&game, game.level_start);

    last_time = glfwGetTime();

    while (!glfwWindowShouldClose(window))
    {
        GLenum err;
        while ((err = glGetError()) != GL_NO_ERROR) {
            printf("OpenGL error: 0x%X\n", err);
        }
        new_time = get_time();
        delta_time = (float)(new_time - last_time);
        last_time = new_time;

        i_process(window);
        update_key_blocks(&game);
        request_new_turn_if_needed(&game);

        if(game.level.is_door_reached)
        {
            game.level = game.level_start;
        }

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        render_level(&game.level);

        glfwSwapBuffers(window);
        i_clear_pressed();
        glfwPollEvents();
    }

    serialize_level(game.level, "resources/level/test.level");

    glfwTerminate();
    return 0;
}
