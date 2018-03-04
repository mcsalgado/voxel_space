#include "../src/precompiled.h"

#include <stdio.h>
#include <time.h>
#include <assert.h>
#include <string.h>

// XXX(mcsalgado): lol if your testing code isn't complete dogshit like this one... just lol

#define RENDER_TEST(test_name, function1_call, function2_call) \
printf("%s... ", #test_name);\
function1_call;\
memcpy(comparison_buffer, video.buffer, video.width * video.height * sizeof(u32));\
function2_call;\
for (u32 i = 0; i < video.width*video.height; ++i) {\
    assert(video.buffer[i] == comparison_buffer[i]);\
}\
printf("OK!\n");

#define TIME_TEST(test_name, n, function_call)\
f32 test_name##_results[n] = {};\
for (u32 i = 0; i < n; ++i) {\
    start = clock();\
    function_call;\
    end = clock();\
    test_name##_results[i] = (f32) (end - start);\
}\
printf("%s %f %f\n", #test_name, median(test_name##_results, n), standard_deviation(test_name##_results, n));

enum {
    SCREEN_WIDTH = 1920,
    SCREEN_HEIGHT = 1080
};

f32 median(f32 a[], u32 n)
{
    f32 ret;
    f32 temp;
    for (size_t i = 0; i < n-1; ++i) {
        for (size_t j = i+1; j < n; ++j) {
            if (a[j] < a[i]) {
                temp = a[i];
                a[i] = a[j];
                a[j] = temp;
            }
        }
    }

    if (n % 2 == 0) {
        ret = ((a[n/2] + a[(n/2)-1])/2.f);
        return ret;
    }

    ret = a[n/2];
    return ret;
}

f32 mean(f32 a[], u32 n)
{
    f32 sum = 0.f;
    for (size_t i = 0; i < n; ++i) {
        sum += a[i];
    }
    float ret = sum/n;
    return ret;
}

f32 standard_deviation(f32 a[], u32 n)
{
    f32 ret = 0.f;
    f32 m = mean(a, n);
    for (size_t i = 0; i < n; ++i) {
        ret += (a[i]-m)*(a[i]-m);
    }
    ret = _sqrt(ret/n);
    return ret;
}

int main(int argc, char **argv)
{
    const u8 runs = 20;

    state_t state = {};
    state.camera.position = complex{500.f, 0.f};
    state.camera.direction = complex{0.f, 1.f};
    state.camera.height = 70.f;
    state.camera.projection_screen_height = -500.f;
    state.camera.draw_distance = 2000.f;

    state.debug_interface.is_map_shown = false;
    state.debug_interface.is_heightmap_shown = false;
    state.debug_interface.is_scanlines_enabled = false;

    clock_t start = 0;
    clock_t end = 0;

    auto map = load_maps(HEIGHTMAPS[0], COLORMAPS[0]);

    video_t video = {};
    video.width = SCREEN_WIDTH;
    video.height = SCREEN_HEIGHT;
    video.buffer = (u32 *) new u32[SCREEN_WIDTH][SCREEN_HEIGHT];
    u32 *comparison_buffer = (u32 *) new u32[SCREEN_WIDTH][SCREEN_HEIGHT];
    video.ybuffer = new u32[SCREEN_WIDTH];

    RENDER_TEST(sanity_check,
                render(&video, &state.camera, &map, state.debug_interface),
                render(&video, &state.camera, &map, state.debug_interface));
    printf("\n");

    TIME_TEST(vanilla, runs,
              render(&video, &state.camera, &map, state.debug_interface));
    TIME_TEST(vanilla_again, runs,
              render(&video, &state.camera, &map, state.debug_interface));

    state.debug_interface.is_map_shown = true;
    TIME_TEST(displaymap, runs,
              render(&video, &state.camera, &map, state.debug_interface));

    state.debug_interface.is_map_shown = false;
    state.debug_interface.is_scanlines_enabled = true;
    TIME_TEST(scanlines, runs,
              render(&video, &state.camera, &map, state.debug_interface));

    printf("\n");

    printf("tests completed, press any key to exit...\n");
    getchar();
    return 0;
}
