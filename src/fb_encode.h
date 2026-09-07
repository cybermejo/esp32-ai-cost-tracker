#pragma once
#include <stdint.h>

// Framebuffer pixel encoding contract: the QSPI bus runs setNoSwap(true),
// i.e. display_flush() memcpys framebuffer bytes straight to the panel with
// no per-pixel CPU swap — so the framebuffer must hold big-endian RGB565.
// All writers MUST store via fb_encode(). (Byte-symmetric black/white masked
// a missing swap here until tinted themes exposed it: cream read lavender,
// near-black read green on glass.)
inline uint16_t fb_encode(uint16_t color) {
  return (uint16_t)((color >> 8) | (color << 8));
}
