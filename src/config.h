#pragma once
#define LCD_WIDTH 368
#define LCD_HEIGHT 448
#define LCD_CS    12
#define LCD_SCLK  11
#define LCD_SDIO0 4
#define LCD_SDIO1 5
#define LCD_SDIO2 6
#define LCD_SDIO3 7
#define IIC_SDA 15
#define IIC_SCL 14
#define IIC_FREQ_HZ 400000
#define TOUCH_ADDR_V1 0x38
#define TOUCH_ADDR_V2 0x15
#define TP_INT 21
#define AXP2101_ADDR 0x34
#define EXPANDER_ADDR 0x20
#define FW_VERSION "1.0.0"
// Screen-safe margins: the glass bezel clips the outer ~8px, so chrome and
// cards live between UI_MARGIN (20) and UI_RIGHT (348).
#define UI_MARGIN 20
#define UI_RIGHT 348
#define UI_FPS 30
#define UI_FRAME_MS (1000 / UI_FPS)
#define PIE_DIAMETER 260
#define DOT_RADIUS 1
#define DOT_PITCH_X 7
#define DOT_PITCH_Y 9
