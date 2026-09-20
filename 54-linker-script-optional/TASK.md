# 54 — custom linker script (practice) — OPTIONAL

Read `LESSON.md` first. This file is only the lab. Skip if short on time.

## Do (optional)

1. Add `.section .mysec, "aw"` (or C section attribute) with a global symbol.
2. Draft `link.ld` so `.mysec` survives (`info ld`); no handed-out script.
3. `ld -T link.ld -o prog start.o` (adjust); `readelf -S ./prog` confirms `.mysec`.

## Done when

- `.mysec` visible in `readelf -S`; program exits 0 — or you consciously skipped.

## Lookup

`info ld`, `man 1 ld`, `man 1 readelf`, `man 5 elf`, `man 1 as`.
