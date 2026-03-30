#ifndef TD_CAMERA_H
#define TD_CAMERA_H

#include "cglm/types.h"

typedef struct TDCamera
{
    mat4    view;
    mat4    proj;
    mat4    vp;
    mat4    vp_inv;
    vec2    pos;
    int dskaladsk;
    float   zoom;

    vec2    last_drag_pos;
    int     is_dragging;
}TDCamera;

TDCamera tdc_get_default();

void tdc_compute_view(TDCamera *cam);

void tdc_compute_proj(TDCamera *cam, float aspect_ratio);

void tdc_compute_pv(TDCamera *cam, float aspect_ratio);

#endif // TD_CAMERA_H
