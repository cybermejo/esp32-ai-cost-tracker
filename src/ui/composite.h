#pragma once
#include <stdint.h>
#include <string.h>

// Slide compositor: settled screens are cached bitmaps; a transition frame
// copies one column-run from each cache. Both layers move rigidly with the
// same offset delta, so exactly one source owns every output column (clean
// vertical split, no overlap, no gap). Pure memory math, host-testable; the
// device flushes framebuffer right after compositing.
inline void compositeSlide(const uint16_t* prev, const uint16_t* cur, uint16_t* out,
                           int W, int H, int dir, float p) {
  int offP = (int)(dir * p * W);
  int offC = offP - dir * W;
  int xs = dir < 0 ? offP + W : offP;
  if (xs < 0) xs = 0;
  if (xs > W) xs = W;
  for (int y = 0; y < H; y++) {
    if (dir < 0) {
      int n0 = xs, s0 = 0 - offP;
      int n1 = W - xs, s1 = xs - offC;
      if (n0 > 0) memcpy(&out[(size_t)y * W], &prev[(size_t)y * W + s0], (size_t)n0 * 2);
      if (n1 > 0) memcpy(&out[(size_t)y * W + xs], &cur[(size_t)y * W + s1], (size_t)n1 * 2);
    } else {
      int n0 = xs, s0 = 0 - offC;
      int n1 = W - xs, s1 = xs - offP;
      if (n0 > 0) memcpy(&out[(size_t)y * W], &cur[(size_t)y * W + s0], (size_t)n0 * 2);
      if (n1 > 0) memcpy(&out[(size_t)y * W + xs], &prev[(size_t)y * W + s1], (size_t)n1 * 2);
    }
  }
}
