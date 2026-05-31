# Changelog

## v1.2 — May 2026
- Fixed crash on real hardware: replaced `GPoint[]` compound literals with pre-declared array
- Fixed crash: moved `#define` constants out of function scope to file scope
- Empty triangles now render as grey (muted) instead of transparent
- Stat labels now dark grey, clearly distinct from values
- Wrap mode large font bumped to GOTHIC_18 (sz≈15) for better readability on watch
- Removed Heart Rate stat (Basalt/Pebble Time Steel has no HR sensor)
- Health stats revised to 4 Basalt-available metrics: Steps, Sleep, Calories, Distance

## v1.1 — May 2026
- Wrap mode large font (sz=11) set as default
- Triangle-flush wrap: chars hug actual silhouette per pixel row scan
- Stat label muted, value dark — drawn separately
- appinfo.json version bump

## v1.0 — May 2026
- Initial release — Basalt only (Pebble Time, Pebble Time Steel)
- Standard layout: full-width pyramid, band date chars, uniform/scaled font
- Wrap layout: smaller pyramid at left/center/right, text wraps all sides
- 3 backgrounds: ePaper Cream, Black, White
- 4 configurable health stats
- Short/Full stat label styles
- All settings persisted via persist_write/read
- PebbleKit JS inline config page (no external hosting)
