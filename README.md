# PyramidWatch

**A Pebble watchface by [Manaksu](https://manaksuwebsite.vercel.app)**

![PyramidWatch](preview.gif)

An original watchface concept built around geometry and typography. Ten inverted equilateral triangles form a pyramid — each triangle represents 10% battery, draining from the tip upward. The surrounding space fills with alphanumeric noise, with the date hidden inside it. Time and health stats anchor the bottom.

> **This concept is original.** An inverted triangle pyramid as a battery indicator, combined with typographic noise wrapping around geometry, does not exist on any other watchface platform as of May 2026.

---

## Compatibility

| Platform | Hardware | Screen |
|---|---|---|
| **Basalt** | Pebble Time, Pebble Time Steel | Color 144×168 |
| **Diorite** | Pebble 2, Pebble 2 SE, Pebble 2 HR | B&W 144×168 |

> **Note:** Heart Rate stat requires Pebble 2 HR. On Pebble Time / Time Steel the HR stat will show 0 — use Steps, Sleep, Calories, or Distance instead.

---

## Two layout modes

### Standard
Full-width pyramid. Date chars fill the gaps on either side — left = day+month, right = weekday — highlighted in noise. Uniform or scaled font (chars grow larger toward the bottom as gaps widen).

### Wrap
Smaller pyramid positioned at left, center, or right. Date text fills the entire canvas — above, below, and on both sides — wrapping flush against the actual triangle silhouette per pixel row. Small or large char font.

---

## Bottom section (both modes)
- Left: `HH` / `MM` stacked, Space Grotesk Medium
- Right of time: 4 live health stats — label muted, value highlighted

---

## Screenshots

| Standard · Cream | Standard · Black | Wrap · Center | Wrap · Left |
|---|---|---|---|
| ![](screenshots/01_cream_std_100.png) | ![](screenshots/04_black_std_70.png) | ![](screenshots/11_wrap_center_lg.png) | ![](screenshots/13_black_wrap_lg.png) |

---

## Settings

| Setting | Options |
|---|---|
| Background | ePaper Cream / Black / White |
| Layout mode | Standard / Wrap |
| Pyramid position | Left / Center / Right *(wrap mode)* |
| Wrap text size | Small / Large *(wrap mode, default: Large)* |
| Date font style | Uniform / Scaled *(standard mode)* |
| Stat label style | Short (STP/HR/SL/CAL) / Full (Steps/Heart Rate/Sleep/Calories) |
| Stat 1–4 | Steps, Heart Rate*, Sleep, Calories, Distance |

*Heart Rate requires Pebble 2 HR

All settings persist across restarts. Date band regenerates each hour via seeded PRNG.

---

## CloudPebble setup

1. **Create Project → Import → Import from ZIP**
2. **Settings → App Message Keys** — add all:

| Key | Value |
|---|---|
| BG_CHOICE | 0 |
| STAT_STYLE | 1 |
| STAT_1 | 2 |
| STAT_2 | 3 |
| STAT_3 | 4 |
| STAT_4 | 5 |
| DATE_STYLE | 6 |
| LAYOUT_MODE | 7 |
| PYR_POS | 8 |
| WRAP_FONT | 9 |

3. **Resources** — two fonts in `resources/fonts/`:
   - `SpaceGrotesk-Medium.ttf` → name `FONT_SG_MEDIUM_28`, characterRegex `[0-9]`
   - `SpaceGrotesk-Regular.ttf` → name `FONT_SG_REGULAR_10`, characterRegex `[0-9A-Za-z :./]`

4. Ensure `appinfo.json` has `"capabilities": ["configurable", "health"]`

---

## Pixel budget

```
Screen:      144 × 168px
PYR_OY=4     PYR_H=103    TIP_Y=107    BOT_H=61
Time:        2 × 27px = 54px  (sz=28, fits in 61px)
Stats:       4 × 10px + 3×2px gaps = 46px (side-by-side with time)

Wrap pyramid: S=14  PYR_WW=65  PYR_HW=57  PYR_TOP=25
Wrap flush:   scans every pixel line per char row for exact triangle edge
```

---

## File structure

```
src/
  main.c                — watchface C code
  js/
    index.js            — PebbleKit JS settings UI (inline data:text/html)
resources/
  fonts/
    SpaceGrotesk-Medium.ttf
    SpaceGrotesk-Regular.ttf
screenshots/            — store + README screenshots
preview.gif             — animated preview
appinfo.json            — targets basalt + diorite
triangle_battery.c      — saved modules (not compiled):
                          Module A: triangle pyramid battery
                          Module B: 5-star vertical column battery
```

---

## Changelog

### v1.1
- Added Diorite platform support (Pebble 2, Pebble 2 HR)
- Fixed stat label/value colour — label now muted, value highlighted
- Wrap mode large font is now default
- Triangle-flush wrap: chars hug actual silhouette per pixel row

### v1.0
- Initial release
- Basalt only (Pebble Time, Pebble Time Steel)
- Standard and Wrap layout modes
- 10 configurable settings

---

## License

© 2026 Manaksu. All rights reserved.

The PyramidWatch concept, design, and implementation are original works. You may view and study this code. You may not use, copy, modify, distribute, or create derivative works — commercial or otherwise — without explicit written permission.

For licensing enquiries: https://manaksuwebsite.vercel.app
