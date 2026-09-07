#pragma once
#include <stdint.h>

struct TouchPoint { bool down = false; int x = 0, y = 0; };

// Minimal raw-Wire CST820 driver (R2: no Arduino_DriveBus on this machine).
// Probes 0x15 (V2) then 0x38 (V1); logs which ACKed. Call AFTER display_init.
bool touchInit();               // false when neither address ACKs (PWR fallback)
bool touchPoll(TouchPoint &p);  // burst-read regs 0x01..0x07; true = sample read
