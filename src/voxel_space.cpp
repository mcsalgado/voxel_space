#include "precompiled.h"

#include "voxel_space.h"

void initialize_state_and_map(state_t *state, map_t *map)
{
    state->camera.position = complex{500.f, 0.f};
    state->camera.direction = complex{0.f, 1.f};
    state->camera.height = 70.f;
    state->camera.projection_screen_height = -500.f;
    state->camera.draw_distance = 300.f;

    state->speed = 0.f;
    state->debug_interface.is_map_shown = false;
    state->debug_interface.is_heightmap_shown = false;
    state->debug_interface.is_scanlines_enabled = false;
    state->map_index = 0;
    state->map_change = false;

    *map = load_maps(HEIGHTMAPS[state->map_index], COLORMAPS[state->map_index]);
}

void process_controls(state_t *state, controls_t *controls, f32 dt)
{
    if (controls->accelerate_up.is_down) {
        state->speed += .05f*dt;
    }
    if (controls->accelerate_down.is_down) {
        state->speed -= .05f*dt;
    }
    if (controls->turn_left.is_down) {
        state->camera.direction *= complex{cos(.02f*dt), sin(.02f*dt)};
    }
    if (controls->turn_right.is_down) {
        state->camera.direction *= complex{cos(-.02f*dt), sin(-.02f*dt)};
    }

    if (controls->show_display_map.is_pressed) {
        state->debug_interface.is_map_shown = !state->debug_interface.is_map_shown;
    }
    if (controls->show_height_map.is_pressed) {
        state->debug_interface.is_heightmap_shown = !state->debug_interface.is_heightmap_shown;
    }
    if (controls->change_map.is_pressed) {
        state->map_change = true;
    }
    if (controls->toggle_scanlines.is_pressed) {
        state->debug_interface.is_scanlines_enabled = !state->debug_interface.is_scanlines_enabled;
    }

    if (controls->camera_height_up.is_down) {
        state->camera.height += 3.f*dt;
    }
    if (controls->camera_height_down.is_down) {
        state->camera.height -= 3.f*dt;
    }
    if (controls->projection_screen_height_up.is_down) {
        state->camera.projection_screen_height += 3.f*dt;
    }
    if (controls->projection_screen_height_down.is_down) {
        state->camera.projection_screen_height -= 3.f*dt;
    }
    if (controls->draw_distance_up.is_down) {
        state->camera.draw_distance += 3.f*dt;
    }
    if (controls->draw_distance_down.is_down) {
        state->camera.draw_distance -= 3.f*dt;
    }
    if (controls->focal_length_up.is_down) {
        state->camera.focal_length += 3.f*dt;
    }
    if (controls->focal_length_down.is_down) {
        state->camera.focal_length -= 3.f*dt;
    }
}

void voxel_space_update(video_t *video, state_t *state, map_t *map, controls_t *controls, f32 dt)
{
    process_controls(state, controls, dt);
    state->camera.position += state->speed*state->camera.direction*dt;

    if (state->map_change) {
        state->map_index = (state->map_index + 1) % ARRAY_LENGTH(COLORMAPS);
        *map = load_maps(HEIGHTMAPS[state->map_index], COLORMAPS[state->map_index]);
    }
    state->map_change = false;

    // TODO(mcsalgado): maybe I should adjust the camera focal length every time here?

    render(video, &state->camera, map, state->debug_interface);
}
