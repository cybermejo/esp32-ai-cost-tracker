#pragma once
#include <stdint.h>
#include <vector>
// Unconditional include: the include path decides which header this is —
// test/shim stub on host builds (-Itest/shim), real driver on device.
// Never include via a relative test/... path from src/.
#include <Arduino_GFX_Library.h>
#include "../data/model.h"
#include "../data/Aggregator.h"
class PieScene {
 public:
  void enter(const std::vector<Day>& days, uint32_t now);
  void draw(Arduino_GFX* g, uint32_t now, int xOff = 0);
  bool animating(uint32_t now) const;  // grow/first-frame still running
 private:
  std::vector<Slice> slices; float total = 0; uint32_t t0 = 0;
  char rangeA[6] = {'?','?','-','?','?',0};  // oldest day MM-DD (set in enter)
  char rangeB[6] = {'?','?','-','?','?',0};  // newest day MM-DD (set in enter)
};
class ListScene {
 public:
  void enter(const std::vector<Day>& days, uint32_t now);
  int rowAt(int y) const;          // 0-based row or -1 when y hits no row
  void draw(Arduino_GFX* g, uint32_t now, int xOff = 0);
 private:
  std::vector<Day> days; uint32_t t0 = 0;
};
class DetailScene {  // R10 header slice: Task 8 adds model rows below y=64
 public:
  void enter(const Day& d, uint32_t now);
  void scrollBy(int dy);
  int maxScroll() const;
  int scroll() const { return scrollY; }
  void draw(Arduino_GFX* g, uint32_t now, int xOff = 0);
 private:
  Day day; uint32_t t0 = 0; int scrollY = 0;
};
class VersionScene {  // modal: project name + version badge, no scroll
 public:
  void enter(uint32_t now) { (void)now; }
  // yOff shifts the whole sheet for the top pull-down transition.
  void draw(Arduino_GFX* g, uint32_t now, int xOff = 0, int yOff = 0);
};
