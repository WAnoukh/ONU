#ifndef TRANSFORM_H
#define TRANSFORM_H

#include "cglm/mat3.h"

typedef struct {
    vec2    position;
    vec2    scale;
    float   rotation;
} Transform2D;

typedef struct{
    vec3 position;
    vec3 scale;
    vec3 rotation;
} Transform;

Transform2D transform_2D_identity();

Transform transform_identity();

void compute_transform_2D(Transform2D transform, mat3 out_mat);

void compute_transform(Transform transform, mat4 out_mat);

#endif // TRANSFORM_H
