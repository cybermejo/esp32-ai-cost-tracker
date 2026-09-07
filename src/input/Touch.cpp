#include "Touch.h"
#include <Arduino.h>
#include <Wire.h>
#include <cstdio>
#include "../config.h"

static uint8_t s_addr = 0;

static bool ack(uint8_t a) {
  Wire.beginTransmission(a);
  return Wire.endTransmission() == 0;
}

bool touchInit() {
  // Idempotent re-probe (display_init already probed the same addrs; no
  // shared result is exposed, so probe again — harmless on the bus).
  if (ack(TOUCH_ADDR_V2)) s_addr = TOUCH_ADDR_V2;
  else if (ack(TOUCH_ADDR_V1)) s_addr = TOUCH_ADDR_V1;
  else s_addr = 0;
  if (s_addr) {
    char b[32]; snprintf(b, sizeof b, "[touch] CST820 @0x%02x ok", s_addr);
    Serial.println(b);
    return true;
  }
  Serial.println("[touch] none, PWR fallback");
  return false;
}

bool touchPoll(TouchPoint &p) {
  if (!s_addr) return false;
  Wire.beginTransmission(s_addr);
  Wire.write(0x01);
  if (Wire.endTransmission(false) != 0) return false;
  if (Wire.requestFrom(s_addr, (uint8_t)7) < 7) return false;
  uint8_t d[7];
  for (int i = 0; i < 7; i++) d[i] = (uint8_t)Wire.read();
  if (d[1] == 0) { p.down = false; return true; }
  p.down = true;
  p.x = (((int)d[2] & 0x0F) << 8) | d[3];
  p.y = (((int)d[4] & 0x0F) << 8) | d[5];
  return true;
}
