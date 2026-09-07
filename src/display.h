#pragma once

#include <cstdint>
#include "config.h"

// Display bring-up for the Waveshare ESP32-S3-Touch-AMOLED-1.8.
// Auto-detects V1 (SH8601) vs V2 (CO5300) by probing the touch chip on the
// shared I2C bus, then drives the panel through the vendored Arduino_GFX
// (Arduino_ESP32QSPI bus + vendor display class). We do NOT use Arduino_GFX
// drawing primitives for the fluid itself — we own a framebuffer and push it
// with draw16bitRGBBitmap, which becomes DMA transfers on the QSPI bus.

enum PanelVariant {
  PANEL_V2_CO5300 = 0,  // current hardware (ships since mid-2026)
  PANEL_V1_SH8601,
};

bool display_init();                       // detect variant, init panel
void display_backlight(uint8_t level);     // AMOLED brightness register
bool gfx_draw_bitmap(uint16_t *fb);        // implemented on the active panel
inline void display_flush(uint16_t *fb) {  // blocking full-frame push
  gfx_draw_bitmap(fb);
}

extern PanelVariant panel_variant;
extern uint16_t *framebuffer;  // LCD_WIDTH*LCD_HEIGHT RGB565, PSRAM
