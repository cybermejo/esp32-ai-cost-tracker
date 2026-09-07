// Host-only stub of Arduino_GFX (framebuffer + BMP dump). Test path only.
#pragma once
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
class Arduino_GFX {
 public:
  static const int W = 368, H = 448;
  uint16_t fb[H][W];
  Arduino_GFX() { fillScreen(0); }
  void fillScreen(uint16_t c) { for (int y = 0; y < H; y++) for (int x = 0; x < W; x++) fb[y][x] = c; }
  void drawPixel(int x, int y, uint16_t c) { if (x >= 0 && x < W && y >= 0 && y < H) fb[y][x] = c; }
  void fillCircle(int x, int y, int r, uint16_t c) { for (int dy = -r; dy <= r; dy++) for (int dx = -r; dx <= r; dx++) if (dx*dx + dy*dy <= r*r) drawPixel(x+dx, y+dy, c); }
  void fillRect(int x, int y, int w, int h, uint16_t c) { for (int j = y; j < y+h; j++) for (int i = x; i < x+w; i++) drawPixel(i, j, c); }
  void drawLine(int x0, int y0, int x1, int y1, uint16_t c) {
    int dx = abs(x1 - x0), sx = x0 < x1 ? 1 : -1;
    int dy = -abs(y1 - y0), sy = y0 < y1 ? 1 : -1;
    int err = dx + dy, e2;
    for (;;) {
      drawPixel(x0, y0, c);
      if (x0 == x1 && y0 == y1) break;
      e2 = 2 * err;
      if (e2 >= dy) { err += dy; x0 += sx; }
      if (e2 <= dx) { err += dx; y0 += sy; }
    }
  }
  void fillRoundRect(int x, int y, int w, int h, int r, uint16_t c) {
    fillRect(x+r, y, w-2*r, h, c); fillRect(x, y+r, w, h-2*r, c);
    for (int dy = 0; dy < r; dy++) for (int dx = 0; dx < r; dx++) {
      if (dx*dx + dy*dy >= (r-1)*(r-1)) continue;
      drawPixel(x+r-1-dx, y+r-1-dy, c); drawPixel(x+w-r+dx, y+r-1-dy, c);
      drawPixel(x+r-1-dx, y+h-r+dy, c); drawPixel(x+w-r+dx, y+h-r+dy, c);
    }
  }
  void drawRoundRect(int x, int y, int w, int h, int r, uint16_t c) {
    fillRect(x+r, y, w-2*r, 1, c); fillRect(x+r, y+h-1, w-2*r, 1, c);
    fillRect(x, y+r, 1, h-2*r, c); fillRect(x+w-1, y+r, 1, h-2*r, c);
    for (int deg = 0; deg <= 90; deg += 6) {
      float t = deg * 3.14159f / 180.0f;
      int dx = (int)((r-1) * cosf(t)), dy = (int)((r-1) * sinf(t));
      drawPixel(x+r-1-dx, y+r-1-dy, c); drawPixel(x+w-r+dx, y+r-1-dy, c);
      drawPixel(x+r-1-dx, y+h-r+dy, c); drawPixel(x+w-r+dx, y+h-r+dy, c);
    }
  }
  bool dumpBMP(const char *path) {
    FILE *f = fopen(path, "wb"); if (!f) return false;
    int rowpad = (4 - ((W*3) % 4)) % 4, sz = 54 + (W*3 + rowpad) * H;
    uint8_t hdr[54] = {0};
    hdr[0]='B'; hdr[1]='M'; *(int*)(hdr+2)=sz; *(int*)(hdr+10)=54; *(int*)(hdr+14)=40;
    *(int*)(hdr+18)=W; *(int*)(hdr+22)=H; hdr[26]=1; hdr[28]=24;
    fwrite(hdr,1,54,f);
    uint8_t pad[3] = {0};
    for (int y = H-1; y >= 0; y--) { for (int x = 0; x < W; x++) { uint16_t c = fb[y][x];
        uint8_t px[3] = {(uint8_t)((c&31)*255/31), (uint8_t)(((c>>5)&63)*255/63), (uint8_t)(((c>>11)&31)*255/31)};
        fwrite(px,1,3,f); } fwrite(pad,1,rowpad,f); }
    fclose(f); return true;
  }
};
