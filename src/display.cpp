#include <Arduino.h>
#include <Wire.h>
#include <esp_heap_caps.h>
#include "Arduino_GFX_Library.h"
#include "display.h"

PanelVariant panel_variant = PANEL_V2_CO5300;
uint16_t *framebuffer = nullptr;

static Arduino_DataBus *bus = nullptr;
static Arduino_TFT *panel = nullptr;      // drawing API (shared base)
static Arduino_CO5300 *co5300 = nullptr;  // V2 panel (owns setBrightness)
static Arduino_SH8601 *sh8601 = nullptr;  // V1 panel (owns setBrightness)

// The two hardware revisions share a PCB and pin map but use different display
// driver ICs (and different touch chips, conveniently on the same I2C bus).
// Probing the touch address is a reliable proxy for which display IC is fitted.
static bool i2c_ack(uint8_t addr) {
  Wire.beginTransmission(addr);
  return Wire.endTransmission() == 0;
}

bool display_init() {
  // -1 = unknown/absent touch chip; fall back to V2 (CO5300), current hardware.
  int touch = -1;
  if (i2c_ack(TOUCH_ADDR_V2)) touch = TOUCH_ADDR_V2;
  else if (i2c_ack(TOUCH_ADDR_V1)) touch = TOUCH_ADDR_V1;
  panel_variant = (touch == TOUCH_ADDR_V1) ? PANEL_V1_SH8601 : PANEL_V2_CO5300;

  bus = new Arduino_ESP32QSPI(LCD_CS, LCD_SCLK, LCD_SDIO0, LCD_SDIO1,
                              LCD_SDIO2, LCD_SDIO3);
  // our framebuffer is pre-swapped (big-endian RGB565) — see src/main.cpp (FbGfx) —
  // so the bus can memcpy chunks instead of swapping pixels on the CPU
  static_cast<Arduino_ESP32QSPI *>(bus)->setNoSwap(true);

  if (panel_variant == PANEL_V1_SH8601) {
    sh8601 = new Arduino_SH8601(bus, GFX_NOT_DEFINED, 0, LCD_WIDTH, LCD_HEIGHT);
    panel = sh8601;
  } else {
    // Waveshare's own V2 example passes col_offset1=16 for this panel.
    co5300 = new Arduino_CO5300(bus, GFX_NOT_DEFINED, 0, LCD_WIDTH, LCD_HEIGHT,
                                16, 0, 0, 0);
    panel = co5300;
  }

  if (!panel->begin()) return false;
  panel->fillScreen(RGB565_BLACK);
  display_backlight(255);

  // Framebuffer lives in PSRAM (368*448*2 = 330 KB does not fit in SRAM
  // alongside the Arduino core). 4-byte aligned for clean DMA access.
  framebuffer = (uint16_t *)heap_caps_calloc(
      LCD_WIDTH * LCD_HEIGHT, 2, MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
  return framebuffer != nullptr;
}

void display_backlight(uint8_t level) {
  // setBrightness is per-IC (brightness register), not on the TFT base class
  if (co5300) co5300->setBrightness(level);
  else if (sh8601) sh8601->setBrightness(level);
}

bool gfx_draw_bitmap(uint16_t *fb) {
  panel->draw16bitRGBBitmap(0, 0, fb, LCD_WIDTH, LCD_HEIGHT);
  return true;
}
