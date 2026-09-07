// Render harness: draw every scene in BOTH themes off-device, dump BMPs.
// Run: make -f test/Makefile render
// NOTE: a FRESH Arduino_GFX per dump — a reused instance leaves the
// previous frame's pixels under slide xOffsets (stale-strip artifact).
#include <cstdio>
#include <vector>
#include "Arduino.h"
#include <Arduino_GFX_Library.h>
#include "../src/data/SampleProvider.h"
#include "../src/ui/scenes.h"
#include "../src/ui/Theme.h"
HostSerial Serial;
static int renderTheme(Theme t, const char* tag, const std::vector<Day>& days, const Day& d01) {
  setActiveTheme(t);
  { Arduino_GFX g; PieScene pie; pie.enter(days, 1000);
    pie.draw(&g, 2000); char p[64]; snprintf(p, sizeof p, "build/%s_pie.bmp", tag);
    if (!g.dumpBMP(p)) return 1; }
  { Arduino_GFX g; ListScene list; list.enter(days, 1000);
    list.draw(&g, 2000); char p[64]; snprintf(p, sizeof p, "build/%s_list.bmp", tag);
    if (!g.dumpBMP(p)) return 1; }
  { Arduino_GFX g; DetailScene det; det.enter(d01, 1000);
    det.draw(&g, 1300); char p[64]; snprintf(p, sizeof p, "build/%s_detail_top.bmp", tag);
    if (!g.dumpBMP(p)) return 1; }
  { Arduino_GFX g; DetailScene det; det.enter(d01, 1000);
    det.scrollBy(det.maxScroll());
    printf("%s detail maxScroll=%d rows=%u\n", tag, det.maxScroll(), (unsigned)d01.models.size());
    det.draw(&g, 2500); char p[64]; snprintf(p, sizeof p, "build/%s_detail_bottom.bmp", tag);
    if (!g.dumpBMP(p)) return 1; }
  { Arduino_GFX g; VersionScene vs; vs.enter(1000);
    vs.draw(&g, 1000); char p[64]; snprintf(p, sizeof p, "build/%s_version.bmp", tag);
    if (!g.dumpBMP(p)) return 1; }
  return 0;
}
int main() {
  SampleProvider p; std::vector<Day> days;
  if (!p.load(days)) { printf("load FAILED\n"); return 1; }
  Day d01; for (size_t i = 0; i < days.size(); i++)
    if (days[i].date == "2026-09-01") d01 = days[i];
  if (renderTheme(Theme::Light, "light", days, d01)) return 1;
  if (renderTheme(Theme::Dark, "dark", days, d01)) return 1;
  printf("rendered 8 frames\n"); return 0;
}
