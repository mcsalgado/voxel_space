#ifndef __MATH_H__
#define __MATH_H__

#include <stdint.h>
#ifdef _WIN32
#include <intrin.h>
#elif __linux__
#include <x86intrin.h>
#endif

// NOTE(mcsalgado): yes, I'm using a few SSE instructions in this header, fuck the police

typedef uint_least8_t u8;
typedef uint_least16_t u16;
typedef uint_least32_t u32;
typedef uint_least64_t u64;

typedef int_least8_t s8;
typedef int_least16_t s16;
typedef int_least32_t s32;
typedef int_least64_t s64;

typedef float f32;
typedef double f64;

#define ARRAY_LENGTH(array) (sizeof(array) / sizeof((array)[0]))
#define MAX(a, b)  ((a) > (b) ? (a) : (b))
#define MIN(a, b)  ((a) < (b) ? (a) : (b))

inline s32 f32_to_s32(f32 x)
{
    s32 ret = _mm_cvtss_si32(_mm_set_ss(x));
    return ret;
}

inline u32 f32_to_u32(f32 x)
{
    u32 ret = (u32) f32_to_s32(x);
    return ret;
}

inline u8 f32_to_u8(f32 x) {
    // NOTE(mcsalgado): clamps to [0, 255]
    __m128 m = _mm_load_ss(&x);
    m = _mm_max_ss(m, __m128{0.f, 0.f, 0.f, 0.f});
    m = _mm_min_ss(m, __m128{255.f, 255.f, 255.f, 255.f});
    u8 ret = (u8) _mm_cvttss_si32(m);
    return ret;
}

inline f32 sqrt(f32 x)
{
    f32 ret = _mm_cvtss_f32(_mm_sqrt_ss(_mm_set_ss(x)));
    return ret;
}

// NOTE(mcsalgado): Remez approximation of degree 4 for [-.5, .5]
inline f32 sin(f32 x)
{
    f32 ret = (-.16407949f*x*x + .99983873f)*x;
    return ret;
}

inline f32 cos(f32 x)
{
    auto s = sin(x);
    f32 ret = sqrt(1-(s*s));
    return ret;
}

union complex {
    struct {
        f32 r, i;
    };
    struct {
        f32 x, y;
    };
};

inline complex operator+(complex z, complex w)
{
    auto ret = complex{(z.r+w.r), (z.i+w.i)};
    return ret;
}

inline complex &operator+=(complex &z, complex w)
{
    z = z + w;
    return z;
}

inline complex operator-(complex z, complex w)
{
    auto ret = complex{(z.r-w.r), (z.i-w.i)};
    return ret;
}

inline complex operator-(complex z)
{
    auto ret = complex{-z.r, -z.i};
    return ret;
}

inline complex operator*(complex z, complex w)
{
    auto ret = complex{(z.r*w.r) - (z.i*w.i), (z.r*w.i) + (z.i*w.r)};
    return ret;
}

inline complex operator*(f32 c, complex z)
{
    auto ret = complex{c*z.r, c*z.i};
    return ret;
}

inline complex operator*(complex z, f32 c)
{
    auto ret = complex{c*z.r, c*z.i};
    return ret;
}

inline complex &operator*=(complex &z, complex w)
{
    z = z * w;
    return z;
}

inline complex operator/(complex z, f32 c)
{
    auto ret = (1.f/c)*z;
    return ret;
}

inline bool operator==(complex &z, complex w)
{
    bool ret = (z.r == w.r) && (z.i == w.i);
    return ret;
}

inline f32 norm(complex c)
{
    f32 ret = sqrt(c.r*c.r + c.i*c.i);
    return ret;
}

inline complex normalize(complex c)
{
    complex ret = (1.f/norm(c))*c;
    return ret;
}

inline complex lerp(complex z, complex w, f32 t)
{
    auto ret = (1.f-t)*z + t*w;
    return ret;
}


union v4 {
    struct {
        f32 x, y, z, w;
    };
    struct {
        f32 r, g, b, a;
    };
};

inline v4 operator+(v4 a, v4 b)
{
    auto ret = v4{(a.x+b.x),
                  (a.y+b.y),
                  (a.z+b.z),
                  (a.w+b.w)};
    return ret;
}

inline v4 &operator+=(v4 &a, v4 b)
{
    a = a + b;
    return a;
}

inline v4 operator-(v4 a, v4 b)
{
    auto ret = v4{(a.x-b.x),
                  (a.y-b.y),
                  (a.z-b.z),
                  (a.w-b.w)};
    return ret;
}

inline v4 operator-(v4 v)
{
    auto ret = v4{-v.x,
                  -v.y,
                  -v.z,
                  -v.w};
    return ret;
}

inline v4 operator*(f32 c, v4 v)
{
    auto ret = v4{(c*v.x),
                  (c*v.y),
                  (c*v.z),
                  (c*v.w)};
    return ret;
}

inline u32 bgra_pack(v4 color)
{
    u8 a = f32_to_u8(255.f*color.a);
    u8 r = f32_to_u8(255.f*color.r);
    u8 g = f32_to_u8(255.f*color.g);
    u8 b = f32_to_u8(255.f*color.b);
    u32 ret = (a << 24) | (r << 16) | (g << 8) | (b << 0);
    return ret;
}

inline v4 bgra_unpack(u32 color)
{
    v4 ret = {};
    ret.a = ((1.f/255.f)*((color >> 24) & 255));
    ret.r = ((1.f/255.f)*((color >> 16) & 255));
    ret.g = ((1.f/255.f)*((color >> 8) & 255));
    ret.b = ((1.f/255.f)*((color >> 0) & 255));
    return ret;
}

const auto V4_RED = v4{1.f, 0.f, 0.f, 1.f};
const auto V4_GREEN = v4{0.f, 1.f, 0.f, 1.f};
const auto V4_BLUE = v4{0.f, 0.f, 1.f, 1.f};
const auto V4_BLACK = v4{0.f, 0.f, 0.f, 1.f};
const auto V4_WHITE = v4{1.f, 1.f, 1.f, 1.f};

#endif
