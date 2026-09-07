#pragma once

// XCA9554 GPIO expander (@0x20): releases the LCD/touch reset lines and
// powers the SD slot. Must run before display/touch init — without it the
// panel can stay in reset on cold boot.
bool expanderReset();
