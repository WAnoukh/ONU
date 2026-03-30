#ifndef WINDOW_CONTEXT_H
#define WINDOW_CONTEXT_H

struct WindowContext 
{
    int     width;
    int     height;
    float   ratio;
    int     is_framebuffer_resized;
};

#endif // WINDOW_CONTEXT_H
