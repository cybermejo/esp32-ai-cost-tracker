#pragma once

#include <cstdint>

// AXP2101 PMIC via XPowersLib: panel rail (BLDO1 = OLED VDD 3.3V, must be
// enabled before gfx->begin() or cold boots come up black) + PWR side-button
// short-press detection (polled IRQ status, no IRQ pin).
// Long-press power-off stays at PMIC defaults (escape hatch when USB hangs).

bool pmu_init();            // probe + enable PWR-key short-press IRQ
void pmuEnablePanel();      // BLDO1 3.3V on; call before display init
bool pmu_pwr_key_pressed(); // true once per short press (self-clearing)
