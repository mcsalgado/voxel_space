#ifndef __WIN32_H__
#define __WIN32_H__

enum {
    SCREEN_WIDTH = 1920,
//  SCREEN_WIDTH = 3840;
    SCREEN_HEIGHT = 1080,
//  SCREEN_HEIGHT = 2160;
    FPS = 60
};

const char *WINDOW_CLASS_NAME = "VOXEL_SPACE";

struct win32_application_t {
    bool is_running;

    HINSTANCE instance_handle;
    HWND window_handle;
    HDC device_context;

    s32 window_width;
    s32 window_height;

    s64 ticks_per_second;
    s64 ticks_per_frame;
    s64 ticks = 0;
    s64 dticks = 0;

    s32 bits_per_pixel;
    s32 monitor_width;
    s32 monitor_height;
    s32 vertical_refresh_rate;

    SYSTEM_INFO system_info;

    BITMAPINFO gdi_info;
};

#endif
