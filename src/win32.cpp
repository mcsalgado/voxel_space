#include "precompiled.h"

#include <windows.h>

#include "win32.h"

static win32_application_t win32_application;


static void pump_messages()
{
    MSG message;
    while (PeekMessage(&message, 0, 0, 0, PM_REMOVE)) {
        if (WM_QUIT == message.message) {
            win32_application.is_running = false;
            continue;
        }
        TranslateMessage(&message);
        DispatchMessageA(&message);
    }
}

static void allocate_gdi_framebuffer(video_t *video)
{
    if (video->buffer) VirtualFree((void *) video->buffer, 0, MEM_RELEASE);

    video->width = SCREEN_WIDTH;
    video->height = SCREEN_HEIGHT;

    win32_application.gdi_info.bmiHeader.biSize = sizeof(win32_application.gdi_info.bmiHeader);
    win32_application.gdi_info.bmiHeader.biWidth = video->width;
    win32_application.gdi_info.bmiHeader.biHeight = video->height;
    win32_application.gdi_info.bmiHeader.biPlanes = 1;
    win32_application.gdi_info.bmiHeader.biBitCount = sizeof(u32)*8;
    win32_application.gdi_info.bmiHeader.biCompression = BI_RGB;
    win32_application.gdi_info.bmiHeader.biSizeImage = video->width * video->height * sizeof(u32);

    video->buffer = (u32 *) VirtualAlloc(0,
                                         win32_application.gdi_info.bmiHeader.biSizeImage,
                                         MEM_RESERVE | MEM_COMMIT,
                                         PAGE_READWRITE);
}

static void display_gdi_framebuffer(video_t *video)
{
    win32_application.device_context = GetDC(win32_application.window_handle);
    SetDIBitsToDevice(win32_application.device_context,
                      0, 0,
                      video->width, video->height,
                      0, 0,
                      0,
                      video->height,
                      (void *) video->buffer,
                      &win32_application.gdi_info,
                      DIB_RGB_COLORS);
    ReleaseDC(win32_application.window_handle, win32_application.device_context);
}

inline void update_ticks()
{
    auto previous_ticks = win32_application.ticks;
    QueryPerformanceCounter((LARGE_INTEGER *) &win32_application.ticks);
    win32_application.dticks = win32_application.ticks - previous_ticks;
}

inline s64 get_elapsed_ticks()
{
    s64 ticks_now;
    QueryPerformanceCounter((LARGE_INTEGER *) &ticks_now);
    auto ret = ticks_now - win32_application.ticks;
    return ret;
}

static LRESULT CALLBACK main_window_proc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    LRESULT ret = 0;

    if (WM_SETCURSOR == uMsg && LOWORD(lParam) == HTCLIENT) {
        SetCursor(0);
    } else if (WM_CLOSE == uMsg) {
        win32_application.is_running = false;
    } else if (WM_DESTROY == uMsg) {
        win32_application.is_running = false;
    } else {
        ret = DefWindowProcA(hwnd, uMsg, wParam, lParam);
    }

    return ret;
}


int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    win32_application.instance_handle = hInstance;
    win32_application.is_running = true;

    // NOTE(mcsalgado): ensure better precision for Sleep
    timeBeginPeriod(1);

    QueryPerformanceFrequency((LARGE_INTEGER *) &win32_application.ticks_per_second);
    win32_application.ticks_per_frame = win32_application.ticks_per_second/FPS;

    state_t state = {};
    map_t map;
    initialize_state_and_map(&state, &map);


    WNDCLASS window_class = {};
    window_class.style = CS_HREDRAW | CS_VREDRAW;
    window_class.lpfnWndProc = main_window_proc;
    window_class.hInstance = win32_application.instance_handle;
    window_class.lpszClassName = WINDOW_CLASS_NAME;

    if (!RegisterClassA(&window_class)) {
        OutputDebugStringA("Failed to register window class.\n");
        return 0;
    }

    video_t video = {};
    allocate_gdi_framebuffer(&video);

    // NOTE(mcsalgado): there are some shenanigans here to pretend this is full screen
    win32_application.window_handle = CreateWindowExA(0,
                                                      window_class.lpszClassName,
                                                      APPLICATION_NAME,
                                                      WS_POPUP | WS_DLGFRAME | WS_VISIBLE,
                                                      -3,
                                                      -3,
                                                      video.width+6,
                                                      video.height+6,
                                                      0,
                                                      0,
                                                      win32_application.instance_handle,
                                                      0);

    if (!win32_application.window_handle) {
        OutputDebugStringA("Failed to create window.\n");
        return 0;
    }

    video.ybuffer = (u32 *) VirtualAlloc(0,
                                         video.height*sizeof(u32),
                                         MEM_RESERVE | MEM_COMMIT,
                                         PAGE_READWRITE);

    adjust_focal_length(&state.camera, video.height, video.width);

    win32_application.device_context = GetDC(win32_application.window_handle);
    win32_application.bits_per_pixel = GetDeviceCaps(win32_application.device_context, BITSPIXEL);
    win32_application.monitor_width = GetDeviceCaps(win32_application.device_context, HORZRES);
    win32_application.monitor_height = GetDeviceCaps(win32_application.device_context, VERTRES);
    win32_application.vertical_refresh_rate = GetDeviceCaps(win32_application.device_context, VREFRESH);
    ReleaseDC(win32_application.window_handle, win32_application.device_context);

    GetSystemInfo(&win32_application.system_info);

    controls_t controls = {};

    while (win32_application.is_running) {
        update_ticks();
        pump_messages();

        update_button(&controls.accelerate_up, GetKeyState(VK_UP) & 0x8000);
        update_button(&controls.accelerate_down, GetKeyState(VK_DOWN) & 0x8000);
        update_button(&controls.turn_left, GetKeyState(VK_LEFT) & 0x8000);
        update_button(&controls.turn_right, GetKeyState(VK_RIGHT) & 0x8000);

        update_button(&controls.show_display_map, GetKeyState(VK_SPACE) & 0x8000);
        update_button(&controls.show_height_map, GetKeyState(VK_TAB) & 0x8000);
        update_button(&controls.change_map, GetKeyState(VK_RETURN) & 0x8000);
        update_button(&controls.toggle_scanlines, GetKeyState('Z') & 0x8000);

        update_button(&controls.camera_height_up, GetKeyState('Q') & 0x8000);
        update_button(&controls.camera_height_down, GetKeyState('A') & 0x8000);
        update_button(&controls.projection_screen_height_up, GetKeyState('W') & 0x8000);
        update_button(&controls.projection_screen_height_down, GetKeyState('S') & 0x8000);
        update_button(&controls.draw_distance_up, GetKeyState('E') & 0x8000);
        update_button(&controls.draw_distance_down, GetKeyState('D') & 0x8000);
        update_button(&controls.focal_length_up, GetKeyState('R') & 0x8000);
        update_button(&controls.focal_length_down, GetKeyState('F') & 0x8000);

        RECT rect;
        if (GetClientRect(win32_application.window_handle, &rect)) {
            win32_application.window_width = rect.right - rect.left;
            win32_application.window_height = rect.bottom - rect.top;
        }

        voxel_space_update(&video, &state, &map, &controls,
                           ((f32) win32_application.dticks)/100000.f);
        display_gdi_framebuffer(&video);

        // NOTE(mcsalgado): lock FPS
        auto remaining_ticks = win32_application.ticks_per_frame - get_elapsed_ticks();
        if (remaining_ticks > 0) {
            Sleep((DWORD) (1000*remaining_ticks/win32_application.ticks_per_second));
        }
    }

    return 0;
}
