#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_XCA9554.h>
#include "expander.h"
#include "config.h"

// Same sequence as the Waveshare examples / TamaPoke on this board: pins
// 0-2 (LCD/touch reset) pulse LOW->HIGH, pin 7 (SD power) HIGH.
static Adafruit_XCA9554 expander;

bool expanderReset() {
  if (!expander.begin(EXPANDER_ADDR)) {
    Serial.println("[expander] XCA9554 missing, no LCD/touch reset");
    return false;
  }
  expander.pinMode(0, OUTPUT);
  expander.pinMode(1, OUTPUT);
  expander.pinMode(2, OUTPUT);
  expander.digitalWrite(0, LOW);
  expander.digitalWrite(1, LOW);
  expander.digitalWrite(2, LOW);
  delay(20);
  expander.digitalWrite(0, HIGH);
  expander.digitalWrite(1, HIGH);
  expander.digitalWrite(2, HIGH);
  expander.pinMode(7, OUTPUT);
  expander.digitalWrite(7, HIGH);
  delay(50);
  return true;
}
