#include "scenes.h"
#include "../data/Aggregator.h"
#include "Tween.h"
#include "DotFont.h"
#include "../config.h"
#include <algorithm>
#include <cmath>
#include <cstdio>
#include <cstring>

// Portable YYYY-MM-DD -> MM-DD. String::length()/c_str() exist on both
// Arduino String and host std::string, so no ifdef is needed.
static void mmdd(const String& iso, char out[6]) {
  if (iso.length() >= 10) {
    const char* c = iso.c_str();
    out[0] = c[5]; out[1] = c[6]; out[2] = '-'; out[3] = c[8]; out[4] = c[9];
  } else {
    out[0] = '?'; out[1] = '?'; out[2] = '-'; out[3] = '?'; out[4] = '?';
  }
  out[5] = 0;
}

void PieScene::enter(const std::vector<Day>& days, uint32_t now) {  slices = Aggregator::rollup(days); total = Aggregator::weekTotal(days); t0 = now;
  if (!days.empty()) {
    // Provider pushes newest-first, so back() is the oldest day.
    mmdd(days.back().date, rangeA);
    mmdd(days.front().date, rangeB);
  }
}

bool PieScene::animating(uint32_t now) const { return now - t0 < 800; }

void PieScene::draw(Arduino_GFX* g, uint32_t now, int xOff) {
  const Palette& P = activePalette();
  if (xOff == 0) g->fillScreen(P.bg);
  else g->fillRect(xOff, 0, LCD_WIDTH, LCD_HEIGHT, P.bg);
  char buf[48]; snprintf(buf, sizeof buf, "%s>%s", rangeA, rangeB);
  uiText(g, 32 + xOff, 12, buf, P.ink60);
  uiPill(g, 288 + xOff, 8, 48, 24, "7D");
  uiRule(g, 28 + xOff, 34, 312, P.hairline);
  uiText(g, 28 + xOff, 44, "MODEL COST", P.ink);
  // Flat full pie with canvas gaps, scanline-filled per wedge: for each
  // row, the wedge/row intersection is a set of x-intervals found from ray
  // crossings + circle chord ends (+ center x to split the atan2-degenerate
  // middle), each interval midpoint-tested against the wedge. Exact by
  // construction — no pinholes at any angle — and a frame is ~31k memcpy'd
  // pixels instead of ~150k plotted spoke pixels.
  const float R = 100.0f;
  const int CX = 184, CY = 174;
  const float GAP = 2.5f, D2R = 3.14159265f / 180.0f, TAU = 2.0f * 3.14159265f;
  Tween grow{0, 1, 800, t0}; float p = grow.at(now);
  float a0 = -90.0f;
  for (size_t i = 0; i < slices.size(); i++) {
    float sweep = 360.0f * (slices[i].pct / 100.0f) * p;
    uint16_t c = i < 5 ? P.slice[i] : P.other;
    float A0 = (a0 + GAP) * D2R, A1 = (a0 + sweep - GAP) * D2R;
    a0 += 360.0f * (slices[i].pct / 100.0f);
    if (A1 <= A0) continue;
    float s0 = sinf(A0), c0 = cosf(A0), s1 = sinf(A1), c1 = cosf(A1);
    for (int y = CY - (int)R; y <= CY + (int)R; y++) {
      float dy = (float)(y - CY);
      float hc2 = R * R - dy * dy;
      if (hc2 < 0) continue;
      float hc = sqrtf(hc2);
      float pts[5]; int n = 0;
      pts[n++] = (float)CX;  // splits the atan2-degenerate center
      if (s0 > 1e-6f || s0 < -1e-6f) {
        float t = dy / s0;
        if (t > 0) pts[n++] = CX + t * c0;
      }
      if (s1 > 1e-6f || s1 < -1e-6f) {
        float t = dy / s1;
        if (t > 0) pts[n++] = CX + t * c1;
      }
      pts[n++] = CX - hc;
      pts[n++] = CX + hc;
      // Insertion sort (n <= 5).
      for (int k = 1; k < n; k++) {
        float v = pts[k]; int m = k - 1;
        while (m >= 0 && pts[m] > v) { pts[m + 1] = pts[m]; m--; }
        pts[m + 1] = v;
      }
      for (int k = 0; k + 1 < n; k++) {
        // Clip to the disc chord FIRST: an interval can pass the angle test
        // while lying entirely outside the disc (both ends beyond it).
        float clo = pts[k] < CX - hc ? CX - hc : pts[k];
        float chi = pts[k + 1] > CX + hc ? CX + hc : pts[k + 1];
        int ix0 = (int)ceilf(clo), ix1 = (int)floorf(chi);
        if (ix0 > ix1) continue;
        int mx = (ix0 + ix1) / 2;
        float m = atan2f(dy, (float)(mx - CX));
        while (m < A0) m += TAU;
        if (m > A1) continue;
        g->fillRect(ix0 + xOff, y, ix1 - ix0 + 1, 1, c);
      }
    }
  }
  Tween cnt{0, total, 600, t0};
  char tb[24]; snprintf(tb, sizeof tb, "$%.2f", cnt.at(now));
  int tx = 340 - (int)strlen(tb) * 12 + xOff;
  uiText(g, tx, 44, tb, P.accent);
  uiCard(g, UI_MARGIN + xOff, 292, UI_RIGHT - UI_MARGIN, 148);
  int ly = 300;
  std::vector<String> snm;
  for (size_t i = 0; i < slices.size(); i++) snm.push_back(slices[i].name);
  std::vector<String> labels = Aggregator::uniqueShortNames(snm);
  for (size_t i = 0; i < slices.size() && ly < 440; i++, ly += 22) {
    uint16_t c = i < 5 ? P.slice[i] : P.other;
    g->fillRect(32 + xOff, ly + 3, 10, 10, c);
    // Value column is right-aligned ending at x=340; the name is truncated
    // with "~" so it never runs into the values (pitch: 6px * scale 2).
    char rb[32]; snprintf(rb, sizeof rb, "$%.2f %.0f%%", slices[i].cost, slices[i].pct);
    int vx = 340 - (int)strlen(rb) * 12 + xOff;
    int maxChars = (vx - 8 - 48) / 12;
    if (maxChars < 4) maxChars = 4;
    String nm = labels[i];
    char nb[32];
    if ((int)nm.length() > maxChars)
      snprintf(nb, sizeof nb, "%.*s~", maxChars - 1, nm.c_str());
    else
      snprintf(nb, sizeof nb, "%s", nm.c_str());
    uiText(g, 48 + xOff, ly, nb, P.ink);
    uiText(g, vx, ly, rb, P.ink60);
  }
}
