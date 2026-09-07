#include "Theme.h"

static Theme g_theme = Theme::Light;

#ifdef UNIT_TEST
// Host: plain in-memory persistence so toggle round-trips in tests.
void themeInit() { g_theme = Theme::Light; }
Theme loadTheme() { return g_theme; }
void saveTheme(Theme t) { g_theme = t; }
#else
#include <Preferences.h>
void themeInit() {
  g_theme = Theme::Light;
  Preferences pf;
  if (pf.begin("aicost", true))
    g_theme = pf.getUChar("theme", 0) ? Theme::Dark : Theme::Light;
  pf.end();
}
Theme loadTheme() { return g_theme; }
void saveTheme(Theme t) {
  g_theme = t;
  Preferences pf;
  if (pf.begin("aicost", false)) pf.putUChar("theme", t == Theme::Dark ? 1 : 0);
  pf.end();
}
#endif

Theme activeTheme() { return g_theme; }
void setActiveTheme(Theme t) { saveTheme(t); }
