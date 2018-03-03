#include "precompiled.h"

#include <stdio.h>
#include <stdlib.h>

#include "map.h"

// TODO(mcsalgado): this too hacky, maybe I should have separate platform
// specific headers for these things
#ifdef __linux__
void fopen_s(FILE **fp, const char *file_name, const char *mode) {
    *fp = fopen(file_name, mode);
}
#endif

bitmap8_t load_heightmap(const char *file_name)
{
    bitmap8_t ret = {};
    FILE *fp;

    fopen_s(&fp, file_name, "rb");
    ret.width = MAP_SIZE;
    ret.height = MAP_SIZE;
    ret.size = sizeof(u8)*ret.width*ret.height;

    // NOTE(mcsalgado): get the offset from the bitmap header
    fseek(fp, BITMAP_HEADER_OFFSET_OFFSET, SEEK_SET);
    u32 offset;
    fread(&offset, 4, 1, fp);

    // NOTE(mcsalgado): skip the bullshit, let's go straight to the bitmap data
    fseek(fp, offset, SEEK_SET);

    ret.pixels = (u8 *) malloc(ret.size);
    fread(ret.pixels, ret.size, 1, fp);
    fclose(fp);

    return ret;
}

bitmap32_t load_colormap(const char *file_name)
{
    bitmap32_t ret = {};
    FILE *fp;

    fopen_s(&fp, file_name, "rb");
    ret.width = MAP_SIZE;
    ret.height = MAP_SIZE;
    ret.size = sizeof(u32)*ret.width*ret.height;

    // NOTE(mcsalgado): get the offset from the bitmap header
    fseek(fp, BITMAP_HEADER_OFFSET_OFFSET, SEEK_SET);
    u32 offset;
    fread(&offset, 4, 1, fp);

    // NOTE(mcsalgado): skip the bullshit, let's go straight to the bitmap data
    fseek(fp, offset, SEEK_SET);

    ret.pixels = (u32 *) malloc(ret.size);
    fread(ret.pixels, ret.size, 1, fp);
    fclose(fp);

    return ret;
}

map_t load_maps(const char *height_file_name, const char *color_file_name)
{
    map_t ret = {};

    ret.height = load_heightmap(height_file_name);
    ret.color = load_colormap(color_file_name);
    ret.display = load_colormap(color_file_name);

    return ret;
}
