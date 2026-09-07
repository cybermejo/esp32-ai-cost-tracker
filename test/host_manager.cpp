// Host verification: stillness gate — draw only while animating or dirty.
// Run: make -f test/Makefile manager
#include <cstdio>
#include <vector>
#include "Arduino.h"
#include <Arduino_GFX_Library.h>
#include "../src/data/SampleProvider.h"
#include "../src/ui/SceneManager.h"
#include "../src/ui/composite.h"

HostSerial Serial;
static int failures = 0;
#define EXPECT_TRUE(name, expr) do { bool _ok = (expr); printf("%-44s %s\n", name, _ok ? "OK" : "FAIL"); if (!_ok) failures++; } while (0)

int main() {
  SampleProvider p; std::vector<Day> days;
  if (!p.load(days)) { printf("load FAILED\n"); return 1; }
  Arduino_GFX g;
  SceneManager mgr; mgr.begin(days, 1000);
  EXPECT_TRUE("first frame draws", mgr.needsDraw(1000));
  mgr.draw(&g, 1000);
  EXPECT_TRUE("pie grow animates", mgr.needsDraw(1100));
  mgr.draw(&g, 1100);
  EXPECT_TRUE("settle frame draws once", mgr.needsDraw(2000));
  mgr.draw(&g, 2000);
  EXPECT_TRUE("settled pie is still", !mgr.needsDraw(2000));
  mgr.showList(2000);
  EXPECT_TRUE("slide animates", mgr.needsDraw(2000));
  mgr.draw(&g, 2000);
  EXPECT_TRUE("slide settle frame draws once", mgr.needsDraw(2300));
  mgr.draw(&g, 2300);
  EXPECT_TRUE("settled list is still", !mgr.needsDraw(2300));
  mgr.markDirty();
  EXPECT_TRUE("dirty draws once", mgr.needsDraw(2300));
  EXPECT_TRUE("dirty is consumed", !mgr.needsDraw(2300));
  // Full swipe chain: Pie -L-> List -L-> Detail(newest) -L-> older -R-> List -R-> Pie.
  SceneManager nav; nav.begin(days, 4000);
  EXPECT_TRUE("starts on Pie", nav.screen() == Screen::Pie);
  nav.onEvent(Ev::SwipeLeft, 300, 200, 4100);
  EXPECT_TRUE("Pie left goes List", nav.screen() == Screen::List);
  nav.onEvent(Ev::SwipeLeft, 300, 200, 4200);
  EXPECT_TRUE("List left goes Detail", nav.screen() == Screen::Detail);
  for (int i = 0; i < 10; i++) nav.onEvent(Ev::SwipeLeft, 300, 200, 4300 + i * 100);
  EXPECT_TRUE("left at oldest stays Detail", nav.screen() == Screen::Detail);
  nav.onEvent(Ev::SwipeRight, 100, 200, 5500);
  EXPECT_TRUE("Detail right goes List", nav.screen() == Screen::List);
  nav.onEvent(Ev::SwipeRight, 100, 200, 5600);
  EXPECT_TRUE("List right goes Pie", nav.screen() == Screen::Pie);
  // Version modal: down from anywhere opens, up/right/BACK returns.
  SceneManager vm; vm.begin(days, 5000);
  vm.onEvent(Ev::SwipeDown, 180, 30, 5100);
  EXPECT_TRUE("down opens version", vm.screen() == Screen::Version);
  vm.onEvent(Ev::SwipeDown, 180, 30, 5150);
  EXPECT_TRUE("down on version stays", vm.screen() == Screen::Version);
  vm.onEvent(Ev::SwipeUp, 180, 300, 5200);
  EXPECT_TRUE("up dismisses to Pie", vm.screen() == Screen::Pie);
  vm.onEvent(Ev::SwipeLeft, 300, 200, 5300);
  vm.onEvent(Ev::SwipeDown, 180, 30, 5400);
  EXPECT_TRUE("down from List opens", vm.screen() == Screen::Version);
  vm.onEvent(Ev::SwipeRight, 100, 200, 5500);
  EXPECT_TRUE("right returns to List", vm.screen() == Screen::List);
  vm.onEvent(Ev::Tap, 100, 100, 5600);
  EXPECT_TRUE("tap opens detail", vm.screen() == Screen::Detail);
  vm.onEvent(Ev::SwipeDown, 180, 30, 5700);
  EXPECT_TRUE("down from Detail opens", vm.screen() == Screen::Version);
  vm.onEvent(Ev::Tap, 300, 20, 5800);
  EXPECT_TRUE("BACK returns to Detail", vm.screen() == Screen::Detail);
  // compositeSlide: clean vertical split tracking slide geometry.
  {
    const int W = 368, H = 448;
    static uint16_t pv[368 * 448], cu[368 * 448], out[368 * 448];
    for (int i = 0; i < W * H; i++) { pv[i] = 0x1111; cu[i] = 0x2222; out[i] = 0; }
    compositeSlide(pv, cu, out, W, H, -1, 0.5f);
    EXPECT_TRUE("slide halves split", out[200 * W + 10] == 0x1111 && out[200 * W + 360] == 0x2222);
    compositeSlide(pv, cu, out, W, H, -1, 0.0f);
    EXPECT_TRUE("p=0 shows prev", out[200 * W + 10] == 0x1111 && out[200 * W + 360] == 0x1111);
    compositeSlide(pv, cu, out, W, H, -1, 1.0f);
    EXPECT_TRUE("p=1 shows cur", out[200 * W + 10] == 0x2222 && out[200 * W + 360] == 0x2222);
    compositeSlide(pv, cu, out, W, H, +1, 0.5f);
    EXPECT_TRUE("dir+1 mirrors", out[200 * W + 10] == 0x2222 && out[200 * W + 360] == 0x1111);
  }
  if (failures) { printf("%d FAILURES\n", failures); return 1; }
  printf("all host manager checks passed\n"); return 0;
}
