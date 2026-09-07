# AI Cost Tracker v1.0.0 — flash guide

1. Install esptool: `pip install esptool`, plug in the board over USB-C.
2. Flash at offset `0x00`: `esptool --chip esp32s3 --port /dev/cu.usbmodem* write_flash 0x00 ai-cost-tracker-v1.0.0-factory.bin`
3. Wait for "Hard resetting via RTS pin", then the light pie appears (~5s).
4. Stuck on "Waiting for download"? Hold BOOT, tap reset, release BOOT, retry step 2.
5. Swipe left/right to browse, tap a day to drill in, PWR toggles light/dark.
