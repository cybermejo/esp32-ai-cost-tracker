#pragma once
#include <stdint.h>

// Notion reskin theme system: Light (warm paper, default) + Dark (legacy
// black AMOLED theme). All 565 colors derive from 8-bit hex via constexpr
// helpers — no hand-computed magic numbers.
enum class Theme : uint8_t { Light = 0, Dark = 1 };

constexpr uint16_t rgb565(uint8_t r, uint8_t g, uint8_t b) {
  return (uint16_t)(((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3));
}
constexpr uint8_t mix8(uint8_t fg, uint8_t bg, int fgPct) {
  return (uint8_t)((fg * fgPct + bg * (100 - fgPct)) / 100);
}

struct Palette {
  uint16_t bg;        // page canvas
  uint16_t card;      // card surface
  uint16_t ink;       // primary text
  uint16_t ink60;     // secondary text
  uint16_t ink40;     // muted text
  uint16_t hairline;  // 1px card borders
  uint16_t pillBg;    // tinted pill wash (sky tint light / deep blue dark)
  uint16_t accent;    // totals, BACK, max-day (blue light / red dark)
  uint16_t slice[5];  // pie / bar colors
  uint16_t other;     // Other bucket + dimmed $0 rows
  uint8_t brightness;  // default backlight for the theme
};

// Uniform two-tone chrome: everything except the pie is bg/text blends.
// Light: bg #FAF9F5, text #181715. Cards share the canvas (hairline-bound);
// pills are a hairline-tone wash; accent collapses to ink.
inline Palette lightPalette() {
  Palette p;
  p.bg = rgb565(0xFA, 0xF9, 0xF5);
  p.card = rgb565(0xFA, 0xF9, 0xF5);
  p.ink = rgb565(0x18, 0x17, 0x15);
  p.ink60 = rgb565(mix8(0x18, 0xFA, 60), mix8(0x17, 0xF9, 60), mix8(0x15, 0xF5, 60));
  p.ink40 = rgb565(mix8(0x18, 0xFA, 40), mix8(0x17, 0xF9, 40), mix8(0x15, 0xF5, 40));
  p.hairline = rgb565(mix8(0x18, 0xFA, 10), mix8(0x17, 0xF9, 10), mix8(0x15, 0xF5, 10));
  p.pillBg = rgb565(mix8(0x18, 0xFA, 10), mix8(0x17, 0xF9, 10), mix8(0x15, 0xF5, 10));
  p.accent = rgb565(0x18, 0x17, 0x15);
  p.slice[0] = rgb565(0x1E, 0x88, 0xE5);  // material blue 600
  p.slice[1] = rgb565(0xFF, 0xB3, 0x00);  // material amber 600
  p.slice[2] = rgb565(0x00, 0x89, 0x7B);  // material teal 600
  p.slice[3] = rgb565(0xFB, 0x8C, 0x00);  // material orange 600
  p.slice[4] = rgb565(0x8E, 0x24, 0xAA);  // material purple 600
  p.other = rgb565(0x75, 0x75, 0x75);     // material gray 600
  p.brightness = 150;
  return p;
}

// Dark mirror: bg #181715, text #FAF9F5, same mono discipline. Pie keeps
// the shared material-600 set (readable on navy).
inline Palette darkPalette() {
  Palette p;
  p.bg = rgb565(0x18, 0x17, 0x15);
  p.card = rgb565(0x18, 0x17, 0x15);
  p.ink = rgb565(0xFA, 0xF9, 0xF5);
  p.ink60 = rgb565(mix8(0xFA, 0x18, 60), mix8(0xF9, 0x17, 60), mix8(0xF5, 0x15, 60));
  p.ink40 = rgb565(mix8(0xFA, 0x18, 40), mix8(0xF9, 0x17, 40), mix8(0xF5, 0x15, 40));
  p.hairline = rgb565(mix8(0xFA, 0x18, 12), mix8(0xF9, 0x17, 12), mix8(0xF5, 0x15, 12));
  p.pillBg = rgb565(mix8(0xFA, 0x18, 12), mix8(0xF9, 0x17, 12), mix8(0xF5, 0x15, 12));
  p.accent = rgb565(0xFA, 0xF9, 0xF5);
  p.slice[0] = rgb565(0x1E, 0x88, 0xE5);  // material blue 600
  p.slice[1] = rgb565(0xFF, 0xB3, 0x00);  // material amber 600
  p.slice[2] = rgb565(0x00, 0x89, 0x7B);  // material teal 600
  p.slice[3] = rgb565(0xFB, 0x8C, 0x00);  // material orange 600
  p.slice[4] = rgb565(0x8E, 0x24, 0xAA);  // material purple 600
  p.other = rgb565(0x75, 0x75, 0x75);     // material gray 600
  p.brightness = 200;
  return p;
}

void themeInit();    // call once in setup(): loads NVS choice (default Light)
Theme loadTheme();   // last saved choice (in-memory on host)
void saveTheme(Theme t);
Theme activeTheme();
void setActiveTheme(Theme t);  // updates state + persists
inline const Palette& activePalette() {
  static Palette light = lightPalette();
  static Palette dark = darkPalette();
  return activeTheme() == Theme::Light ? light : dark;
}
