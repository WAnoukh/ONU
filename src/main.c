#include <stdio.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "cglm/ivec2.h"
#include "level.h"
#include "window/input.h"
#include "window/window.h"
#include "rendering/rendering.h"

#define HISTORY_SIZE 1000

struct Level main_level;

struct Level history[HISTORY_SIZE];
int history_size = 0;

double last_time;
double new_time;
float delta_time;

float time_between_input = 0.3f;
float last_action_time = 0.f;

int history_register(struct Level *level)
{
    if(history_size >= HISTORY_SIZE) {
        perror("History full");
        return 0;
    }
    history[history_size++] = *level;
    return 1;
}

int history_empty()
{
    return history_size <= 0;
}

struct Level history_pop()
{
   if(history_size <= 0) 
   {
        perror("Trying to pop an empty history\n");
        exit(1);
   }
   return history[--history_size];
}

void request_new_turn(enum PlayerAction action)
{
    print_level(&main_level);
    struct Entity *player =get_player(&main_level);
    if(player == NULL)
    {
        perror("Player not found in that level.");
        return;
    }

    if(action == PA_UNDO)
    {
        if(!history_empty())
        {
           main_level = history_pop(); 
        }
        return;
    }

    history_register(&main_level);

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

    if(!is_tilemap_solid_at(&main_level, new_pos))
    {
        push_entity(&main_level, player, player_movement);
    }
}

void request_new_turn_if_needed()
{
    enum PlayerAction player_action = i_get_current_player_action();
    if(player_action == PA_NONE) return;

    float new_time_f = (float)new_time;

    if(i_player_action_just_changed())
    {
        last_action_time = new_time_f;
        request_new_turn(player_action);
        return;
    }

    if(new_time_f < last_action_time + time_between_input)
    {
        return; 
    }

    last_action_time = new_time_f;
    request_new_turn(player_action);
}

void update_key_blocks()
{
    for(int i = 0; i < main_level.key_block_data_count; ++i)
    {
        struct KeyBlockData *key_data = main_level.key_block_data+i;
        key_data->is_pressed = i_key_down(key_data->key);
        if(key_data->is_pressed)
        {
            printf("Nice \n");
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
 
    get_default_level(&main_level);
    last_time = glfwGetTime();


    while (!glfwWindowShouldClose(window))
    {
        new_time = get_time();
        delta_time = (float)(new_time - last_time);
        last_time = new_time;

        i_process(window);
        update_key_blocks();
        request_new_turn_if_needed();

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        render_level(&main_level);

        glfwSwapBuffers(window);
        i_clear_pressed();
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
