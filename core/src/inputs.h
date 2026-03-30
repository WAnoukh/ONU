#ifndef INPUT_DEF_H
#define INPUT_DEF_H

#define KEY_COUNT 348 

struct InputInfo
{
    int keys_pressed[KEY_COUNT];
    int keys_released[KEY_COUNT];
    int keys_down[KEY_COUNT];
    int mouse_buttons_pressed[5];
    int mouse_buttons_released[5];
    int mouse_buttons_down[5];
    float mouse_x, mouse_y;
    float scroll_x, scroll_y;
};

static inline struct InputInfo inputinfo_default()
{
    struct InputInfo inputinfo;
    for(int i = 0; i < KEY_COUNT; ++i)
    {
        inputinfo.keys_down[i] = 0;
        inputinfo.keys_pressed[i] = 0;
        inputinfo.keys_released[i] = 0;
    }
    for(int i = 0; i < 5; ++i)
    {
        inputinfo.mouse_buttons_pressed[i] = 0;
        inputinfo.mouse_buttons_released[i] = 0;
        inputinfo.mouse_buttons_down[i] = 0;
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

static inline int is_mouse_pressed(struct InputInfo inputinfo, int button)
{
    return inputinfo.mouse_buttons_pressed[button];
}

static inline int is_mouse_released(struct InputInfo inputinfo, int button)
{
    return inputinfo.mouse_buttons_released[button];
}

static inline int is_mouse_down(struct InputInfo inputinfo, int button)
{
    return inputinfo.mouse_buttons_down[button];
}

static inline void get_mouse_pixel_position(struct InputInfo inputinfo, float *out_x, float *out_y)
{
    *out_x = (float)inputinfo.mouse_x;
    *out_y = (float)inputinfo.mouse_y;
}

static inline void get_scroll(struct InputInfo inputinfo, float *out_x, float *out_y)
{
    *out_x = (float)inputinfo.scroll_x; 
    *out_y = (float)inputinfo.scroll_y; 
}
#endif // INPUT_DEF_H
