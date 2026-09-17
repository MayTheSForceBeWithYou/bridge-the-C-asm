# 54 — custom linker script (Track E) — OPTIONAL STRETCH

## Goal
**Optional.** Write a minimal linker script that places a custom section
(e.g. `.mysec`) and verify with `readelf -S`.

Skip this exercise if short on time — Tracks F/G do not depend on it.

## Do
1. Add a `.section .mysec, "aw"` with a global symbol in `start.s` (or C
   `__attribute__((section(".mysec")))`).
2. Write `link.ld` that keeps standard layout but ensures `.mysec` appears
   (see `info ld` linker script node). Do **not** expect a handed-out script —
   draft one from docs.
3. Link: `ld -T link.ld -o prog start.o` (adjust).
4. `readelf -S ./prog` and confirm `.mysec` exists with expected flags.

## Success
`.mysec` visible in `readelf -S`; program still runs (exit 0).

## Refs
`info ld`, `man 1 ld`, `man 1 readelf`, `man 5 elf`, `man 1 as`
