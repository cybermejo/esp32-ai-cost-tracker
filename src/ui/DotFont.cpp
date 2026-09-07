#include "DotFont.h"
#include "../config.h"
#ifndef UNIT_TEST
void uiText(Arduino_GFX* g, int x, int y, const String& s, uint16_t color, int scale) {
  int cx = x;
  for (unsigned i = 0; i < s.length(); i++) {
    char c = s[i];
    if (c < 32 || c > 127) { cx += 6 * scale; continue; }
    const uint8_t* gl = FONT5X7[c - 32];
    for (int col = 0; col < 5; col++)
      for (int row = 0; row < 7; row++)
        if (gl[col] & (1 << row))
          g->fillRect(cx + col * scale, y + row * scale, scale, scale, color);
    cx += 6 * scale;
  }
}
void uiRule(Arduino_GFX* g, int x, int y, int w, uint16_t color) {
  g->fillRect(x, y, w, 1, color);
}
void uiCard(Arduino_GFX* g, int x, int y, int w, int h) {
  const Palette& P = activePalette();
  g->fillRoundRect((int16_t)x, (int16_t)y, (int16_t)w, (int16_t)h, 12, P.card);
  g->drawRoundRect((int16_t)x, (int16_t)y, (int16_t)w, (int16_t)h, 12, P.hairline);
}
void uiPill(Arduino_GFX* g, int x, int y, int w, int h, const String& label) {
  const Palette& P = activePalette();
  int r = h / 2;
  g->fillRoundRect((int16_t)x, (int16_t)y, (int16_t)w, (int16_t)h, (int16_t)r, P.pillBg);
  int tw = (int)label.length() * 6 * 2;
  uiText(g, x + (w - tw) / 2, y + (h - 14) / 2, label, P.ink);
}
#else
// Host port: logic IDENTICAL to the device branch above. Compiles here
// because on host String == std::string (supports s[i]/s.length()) and the
// test/shim Arduino_GFX stub provides fillRect.
void uiText(Arduino_GFX* g, int x, int y, const String& s, uint16_t color, int scale) {
  int cx = x;
  for (unsigned i = 0; i < s.length(); i++) {
    char c = s[i];
    if (c < 32 || c > 127) { cx += 6 * scale; continue; }
    const uint8_t* gl = FONT5X7[c - 32];
    for (int col = 0; col < 5; col++)
      for (int row = 0; row < 7; row++)
        if (gl[col] & (1 << row))
          g->fillRect(cx + col * scale, y + row * scale, scale, scale, color);
    cx += 6 * scale;
  }
}
void uiRule(Arduino_GFX* g, int x, int y, int w, uint16_t color) {
  g->fillRect(x, y, w, 1, color);
}
void uiCard(Arduino_GFX* g, int x, int y, int w, int h) {
  const Palette& P = activePalette();
  g->fillRoundRect(x, y, w, h, 12, P.card);
  g->drawRoundRect(x, y, w, h, 12, P.hairline);
}
void uiPill(Arduino_GFX* g, int x, int y, int w, int h, const String& label) {
  const Palette& P = activePalette();
  int r = h / 2;
  g->fillRoundRect(x, y, w, h, r, P.pillBg);
  int tw = (int)label.length() * 6 * 2;
  uiText(g, x + (w - tw) / 2, y + (h - 14) / 2, label, P.ink);
}
#endif
