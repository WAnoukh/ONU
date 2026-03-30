#ifndef TRANSFORM_H
#define TRANSFORM_H

#include "cglm/affine2d.h"
#include "cglm/mat3.h"

static void compute_transform(mat3 out_transform, vec2 pos, const vec2 size)
{
    glm_mat3_identity(out_transform);
    glm_translate2d(out_transform, pos);
    out_transform[0][0] = size[0];
    out_transform[1][1] = size[1];
}

static void compute_complete_transform(mat3 out_transform, vec2 pos, vec2 size, float rotation)
{
    glm_mat3_identity(out_transform);
    glm_translate2d(out_transform, pos);
    glm_rotate2d(out_transform, rotation);
    glm_scale2d(out_transform, size);
}
#endif // TRANSFORM_H
