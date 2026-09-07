#include "scenes.h"
#include "DotFont.h"
#include "../config.h"
#include <cstdio>
#include <cstring>

// Version modal: centered project name + version badge pill. Static content,
// no scroll; dismissed via swipe up / swipe right / BACK tap (SceneManager).
void VersionScene::draw(Arduino_GFX* g, uint32_t now, int xOff, int yOff) {
  (void)now;
  const Palette& P = activePalette();
  // Drawer-aware background: settled (yOff==0) paints full screen; mid-drop
  // paints only the visible sheet rows [0, H+yOff) so the base screen drawn
  // underneath stays visible below the descending edge.
  if (yOff == 0) {
    if (xOff == 0) g->fillScreen(P.bg);
    else g->fillRect(xOff, 0, LCD_WIDTH, LCD_HEIGHT, P.bg);
  } else {
    int h = LCD_HEIGHT + yOff;
    if (h > 0) g->fillRect(xOff, 0, LCD_WIDTH, h, P.bg);
  }
  // yOff shifts all content with the sheet; the host stub clips negative y
  // (device fillRect clips too), so offscreen parts simply don't paint.
  uiText(g, 264 + xOff, 10 + yOff, "< BACK", P.accent);
  const char* name = "AI Cost Tracker";
  int nx = 184 - (int)strlen(name) * 6 + xOff;
  uiText(g, nx, 180 + yOff, name, P.ink);
  char vb[16]; snprintf(vb, sizeof vb, "v%s", FW_VERSION);
  int pw = (int)strlen(vb) * 12 + 24;
  uiPill(g, 184 - pw / 2 + xOff, 210 + yOff, pw, 28, vb);
  const char* hint = "swipe up to close";
  int hx = 184 - (int)strlen(hint) * 6 + xOff;
  uiText(g, hx, 252 + yOff, hint, P.ink60);
}
