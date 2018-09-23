#ifndef __RENDER_H__
#define __RENDER_H__

const u32 SKY_COLOR = 0xFF60A0CC;
const f32 FOCAL_LENGTH_COEFFICIENT = 350.f;
const f32 LEVEL_OF_DETAIL_STEP = .002f;

struct video_t {
    u32 *buffer;
    u32 *ybuffer;
    u32 width;
    u32 height;
};

struct camera_t {
    complex position;
    complex direction;

    f32 height;
    f32 focal_length;
    f32 projection_screen_height;

    // NOTE(mcsalgado): the maximum distance that is drawn by the renderer
    f32 draw_distance;
};

struct debug_interface_t {
    bool is_map_shown;
    bool is_heightmap_shown;
    bool is_scanlines_enabled;
};

void render(video_t *video, camera_t *camera, map_t *map, debug_interface_t debug_interface);
void adjust_focal_length(camera_t *camera, u32 screen_height, u32 screen_width);

#endif
