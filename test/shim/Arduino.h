// Minimal Arduino.h shim so the firmware's portable modules (fluid, imu
// attitude logic) compile and run on the host for verification tests.
#pragma once

#include <cstdint>
#include <cstdarg>
#include <cstdio>

struct HostSerial {
  void begin(unsigned long) {}
  void setTxTimeoutMs(unsigned long) {}
  void printf(const char *, ...) {}
  void println(const char * = "") {}
  void print(const char *) {}
  void flush() {}
  int available() { return 0; }
  int read() { return -1; }
};
extern HostSerial Serial;
