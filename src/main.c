#include <stdio.h>
#include <stdlib.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "Window/Input.h"
#include "Window/Window.h"
#include "Entity.h"
#include "Rendering/Rendering.h"
struct entity *entities;
int entity_count;

double last_time;
double new_time;
float delta_time;

void initialize_entities()
{
    entity_count = 2;
    entities = (struct entity*)calloc(entity_count, sizeof(struct entity));
    struct entity *player = entities;
    player->type = Player;
    player->color[0] = 1;
    player->width = 1;
    player->height = 1.2f;
    struct entity *guard = entities+1;
    guard->type = Guard;
    guard->color[1] = 1;
    guard->width = 1;
    guard->height = 1;
}

void update_player()
{
    const float player_speed = 4;
    struct entity *player = entities;
    vec2 input;
    get_player_input(input);
    glm_vec2_scale(input, player_speed * delta_time, input);
    glm_vec2_add(player->pos, input, player->pos);
}

void update_entities()
{
    entities[1].pos[0] = sinf((float)new_time);
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
    initialize_entities();
    initialize_input(window);

    last_time = glfwGetTime();

    while (!glfwWindowShouldClose(window))
    {
        processInput(window);
        new_time = glfwGetTime();
        delta_time = (float)(new_time - last_time);
        last_time = new_time;

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        update_player();
        update_entities();
        render_entities(entities, entity_count);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}