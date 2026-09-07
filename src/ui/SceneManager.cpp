#include "SceneManager.h"
#include "composite.h"
#include "../config.h"
#include "../display.h"
#include <cstdio>
#ifndef UNIT_TEST
#include <esp_heap_caps.h>
#endif

void SceneManager::begin(const std::vector<Day>& d, uint32_t now) {
  days = d;
  cur = prev = Screen::Pie; slideDir = 0;
  slide = Tween{0, 1, 200, 0};  // done for any now > 200
  pie.enter(days, now);
}

void SceneManager::goTo(Screen s, int dir, uint32_t now) {
  prev = cur; cur = s; slideDir = dir;
  slide = Tween{0, 1, 200, now};
}

void SceneManager::showPie(uint32_t now) {
  pie.enter(days, now);
  goTo(Screen::Pie, +1, now);
}

void SceneManager::showList(uint32_t now) {
  list.enter(days, now);
  goTo(Screen::List, cur == Screen::Detail ? +1 : -1, now);
}

void SceneManager::showDetail(const String& date, uint32_t now) {
  for (size_t i = 0; i < days.size(); i++) {
    if (days[i].date == date) {
      detailIdx = (int)i;
      detail.enter(days[i], now);
      // The entering day's rows differ from whatever day this cache was
      // snapshotted from: drop it, or the slide composites (and freezes on)
      // the previous day's rows until some interaction forces a live draw.
      // List never invalidates (days are static all session); pie replays
      // its grow on every showPie and re-caches at settle by itself.
      cacheValid[screenIdx(Screen::Detail)] = false;
      goTo(Screen::Detail, -1, now);
      return;
    }
  }
}

void SceneManager::showVersion(uint32_t now) {
  if (cur == Screen::Version) return;
  returnTo = cur;
  version.enter(now);
  goTo(Screen::Version, -1, now);
}

void SceneManager::dismissVersion(uint32_t now) {
  Screen dst = returnTo == Screen::Version ? Screen::Pie : returnTo;
  if (dst == Screen::List) { showList(now); return; }
  if (dst == Screen::Detail && !days.empty() &&
      detailIdx >= 0 && detailIdx < (int)days.size()) {
    showDetail(days[(size_t)detailIdx].date, now);
    return;
  }
  showPie(now);
}

void SceneManager::onEvent(Ev e, int x, int y, uint32_t now) {
  if (e == Ev::None) return;
  if (e == Ev::SwipeDown) { showVersion(now); return; }
  switch (cur) {
    case Screen::Pie:
      if (e == Ev::SwipeLeft) showList(now);
      break;
    case Screen::List:
      if (e == Ev::SwipeRight) showPie(now);
      else if (e == Ev::SwipeLeft) {
        if (!days.empty()) showDetail(days[0].date, now);  // newest first
      } else if (e == Ev::Tap) {
        int r = list.rowAt(y);
        if (r >= 0 && r < (int)days.size()) showDetail(days[(size_t)r].date, now);
      }
      break;
    case Screen::Detail:
      if (e == Ev::SwipeRight) showList(now);
      else if (e == Ev::SwipeLeft) {
        if (detailIdx + 1 < (int)days.size()) showDetail(days[(size_t)(detailIdx + 1)].date, now);
      } else if (e == Ev::Tap && x > 256 && y < 40) showList(now);
      break;
    case Screen::Version:
      if (e == Ev::SwipeUp || e == Ev::SwipeRight) dismissVersion(now);
      else if (e == Ev::Tap && x > 256 && y < 40) dismissVersion(now);
      break;
  }
}

void SceneManager::onDrag(int dy) {
  if (cur == Screen::Detail) { detail.scrollBy(-dy); dirty = true; }
}

bool SceneManager::needsDraw(uint32_t now) {
  if (dirty) { dirty = false; return true; }
  if (!slide.done(now)) { slideWasActive = true; return true; }
  // Settle frame: the slide just ended (possibly after a single slow live
  // frame) — draw once more so the layout lands centered AND the entering
  // screen's cache warms. Without this, stillness freezes mid-transition
  // and only the next touch repaints.
  if (slideWasActive) { slideWasActive = false; return true; }
  // The settle frame: pie just finished growing — draw once more so the
  // pristine frame hits the panel AND the pie cache warms. Without this,
  // stillness would freeze before snapshotting and every future slide
  // involving pie would fall back to slow live-drawing.
  bool growing = pie.animating(now);
  if (growing) { pieWasGrowing = true; return true; }
  if (pieWasGrowing) { pieWasGrowing = false; return true; }
  return false;
}

void SceneManager::drawScreen(Screen s, Arduino_GFX* g, uint32_t now, int xOff) {
  switch (s) {
    case Screen::Pie: pie.draw(g, now, xOff); break;
    case Screen::List: list.draw(g, now, xOff); break;
    case Screen::Detail: detail.draw(g, now, xOff); break;
    case Screen::Version: version.draw(g, now, xOff); break;
  }
}

void SceneManager::draw(Arduino_GFX* g, uint32_t now) {
  bool settled = (slideDir == 0 || slide.done(now));
  // Vertical drawer: the version sheet drops from the top over the current
  // screen (open) and lifts back up (dismiss). Live-drawn both layers —
  // the sheet is cheap (text + pill), the base is one static scene.
  if (!settled && (cur == Screen::Version || prev == Screen::Version)) {
    float p = slide.at(now);
    int yOff;
    Screen base;
    if (cur == Screen::Version) {
      base = prev;
      yOff = (int)(-LCD_HEIGHT * (1.0f - p));
    } else {
      base = cur;
      yOff = (int)(-LCD_HEIGHT * p);
    }
    drawScreen(base, g, now, 0);
    version.draw(g, now, 0, yOff);
    return;
  }
  // Fast path: composite the transition from settled-screen caches. The
  // version modal is never cached (screenIdx -1): guard before indexing.
  if (!settled && cacheable(prev) && cacheable(cur) &&
      cacheValid[screenIdx(prev)] && cacheValid[screenIdx(cur)] && blit(now)) return;
  if (!settled) {
    float p = slide.at(now);
    int w = LCD_WIDTH;
    drawScreen(prev, g, now, (int)(slideDir * p * w));
    drawScreen(cur, g, now, (int)(slideDir * (p - 1.0f) * w));
    return;
  }
  drawScreen(cur, g, now, 0);
  // Snapshot settled static screens so later slides composite from cache.
  // A still-growing pie is explicitly excluded (it repaints next frame).
  // The version modal is never cached.
  if (cur == Screen::Pie && pie.animating(now)) return;
  if (cacheable(cur)) snapshot(screenIdx(cur));
}

bool SceneManager::ensureCache(int i) {
  if (cache[i]) return true;
#ifndef UNIT_TEST
  cache[i] = (uint16_t*)heap_caps_malloc((size_t)LCD_WIDTH * LCD_HEIGHT * 2,
                                         MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
#else
  cache[i] = new uint16_t[(size_t)LCD_WIDTH * LCD_HEIGHT];
#endif
  return cache[i] != nullptr;
}

void SceneManager::snapshot(int i) {
#ifndef UNIT_TEST
  if (!ensureCache(i) || !framebuffer) return;
  memcpy(cache[i], framebuffer, (size_t)LCD_WIDTH * LCD_HEIGHT * 2);
  cacheValid[i] = true;
#else
  (void)i;
#endif
}

bool SceneManager::blit(uint32_t now) {
#ifndef UNIT_TEST
  if (!framebuffer) return false;
  compositeSlide(cache[screenIdx(prev)], cache[screenIdx(cur)], framebuffer,
                 LCD_WIDTH, LCD_HEIGHT, slideDir, slide.at(now));
  return true;
#else
  (void)now;
  return false;
#endif
}

void SceneManager::warmCaches(Arduino_GFX* g) {
  if (days.empty()) return;
  const uint32_t done = 100000;  // tweens long complete: settled pixels
  list.enter(days, done);
  list.draw(g, done, 0);
  snapshot(screenIdx(Screen::List));
  detail.enter(days[0], done);
  detail.draw(g, done, 0);
  snapshot(screenIdx(Screen::Detail));
}
