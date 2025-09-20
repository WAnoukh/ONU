#include "window/window.h"
#include <stdio.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#define CIMGUI_DEFINE_ENUMS_AND_STRUCTS
#include "cimgui.h"
#include "window/input.h"

const int SCR_WIDTH = 800;
const int SCR_HEIGHT = 600;
int is_framebuffer_resized_flag = 0;

float framebuffer_ratio = (float)SCR_WIDTH/(float)SCR_HEIGHT;
int framebuffer_width = SCR_WIDTH, framebuffer_height = SCR_HEIGHT;

float scale_x = 1;
float scale_y = 1;

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
    framebuffer_ratio = (float)width/(float)height;
    framebuffer_width = width;
    framebuffer_height = height;
    is_framebuffer_resized_flag = 1;
    ImGuiIO* io = igGetIO_Nil();
    io->DisplayFramebufferScale.x = 1.0f;
    io->DisplayFramebufferScale.y = 1.0f;
    io->DisplaySize.x = (float)width;
    io->DisplaySize.y = (float)height;

    int win_w, win_h;
}

void scroll_callback(GLFWwindow* window, double x_offset, double y_offset)
{
    ImGuiIO *io = igGetIO_Nil();
    if(io->WantCaptureMouse) return;
    register_scroll_state((float)x_offset, (float)y_offset);
}

int is_framebuffer_resized()
{
    return is_framebuffer_resized_flag;
}

void clear_framebuffer_resized()
{
    is_framebuffer_resized_flag = 0;
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int opt)
{
    ImGuiIO *io = igGetIO_Nil();
    if(io->WantCaptureMouse && action != GLFW_RELEASE) return;
    register_key_state(key, action);
}

void mouse_button_callback(GLFWwindow *window, int button, int action, int mode)
{
    ImGuiIO *io = igGetIO_Nil();
    if(io->WantCaptureMouse && action != GLFW_RELEASE) return;
    register_mouse_state(button, action); 
}

void mouse_move_callback(GLFWwindow *window, double x_offset, double y_offset)
{
    register_mouse_move_state(x_offset, y_offset);
}

void APIENTRY glDebugOutput(GLenum source, GLenum type, GLuint id,
                            GLenum severity, GLsizei length,
                            const GLchar *message, const void *userParam)
{
    if(type == GL_DEBUG_TYPE_OTHER) 
    {
        return;
    }
    fprintf(stderr, "GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
            (type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : ""),
            type, severity, message);
}

int initGl(GLFWwindow **window)
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE); 

    *window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (*window == NULL)
    {
        perror("Failed to create GLFW window\n");
        glfwTerminate();
        return 0;
    }
    
    if (glfwRawMouseMotionSupported())
    {
        glfwSetInputMode(*window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
    }
    else
    {
        printf("Error initializing the window : Raw mouse not supported");
    }

    glfwMakeContextCurrent(*window);
    glfwSetFramebufferSizeCallback(*window, framebuffer_size_callback);
    glfwSetScrollCallback(*window, scroll_callback);
    glfwSetKeyCallback(*window, key_callback);
    glfwSetMouseButtonCallback(*window, mouse_button_callback);
    glfwSetCursorPosCallback(*window, mouse_move_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        perror("Failed to initialize GLAD\n");
        return 0;
    }

    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    glDebugMessageCallback(glDebugOutput, 0);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glfwWindowHint(GLFW_SCALE_TO_MONITOR, GLFW_FALSE);
    return 1;
}

double get_time()
{
   return glfwGetTime();
}

float window_get_screen_ratio()
{
    return framebuffer_ratio;
}

void window_get_size(int *width, int *height)
{
    *width = framebuffer_width;
    *height = framebuffer_height;
}
