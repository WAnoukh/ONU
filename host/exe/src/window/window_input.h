#ifndef INPUT_H
#define INPUT_H

struct InputInfo;

void i_initialize();

struct InputInfo i_get_info();

void i_clear();

void register_key_state(int key, int action);

void register_scroll_state(float x, float y);

void register_mouse_move_state(double x, double y);

void register_mouse_state(int button, int action);

#endif //INPUT_H
