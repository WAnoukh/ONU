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
    const int loc = glGetUniformLocation(program, loc_name);
    glUniformMatrix4fv(loc, 1, GL_FALSE, (GLfloat*)matrice);
}

static void shader_set_mat3(const GLuint program, const char *loc_name, mat3 matrice)
{
    const int loc = glGetUniformLocation(program, loc_name);
    glUniformMatrix3fv(loc, 1, GL_FALSE, (GLfloat*)matrice);
}

static void shader_set_vec3(const GLuint program, const char *loc_name, vec3 vector)
{
    const int loc = glGetUniformLocation(program, loc_name);
    glUniform3f(loc, vector[0], vector[1], vector[2]);
}

static void shader_set_vec2(const GLuint program, const char *loc_name, float x, float y)
{
    const int loc = glGetUniformLocation(program, loc_name);
    glUniform2f(loc, x, y);
}

static inline void shader_set_int(const GLuint program, const char *loc_name, int number)
{
    const int loc = glGetUniformLocation(program, loc_name);
    glUniform1i(loc, number);
}
#endif //SHADER_H
