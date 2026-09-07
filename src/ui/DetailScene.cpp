#include "scenes.h"
#include "DotFont.h"
#include "../config.h"
#include <cstdio>

// R10 header slice (Task 8's exact header lines; Task 8 adds model rows below y=64).
void DetailScene::enter(const Day& d, uint32_t now) {
  day = d; t0 = now; scrollY = 0;
}

int DetailScene::maxScroll() const {
  int need = (int)day.models.size() * 56 - 8 - (448 - 64);
  return need > 0 ? need : 0;
}

void DetailScene::scrollBy(int dy) {
  int ms = maxScroll();
  scrollY += dy;
  if (scrollY < 0) scrollY = 0;
  if (scrollY > ms) scrollY = ms;
}

void DetailScene::draw(Arduino_GFX* g, uint32_t now, int xOff) {
  (void)now;
  const Palette& P = activePalette();
  if (xOff == 0) g->fillScreen(P.bg);
  else g->fillRect(xOff, 0, LCD_WIDTH, LCD_HEIGHT, P.bg);
  std::vector<String> names;
  for (size_t i = 0; i < day.models.size(); i++) names.push_back(day.models[i].name);
  std::vector<String> labels = Aggregator::uniqueShortNames(names);
  int y = 64;
  for (size_t i = 0; i < day.models.size(); i++, y += 56) {
    int ry = y - scrollY;
    if (ry + 48 < 64) continue;  // fully under the opaque header band
    if (ry >= 448) break;
    auto &m = day.models[i];
    uint16_t tc = m.costUSD == 0 ? P.ink40 : P.ink;
    uiCard(g, UI_MARGIN + xOff, ry, UI_RIGHT - UI_MARGIN, 48);
    uiText(g, 28 + xOff, ry + 8, labels[i], tc);
    char rb[40]; snprintf(rb, sizeof rb, "%d tr $%.4g", m.traces, (double)m.costUSD);
    uiText(g, 28 + xOff, ry + 24, rb, P.ink60);
  }
  // Opaque header band LAST: scrolled rows slide underneath it instead of
  // overdrawing the date / BACK / DAY total.
  g->fillRect(xOff, 0, LCD_WIDTH, 64, P.bg);
  uiText(g, 32 + xOff, 12, day.date, P.ink);
  uiText(g, 264 + xOff, 10, "< BACK", P.accent);
  uiRule(g, 28 + xOff, 28, 312, P.hairline);
  char b[32]; snprintf(b, sizeof b, "DAY $%.2f", day.costUSD);
  uiText(g, 28 + xOff, 38, b, P.accent);
}
