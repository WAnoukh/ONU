#include <stdio.h>

#include "Rendering/Shader.h"
#include <stdlib.h>

const   int MAX_SHADER_LENGTH = 1024;

int load_shader_from_file(const char *path, char** out_shader)
{

    *out_shader = malloc(sizeof(char) * MAX_SHADER_LENGTH);
    (*out_shader)[MAX_SHADER_LENGTH - 1] = '\0';

    FILE *file = fopen(path, "r");
    if (!file)
    {
        perror("File not found\n");
        return 0;
    }

    char *cur = *out_shader;
    int read_char;
    int i = 0;
    do
    {
        read_char = fgetc(file);
        *cur++ = (char)read_char;
        i++;
    }while (read_char != EOF && cur < *out_shader + MAX_SHADER_LENGTH - 1);
    *(cur-1) = '\0';
    return 1;
}

GLuint compile_shader(const char* shader_source, GLenum shader_type)
{
    if (shader_type != GL_VERTEX_SHADER
        && shader_type != GL_FRAGMENT_SHADER
        && shader_type != GL_GEOMETRY_SHADER)
    {
        perror("Shader type not valid\n");
        return 0;
    }

    GLuint shader = glCreateShader(shader_type);
    if (shader == 0)
    {
        perror("Failed to create shader\n");
        return 0;
    }

    glShaderSource(shader, 1, &shader_source, NULL);
    glCompileShader(shader);

    int success;
    char infoLog[512];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(shader, 512, NULL, infoLog);
        char* type = shader_type == GL_VERTEX_SHADER ? "vertex" : (shader_type == GL_FRAGMENT_SHADER ? "fragment" : "geometry");
        printf("ERROR::SHADER::%s::COMPILATION_FAILED \n %s\n", type, infoLog);
        return 0;
    }

    return shader;
}

GLuint create_shader_program(const char *vertex_shader_path, const char *fragment_shader_path)
{
    char *vertex_source, *fragment_source;
    load_shader_from_file(vertex_shader_path, &vertex_source);
    load_shader_from_file(fragment_shader_path, &fragment_source);

    GLuint vertex_shader = compile_shader(vertex_source, GL_VERTEX_SHADER);
    free(vertex_source);
    
    GLuint fragment_shader = compile_shader(fragment_source, GL_FRAGMENT_SHADER);
    free(fragment_source);

    if (vertex_shader == 0)
    {
        perror("Vertex shader compilation failed\n");
        return 0;
    }
    if (fragment_shader == 0)
    {
        perror("Fragment shader compilation failed\n");
        return 0;
    }
    unsigned int program = glCreateProgram();
    glAttachShader(program, vertex_shader);
    glAttachShader(program, fragment_shader);
    glLinkProgram(program);

    int success;
    char infoLog[512];
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(program, 512, NULL, infoLog);
        printf("ERROR::SHADER::PROGRAM::LINKING_FAILED \n %s\n", infoLog);
    }
    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);
    return program;
}
