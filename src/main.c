#include <stdio.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "cglm/ivec2.h"
#include "level.h"
#include "window/input.h"
#include "window/window.h"
#include "rendering/rendering.h"

struct Level main_level;

double last_time;
double new_time;
float delta_time;

float time_between_input = 0.3f;
float last_action_time = 0.f;

void request_new_turn(enum PlayerAction action)
{
    print_level(&main_level);
    struct Entity *player =get_player(&main_level);
    if(player == NULL)
    {
        perror("Player not found in that level.");
        return;
    }

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
        //glm_ivec2_copy(new_pos, player->position);
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
 
    main_level = get_default_level();
    last_time = glfwGetTime();


    while (!glfwWindowShouldClose(window))
    {
        new_time = get_time();
        delta_time = (float)(new_time - last_time);
        last_time = new_time;

        i_process(window);
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
