#ifndef MATERIAL_H
#define MATERIAL_H

#include "rendering/rendering.h"

typedef struct PbrMaterial
{
    GLuint  texture_albedo;
    GLuint  texture_normal;
    GLuint  texture_roughness;
    GLuint  texture_metallic;
    GLuint  texture_ao;
    GLuint  texture_alpha;

    int     albedo_enable;
    int     normal_enable;
    int     roughness_enable;
    int     metallic_enable;
    int     ao_enable;
    int     alpha_enable;
} PbrMaterial;

PbrMaterial pbrmat_load(char *albedo_path, char *normal_path, char *roughness_path, char *metallic_path, char *ao_path, char *alpha_path);

void shader_set_pbrmat(GLuint shader, PbrMaterial *mat);

#endif // MATERIAL_H
