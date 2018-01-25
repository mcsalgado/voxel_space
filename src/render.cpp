#include "precompiled.h"

#include "render.h"

static void draw_map(video_t *video, bitmap32_t *color_map, bitmap32_t *display_map)
{
    u32 *screen = video->buffer;
    u32 *display_map_pixels = display_map->pixels;
    for (u32 y = 0; y < video->height; ++y) {
        for (u32 x = 0; x < video->width; ++x) {
            if ((y < display_map->height) && (x < display_map->width)) {
                *screen = *display_map_pixels++;
            }
            screen++;
        }
    }

    // NOTE(mcsalgado): clear the display map for the next render
    display_map_pixels = display_map->pixels;
    u32 *color_map_pixels = color_map->pixels;
    for (u32 y = 0; y < display_map->height; ++y) {
        for (u32 x = 0; x < display_map->width; ++x) {
            *display_map_pixels++ = *color_map_pixels++;
        }
    }
}

static void draw_heightmap(video_t *video, bitmap8_t *height_map)
{
    u32 *screen = video->buffer;
    u8 *height_map_pixels = height_map->pixels;
    for (u32 y = 0; y < video->height; ++y) {
        for (u32 x = 0; x < video->width; ++x) {
            if (y < height_map->height && x < height_map->width) {
                auto grayscale_color = *height_map_pixels++;
                *screen = ((0xFF << 24) |
                           (grayscale_color << 16) |
                           (grayscale_color << 8) |
                           (grayscale_color << 0));
            }
            screen++;
        }
    }
}

static void scanlines_filter(video_t *video)
{
    u32 *screen = video->buffer;
    for (u32 y = 0; y < video->height; ++y) {
        for (u32 x = 0; x < video->width; ++x) {
            if ((y % 4 == 0) || (y % 4 == 1)) {
                auto previous_color = bgra_unpack(*screen);
                auto new_color = .7f*previous_color;
                *screen = bgra_pack(new_color);
            }
            screen++;
        }
    }
}

void adjust_focal_length(camera_t *camera, u32 screen_height, u32 screen_width)
{
    auto aspect_ratio = ((f32) screen_width)/((f32) screen_height);
    camera->focal_length = FOCAL_LENGTH_COEFFICIENT*aspect_ratio;
}

void render(video_t *video, camera_t *camera, map_t *map, debug_interface_t debug_interface)
{

    // NOTE(mcsalgado): mark the current location on the displaymap with a white dot
    // TODO(mcsalgado): I don't like marking the map pixel here inside this function
    // but whatever for now
    if (debug_interface.is_map_shown) {
        mark_pixel(&map->display,
                   f32_to_u32(camera->position.x) % map->display.width,
                   f32_to_u32(camera->position.y) % map->display.height,
                   V4_WHITE);
    }

    // NOTE(mcsalgado): clear the buffer (with a blue sky)
    u32 *screen = video->buffer;
    for (u32 y = 0; y < video->height; ++y) {
        for (u32 x = 0; x < video->width; ++x) {
            *screen++ = SKY_COLOR;
        }
    }

    screen = video->buffer;
    u32 *ybuffer = video->ybuffer;

    for (u32 i = 0; i < video->width; ++i) {
        ybuffer[i] = 0;
    }

    f32 horizon = camera->height - camera->projection_screen_height;
    f32 i_scale = 1.f/((f32) video->width);

    // NOTE(mcsalgado): z tracks the fov distance from the camera
    auto z = 1.f;
    auto dz = 1.f;
    while (z < camera->draw_distance) {

        // NOTE(mcsalgado): those are the end points of the line segment that scans the fov
        auto p0 = complex{z, z}*camera->direction + camera->position;
        auto p1 = complex{z, -z}*camera->direction + camera->position;

        for (u32 i = 0; i < video->width; ++i) {
            auto p = lerp(p0, p1, ((f32) i)*i_scale);
            auto map_x = f32_to_u32(p.x) % map->color.width;
            auto map_y = f32_to_u32(p.y) % map->color.height;
            auto voxel_height = (f32) get_height(&map->height, map_x, map_y);
            auto height_on_screen_f = ((voxel_height - camera->height)*camera->focal_length)/z + horizon;
            if (height_on_screen_f < 0) continue;
            auto height_on_screen = f32_to_u32(height_on_screen_f);

            if (height_on_screen >= video->height) height_on_screen = video->height-1;

            auto strip_color = get_pixel(&map->color, map_x, map_y);

            // NOTE(mcsalgado): draw the vertical strip
            for (u32 j = ybuffer[i]; j < height_on_screen+1; ++j) {
                screen[i + j*video->width] = strip_color;
            }

            if (height_on_screen > ybuffer[i]) ybuffer[i] = height_on_screen;

            // TODO(mcsalgado): I don't like marking the map pixel here inside this function
            // but whatever for now
            if (debug_interface.is_map_shown) {
                mark_pixel(&map->display, map_x, map_y, V4_BLUE);
            }
        }
        z += dz;
        dz += .002f;
    }

    if (debug_interface.is_map_shown) {
        draw_map(video, &map->color, &map->display);
    } else if (debug_interface.is_heightmap_shown) {
        draw_heightmap(video, &map->height);
    }

    if (debug_interface.is_scanlines_enabled) scanlines_filter(video);
}
