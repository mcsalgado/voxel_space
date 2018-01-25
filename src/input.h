#ifndef __INPUT_H__
#define __INPUT_H__

struct button_t {
    bool is_down;
    bool is_pressed;
    bool is_released;
};

struct controls_t {
    button_t accelerate_up;
    button_t accelerate_down;
    button_t turn_left;
    button_t turn_right;

    button_t show_display_map;
    button_t show_height_map;
    button_t change_map;
    button_t toggle_scanlines;

    button_t camera_height_up;
    button_t camera_height_down;
    button_t projection_screen_height_up;
    button_t projection_screen_height_down;
    button_t draw_distance_up;
    button_t draw_distance_down;
    button_t focal_length_up;
    button_t focal_length_down;
};

inline void update_button(button_t *button, bool is_down) {
    bool was_down = button->is_down;
    button->is_down = is_down;
    button->is_pressed = !was_down && is_down;
    button->is_released = was_down && !is_down;
}

#endif
