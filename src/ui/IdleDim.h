#pragma once
#include <stdint.h>

// Idle auto-dim: dims the panel after 30s without interaction, wakes on the
// next touch contact or PWR press. Edge-triggered: activity()/poll() return
// true only on a dimmed-state transition, so callers apply brightness then.
// Pure logic (millis timestamps in, no hardware) — fully host-testable;
// main.cpp owns backlight calls, gesture suppression, and PWR policy.
struct IdleDim {
  static const uint32_t TIMEOUT = 30000;
  static const uint8_t DIM = 25;
  uint32_t last = 0;
  bool dimmed = false;
  bool started = false;
  void begin(uint32_t now) { last = now; started = true; dimmed = false; }
  // Interaction happened. Returns true if this woke the display.
  bool activity(uint32_t now) {
    last = now;
    if (dimmed) { dimmed = false; return true; }
    return false;
  }
  // Call every loop. Returns true once, on the dimming edge.
  bool poll(uint32_t now) {
    if (!started || dimmed) return false;
    if (now - last >= TIMEOUT) { dimmed = true; return true; }
    return false;
  }
  bool isDimmed() const { return dimmed; }
};
