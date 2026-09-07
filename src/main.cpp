#include <Arduino.h>
#include <Wire.h>
#include <string.h>
#include <cstdio>
#include <Arduino_GFX_Library.h>
#include "config.h"
#include "display.h"
#include "pmu_axp2101.h"
#include "data/SampleProvider.h"
#include "expander.h"
#include "fb_encode.h"
#include "input/Touch.h"
#include "input/Gesture.h"
#include "ui/SceneManager.h"
#include "ui/Theme.h"
#include "ui/IdleDim.h"

// SceneManager owns Pie/List/Detail + slide transitions. Touch (raw-Wire
// CST820, see input/Touch) feeds Gesture on contact edges; the one-line
// [touch] log is the permanent on-device gesture diagnostic.

static bool g_touchOk = false;

void setup() {
  Serial.begin(115200);
  // Non-blocking USB logs. With ARDUINO_USB_MODE=1 + CDC_ON_BOOT=1, Serial
  // IS the native USB-Serial-JTAG CDC; the TinyUSB `USBSerial` object only
  // exists when ARDUINO_USB_MODE=0, so it cannot be referenced here
  // (USBCDC.h: `#if !ARDUINO_USB_MODE && ARDUINO_USB_CDC_ON_BOOT`).
  Serial.setTxTimeoutMs(0);  // never block the loop on an unread serial port
  Wire.begin(IIC_SDA, IIC_SCL, IIC_FREQ_HZ);
  themeInit();  // NVS light/dark choice before first draw
  expanderReset();  // release LCD/touch reset (pins 0-2) + SD power (pin 7);
                    // panel/touch can stay dead on cold boot without this
  pmu_init();
  pmuEnablePanel();  // BLDO1 = OLED VDD 3.3V BEFORE display init; after total
                     // power loss the rail defaults OFF and the screen stays
                     // black although everything else boots
  while (!display_init()) {
    Serial.println("[display] init FAILED — check panel/power, retrying");
    delay(2000);
  }
  if (framebuffer) {
    memset(framebuffer, 0, (size_t)LCD_WIDTH * LCD_HEIGHT * 2);
    display_flush(framebuffer);
  }
  display_backlight(activePalette().brightness);
  g_touchOk = touchInit();  // logs "[touch] ..." itself; bus already up
  Serial.println("AICOST boot ok");
}

// Minimal Arduino_GFX adapter over the PSRAM framebuffer. Scenes draw
// through the Arduino_GFX API; this routes pixels into ::framebuffer, which
// loop() pushes with display_flush(). display.* and the hidden panel object
// are untouched.
class FbGfx : public Arduino_GFX {
 public:
  FbGfx() : Arduino_GFX(LCD_WIDTH, LCD_HEIGHT) {}
  bool begin(int32_t = GFX_NOT_DEFINED) override { return true; }
  void writePixelPreclipped(int16_t x, int16_t y, uint16_t color) override {
    if (x >= 0 && y >= 0 && x < LCD_WIDTH && y < LCD_HEIGHT && framebuffer)
      framebuffer[(int)y * LCD_WIDTH + (int)x] = fb_encode(color);
  }
};

static void handleGestureEvent(Ev e, int x, int y, uint32_t now,
                               SceneManager& mgr) {
  if (e == Ev::None) return;
  char b[48];
  if (e == Ev::Tap) snprintf(b, sizeof b, "[touch] tap %d %d", x, y);
  else if (e == Ev::SwipeLeft) snprintf(b, sizeof b, "[touch] swipe L");
  else if (e == Ev::SwipeRight) snprintf(b, sizeof b, "[touch] swipe R");
  else if (e == Ev::SwipeDown) snprintf(b, sizeof b, "[touch] swipe D");
  else if (e == Ev::SwipeUp) snprintf(b, sizeof b, "[touch] swipe U");
  else return;
  Serial.println(b);
  Screen before = mgr.screen();
  mgr.onEvent(e, x, y, now);
  if (mgr.screen() != before) {
    const char* nm = mgr.screen() == Screen::Pie ? "pie"
        : mgr.screen() == Screen::List ? "list"
        : mgr.screen() == Screen::Detail ? "detail" : "version";
    char sb[32]; snprintf(sb, sizeof sb, "[scene] %s enter", nm);
    Serial.println(sb);
  }
}

void loop() {
  static uint32_t last = 0;
  static FbGfx gfx;
  static SceneManager mgr;
  static Gesture gesture;
  static bool entered = false;
  static std::vector<Day> days;
  static bool lastDown = false;
  static int lastX = 0, lastY = 0;
  static IdleDim idle;
  static bool suppressGestures = false;  // waking contact never navigates
  uint32_t now = millis();

  if (g_touchOk) {
    TouchPoint tp;
    tp.down = lastDown;
    tp.x = lastX;
    tp.y = lastY;
    if (touchPoll(tp)) {
      if (tp.down && idle.activity(now)) {
        display_backlight(activePalette().brightness);
        Serial.println("[display] wake");
        suppressGestures = true;
      }
      if (!tp.down) suppressGestures = false;
      if (tp.down && !lastDown) {
        if (!suppressGestures) gesture.feed(true, tp.x, tp.y, now);
      } else if (!tp.down && lastDown) {
        if (!suppressGestures) {
          handleGestureEvent(gesture.feed(false, lastX, lastY, now),
                             lastX, lastY, now, mgr);
        }
      } else if (tp.down && lastDown) {
        if (!suppressGestures && (tp.x != lastX || tp.y != lastY)) {
          if (gesture.feedMove(tp.x, tp.y) == Ev::Drag)
            mgr.onDrag(gesture.dragDy());
        }
      }
      if (tp.down) {
        lastX = tp.x;
        lastY = tp.y;
      }
      lastDown = tp.down;
    }
  }

  // PWR short-press: wakes when dimmed (consumes the press, no theme flip);
  // otherwise toggles Light/Dark. Long-press stays PMIC hardware power-off.
  if (entered && pmu_pwr_key_pressed()) {
    bool wasDimmed = idle.isDimmed();
    idle.activity(now);
    if (wasDimmed) {
      display_backlight(activePalette().brightness);
      Serial.println("[display] wake");
    } else {
      setActiveTheme(activeTheme() == Theme::Light ? Theme::Dark : Theme::Light);
      display_backlight(activePalette().brightness);
      mgr.markDirty();  // stillness gate would otherwise skip the repaint
      Serial.println(activeTheme() == Theme::Light ? "[theme] light" : "[theme] dark");
    }
  }

  // Idle auto-dim runs outside the frame gate: stillness must not stop it.
  if (entered && idle.poll(now)) {
    display_backlight(IdleDim::DIM);
    Serial.println("[display] dim");
  }

  if (now - last >= UI_FRAME_MS) {
    last = now;
    if (!entered) {
      SampleProvider p;
      p.load(days);
      mgr.begin(days, now);
      entered = true;
      mgr.warmCaches(&gfx);  // pre-render static screens; first swipe blits
      idle.begin(now);
      Serial.println("[scene] pie enter");
    }
    if (mgr.needsDraw(now)) {
      mgr.draw(&gfx, now);
      if (framebuffer) display_flush(framebuffer);
    }
  }
}
