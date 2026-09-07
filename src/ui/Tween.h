#pragma once
#include <stdint.h>
inline float easeOutCubic(float t) { float u = 1 - t; return 1 - u*u*u; }
struct Tween {
  float from = 0, to = 0, durMs = 250; uint32_t t0 = 0;
  float at(uint32_t now) const {
    float t = durMs <= 0 ? 1 : (float)(now - t0) / durMs;
    if (t < 0) t = 0; if (t > 1) t = 1;
    return from + (to - from) * easeOutCubic(t);
  }
  bool done(uint32_t now) const { return now - t0 >= (uint32_t)durMs; }
};
