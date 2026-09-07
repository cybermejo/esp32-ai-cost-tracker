#include "scenes.h"
#include "DotFont.h"
#include "../config.h"
#include <cstdio>

void ListScene::enter(const std::vector<Day>& d, uint32_t now) {
  days = d; t0 = now;
}

int ListScene::rowAt(int y) const {
  if (y < 66) return -1;
  int m = (y - 66) % 54;
  if (m >= 46) return -1;  // 8px inter-card gap is not a row
  int r = (y - 66) / 54;
  if (r < 0 || r >= (int)days.size()) return -1;
  return r;
}

void ListScene::draw(Arduino_GFX* g, uint32_t now, int xOff) {
  const Palette& P = activePalette();
  if (xOff == 0) g->fillScreen(P.bg);
  else g->fillRect(xOff, 0, LCD_WIDTH, LCD_HEIGHT, P.bg);
  uiText(g, 32 + xOff, 12, "DAILY COST 7D", P.ink);
  uiPill(g, 288 + xOff, 8, 48, 24, "7D");
  uiRule(g, 28 + xOff, 34, 312, P.hairline);
  float mx = 0.01f; int maxIdx = -1;
  for (size_t i = 0; i < days.size(); i++)
    if (days[i].costUSD > mx) { mx = days[i].costUSD; maxIdx = (int)i; }
  for (int i = 0; i < (int)days.size(); i++) {
    int y = 66 + i * 54;
    uiCard(g, UI_MARGIN + xOff, y, UI_RIGHT - UI_MARGIN, 46);
    uiText(g, 28 + xOff, y + 15, days[i].date, P.ink);
    char b[16]; snprintf(b, sizeof b, "$%.2f", days[i].costUSD);
    int vx = 340 - (int)strlen(b) * 12 + xOff;
    uiText(g, vx, y + 15, b, i == maxIdx ? P.accent : P.ink);
  }
}
