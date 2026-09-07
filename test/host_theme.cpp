// Host verification: Theme palettes, toggle, persistence round-trip.
// Run: make -f test/Makefile theme
#include <cstdio>
#include "Arduino.h"
#include "../src/ui/Theme.h"

HostSerial Serial;
static int failures = 0;
#define EXPECT_TRUE(name, expr) do { bool _ok = (expr); printf("%-44s %s\n", name, _ok ? "OK" : "FAIL"); if (!_ok) failures++; } while (0)

int main() {
  themeInit();
  EXPECT_TRUE("default is Light", activeTheme() == Theme::Light);
  EXPECT_TRUE("light bg is cream, not black", lightPalette().bg != 0x0000);
  EXPECT_TRUE("light card equals bg (uniform)", lightPalette().card == lightPalette().bg);
  EXPECT_TRUE("light accent is ink (mono chrome)", lightPalette().accent == rgb565(0x18, 0x17, 0x15));
  EXPECT_TRUE("dark bg is navy", darkPalette().bg == rgb565(0x18, 0x17, 0x15));
  EXPECT_TRUE("themes differ", lightPalette().bg != darkPalette().bg);
  EXPECT_TRUE("dark card equals bg (uniform)", darkPalette().card == darkPalette().bg);
  EXPECT_TRUE("dark accent is cream (mono chrome)", darkPalette().accent == rgb565(0xFA, 0xF9, 0xF5));
  EXPECT_TRUE("light brightness lower", lightPalette().brightness < darkPalette().brightness);
  setActiveTheme(Theme::Dark);
  EXPECT_TRUE("light ink is #181715", lightPalette().ink == rgb565(0x18, 0x17, 0x15));
  EXPECT_TRUE("light slices are material 600s",
    lightPalette().slice[0] == rgb565(0x1E, 0x88, 0xE5) &&
    lightPalette().slice[1] == rgb565(0xFF, 0xB3, 0x00) &&
    lightPalette().slice[4] == rgb565(0x8E, 0x24, 0xAA));
  EXPECT_TRUE("dark 4th slice is orange 600", darkPalette().slice[3] == rgb565(0xFB, 0x8C, 0x00));
  EXPECT_TRUE("toggle to Dark", activeTheme() == Theme::Dark);
  EXPECT_TRUE("persist round-trip", loadTheme() == Theme::Dark);
  EXPECT_TRUE("active palette follows", activePalette().bg == darkPalette().bg);
  setActiveTheme(Theme::Light);
  EXPECT_TRUE("toggle back to Light", activeTheme() == Theme::Light);
  if (failures) { printf("%d FAILURES\n", failures); return 1; }
  printf("all host theme checks passed\n"); return 0;
}
