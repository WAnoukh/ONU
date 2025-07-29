#ifndef SHADER_H
#define SHADER_H
#include "glad/glad.h"
#include "cglm/cglm.h"

struct shader_program
{
    char *vertex;
    char *fragment;
    unsigned int program_id;
};

GLuint create_shader_program(const char *vertex_shader_path,const char *fragment_shader_path);

static void shader_set_mat4(const GLuint program, const char *loc_name, mat4 matrice)
{
    int loc = glGetUniformLocation(program, loc_name);
    glUniformMatrix4fv(loc, 1, GL_FALSE, (GLfloat*)matrice);
}

static void shader_set_mat3(const GLuint program, const char *loc_name, mat3 matrice)
{
    int loc = glGetUniformLocation(program, loc_name);
    glUniformMatrix3fv(loc, 1, GL_FALSE, (GLfloat*)matrice);
}
#endif //SHADER_H
