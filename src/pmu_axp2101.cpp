#define XPOWERS_CHIP_AXP2101
#include <Arduino.h>
#include <Wire.h>
#include <XPowersLib.h>
#include "pmu_axp2101.h"
#include "config.h"

// AXP2101 via XPowersLib — same call pattern as the TamaPoke project on this
// exact board. Cold-boot lesson (was: raw register writes assuming rails
// stay up): after total power loss BLDO1 (OLED VDD) defaults OFF and the
// panel stays black although everything else boots, so the rail must be
// enabled explicitly before gfx->begin(). Long-press power-off stays at
// PMIC defaults (escape hatch when USB hangs).
static XPowersPMU pmu;
static bool pmuOk = false;

bool pmu_init() {
  pmuOk = pmu.begin(Wire, AXP2101_SLAVE_ADDRESS, IIC_SDA, IIC_SCL);
  if (!pmuOk) return false;
  pmu.setPowerKeyPressOffTime(XPOWERS_POWEROFF_4S);
  pmu.disableIRQ(XPOWERS_AXP2101_ALL_IRQ);
  pmu.enableIRQ(XPOWERS_AXP2101_PKEY_SHORT_IRQ);
  pmu.clearIrqStatus();
  return true;
}

void pmuEnablePanel() {
  if (!pmuOk && !pmu.begin(Wire, AXP2101_SLAVE_ADDRESS, IIC_SDA, IIC_SCL)) {
    Serial.println("[pmu] AXP2101 missing, panel rail not enabled");
    return;
  }
  pmu.setBLDO1Voltage(3300);  // OLED VDD
  pmu.enableBLDO1();
}

bool pmu_pwr_key_pressed() {
  if (!pmuOk) return false;
  pmu.getIrqStatus();
  bool hit = pmu.isPekeyShortPressIrq();
  if (hit) pmu.clearIrqStatus();
  return hit;
}
