#pragma once
#include <stdint.h>
#include <vector>
#include "scenes.h"
#include "../input/Gesture.h"
#include "Tween.h"

enum class Screen { Pie, List, Detail, Version };

class SceneManager {
 public:
  void begin(const std::vector<Day>& d, uint32_t now);
  void showPie(uint32_t now);
  void showList(uint32_t now);
   void showDetail(const String& date, uint32_t now);
  void showVersion(uint32_t now);
  void dismissVersion(uint32_t now);
   void onEvent(Ev e, int x, int y, uint32_t now);
   void onDrag(int dy);
  void draw(Arduino_GFX* g, uint32_t now);
  // Stillness gate: the loop skips draw+flush unless something animates or
  // a state change (theme toggle, scroll) needs one fresh frame.
  bool needsDraw(uint32_t now);
  void markDirty() {
    dirty = true;
    cacheValid[0] = cacheValid[1] = cacheValid[2] = false;
  }
  // Pre-render static screens once at boot so the first swipe composites
  // from cache instead of live-drawing cold.
  void warmCaches(Arduino_GFX* g);
  Screen screen() const { return cur; }
 private:
  void goTo(Screen s, int dir, uint32_t now);
  void drawScreen(Screen s, Arduino_GFX* g, uint32_t now, int xOff);
  // Caches exist only for Pie/List/Detail; the version modal is never
  // cached (always live-drawn, never a blit source).
  int screenIdx(Screen s) const {
    return s == Screen::Pie ? 0 : s == Screen::List ? 1 : s == Screen::Detail ? 2 : -1;
  }
  bool cacheable(Screen s) const { return screenIdx(s) >= 0; }
  bool ensureCache(int i);
  void snapshot(int i);
  bool blit(uint32_t now);
  Screen cur = Screen::Pie, prev = Screen::Pie;
  Screen returnTo = Screen::Pie;  // version modal dismiss target
  int slideDir = 0;
  int detailIdx = -1;  // index into days of the open Detail, -1 when none
  bool dirty = true;
  bool pieWasGrowing = false;  // one settle frame after grow, to snapshot pie
  bool slideWasActive = false;  // one settle frame after slide, to center + cache
  Tween slide{0, 1, 200, 0};
  PieScene pie; ListScene list; DetailScene detail; VersionScene version;
  std::vector<Day> days;
  uint16_t* cache[3] = {nullptr, nullptr, nullptr};
  bool cacheValid[3] = {false, false, false};
};
