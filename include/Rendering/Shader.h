#ifndef SHADER_H
#define SHADER_H
#include "glad/glad.h"

struct shader_program
{
    char *vertex;
    char *fragment;
    unsigned int program_id;
};

GLuint create_shader_program(const char *vertex_shader_path,const char *fragment_shader_path);

#endif //SHADER_H
