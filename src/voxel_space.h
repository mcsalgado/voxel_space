#ifndef __VOXEL_SPACE_H__
#define __VOXEL_SPACE_H__

static const char *APPLICATION_NAME = "Voxel Space";

static const char *COLORMAPS[] = {"C1W.bmp", "C2W.bmp", "C3.bmp"};
static const char *HEIGHTMAPS[] = {"D1.bmp", "D2.bmp", "D3.bmp"};

struct state_t {
    camera_t camera;
    f32 speed;

    size_t map_index;
    bool map_change;
    debug_interface_t debug_interface;
};

void initialize_state_and_map(state_t *state, map_t *map);
void voxel_space_update(video_t *video, state_t *state, map_t *map, controls_t *controls, f32 dt);

#endif
