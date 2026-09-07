#pragma once
#include <stdint.h>
enum class Ev { None, Tap, SwipeLeft, SwipeRight, SwipeDown, SwipeUp, Drag };
class Gesture {
 public:
  Ev feed(bool down, int x, int y, uint32_t ms);
  Ev feedMove(int x, int y);
  int dragDy() const { return lastDy; }
 private:
  bool armed = false; int x0 = 0, y0 = 0; uint32_t m0 = 0;
  int lastX = 0, lastY = 0; int lastDy = 0;
};
