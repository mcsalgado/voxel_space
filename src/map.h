#ifndef __MAP_H__
#define __MAP_H__

enum {
    MAP_SIZE = 1024,
    BITMAP_HEADER_OFFSET_OFFSET = 0xA
};

struct bitmap8_t {
    u32 width;
    u32 height;
    u32 size;
    u8 *pixels;
};

struct bitmap32_t {
    u32 width;
    u32 height;
    u32 size;
    u32 *pixels;
};

struct map_t {
    bitmap8_t height;
    bitmap32_t color;

    // NOTE(mcsalgado): same as above but this is for tracking FOV and position
    // on screen
    bitmap32_t display;
};

bitmap8_t load_heightmap(const char *file_name);
bitmap32_t load_colormap(const char *file_name);
map_t load_maps(const char *height_file_name, const char *color_file_name);

inline u8 get_height(map_t *map, u32 x, u32 y)
{
    u8 ret = map->height.pixels[x + y*map->height.width];
    return ret;
}

inline u32 get_color(map_t *map, u32 x, u32 y)
{
    u32 ret = map->color.pixels[x + y*map->color.width];
    return ret;
}

inline void mark_pixel(bitmap32_t *bitmap, u32 x, u32 y, v4 color)
{
    auto previous_color = bgra_unpack(bitmap->pixels[x + y*bitmap->width]);
    bitmap->pixels[x + y*bitmap->width] = bgra_pack(previous_color + color);
}

#endif
