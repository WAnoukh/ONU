#ifndef TRANSFORM_H
#define TRANSFORM_H

#include "cglm/affine2d.h"
#include "cglm/mat3.h"

static void compute_transform(mat3 out_transform, vec2 pos, const vec2 size)
{
    glm_mat3_identity(out_transform);
    glm_translate2d(out_transform, pos);
    mat3 scale;
    glm_mat3_identity(scale);
    out_transform[0][0] = size[0];
    out_transform[1][1] = size[1];
}

#endif // TRANSFORM_H
