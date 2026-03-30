#include "transform.h"
#include "cglm/affine2d.h"
#include "cglm/affine.h"

Transform2D transform_2D_identity()
{
    return (Transform2D)
    {
        .position   = {0, 0},
        .scale      = {1, 1},
        .rotation   = 0,
    };
}

Transform transform_identity()
{
    return (Transform)
    {
        .position   = {0, 0, 0},
        .scale      = {1, 1, 1},
        .rotation   = {0, 0, 0},
    };
}

void compute_transform_2D(Transform2D transform, mat3 out_mat)
{
    glm_mat3_identity(out_mat);
    glm_translate2d(out_mat, transform.position);
    glm_scale2d(out_mat, transform.scale);
    glm_rotate2d(out_mat, transform.rotation);
}

void compute_transform(Transform transform, mat4 out_mat)
{
    glm_mat4_identity(out_mat);
    glm_rotate_x(out_mat, transform.rotation[0], out_mat);
    glm_rotate_y(out_mat, transform.rotation[0], out_mat);
    glm_rotate_z(out_mat, transform.rotation[0], out_mat);
    glm_translate(out_mat, transform.position);
    glm_scale(out_mat, transform.scale);
}
