#ifndef INPUT_INFO_H
#define INPUT_INFO_H

#include "interface.h"

static inline struct InputInfo inputinfo_default()
{
    struct InputInfo inputinfo;
    inputinfo.delta_time = 0;
    for(int i = 0; i < KEY_COUNT; ++i)
    {
        inputinfo.keys_down[i] = 0;
        inputinfo.keys_pressed[i] = 0;
        inputinfo.keys_released[i] = 0;
    }
    return inputinfo;
}

static inline int is_key_pressed(struct InputInfo inputinfo, int key) 
{
    return inputinfo.keys_pressed[key];
}

static inline int is_key_released(struct InputInfo inputinfo, int key) 
{
    return inputinfo.keys_released[key];
}

static inline int is_key_down(struct InputInfo inputinfo, int key) 
{
    return inputinfo.keys_down[key];
}

#endif // INPUT_INFO_H
