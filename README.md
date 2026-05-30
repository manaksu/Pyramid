# PyramidWatch

A Pebble Time Steel (Basalt) watchface by Manaksu.

## Layout

**Top — Pyramid + Date band**
- Inverted pyramid of 10 ▽ equilateral triangles = battery indicator
- Drains bottom-up (bottom tip first, top row last)
- Full-width char band between pyramid base line and tip line
- Left side: Day + Month (e.g. `26May`) highlighted in noise chars
- Right side: Full weekday (e.g. `Monday`) highlighted in noise chars
- Band regenerates each hour using a seeded PRNG (same seed = same layout)

**Bottom — Time + Health**
- Left: `HH` / `MM` stacked, Space Grotesk Medium 40pt
- Right of time: 3 health stats, label muted + value highlighted

## Settings (via Pebble app)

| Setting | Options |
|---|---|
| Background | ePaper Cream / Black / White |
| Stat label style | Short (STP/HR/SL) or Full (Steps/Heart/Sleep) |
| Stat 1 / 2 / 3 | Steps, Heart Rate, Sleep, Calories, Distance |

## CloudPebble setup

1. **Create Project → Import → Import from ZIP** → upload `pyramidwatch.zip`
2. **Settings → App Message Keys** — add:
   - `BG_CHOICE = 0`
   - `STAT_STYLE = 1`
   - `STAT_1 = 2`
   - `STAT_2 = 3`
   - `STAT_3 = 4`
3. **Resources** — two fonts (already in zip under `resources/fonts/`):
   - `SpaceGrotesk-Medium.ttf` → name `FONT_SG_MEDIUM_40`, characterRegex `[0-9]`
   - `SpaceGrotesk-Regular.ttf` → name `FONT_SG_REGULAR_14`, characterRegex `[0-9A-Za-z :./]`
4. Ensure `appinfo.json` has `"capabilities": ["configurable", "health"]`

## Pixel budget

```
Screen: 144 × 168px
PYR_OY=4  PYR_H=103  TIP_Y=107  BOT_H=61
Time:  2 × 29px = 58px  (fits in 61px)
Stats: 3 × 13px = 39px  (fits in 61px, side-by-side with time)
```

## File structure

```
src/
  main.c              — watchface C code
  js/
    index.js          — PebbleKit JS settings UI (inline data:text/html)
resources/
  fonts/
    SpaceGrotesk-Medium.ttf
    SpaceGrotesk-Regular.ttf
appinfo.json
README.md
triangle_battery.c    — saved battery modules (triangles + stars), not compiled
```

## Saved modules (triangle_battery.c)

Contains two standalone battery indicator implementations for future use:
- **Module A**: Triangle pyramid battery (same as this watchface)
- **Module B**: 5-star vertical column battery (20% per star)
