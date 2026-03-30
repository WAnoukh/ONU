#include "td_camera.h"
#include "cglm/affine.h"
#include "cglm/clipspace/ortho_lh_no.h"
#include "cglm/mat4.h"

TDCamera tdc_get_default()
{
    return (TDCamera){
        .pos = {0, 0},
        .zoom = 0.3f,
    };
}

void tdc_compute_view(TDCamera *cam)
{
    glm_mat4_identity(cam->view);
    glm_scale_uni(cam->view, cam->zoom);
    glm_translate_x(cam->view, -cam->pos[0]);
    glm_translate_y(cam->view, -cam->pos[1]);
}

void tdc_compute_proj(TDCamera *cam, float aspect_ratio)
{
    glm_ortho_default_lh_no(aspect_ratio, cam->proj);
}

void tdc_compute_pv(TDCamera *cam, float aspect_ratio)
{
    tdc_compute_view(cam);
    tdc_compute_proj(cam, aspect_ratio);
    glm_mat4_mul(cam->proj, cam->view, cam->vp);
    glm_mat4_inv_precise_sse2(cam->vp, cam->vp_inv);
}
