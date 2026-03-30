#include "material.h"
#include "rendering/shader.h"
#include "rendering/texture.h"

PbrMaterial pbrmat_load(char *albedo_path, char *normal_path, char *roughness_path, char *metallic_path, char *ao_path, char *alpha_path)
{
    PbrMaterial mat = {};
    
    if(albedo_path)
    {
        mat.texture_albedo = texture_load(albedo_path);
        mat.albedo_enable = 1;
    }

    if(normal_path)
    {
        mat.texture_normal = texture_load(normal_path);
        mat.normal_enable = 1;
    }

    if(roughness_path)
    {
        mat.texture_roughness = texture_load(roughness_path);
        mat.roughness_enable = 1;
    }

    if(metallic_path)
    {
        mat.texture_metallic = texture_load(metallic_path);
        mat.metallic_enable = 1;
    }

    if(ao_path)
    {
        mat.texture_ao = texture_load(ao_path);
        mat.ao_enable = 1;
    }

    if(alpha_path)
    {
        mat.texture_alpha = texture_load(alpha_path);
        mat.alpha_enable = 1;
    }

    return mat;
}

void shader_set_pbrmat(GLuint shader, PbrMaterial *mat)
{
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, mat->texture_albedo); 

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, mat->texture_normal); 

    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, mat->texture_roughness); 

    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, mat->texture_ao); 

    glActiveTexture(GL_TEXTURE4);
    glBindTexture(GL_TEXTURE_2D, mat->texture_alpha); 

    shader_set_texture(shader, "albedoMap", 0);
    shader_set_texture(shader, "normalMap", 1);
    shader_set_int(shader, "roughMap", 2);
    shader_set_int(shader, "aoMap", 3);
    shader_set_int(shader, "alphaMap", 4);

    shader_set_int(shader, "albedoEnable", mat->albedo_enable);
    shader_set_int(shader, "normalEnable", mat->normal_enable);
    shader_set_int(shader, "roughnessEnable", mat->roughness_enable);
    shader_set_int(shader, "metallicEnable", mat->metallic_enable);
    shader_set_int(shader, "aoEnable", mat->ao_enable);
    shader_set_int(shader, "alphaEnable", mat->alpha_enable);
}
