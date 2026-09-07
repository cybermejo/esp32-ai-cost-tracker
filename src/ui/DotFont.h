#pragma once
#include <stdint.h>
#include "../data/model.h"
#include "font5x7.h"
#include "Theme.h"
#include <Arduino_GFX_Library.h>

// Shared Notion chrome: solid-block 5x7 text, hairline rules, cards, pills.
// All colors come from activePalette() (Theme.h) — no legacy constants.
void uiText(Arduino_GFX* g, int x, int y, const String& s, uint16_t color, int scale = 2);
void uiRule(Arduino_GFX* g, int x, int y, int w, uint16_t color);
// Notion chrome (colors from activePalette): 12px white/dark card + hairline,
// tinted pill with centered label.
void uiCard(Arduino_GFX* g, int x, int y, int w, int h);
void uiPill(Arduino_GFX* g, int x, int y, int w, int h, const String& label);
