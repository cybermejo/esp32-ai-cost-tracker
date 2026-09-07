// Host verification: easeOutCubic, Tween, Gesture tap/swipe.
// Run: make -f test/Makefile input
#include <cmath>
#include <cstdio>
#include "Arduino.h"
#include "../src/ui/Tween.h"
#include "../src/input/Gesture.h"

HostSerial Serial;
static int failures = 0;
#define EXPECT_TRUE(name, cond) do { bool _ok = (cond); printf("%-44s %s\n", name, _ok ? "OK" : "FAIL"); if (!_ok) failures++; } while (0)

int main() {
  EXPECT_TRUE("ease(0)==0", fabsf(easeOutCubic(0) - 0.0f) < 0.0001f);
  EXPECT_TRUE("ease(1)==1", fabsf(easeOutCubic(1) - 1.0f) < 0.0001f);
  EXPECT_TRUE("ease(0.5)>0.5", easeOutCubic(0.5f) > 0.5f);
  Tween tw{0, 100, 250, 1000};
  EXPECT_TRUE("tween midpoint 87.5", fabsf(tw.at(1125) - 87.5f) < 1.0f);
  EXPECT_TRUE("tween done@1250", tw.done(1250));
  EXPECT_TRUE("tween not done early", !tw.done(1100));
  Gesture g1; g1.feed(true, 100, 100, 0);
  EXPECT_TRUE("tap", g1.feed(false, 104, 102, 200) == Ev::Tap);
  Gesture g2; g2.feed(true, 200, 100, 0);
  EXPECT_TRUE("swipe left", g2.feed(false, 100, 105, 250) == Ev::SwipeLeft);
  Gesture g3; g3.feed(true, 100, 100, 0);
  EXPECT_TRUE("swipe right", g3.feed(false, 200, 95, 250) == Ev::SwipeRight);
  Gesture g4; g4.feed(true, 100, 100, 0);
  EXPECT_TRUE("slow drift is none", g4.feed(false, 130, 130, 2000) == Ev::None);
  Gesture g5;
  EXPECT_TRUE("release w/o press is none", g5.feed(false, 50, 50, 100) == Ev::None);
  Gesture g8; g8.feed(true, 100, 100, 0);
  EXPECT_TRUE("wobbly tap still tap", g8.feed(false, 115, 108, 250) == Ev::Tap);
  Gesture g9; g9.feed(true, 100, 100, 0);
  EXPECT_TRUE("slow tap is none", g9.feed(false, 105, 105, 450) == Ev::None);
  Gesture g10; g10.feed(true, 200, 100, 0);
  EXPECT_TRUE("diagonal swipe counts", g10.feed(false, 80, 150, 250) == Ev::SwipeLeft);
  Gesture g11; g11.feed(true, 100, 100, 0);
  EXPECT_TRUE("steep diagonal is none", g11.feed(false, 170, 160, 250) == Ev::None);
  Gesture g12; g12.feed(true, 100, 100, 0);
  EXPECT_TRUE("short flick counts", g12.feed(false, 155, 105, 200) == Ev::SwipeRight);
  Gesture g13; g13.feed(true, 180, 20, 0);
  EXPECT_TRUE("top pull-down opens", g13.feed(false, 185, 140, 250) == Ev::SwipeDown);
  Gesture g14; g14.feed(true, 180, 200, 0);
  EXPECT_TRUE("mid-screen pull is none", g14.feed(false, 185, 320, 250) == Ev::None);
  Gesture g15; g15.feed(true, 180, 300, 0);
  EXPECT_TRUE("swipe up", g15.feed(false, 175, 180, 250) == Ev::SwipeUp);
  Gesture g6; g6.feed(true, 100, 100, 0);
  Ev dm1 = g6.feedMove(100, 120);
  EXPECT_TRUE("drag armed", dm1 == Ev::Drag);
  EXPECT_TRUE("drag dy +20", g6.dragDy() == 20);
  Ev dm2 = g6.feedMove(100, 90);
  EXPECT_TRUE("drag up", dm2 == Ev::Drag);
  EXPECT_TRUE("drag dy -30", g6.dragDy() == -30);
  Gesture g7;
  EXPECT_TRUE("move unarmed is none", g7.feedMove(50, 50) == Ev::None);
  if (failures) { printf("%d FAILURES\n", failures); return 1; }
  printf("all host input checks passed\n"); return 0;
}
