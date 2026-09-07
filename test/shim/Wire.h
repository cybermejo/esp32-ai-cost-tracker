// Host shim for Wire.h — I2C calls become harmless no-ops so the attitude
// code inside imu_qmi8658.cpp can be exercised without hardware.
#pragma once

#include <cstdint>
#include <cstddef>

class TwoWire {
 public:
  void begin(int, int, uint32_t = 100000) {}
  void beginTransmission(uint8_t) {}
  uint8_t endTransmission(bool = true) { return 0; }
  size_t write(uint8_t) { return 1; }
  int requestFrom(int, int) { return 0; }
  int read() { return 0; }
};
extern TwoWire Wire;
