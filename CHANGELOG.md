# Changelog

## v1.1 — May 2026
- Added **Diorite** platform support (Pebble 2, Pebble 2, Pebble 2 HR)
- Fixed stat label/value colour bug — label muted, value dark
- Wrap mode large font (sz=11) is now default
- Wrap text flush against actual triangle silhouette (per-pixel-row scan)
- Version bump in appinfo.json

## v1.0 — May 2026
- Initial release — Basalt only
- Standard layout: full-width pyramid, band date chars, uniform/scaled font
- Wrap layout: smaller pyramid at left/center/right, text wraps all sides
- 3 backgrounds: ePaper Cream, Black, White
- 4 configurable health stats
- Short/Full stat label styles
- All settings persisted via persist_write/read
- PebbleKit JS inline config page (no external hosting)
