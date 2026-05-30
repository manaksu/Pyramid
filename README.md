# PyramidWatch

Pebble Time Steel (Basalt) watchface.

**Top:** Inverted pyramid (battery %) with date chars in the band on either side.
Left = Day+Month highlighted in noise. Right = Weekday highlighted in noise.

**Bottom:** HH/MM stacked left. Health stats right of time.

## CloudPebble setup
1. Import → Import from ZIP
2. Settings → App Message Keys:
   - BG_CHOICE = 0
   - STAT_STYLE = 1
   - STAT_1 = 2
   - STAT_2 = 3
   - STAT_3 = 4
3. Resources → add fonts:
   - `fonts/SpaceGrotesk-Medium.ttf`  → FONT_SG_MEDIUM_40
   - `fonts/SpaceGrotesk-Regular.ttf` → FONT_SG_REGULAR_11
