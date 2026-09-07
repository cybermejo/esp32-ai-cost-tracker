#include "Gesture.h"
#include <stdlib.h>
Ev Gesture::feed(bool down, int x, int y, uint32_t ms) {
  if (down) { armed = true; x0 = x; y0 = y; m0 = ms; lastX = x; lastY = y; lastDy = 0; return Ev::None; }
  if (!armed) return Ev::None; armed = false;
  int dx = x - x0, dy = y - y0;
  Ev e = Ev::None;
  // Tap tolerates finger wobble on lift (20px) and unhurried taps (400ms).
  if (abs(dx) < 20 && abs(dy) < 20 && ms - m0 < 400) e = Ev::Tap;
  // Swipe: 50px minimum with an angle gate (dominant axis must win 2:1).
  // SwipeDown additionally requires starting in the top edge zone (the
  // version-drawer pull); mid-screen downward flicks stay None so detail
  // scroll-drags never open it. SwipeUp has no zone (nothing scrolls up
  // into conflict; unmapped screens ignore it).
  else if (dx <= -50 && -dx > 2 * abs(dy)) e = Ev::SwipeLeft;
  else if (dx >= 50 && dx > 2 * abs(dy)) e = Ev::SwipeRight;
  else if (dy >= 50 && dy > 2 * abs(dx) && y0 < 64) e = Ev::SwipeDown;
  else if (dy <= -50 && -dy > 2 * abs(dx)) e = Ev::SwipeUp;
  return e;
}
Ev Gesture::feedMove(int x, int y) {
  if (!armed) return Ev::None;
  int dy = y - lastY;
  lastX = x; lastY = y; lastDy = dy;
  return Ev::Drag;
}
