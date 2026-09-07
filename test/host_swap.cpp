// Regression test for the byte-order contract: framebuffer holds big-endian
// RGB565 (bus memcpy, setNoSwap). Simulates little-endian ESP32 store +
// memcpy + big-endian panel assembly, over the REAL active palettes.
// Run: make -f test/Makefile swap
#include <cstdio>
#include <stdint.h>
#include <vector>
#include "../src/fb_encode.h"
#include "../src/ui/Theme.h"

static int failures = 0;
#define EXPECT_TRUE(name, expr) do { bool _ok = (expr); printf("%-44s %s\n", name, _ok ? "OK" : "FAIL"); if (!_ok) failures++; } while (0)

// What the panel assembles when framebuffer bytes ship as-is: ESP32 stores
// little-endian (low byte first on the wire), panel reads big-endian.
static uint16_t panel_sees(uint16_t stored) {
  uint8_t lo = stored & 0xFF, hi = (stored >> 8) & 0xFF;
  return (uint16_t)((lo << 8) | hi);
}

int main() {
  std::vector<uint16_t> live;
  const Palette& L = lightPalette();
  const Palette& D = darkPalette();
  live.push_back(L.bg); live.push_back(L.card); live.push_back(L.ink);
  live.push_back(L.ink60); live.push_back(L.ink40); live.push_back(L.hairline);
  live.push_back(L.pillBg); live.push_back(L.accent);
  for (int i = 0; i < 5; i++) live.push_back(L.slice[i]);
  live.push_back(L.other);
  live.push_back(D.bg); live.push_back(D.card); live.push_back(D.ink);
  live.push_back(D.ink60); live.push_back(D.ink40); live.push_back(D.hairline);
  live.push_back(D.accent);
  for (int i = 0; i < 5; i++) live.push_back(D.slice[i]);
  live.push_back(D.other);
  int bad = 0;
  for (size_t i = 0; i < live.size(); i++)
    if (panel_sees(fb_encode(live[i])) != live[i]) bad++;
  char n[64]; snprintf(n, sizeof n, "all %u live colors round-trip", (unsigned)live.size());
  EXPECT_TRUE(n, bad == 0);
  EXPECT_TRUE("black round-trips", panel_sees(fb_encode(0x0000)) == 0x0000);
  EXPECT_TRUE("white round-trips", panel_sees(fb_encode(0xFFFF)) == 0xFFFF);
  if (failures) { printf("%d FAILURES\n", failures); return 1; }
  printf("all swap checks passed\n"); return 0;
}
